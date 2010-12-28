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
using namespace std;

#include "XMLParser_libxml2.hh"
using namespace CSPXMLParser;

#include "CSP.h"
#include "Helvetica.h"

extern Log* g_Log;

namespace Helvetica
{
    /* Relation constructors */

    Relation::Relation( int arity, Semantics semantics )
        : arity( arity ), semantics( semantics )
    {
    }

    bool Relation::test( int n, int v1, ... )
    {
        vector<int> value( n, UNSET );
        va_list lst;

        va_start( lst, v1 );
        value[ 0 ] = v1;
        for( int i = 1; i < n; i++ )
        {
            value[ i ] = va_arg( lst, int );
        }
        va_end( lst );

        return test( value );
    }

    bool ExtensiveConstraint::test( vector< int >& assn )
    {
        // Select indices from assn using scope
        vector<int> v;
        for( int i = 0; i < arity; i++ ) v.push_back( assn[ scope[i] ] );

        return rel->test( v );
    }

    bool GlobalConstraint::test( vector< int >& assn )
    {
        switch( g_type )
        {
            case ALL_DIFFERENT: 
                return all_different( assn );
            default:
                throw runtime_error("unsupported");
        }
    }
    bool GlobalConstraint::all_different( vector< int >& assn )
    {
        // Select indices from assn using scope
        // Check if there are any repeated values
        set<int> v;
        for( int i = 0; i < arity; i++ ) 
        {
            int val = assn[ scope[i] ];
            if( v.find( val ) != v.end() )
            {
                return false;
            }
            else
            {
                v.insert( val );
            }
        }

        return true;

    }

    class HelveticaParse : public CSPParserCallback
    {
        public:
            HelveticaParse( CSP& problem ) :
                problem( problem ) {}

            virtual void beginInstance( const string& name )
            {
                g_Log->info( "Problem Name: %s", name.c_str() );
            }

            virtual void beginDomainsSection(int nbDomains) 
            {  
                // Extend the number of domains to N 
                problem.domains.resize( nbDomains );
                domain_idx = 0;
            }
            virtual void beginDomain(const string & name, int idDomain, int nbValue) 
            {
                assert( domain_idx == (unsigned int) idDomain );
                // Note: This doesn't really hold true - we need to extend to
                // the largest possible value
                problem.domains[ domain_idx ].resize( nbValue, false );
            }
            virtual void addDomainValue(int v) 
            {
                if( (unsigned int) v >= problem.domains[ domain_idx ].size() )
                    problem.domains[ domain_idx ].resize( v+1, false );
                problem.domains[ domain_idx ][ v ] = true;
            }
            virtual void addDomainValue(int first,int last) 
            {
                if( (unsigned int) last >= problem.domains[ domain_idx ].size() )
                    problem.domains[ domain_idx ].resize( last+1, false );
                for( int v = first; v <= last; v++ ) 
                    problem.domains[ domain_idx ][ v ] = true;
            }
            virtual void endDomain() 
            {
                domain_idx++;
            }
            virtual void endDomainsSection() 
            {
                assert( domain_idx == problem.domains.size() );
            }

            virtual void beginVariablesSection(int nbVariables) 
            {
                problem.variables.resize( nbVariables );
                variable_idx = 0;
            }
            virtual void addVariable(const string & name, int idVar,
                    const string & domain, int idDomain) 
            {
                assert( variable_idx == (unsigned int) idVar );
                problem.variables[ variable_idx ] = idDomain;
                variable_idx++;
            }
            virtual void endVariablesSection() 
            {
                assert( variable_idx == problem.variables.size() );
            }

            virtual void beginRelationsSection(int nbRelations) 
            {
                problem.relations.reserve( nbRelations );
                relation_idx = 0;
            }

            virtual void beginRelation(const string & name, int idRel,
                    int arity, int nbTuples, RelType relType) 
            {
                assert( relation_idx == (unsigned int) idRel );
                switch(relType)
                {
                    case REL_SUPPORT:
                        problem.relations.push_back( Relation( arity, Relation::SUPPORTS ) );
                        break;
                    case REL_CONFLICT:
                        problem.relations.push_back( Relation( arity, Relation::CONFLICTS ) );
                        break;
                    case REL_SOFT:
                    default:
                        throw runtime_error("unknown relation type");
                }
            }
            virtual void addRelationTuple(int arity, int values[]) 
            {      
                vector<int> v( arity, UNSET );
                for( int i = 0; i < arity; i++ ) v[ i ] = values[ i ];
                problem.relations[ relation_idx ].values.insert( v );
            }
            virtual void addRelationTuple(int arity, int values[], int cost) 
            {      
                throw runtime_error("unsupported");
            }
            virtual void endRelation() 
            {
                relation_idx++;
            }

            virtual void endRelationsSection() 
            {
                assert( relation_idx == problem.relations.size() );
            }

            virtual void beginPredicatesSection(int nbPredicates) 
            {
                throw runtime_error("unsupported");
            }

            virtual void beginPredicate(const string & name, int idPred) 
            {
            }
            virtual void addFormalParameter(int pos, const string & name, 
                    const string & type) 
            {
            }
            virtual void predicateExpression(AST *tree) 
            {
            }
            virtual void predicateExpression(const string &expr) 
            {
            }
            virtual void endPredicate() 
            {
            }

            virtual void endPredicatesSection() 
            {
            }

            virtual void beginConstraintsSection(int nbConstraints) 
            {
                problem.constraints.reserve( nbConstraints );
                constraint_idx = 0;
            }

            virtual void beginConstraint(const string & name, int idConstr,
                    int arity, 
                    const string & reference, 
                    CSPDefinitionType type, int id,
                    const ASTList &scope)
            {
                assert( constraint_idx == problem.constraints.size() );
                assert( (unsigned int)id < problem.relations.size() );

                vector<int> scope_;
                for(int i=0;i<scope.size();++i)
                    scope_.push_back( scope[i].getVarId() );

                switch( type )
                {
                    case RelationType:
                        problem.constraints.push_back( new
                                ExtensiveConstraint( arity, scope_, &problem.relations[id] ) );
                        break;
                    case GlobalConstraintType:
                        if( reference.compare( "global:alldifferent" ) == 0 )
                        {
                            problem.constraints.push_back( new
                                    GlobalConstraint( arity, scope_, GlobalConstraint::ALL_DIFFERENT ) );
                            break;
                        }
                        // else fall through
                    case PredicateType:
                    default:
                        throw runtime_error("unsupported");
                }
            }
            virtual void constraintParameters(const ASTList &args)
            {
            } 
            virtual void endConstraint() 
            {
                constraint_idx++;
            }

            virtual void endConstraintsSection() 
            {
                assert( constraint_idx == problem.constraints.size() );
            }

            virtual void endInstance() 
            {
            }

        protected:
            CSP& problem;
            unsigned int domain_idx;
            unsigned int variable_idx;
            unsigned int relation_idx;
            unsigned int constraint_idx;
    };

    CSP& CSP::parse( string filename )
    {
        CSP* problem = new CSP();

        // Initialise the parser callback
        HelveticaParse cb( *problem );

        // Initialise the parsers
        XMLParser_libxml2<> parser(cb);
        
        // Parse!
        parser.parse(filename.c_str()); 

        return *problem;
    }

};

