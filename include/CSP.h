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
#include "AST.hh"
using namespace CSPXMLParser;

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

    struct Predicate
    {
        int formalParametersCount;
        AST* expr;

        bool test( vector<int>& value );
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
        vector<NodeType> types; // can be VAR or INT or BOOL
        vector<int> scope;

        Constraint( int arity, Type type )
            : arity( arity ), type( type )
        {
        }

        inline void addArg( NodeType type, int val )
        {
            types.push_back( type ); scope.push_back( val );
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
            vector<int> v;
            for( unsigned int i = 0; i < scope.size(); i++ )
            {
                if( types[ i ] == VAR ) v.push_back( assn[ scope[i] ] );
                else if( types[ i ] == CST_BOOL ) v.push_back( scope[i] );
                else if( types[ i ] == CST_INT ) v.push_back( scope[i] );
            }

            return vtest( v );
        }

        virtual bool vtest( vector< int >& value ) = 0;
    };

    struct ExtensiveConstraint : public Constraint
    {
        Relation* rel;

        ExtensiveConstraint( int arity,  Relation* rel )
            : Constraint( arity, EXTENSION), rel( rel )
        {
        }
        virtual bool vtest( vector< int >& assn );
    };

    struct IntensiveConstraint : public Constraint
    {
        Predicate* pred;

        IntensiveConstraint( int arity,  Predicate* pred )
            : Constraint( arity, EXTENSION), pred( pred )
        {
        }
        virtual bool vtest( vector< int >& assn );
    };

    struct GlobalConstraint : public Constraint
    {
        enum GlobalType
        {
            ALL_DIFFERENT,
        };

        GlobalType g_type;

        GlobalConstraint( int arity,  GlobalType type )
            : Constraint( arity, GLOBAL ), g_type( g_type )
        {
        }
        virtual bool vtest( vector< int >& assn );

        protected:
        bool all_different( vector< int >& assn );
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
        vector<Constraint*> constraints; // Constraints

        vector<Relation> relations; // Relations (Extensive)
        vector<Predicate> predicates; // Predicates (Intensive)

        /**
         * Parse a XML-CSP problem
         * 
         * @args filename - path to xml-csp file
         */
        static CSP& parse( string filename );
    };

};

#endif // CSP_H

