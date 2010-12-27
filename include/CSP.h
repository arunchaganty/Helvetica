/**
* @file CSP.h
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench
*/

#ifndef CSP_H
#define CSP_H

#include <vector>
#include <set>
#include <string>
#include <cassert>
#include <cstdarg>
using namespace std;

#include "Helvetica.h"

namespace Helvetica 
{
    typedef vector<bool> bv_t; // Bit vector
    typedef int val_t;  // Value Index
    typedef bv_t Domain; // A domain is essentially a bitvector - "allowed values are appropriately set"

    struct Relation
    {
        enum Semantics
        {
            SUPPORTS,
            CONFLICTS
        };

        int arity;
        vector<Domain*> domains;
        Semantics semantics;
        set< vector<int> > values;

        Relation( int arity, Semantics semantics );
        inline bool test( vector<int>& value )
        {
            return semantics ^ ( values.find( value ) != values.end() );
        }
        bool test( int n, int v1, ... );
    };

    struct Constraint
    {
        enum Type
        {
            EXTENSION,
            INTENSION,
            GLOBAL
        };

        int arity;
        Type type;
        vector<int> scope;
        Relation* rel;

        Constraint( int arity, Type type, vector<int> scope, Relation* rel )
            : arity( arity ), type( type ), scope( scope ), rel( rel )
        {
        }
        inline bool applicable( vector< int >& assn )
        {
            bool ret = true;
            for( int i = 0; i < arity && ret; i++ ) 
                ret &= ( assn[ scope[ i ] ] != UNSET );

            return ret;
        }

        inline bool test( vector< int >& assn )
        {
            // Select indices from assn using scope
            vector<int> v;
            for( int i = 0; i < arity; i++ ) v.push_back( assn[ scope[i] ] );

            return rel->test( v );
        }
    };

    /**
     * Constraint Satisfaction Problem
     * Contains tuple <D, R>
     * Domains, Relations
     */
    struct CSP
    {
        vector<int> variables; // Domain_indices of variables
        vector<Domain> domains; // Domains
        vector<Constraint> constraints; // Constraints

        vector<Relation> relations; // Relations (Extensive)

        /**
         * Parse a XML-CSP problem
         * 
         * @args filename - path to xml-csp file
         */
        static CSP& parse( string filename );
    };

};

#endif // CSP_H

