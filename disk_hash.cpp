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

#include <cassert>
#include <cstring>
#include <stdint.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <stdexcept>
#include "minimal_external_hash.h"

#if 0
class mapped_file {
public:
    mapped_file( const char *filename, bool read_write ) {
        int mode = O_RDONLY;
        int prot = PROT_READ;
        if( read_write ) {
            mode = O_RDWR;
            prot = PROT_READ | PROT_WRITE;
        }
        
        fd_ = open( filename, mode );
        assert( fd_ != -1 );

        size_ = lseek( fd_, 0, SEEK_END );
        lseek( fd_, 0, SEEK_SET );
        
        base_ = mmap( 0, size_, prot, MAP_SHARED, fd_, 0 );
        assert( base_ != 0 );
        
        if( read_write ) {
            madvise( base_, size_, MADV_SEQUENTIAL );
        }
    }
    size_t size() const {
        return size_;
    }
    ~mapped_file() {
        munmap( base_, size_ );
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
};

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


class hash_builder {
    typedef int64_t int_type;
    const static size_t int_size = sizeof(int_type);
public:
    hash_builder( size_t table_size ) : table_size_(table_size), link_offsets_(table_size), num_extra_hops_(0) {
        
        // link_offsets_ stores the 'next pointers' per bucket. Initially they point to
        // entries in the hash table.
        for( size_t i = 0; i < table_size_; ++i ) {
            link_offsets_[i] = i * int_size;
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
            
//             std::cout << "append_pos " << it->second << "\n";
            
            std::copy( name.begin(), name.end(), append_pos );
            
            append_pos += name.size();
            *append_pos = 0; ++append_pos;
            
            // write file size
            std::ifstream is( it->first );
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
        
        // write bucket chains (i.e., the 'next pointers')
        for( std::vector< std::pair< size_t, size_t > >::iterator it = ptrs_.begin(); it != ptrs_.end(); ++it ) {
            int_type x = it->second;
            char *xcp = (char*)&x;
            
            std::copy( xcp, xcp + int_size, base + it->first );
        }
        
        std::cout << "fill factor: " << ptrs_.size() / float(table_size_) << "\n";
        std::cout << "avg hops: " << (ptrs_.size() + num_extra_hops_) / float( ptrs_.size()) << "\n";
        
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
        ptrs_.push_back( std::make_pair( link_offsets_[bucket], append_pos_ ) );
        
        
        // statistics: count as 'extra hop' if link_offset_[bucket] does not point into hash table (i.e., the bucket is non-empty)
        if( link_offsets_[bucket] >= table_size_ * int_size ) {
            ++num_extra_hops_;
        }
        
        // update 'next pointer'
        link_offsets_[bucket] = append_pos_;
        
        // remember file content location (starts after the chaining link)
        size_t file_pos = append_pos_ + int_size;
        file_dest_.push_back( std::make_pair( std::string(filename), file_pos ));

        file_pos += strlen(filename) + 1 + 8;
        file_pos = align( file_pos );
//         std::cout << "file pos: " << file_pos << "\n";
        append_pos_ = file_pos + size;
        
        return true;
    }
    
    template<typename T>
    static T align( T v ) {
        const size_t a = 32;
        
        return v + (a - (size_t(v) % a)) % a;
    }
private:
    
    std::streampos file_size( const char *filename ) {
        std::ifstream is( filename );
            
        assert( is.good() );
            
        is.seekg( 0, std::ios_base::end );
        
        return is.tellg();
    }
    
    const size_t table_size_;
    std::vector<size_t> link_offsets_;
    std::vector<std::pair<std::string,size_t>> file_dest_;
    std::vector<std::pair<size_t,size_t>> ptrs_;
    size_t append_pos_;
    size_t num_extra_hops_;
};

class disk_hash {
public:
    typedef int64_t int_type;
    const static size_t int_size = sizeof(int_type);
    
    disk_hash( const char *filename ) : mf_(filename, false) {
        base_ = (char *)mf_.base();
        size_t s = mf_.size();
        
        assert( s > int_size);
        int_type ts = get_int( base_ + s - int_size );
        
        std::cout << "table size: " << ts << "\n";
        table_size_ = ts;
        assert( base_ == hash_builder::align(base_));
    }
    
    char *find( const char *filename, size_t &out_size ) {
        size_t name_hash = hash_value( filename );
        size_t bucket = name_hash % table_size_;
        
        // start offset of hash chain
        size_t offset = get_int( base_ + bucket * int_size );
        
        size_t chain_len = 0;
        
        // traverse current hash chain
        while( offset != 0 ) {
            validate_offset( offset );
            
            const size_t next_offset = get_int( base_ + offset );
            offset += int_size;

            if( strncmp( filename, base_ + offset, remaining_size( offset ) ) == 0 ) {
                offset += strlen( filename ) + 1;
                
                size_t size = get_int( base_ + offset );
                offset += int_size;
                offset = hash_builder::align(offset);
//                 std::cout << "found: " << filename << " chunk size: " << size << " at " << std::hex << offset << std::dec << " chain: " << chain_len << "\n";
                if( remaining_size( offset ) < size ) {
                    throw std::runtime_error( "internal error. premature end of hash file." );
                }
                out_size = size;
                return base_ + offset;
                
            }
            ++chain_len;
            offset = next_offset;
        }
        
        // fell through: name not found
        out_size = size_t(-1);
        return 0;
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
    char *base_;
};
#endif

int main() {
        
    //const size_t ht_size = 1024 * 32;
    
    const size_t ht_size = 1024 * 8;
    if( !false )
    {
        meh::file_based_builder hb( ht_size );
    
        {
            std::ifstream is( "files.txt" );
            
            while( is.good() ) {
                std::string name;
                is >> name;
                
//                 std::cout << "name: " << name << "\n";
                
                if( !name.empty() ) {
                    
                    std::ifstream ts( name.c_str() );
                    
                    if( ts.good() ) {
                        hb.add( name.c_str() );
                    }
                }
            }
            
        }
        hb.write( "hash.bin" );
        hb.print_stats();
    }
    
    size_t nfound = 0;
    meh::hash dh( "hash.bin" );
//     while(true)
    {
        
        std::ifstream is( "files.txt" );
        
        while( is.good() ) {
            std::string name;
            is >> name;
            
//             std::cout << "name: " << name << "\n";
            
            if( !name.empty() ) {
                size_t size;
                char *data = dh.find( name.c_str(), size );
                if( data != 0 ) {
                    nfound++;
                }
                
            }
        }
        
        std::cout << "found: " << nfound << "\n";
//         dh.mf().unmap();
//         getchar();
//         dh.mf().map();
    
    }
    
    
    
    
    
    
//     mapped_file mf( "test.bin", false );
    
}