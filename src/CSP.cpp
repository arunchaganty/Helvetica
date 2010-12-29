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

    bool Predicate::test( int n, int v1, ... )
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

    bool getBoolAST( const AST& expr, vector<int>& value );
    int getIntAST( const AST& expr, vector<int>& value );

    bool getBoolAST( const AST& expr, vector<int>& value )
    {

        switch( expr.getType() )
        {
            case VAR:
                { 
                    int id;
                    id = expr.getVarId();
                    if( value[ id ] == 0) return false;
                    else if( value[ id ] == 1) return true;
                    else throw runtime_error( "invalid bool value" );
                }
            case CST_BOOL:
                return expr.getBoolean();
                
            case F_IF:
                return ( getBoolAST( expr.getArg(0), value ) ?  getBoolAST( expr.getArg(1), value ) :  getBoolAST( expr.getArg(2), value ) );  

            case F_EQ:
                return ( getIntAST( expr.getArg(0), value ) == getIntAST( expr.getArg(1), value ) );
            case F_NE:
                return ( getIntAST( expr.getArg(0), value ) != getIntAST( expr.getArg(1), value ) );
            case F_GE:
                return ( getIntAST( expr.getArg(0), value ) >= getIntAST( expr.getArg(1), value ) );
            case F_GT:
                return ( getIntAST( expr.getArg(0), value ) > getIntAST( expr.getArg(1), value ) );
            case F_LE:
                return ( getIntAST( expr.getArg(0), value ) <= getIntAST( expr.getArg(1), value ) );
            case F_LT:
                return ( getIntAST( expr.getArg(0), value ) < getIntAST( expr.getArg(1), value ) );

            case F_NOT:
                return ( !getBoolAST( expr.getArg(0), value ) );
            case F_AND:
                return ( getBoolAST( expr.getArg(0), value ) && getBoolAST( expr.getArg(1), value ) );
            case F_OR:
                return ( getBoolAST( expr.getArg(0), value ) || getBoolAST( expr.getArg(1), value ) );
            case F_XOR:
                return ( getBoolAST( expr.getArg(0), value ) ^ getBoolAST( expr.getArg(1), value ) );
            case F_IFF:
                return ( getBoolAST( expr.getArg(0), value ) == getBoolAST( expr.getArg(1), value ) );

            default:
                throw runtime_error( "Not a  boolean expression" );
        }
    }

    int getIntAST( const AST& expr, vector<int>& value )
    {
        switch( expr.getType() )
        {
            case VAR:
                {
                    int id;
                    id = expr.getVarId();
                    return value[ id ];
                }
            case CST_INT:
                return expr.getInteger();
                
            case F_MIN:
                return std::min( getIntAST( expr.getArg(0), value ) , getIntAST( expr.getArg(1), value ) );
            case F_MAX:
                return std::max( getIntAST( expr.getArg(0), value ) , getIntAST( expr.getArg(1), value ) );

            case F_NEG:
                return -getIntAST( expr.getArg(0), value );
            case F_ABS:
                return abs( getIntAST( expr.getArg(0), value ) );
            case F_ADD:
                return getIntAST( expr.getArg(0), value ) + getIntAST( expr.getArg(1), value );
            case F_SUB:
                return getIntAST( expr.getArg(0), value ) - getIntAST( expr.getArg(1), value );
            case F_MUL:
                return getIntAST( expr.getArg(0), value ) * getIntAST( expr.getArg(1), value );
            case F_DIV:
                return getIntAST( expr.getArg(0), value ) / getIntAST( expr.getArg(1), value );
            case F_MOD:
                return getIntAST( expr.getArg(0), value ) % getIntAST( expr.getArg(1), value );
            case F_POW:
                {
                    int arg0 = getIntAST( expr.getArg(0), value );
                    int arg1 = getIntAST( expr.getArg(1), value );

                    if( arg0 == 0 && arg1 == 0 ) throw runtime_error( "0^0 not defined" );
                    if( arg0 == 0 ) return 0;
                    else if( arg1 == 0 ) return 1;
                    else
                    {
                        int val = 1;
                        for( int i = 0; i < arg1; i++ ) val *= arg0;
                        return val;
                    }
                }

            default:
                throw runtime_error( "Not an integer expression" );
        }
    }

    bool Predicate::test( vector<int>& value )
    {
        return getBoolAST( *expr, value );
    }

    bool ExtensiveConstraint::vtest( vector< int >& v )
    {
        return rel->test( v );
    }

    bool IntensiveConstraint::vtest( vector< int >& v )
    {
        return pred->test( v );
    }

    bool GlobalConstraint::vtest( vector< int >& v )
    {
        switch( g_type )
        {
            case ALL_DIFFERENT: 
                return all_different( v );
            default:
                throw runtime_error("unsupported");
        }
    }
    bool GlobalConstraint::all_different( vector< int >& v )
    {
        // Select indices from assn using scope
        // Check if there are any repeated values
        set<int> s;
        for( int i = 0; i < arity; i++ ) 
        {
            int val = v[ i ];
            if( s.find( val ) != s.end() )
            {
                return false;
            }
            else
            {
                s.insert( val );
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
                problem.predicates.reserve( nbPredicates );
                predicate_idx = 0;
            }

            virtual void beginPredicate(const string & name, int idPred) 
            {
                problem.predicates.push_back( Predicate() );
            }
            virtual void addFormalParameter(int pos, const string & name, 
                    const string & type) 
            {
                Predicate& pr = problem.predicates.back();
                // The types are just bool and int anyways, so nothing to handle.
                assert( pr.formalParametersCount == pos );
                pr.formalParametersCount++;
            }
            virtual void predicateExpression(AST *tree) 
            {
                Predicate& pr = problem.predicates.back();
                pr.expr = tree;
            }
            virtual void predicateExpression(const string &expr) 
            {
            }
            virtual void endPredicate() 
            {
                predicate_idx++;
            }

            virtual void endPredicatesSection() 
            {
                assert( predicate_idx == problem.predicates.size() );
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

                switch( type )
                {
                    case RelationType:
                        assert( (unsigned int)id < problem.relations.size() );
                        problem.constraints.push_back( new
                                ExtensiveConstraint( arity, &problem.relations[id] ) );
                        break;
                    case GlobalConstraintType:
                        if( reference.compare( "global:alldifferent" ) == 0 )
                        {
                            problem.constraints.push_back( new
                                    GlobalConstraint( arity, GlobalConstraint::ALL_DIFFERENT ) );
                            break;
                        }
                        else
                        {
                            throw runtime_error("unsupported");
                        }
                        // else fall through
                    case PredicateType:
                        assert( (unsigned int)id < problem.predicates.size() );
                        problem.constraints.push_back( new
                                IntensiveConstraint( arity, &problem.predicates[id] ) );
                        break;
                    default:
                        throw runtime_error("unsupported");
                }
            }
            virtual void constraintParameters(const ASTList &args)
            {
                Constraint* cnstr = problem.constraints.back();

                for( int i = 0; i < args.nbArg(); i++ )
                {
                    switch( args[i].getType() )
                    {
                        case VAR:
                            cnstr->addArg( VAR, args[i].getVarId() );
                            break;
                        case CST_BOOL:
                            cnstr->addArg( CST_BOOL, args[i].getBoolean() );
                            break;
                        case CST_INT:
                            cnstr->addArg( CST_INT, args[i].getInteger() );
                            break;
                        default:
                            throw runtime_error( "invalid constraint argument type" );
                    }
                }
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
            unsigned int predicate_idx;
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

