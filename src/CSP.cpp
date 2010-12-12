/**
* @file CSP.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench
* Constraint Satisfaction Problem Definitions
*/

#include <cstdlib>
#include <cassert>
#include <string>

#include "CSP.h"
#include "Helvetica.h"

using namespace std;

namespace Helvetica
{
    /* Relation constructors */

    Relation::Relation( int arity, ... )
        : arity( arity )
    {
        va_list domain_lst;

        va_start( domain_lst, arity );
        for( int i = 0; i < arity; i++ )
        {
            domains.push_back( va_arg( domain_lst, Domain* ) );
        }
        va_end( domain_lst );
    }

    Relation::Relation( int arity, va_list domain_lst )
        : arity( arity )
    {
        for( int i = 0; i < arity; i++ )
        {
            domains.push_back( va_arg( domain_lst, Domain* ) );
        }
    }

    /* Accessors */
    int Relation::vindex( int n, int offset, va_list lst )
    {
        int base = 0; 
        for( int i = offset; i < arity; i++ ) base += domains[ i ]->size();

        int idx = 0;
        for( int i = offset; i < arity; i++ ) 
        {
            int val = va_arg( lst, int );
            idx += val * base;
            base -= domains[ i ]->size();
        }
        return idx;
    }
    int Relation::index( int n, val_t v1, val_t v2, val_t v3, ... )
    {
        assert( arity == n );

        int base = 0;
        for( int i = 0; i < arity; i++ ) base += domains[ i ]->size();

        int idx = 0;
        for( int i = 0; i < 3; i++ ) 
        {
            int val = (i == 0) ? v1 : (i == 1 ) ? v2 : v3;
            idx += val * base;
            base -= domains[ i ]->size();
        }

        va_list lst;
        va_start( lst, v3 );
        idx += vindex( n, 3, lst );
        va_end( lst );

        return idx;
    }

    bool Relation::check( int n, val_t v1, val_t v2, val_t v3, ... )
    {
        assert( arity == n );

        int base = 0;
        for( int i = 0; i < arity; i++ ) base += domains[ i ]->size();

        int idx = 0;
        for( int i = 0; i < 3; i++ ) 
        {
            int val = (i == 0) ? v1 : (i == 1 ) ? v2 : v3;
            idx += val * base;
            base -= domains[ i ]->size();
        }

        va_list lst;
        va_start( lst, v3 );
        idx += vindex( n, 3, lst );
        va_end( lst );

        return valid[ idx ];
    }

};

