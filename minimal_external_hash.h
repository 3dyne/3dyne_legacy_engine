/*
 * Copyright (C) 2013 Simon A. Berger
 *
 *  This program is free software; you may redistribute it and/or modify its
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>

#include <cassert>
#include <cstring>
// #include <cstdint>

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <stdexcept>

namespace meh {
#ifndef win32_x86
class mapped_file {
public:
    mapped_file( const char *filename, bool read_write )
      : base_(0),
      read_write_(read_write)
    {
        int mode = O_RDONLY;
        
        if( read_write ) {
            mode = O_RDWR;
        }
        
        fd_ = open( filename, mode );
        assert( fd_ != -1 );

        size_ = lseek( fd_, 0, SEEK_END );
        lseek( fd_, 0, SEEK_SET );
        
        map();
        
    }
    size_t size() const {
        return size_;
    }
    
    void unmap() {
        assert( base_ != 0 );
        munmap( base_, size_ );
        base_ = 0;
    }
    void map() {
        assert( base_ == 0 );
        int prot = PROT_READ;
        if( read_write_ ) {
            prot = PROT_READ | PROT_WRITE;
        }
        
        base_ = mmap( 0, size_, prot, MAP_SHARED, fd_, 0 );
        assert( base_ != 0 );
        
        if( read_write_ ) {
            madvise( base_, size_, MADV_SEQUENTIAL );
        }
    }
    
    ~mapped_file() {
        if( base_ != 0 ) {
            unmap();
        }
        close( fd_ );
    }
    
    void *base() {
        return base_;
    }
 

private:
    void *base_;
//     bool read_write_;
    int fd_;
    off_t size_;
    bool read_write_;
};
#else
#error untested Win32 implementation of mapped_file
class mapped_file {
public:
    mapped_file( const char *filename, bool read_write )
      : base_(0),
      read_write_(read_write)
    {
        DWORD access = GENERIC_READ;
        DWORD share = FILE_SHARE_READ;
	DWORD create = OPEN_EXISTING;
	DWORD prot = PAGE_READONLY;
        if( read_write ) {
            mode |= GENERIC_WRITE;
            prot |= FILE_SHARE_WRITE;
	    create = CREATE_ALWAYS;
            prot = PAGE_READWRITE;
        }
        
        fd_ = CreateFile (filename, access, share, NULL, create, FILE_ATTRIBUTE_NORMAL, NULL);
        assert( fd_ != (HANDLE)INVALID_HANDLE_VALUE );

        DWORD size_low_ = GetFileSize( fd_, &size_high_ );
	mapping_ = CreateFileMapping (fd_, NULL, prot, size_high_, size_low_, NULL);
        assert( mapping_ != 0 );

        map();
    }
    
    size_t size() const {
	// TODO: check bit fiddling (is a left shift up or down...)
	
        size_t size = size_high;
	size <<= 32;
	size |= size_low;
	
	return size;
    }
    
    void unmap() {
        assert( base_ != 0 );
        UnmapViewOfFile( base_ );
	base_ = 0;
    }
    void map() {
        assert( base_ == 0 );
	DWORD access = FILE_MAP_READ;
        if( read_write_ ) {
	    access |= FILE_MAP_WRITE;
        }
        
	base_ = MapViewOfFile ( mapping_, access, 0, 0, 0);
	
        assert( base_ != 0 );
        
    }
    
    ~mapped_file() {
        if( base_ != 0 ) {
            unmap();
        }
        CloseHandle( mapping_ );
        CloseHandle( fd_ );
    }
    
    void *base() {
        return base_;
    }
 

private:
    void *base_;
//     bool read_write_;
    HANDLE fd_;
    HANDLE mapping_;
    DWORD size_low_;
    DWORD size_high_;
    bool read_write_;
};

#endif


template<typename K>
uint64_t hash_value( const K &k ) {
    return k.hash();
}


uint64_t hash_value( const std::string &s ) {
    uint64_t hash = 5381;

    for( size_t i = 0; i < s.size(); ++i ) {
        hash = ((hash << 5) + hash) ^ s[i]; /* hash * 33 + c */
    }
    return hash;
}

    
   
uint64_t hash_value( const char *s ) {
    uint64_t hash = 5381;
    while( *s != 0 ) {
        hash = ((hash << 5) + hash) ^ *s; /* hash * 33 + c */
        ++s;
    }
    
    return hash;
}


template<typename T>
static T align( T v ) {
    const size_t a = 32; // FIXME: magic number
    
    return v + (a - (size_t(v) % a)) % a;
}

class file_based_builder {
    typedef int64_t int_type;
    const static size_t int_size = sizeof(int_type);
public:
    file_based_builder( size_t table_size ) : table_size_(table_size), next_pointers_(table_size), num_extra_hops_(0) {
        
        // next_pointers_ stores the 'next pointers' per bucket. Initially they point to
        // entries in the hash table.
        for( size_t i = 0; i < table_size_; ++i ) {
            next_pointers_[i] = i * int_size;
        }
        
        // start appending after the hash table
        append_pos_ = table_size_ * int_size;
    }
    
    void write( const char *filename ) {
        {
            std::ofstream os( filename );
            os.seekp( append_pos_ );
            
            int_type x = table_size_;
            
            // write the table size at the end of the file
            // this will also grow the file to the necessary size
            os.write( (char*)(&x), sizeof(int_type));
        }
        //std::cout << "append: " << append_pos_ << "\n";
        mapped_file mf( filename, true );
        char *base = (char*) mf.base();
        
        // write file headers (name, size) and contents
        for( std::vector< std::pair< std::string, size_t > >::iterator it = file_dest_.begin(); it != file_dest_.end(); ++it ) {
            
            // write name (0 terminated)
            const std::string &name = it->first;
            char *append_pos = base + it->second;
            std::copy( name.begin(), name.end(), append_pos );
           
            append_pos += name.size();
            *append_pos = 0; ++append_pos;
            
            // write file size
            std::ifstream is( it->first.c_str() );
            assert( is.good() );

            is.seekg( 0, std::ios::end );
            size_t size = is.tellg();
            is.seekg( 0, std::ios::beg );
            {
                int_type x = size;
                char *xcp = (char*)&x;
                std::copy( xcp, xcp + int_size, append_pos );
            }
            append_pos += int_size;
            
            // write file content
            append_pos = align( append_pos );
            is.read( append_pos, size );
        }
        
        // write hash chains (i.e., the 'next pointers')
        for( std::vector< std::pair< size_t, size_t > >::iterator it = chain_links_.begin(); it != chain_links_.end(); ++it ) {
            int_type x = it->second;
            char *xcp = (char*)&x;
            
            std::copy( xcp, xcp + int_size, base + it->first );
        }
        
        
        
    }
    
    bool add( const char *filename ) {
        std::streampos size = file_size( filename );

        if( size > 0xffffffff ) { // WTF? tellg should return -1, instead it returns '-1 with sign bit set to zero...'
            std::cout << "ignore " << filename << "\n";
            return false;
        }
        
        uint64_t name_hash = hash_value( filename );
        size_t bucket = name_hash % table_size_;
        
        // remember current 'next pointer' for writing later 
        chain_links_.push_back( std::make_pair( next_pointers_[bucket], append_pos_ ) );
        
        
        // statistics: count as 'extra hop' if link_offset_[bucket] does not point into hash table (i.e., the bucket is non-empty)
        if( next_pointers_[bucket] >= table_size_ * int_size ) {
            ++num_extra_hops_;
        }
        
        // update 'next pointer'
        next_pointers_[bucket] = append_pos_;
        
        // remember file content location (starts after the chaining link)
        size_t file_pos = append_pos_ + int_size;
        file_dest_.push_back( std::make_pair( std::string(filename), file_pos ));

        file_pos += strlen(filename) + 1 + 8;
        file_pos = align( file_pos );
//         std::cout << "file pos: " << file_pos << "\n";
        append_pos_ = file_pos + size;
        
        return true;
    }
    
    void print_stats() {
        std::cout << "fill factor: " << chain_links_.size() / float(table_size_) << "\n";
        std::cout << "avg hops: " << (chain_links_.size() + num_extra_hops_) / float( chain_links_.size()) << "\n";   
    }
    
private:
    
    std::streampos file_size( const char *filename ) {
        std::ifstream is( filename );
            
        assert( is.good() );
            
        is.seekg( 0, std::ios_base::end );
        
        return is.tellg();
    }
    
    const size_t table_size_;
    std::vector<size_t> next_pointers_;
    std::vector<std::pair<std::string,size_t> > file_dest_;
    std::vector<std::pair<size_t,size_t> > chain_links_;
    size_t append_pos_;
    size_t num_extra_hops_;
};


class hash {
public:
    typedef int64_t int_type;
    const static size_t int_size = sizeof(int_type);
    
    hash( const char *filename ) : mf_(filename, false) {
        char *base = (char *)mf_.base();
        size_t s = mf_.size();
        
        assert( s > int_size);
        int_type ts = get_int( base + s - int_size );
        
        std::cout << "table size: " << ts << "\n";
        table_size_ = ts;
        assert( base == align(base));
    }
    
    char *find( const char *filename, size_t &out_size ) {
        char *base = (char *)mf_.base();
        assert( base == align(base));
        
        size_t name_hash = hash_value( filename );
        size_t bucket = name_hash % table_size_;
        
        // start offset of hash chain
        size_t offset = get_int( base + bucket * int_size );
        
        size_t chain_len = 0;
        
        // traverse current hash chain
        while( offset != 0 ) {
            validate_offset( offset );
            
            const size_t next_offset = get_int( base + offset );
            offset += int_size;

            if( strncmp( filename, base + offset, remaining_size( offset ) ) == 0 ) {
                offset += strlen( filename ) + 1;
                
                size_t size = get_int( base + offset );
                offset += int_size;
                offset = align(offset);
//                 std::cout << "found: " << filename << " chunk size: " << size << " at " << std::hex << offset << std::dec << " chain: " << chain_len << "\n";
                if( remaining_size( offset ) < size ) {
                    throw std::runtime_error( "internal error. premature end of hash file." );
                }
                out_size = size;
                return base + offset;
            }
            ++chain_len;
            offset = next_offset;
        }
        
        // fell through: name not found
        out_size = size_t(-1);
        return 0;
    }
    
    mapped_file& mf() {
        return mf_;
    }
private:
    void validate_offset( size_t ofs ) {
        if( ofs > mf_.size() - int_size ) {
            throw std::runtime_error( "internal error. offset out of range." );
        }
        
    }
    size_t remaining_size( size_t offset ) {
        if( offset > mf_.size() ) {
            throw std::runtime_error( "internal error. offset out of range 2." );
        }
        return mf_.size() - offset;
    }
    int_type get_int( char *ptr ) {
        int_type x;
        char *xcp = (char*)&x;
        
        std::copy( ptr, ptr + int_size, xcp );
        return x;
    }
    
    size_t table_size_;
    mapped_file mf_;
};


class hash_nonmapped {
public:
    typedef int64_t int_type;
    const static size_t int_size = sizeof(int_type);
    
    // WARNING: the input stream is 'moved' to this object (i.e., currently unclean move semantics)
    hash_nonmapped( std::ifstream &is ) {
        assert( is_.good() );
        off_t size = is.seekg( 0, std::ios::end ).tellg();
        is.seekg( size - int_size );
        assert( !is.fail() );
        is.read( (char*)&table_size_, int_size );
        
        std::cout << "table size: " << table_size_ << "\n";
    }
    
    size_t find( const char *filename, std::ifstream &is ) const {
        size_t name_hash = hash_value( filename );
        size_t bucket = name_hash % table_size_;
        
        // start offset of hash chain
        is.seekg( bucket * int_size );
        
        int_type offset;
        is.read( (char*) &offset, int_size );
        assert( !is.fail() );
        
        
        size_t chain_len = 0;
        
        // traverse current hash chain
        std::string read_name;
        
        
        while( offset != 0 ) {
            is.seekg( offset, std::ios::beg );
            assert( !is.fail() );
            
            
            is.read( (char*) &offset, int_size );
            assert( !is.fail() );
            
            //offset += int_size;

            std::getline( is, read_name, '\0' );
            assert( !is.fail() );
            
            if( false ) {
                std::cout << "offset: " << std::ios::hex << offset << " " << read_name << std::endl;
            }
            
            if( read_name == filename ) {
                
                
                int_type size;
                is.read( (char*)&size, int_size );
                
                off_t cur_offs = is.tellg();
                off_t align_offs = align( cur_offs );
                if( cur_offs != align_offs ) {
                    is.seekg( align_offs );
                    assert( !is.fail() );
                }
                
                return size;
            }
            ++chain_len;
        }
        
        // fell through: name not found
        return size_t(-1);
    }
private:
    int_type table_size_;
};


}