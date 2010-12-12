/**
* @file CSP.h
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench
*/

#ifndef CSP_H
#define CSP_H

#include <vector>
#include <string>
#include <cassert>
#include <cstdarg>
using namespace std;

namespace Helvetica 
{
    typedef vector<bool> bv_t; // Bit vector
    typedef int val_t;  // Value Index
    typedef bv_t Domain; // A domain is essentially a bitvector - "allowed values are appropriately set"

    struct Relation
    {
        int arity;
        vector<Domain*> domains;
        bv_t valid;

        Relation( int arity, ... );
        Relation( int arity, va_list domain_lst );
        
        /* Accessors */

        inline int index( val_t v1 )
        {
            assert( arity == 1 );
            int idx = v1;
            return idx;
        }
        inline int index( val_t v1, val_t v2 )
        {
            assert( arity == 2 );
            int idx = v1 * domains[0]->size() + v2;
            return idx;
        }
        inline int index( val_t v1, val_t v2, val_t v3 )
        {
            assert( arity == 3 );
            int idx = v1 * ( domains[0]->size() + domains[1]->size() ) +
                v2 * ( domains[1]->size() ) +
                v3;
            return idx;
        }

        int vindex( int n, int offset, va_list lst );
        int index( int n, val_t v1, val_t v2, val_t v3, ... );

        inline bool check( val_t v1 )
        {
            return valid[ index( v1 ) ];
        }
        inline bool check( val_t v1, val_t v2 )
        {
            return valid[ index( v1, v2 ) ];
        }
        inline bool check( val_t v1, val_t v2, val_t v3 )
        {
            return valid[ index( v1, v2, v3 ) ];
        }
        bool check( int n, val_t v1, val_t v2, val_t v3, ... );
    };

    /**
     * Constraint Satisfaction Problem
     * Contains tuple <D, R>
     * Domains, Relations
     */
    struct CSP
    {
        int V; // Number of variables
        vector<Domain> domains; // Domains
        vector<Relation> relations; // Relations

        /**
         * Parse a XML-CSP problem
         * 
         * @args filename - path to xml-csp file
         */
        static CSP& parse( string filename );
    };

};

#endif // CSP_H

