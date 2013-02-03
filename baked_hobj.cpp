
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>

#include "u_hobj.h"


class hobj_oven {
public:
    hobj_oven( hobj_t *root ) {
        size_t num_obj = 0;
        size_t num_pair = 0;
        
        count( root, num_obj, num_pair );
        
        std::cout << "count: " << num_obj << " " << num_pair << "\n";
    }
    
    void count_pairs( hpair_t *p, size_t &num_hpair ) {
        
        if( p != 0 ) {
            ++num_hpair;
            count_pairs( p->next, num_hpair );
        }
    }
    
    void count( hobj_t *o, size_t &num_hobj, size_t &num_hpair ) {
        if( o != 0 ) {
            ++num_hobj;
            count_pairs( o->pairs, num_hpair );
            count( o->hobjs, num_hobj, num_hpair );
            count( o->next, num_hobj, num_hpair );
        }
    }
   
   
#if 1 
    size_t write_hpair( hpair_t *p ) {
        if( p == 0 ) {
            return 0;
        }
        std::cout << "hpair\n";
        // TODO: writing the value in front of the hpair for simplicity. check if that is good performance wise (i.e. confuses prefetcher too much etc.)
        
        size_t value_base = buf_.size();
        
        char *v = p->value;
        while( (*v) != 0 ) {
            buf_.push_back( *v );
            ++v;
        }
        buf_.push_back( 0 );
        
        while( !is_aligned( buf_.size() ) ) {
            buf_.push_back( 0 );
        }
        
        size_t base = buf_.size();
        
        buf_.resize( base + sizeof( hpair_t ) );
        
        hpair_t wp = *p;
        wp.value = (char *)value_base;

        reloc_.push_back( base + size_t( &((hpair_t *)0)->value ));
        reloc_.push_back( base + size_t( &((hpair_t *)0)->next ));
        
        wp.next = (hpair_t*)write_hpair( p->next );
        
        std::copy( (char *)&wp, (char *)((&wp)+1), buf_.begin() + base );
        
        return base;
    }
    
    size_t write_hobj( hobj_t *o, hobj_t *parent ) {
        if( o == 0 ) {
            return 0;
        }
        std::cout << "hobj\n";
        while( !is_aligned( buf_.size() ) ) {
            buf_.push_back( 0 );
        }
        
        size_t base = buf_.size();
        
        buf_.resize( base + sizeof( hobj_t ) );
        
        hobj_t ow = *o;
        ow.pairs = (hpair_t*)write_hpair( o->pairs );
        ow.hobjs = (hobj_t*)write_hobj( o->hobjs, 0 );
        
        ow.parent = parent;
        ow.next = (hobj_t*)write_hobj( o->next, (hobj_t*)base );
        
        reloc_.push_back( base + size_t( &((hobj_t *)0)->pairs ));
        reloc_.push_back( base + size_t( &((hobj_t *)0)->hobjs ));
        reloc_.push_back( base + size_t( &((hobj_t *)0)->next ));
        
        
        std::copy( (char *)&ow, (char *)((&ow)+1), buf_.begin() + base );
        
        return base;
    }
#endif

    void write( std::ostream &os ) {
        os.write( buf_.data(), buf_.size() );
        
        os.write( (char*)reloc_.data(), reloc_.size() * sizeof( size_t ) );
        
    }

private:
    template<typename T>
    bool is_aligned( const T& v ) {
        const size_t alignment = 4;
        return (v % alignment) == 0;
    }
    
    std::vector<char> buf_;
    std::vector<size_t> reloc_;
};


void bake_hobj( hobj_t *o ) {
    hobj_oven oven(o);
    oven.write_hobj(o, 0);
    
    {
        std::ofstream os( "baked.bin" );
        oven.write( os );
    }
    
    getchar();
    
    
}