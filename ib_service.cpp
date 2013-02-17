/* 
 * 3dyne Legacy Engine GPL Source Code
 * 
 * Copyright (C) 1996-2012 Matthias C. Berger & Simon Berger.
 * 
 * This file is part of the 3dyne Legacy Engine GPL Source Code ("3dyne Legacy
 * Engine Source Code").
 *   
 * 3dyne Legacy Engine Source Code is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 * 
 * 3dyne Legacy Engine Source Code is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * 3dyne Legacy Engine Source Code.  If not, see
 * <http://www.gnu.org/licenses/>.
 * 
 * In addition, the 3dyne Legacy Engine Source Code is also subject to certain
 * additional terms. You should have received a copy of these additional terms
 * immediately following the terms and conditions of the GNU General Public
 * License which accompanied the 3dyne Legacy Engine Source Code.
 * 
 * Contributors:
 *     Matthias C. Berger (mcb77@gmx.de) - initial API and implementation
 *     Simon Berger (simberger@gmail.com) - initial API and implementation
 */



// ib_service.c




#include <fstream>
#include <memory>
#include <sstream>
#include <algorithm>

#include "minimal_external_hash.h"

#include "shock.h"
#include "ib_service.h"

static ibase::service *g_service = 0;

namespace ibase {
    
// class file {
// public:
//     virtual size_t get_size() = 0;
//     virtual size_t get_pos() = 0;
//     virtual void set_pos( size_t ) = 0;
//     virtual size_t read( void *ptr, size_t bsize, size_t nb ) = 0;
//     virtual int get_char() = 0;
//     virtual int get_short() = 0;
//     virtual int get_int() = 0;
//     
//     virtual ~file() {}
//     
// };

class source {
public:
    virtual file *open( const char *filename ) = 0;
    virtual ~source() {}
};



namespace impl_iso_fstream {
    
    
    class file : public ibase::file {
    public:
        file( const char *filename ) 
        : is_(filename) 
        {
            
        }
        bool is_open() {
            return is_.is_open();
        }
        virtual size_t get_size() {
            std::streampos pos = is_.tellg();
            is_.seekg(0, std::ios::end);
            std::streampos size = is_.tellg();
            is_.seekg(pos, std::ios::beg);
            
            return size;
        }
        virtual size_t get_pos() {
            return is_.tellg();
        }
        virtual void set_pos( size_t pos ) {
            is_.seekg( pos, std::ios::beg );
        }
        virtual size_t read( void *ptr, size_t bsize, size_t nb ) {
            if( nb != 1 ) {
                __error( "nb != 1 not supported" );
            }
            
            return is_.readsome( (char*)ptr, bsize );
            
        }
        virtual int get_char() {
            return is_.get();
        }
        virtual int get_short() {
            short s;
            char *x = (char*)&s;
            x[0] = get_char();
            x[1] = get_char();
            return s;
        }
        virtual int get_int() {
            int i;
            char *x = (char*)&i;
            x[0] = get_char();
            x[1] = get_char();
            x[2] = get_char();
            x[3] = get_char();
            
            return i;
        }
        
        virtual const char *map() {
            const size_t size = get_size();
            map_emu_.resize( size );
            set_pos(0);
            read( &map_emu_.front(), size, 1 );
            
            return &map_emu_.front();
        }
        
        virtual void unmap() {
            map_emu_.resize(0);
        }
        
        virtual ~file() {}
    private:
        std::ifstream is_;
        std::vector<char> map_emu_;
    };
    
    class source : public ibase::source {
    public:
        source( const char *path ) : path_(path) {}
        
        file *open( const char *filename ) {
            std::stringstream ss;
            
            ss << path_;
            ss << "/";
            ss << filename;
            __named_message( "try open: %s\n", ss.str().c_str() );
            
            std::auto_ptr<impl_iso_fstream::file> fp( new impl_iso_fstream::file( ss.str().c_str() ));
            if( fp->is_open() ) {
                return fp.release();
            } else {
                return 0;
            }
        }
    private:
        std::string path_;
    };
    
}

namespace impl_meh {
    
    
    class file : public ibase::file {
    public:
        file( const char *begin, const char *end ) 
        : begin_(begin),
        end_(end),
        ptr_(begin)
        {
            
        }
        
        virtual size_t get_size() {
            
            return std::distance(begin_,end_);
        }
        virtual size_t get_pos() {
            return std::distance( begin_, ptr_ );
        }
        virtual void set_pos( size_t pos ) {
            ptr_ = begin_ + pos;
        }
        virtual size_t read( void *ptr, size_t bsize, size_t nb ) {
            if( nb != 1 ) {
                __error( "nb != 1 not supported" );
            }
            
            size_t left = std::distance( ptr_, end_ );
            size_t to_read = std::min( bsize, left );
            
            std::copy( ptr_, ptr_ + to_read, (char*)ptr );
            ptr_ += to_read;
            
            return to_read;
        }
        virtual int get_char() {
            if( ptr_ >= end_ ) {
                return -1;
            } else {
                return *(unsigned char *)(ptr_++);
            }
        }
        virtual int get_short() {
            short s;
            char *x = (char*)&s;
            x[0] = get_char();
            x[1] = get_char();
            return s;
        }
        virtual int get_int() {
            int i;
            char *x = (char*)&i;
            x[0] = get_char();
            x[1] = get_char();
            x[2] = get_char();
            x[3] = get_char();
            
            return i;
        }
        virtual const char *map() {
            return begin_;
        }
        virtual void unmap() {
            
        }
        
        virtual ~file() {}
    private:
        const char *begin_;
        const char *end_;
        const char *ptr_;
    };
    
    class source : public ibase::source {
    public:
        source( const char *filename ) : hash_(filename) {
            
        }
        
        
        ibase::file *open( const char *filename ) {
            std::stringstream ss;
            size_t s = 0;
            char *base = hash_.find(filename, s);
            
            if( base == 0 ) {
                return 0;
            } else {
                __named_message( "open: %s\n", filename );
                return new impl_meh::file( base, base + s );
            }
        }
    private:
        meh::hash hash_;
    };
    
} // namespace impl_meh


namespace impl_meh_nonmapped {
    
    
    class file : public ibase::file {
    public:
        file( const meh::hash_nonmapped &hash, const char *meh_name, const char *entry_name ) 
        : is_(meh_name)
        {
            assert( is_.good() );
            
//             __named_message( "try open: %s\n", entry_name );
            size_ = hash.find( entry_name, is_ );
            
            assert( size_ != size_t(-1) );
            
            if( size_ == size_t(-1) ) {
                is_.close();
//                 std::cout << "not found: " << entry_name << "\n";
            } else {
            
                base_offset = is_.tellg();
                
//                 __named_message( "found %s at %x\n", entry_name, base_offset );
            }
            
        }
        bool is_open() {
            return is_.is_open();
        }
        virtual size_t get_size() {
            return size_;
        }
        virtual size_t get_pos() {
            return is_.tellg() - base_offset;
        }
        virtual void set_pos( size_t pos ) {
            is_.seekg( base_offset + pos, std::ios::beg );
        }
        virtual size_t read( void *ptr, size_t bsize, size_t nb ) {
            if( nb != 1 ) {
                __error( "nb != 1 not supported" );
            }
            
            return is_.readsome( (char*)ptr, bsize );
            
        }
        virtual int get_char() {
            return is_.get();
        }
        virtual int get_short() {
            short s;
            char *x = (char*)&s;
            x[0] = get_char();
            x[1] = get_char();
            return s;
        }
        virtual int get_int() {
            int i;
            char *x = (char*)&i;
            x[0] = get_char();
            x[1] = get_char();
            x[2] = get_char();
            x[3] = get_char();
            
            return i;
        }
        
        virtual const char *map() {
            const size_t size = get_size();
            map_emu_.resize( size );
            set_pos(0);
            read( &map_emu_.front(), size, 1 );
            
            return &map_emu_.front();
        }
        
        virtual void unmap() {
            map_emu_.resize(0);
        }
        
        virtual ~file() {}
    private:
        std::ifstream is_;
        off_t base_offset;
        size_t size_;
        std::vector<char> map_emu_;
    };
    
    class source : public ibase::source {
    public:
        source( const char *meh_filename ) : meh_filename_(meh_filename), 
        meh_is_(meh_filename_.c_str()),
        hash_( meh_is_ ) {}
        
        file *open( const char *filename ) {
            std::stringstream ss;
            
            std::auto_ptr<impl_meh_nonmapped::file> fp( new impl_meh_nonmapped::file( hash_, meh_filename_.c_str(), filename ));
            if( fp->is_open() ) {
                return fp.release();
            } else {
                return 0;
            }
        }
    private:
        std::string meh_filename_;
        std::ifstream meh_is_; // only necessary for calling the hash_nonmapped constructor...
        meh::hash_nonmapped hash_;
    };
    
}

service::service() {
    if(true) {
        logs_.open( "ib_log.txt" );
    }
    
}
void service::add_source ( const char* name, int stype )
{
    if( logs_.is_open() ) {
        logs_ << "add source " << sources_.size() << " type " << stype << " name " << name << "\n";
    }
    
    switch ( stype ) {
    case SOURCE_DISK:
        sources_.push_back ( new impl_iso_fstream::source ( name ) );
        break;

    case SOURCE_MEH:
        if( !false ) {
            __named_message ( "add meh source: %s\n", name );
            sources_.push_back ( new impl_meh::source ( name ) );
        } else {
            __named_message ( "add nonmapped meh source: %s\n", name );
            sources_.push_back ( new impl_meh_nonmapped::source ( name ) );
        }
        break;

    default:
        __error ( "type not implemented: %d\n", stype );
        break;
    }
}
file* service::open ( const char* name )
{
    if( name[0] == '/' && name[1] != 0 ) {
        __warning( "remove trailing / in %s\n", name );
        ++name;
        
    }
    
    for ( std::vector<source *>::reverse_iterator it = sources_.rbegin(); it != sources_.rend(); ++it ) {
        file *f = ( *it )->open ( name );

        if ( f != 0 ) {
            if( logs_.is_open() ) {
                
                logs_ << "open from " << (sources_.size() - std::distance( sources_.rbegin(), it ) - 1) << " " << name << "\n";
            }
            
            last_name_ = name;
            return f;
        }
    }

    return 0;
}
const char* service::last_name_cstr()
{
    return last_name_.c_str();
}
service& service::get_singleton()
{
    if( g_service == 0 ) {
        __error( "global ibase::service instance not initialized\n" );
    }
    
    return *g_service;
}

service::~service() {
    std::for_each( sources_.begin(), sources_.end(), deleter<ibase::source>() );
}

// class service {
// public:
//     void add_source( const char *name, int stype ) {
//         switch( stype )
//         {
//             case SOURCE_DISK:
//                 sources_.push_back( new impl_iso_fstream::source( name ) );
//                 break;
//                 
//             case SOURCE_MEH:
//                 __named_message( "add meh source: %s\n", name );
//                 sources_.push_back( new impl_meh::source( name ) );
//                 break;
//                 
//             default:
//                 __error( "type not implemented: %d\n", stype );
//                 break;
//         }
//     }
// 
//     file* open( const char *name ) {
//         for( std::vector<source *>::reverse_iterator it = sources_.rbegin(); it != sources_.rend(); ++it ) {
//             file *f = (*it)->open( name );
//             
//             if( f != 0 ) {
//                 last_name_ = name;
//                 return f;
//             }
//         }
//         
//         return 0;
//     }
//     
//     const char *last_name_cstr() {
//         return last_name_.c_str();
//     }
//     
// private:
//     std::vector<source *> sources_;
//     std::string last_name_;
// };


}


// legacy interface
// funny ansi-c fact: the wrapper boilerplate code is longer than the actual c++ implementation. 

int ib_lastaction = 0;

int sc_readbytes = 0;
extern int gc_renderframe;

void IB_StartUp() {
    if( g_service != 0 ) {
        __error( "g_service != 0\n" );
    }
    
    g_service = new ibase::service();
}


void IB_ShutDown()
{
    if( g_service != 0 ) {
        delete g_service;
        g_service = 0;
    }
}


void IB_AddSource( const char *path, int stype ) {
    if( g_service == 0 ) {
        __error( "g_service == 0\n" );
    }
    __named_message( "add source: %s\n", path );
    g_service->add_source(path, stype);
}


ib_file_t *IB_Open( const char *name ) {
    if( g_service == 0 ) {
        __error( "g_service == 0\n" );
    }
    
    
    ibase::file *f = g_service->open( name );
    if( f != 0 ) {
        ib_file_t *wrap_f = new ib_file_t;
        wrap_f->f_ = f;
        
        ib_lastaction = gc_renderframe;
        return wrap_f;
    } else {
        return 0;
    }
}

ib_file_t *IB_OpenDiskFile( const char *name ) {
    std::auto_ptr<ibase::impl_iso_fstream::file> f( new ibase::impl_iso_fstream::file( name ));
    
    
    if( f->is_open() ) {
        ib_file_t *wrap_f = new ib_file_t;
        wrap_f->f_ = f.release();
        return wrap_f;
    } else {
        return 0;
    }
}


void IB_DumpLastSar ( const char* outdir )
{
    __warning( "out of order. doing nothing.\n" );
}


void IB_Close( ib_file_t *file ) {
    __chkptr( file );
    __chkptr( file->f_ );
 
    delete file->f_;
    delete file;
}

void IB_SetPos( ib_file_t *file, unsigned int pos ) {
    __chkptr( file );
    __chkptr( file->f_ );
    file->f_->set_pos( pos );
}

unsigned int IB_GetPos( ib_file_t *file ) {
    __chkptr( file );
    __chkptr( file->f_ );
    return file->f_->get_pos(); // TODO: add 32bit range check
}


unsigned int IB_GetSize( ib_file_t *file ) {
    __chkptr( file );
    __chkptr( file->f_ );
    return file->f_->get_size(); // TODO: add 32bit range check
}

unsigned int IB_Read( void *buf, unsigned int size, unsigned int blocks, ib_file_t *file ) {
    __chkptr( file );
    __chkptr( file->f_ );

    sc_readbytes += size;
    return file->f_->read( buf, size, blocks );
}


int IB_GetChar( ib_file_t *file ) {
    __chkptr( file );
    __chkptr( file->f_ );
    
    int c = file->f_->get_char();
    
//     __named_message( "getc: %d\n", c );
    return c;
//     return file->f_->get_char();
}


short IB_GetShort( ib_file_t *file ) {
    __chkptr( file );
    __chkptr( file->f_ );
    return file->f_->get_short();
}

int IB_GetInt( ib_file_t *file ) {
    __chkptr( file );
    __chkptr( file->f_ );
    return file->f_->get_int();
}

void IB_Skip( ib_file_t *file, int size )
{
//      char c;
    int i;

    for( i = 0; i < size; i++ ) {
        /*c = */IB_GetChar( file );
    }
}

int IB_ProbeSource( char *name )
{
    //test for extension
    if( !memcmp( name+( strlen( name ) - 4 ), ".meh", 4 )) {
        // is meh
        return SOURCE_MEH;
    }
    // is a directory
    return SOURCE_DISK;
}

char *IB_GetLastname( ) {
    if( g_service == 0 ) {
        __error( "g_service == 0\n" );
    }
    // FIXME: fix this crap!
    return const_cast<char*>(g_service->last_name_cstr());
    
}
