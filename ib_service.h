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



// ib_service.h

#ifndef ib_service_h
#define ib_service_h

#include <vector>
#include <string>
#include <memory>
#include <fstream>

#include "ib_defs.h"
#include "shock.h"


//#include "defs.h"

extern int	ib_lastaction;
extern int	sc_readbytes;

namespace ibase {
    
template<typename T>
class deleter {
public:
    void operator()( T* ptr ) {
        delete ptr;
    }
};
    
class file {
public:
    virtual size_t get_size() = 0;
    virtual size_t get_pos() = 0;
    virtual void set_pos( size_t ) = 0;
    virtual size_t read( void *ptr, size_t bsize, size_t nb ) = 0;
    virtual int get_char() = 0;
    virtual int get_short() = 0;
    virtual int get_int() = 0;
    virtual const char *map() = 0;
    virtual void unmap() = 0;
    
    
    virtual ~file() {}
    
};

class source;

class service {
public:
    service();
    
    
    void add_source( const char *name, int stype ) ;

    file* open( const char *name ) ;
    
    const char *last_name_cstr() ;

    static service& get_singleton() ;
    
    ~service() ;
    
private:
    std::vector<source *> sources_;
    std::string last_name_;
    std::ofstream logs_;
};


class file_handle {
public:
    class mapping {
    public:
        mapping( file_handle &h ) : h_(h) {
            if( h.is_mapped_ ) {
                __error( "mapping ibase::file more than once is not supported\n" );
            }
            if( !h.is_open() ) {
                   __error( "mapping non-open ibase::file\n" );
            }
            ptr_ = h.file_->map();
        }
        ~mapping() {
            h_.file_->unmap();
        }
        const char *ptr() {
            return ptr_;
        }
    private:
        file_handle &h_;
        const char *ptr_;
    };
    
    
    file_handle( const char *name ) 
    : is_mapped_(false)
    {
        open( name );
    }
    
    file_handle( const char *name, ibase::service &ibs ) 
    : is_mapped_(false)
    {
        open( name, ibs );
    }
    
    void open( const char *name ) {
        file_.reset( ibase::service::get_singleton().open(name) );
        
        if( !is_open() ) {
            __error( "open failed: '%s'\n", name );
        }
    }
    void open( const char *name, ibase::service &ibs ) {
        file_.reset( ibs.open(name) );
    }
    
    bool is_open() const {
        return file_.get() != 0;
    }
    
    size_t get_size() { __chkptr(file_.get()); return file_->get_size(); }
    size_t get_pos() { __chkptr(file_.get()); return file_->get_pos(); }
    void set_pos( size_t s ) { __chkptr(file_.get()); file_->set_pos( s ); };
    size_t read( void *ptr, size_t bsize, size_t nb ) { __chkptr(file_.get()); return file_->read( ptr, bsize, nb ); }
    int get_char() { __chkptr(file_.get()); return file_->get_char(); }
//     virtual int get_short() = 0;
//     virtual int get_int() = 0;
//     

// REMARK: try not to expose these functions directly. see if it is possible to exclusively use RAII wrapper (through mapping class)
//     const char *map() { __chkptr(file_.get()); return file_->map(); }
//     const char *unmap() { __chkptr(file_.get()); file_->unmap(); }
private:
    file_handle() : is_mapped_(false) {}
    
    file_handle( const file_handle & );
    file_handle &operator=( const file_handle & );
    
    std::auto_ptr<file> file_; // WARNING: keep this class non-copyable!
    bool is_mapped_;
};
}


void IB_StartUp();
void IB_ShutDown();

int IB_ProbeSource( char *name );
void IB_AddSource( const char *name, int type );

ib_file_t *IB_Open( const char *name );
ib_file_t *IB_OpenDiskFile( const char *name );
unsigned int IB_Read( void *buf, unsigned int size, unsigned int blocks, ib_file_t *file );
unsigned int IB_GetSize( ib_file_t *file );
void IB_SetPos( ib_file_t *file, unsigned int pos );
unsigned int IB_GetPos( ib_file_t *file );
void IB_Close( ib_file_t *file );
 
char *IB_GetLastname( );
int IB_GetChar( ib_file_t *file );
short IB_GetShort( ib_file_t *file );
int IB_GetInt( ib_file_t *file );
void IB_Skip( ib_file_t *file, int size );
void IB_DumpLastSar( const char *outdir );
#endif
