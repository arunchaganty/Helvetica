
/**
* @file CSP.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench
* Constraint Satisfaction Problem Definitions
*/

#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include <string>

#include "CSP.h"
#include "CSPSolver.h"
#include "Helvetica.h"

using namespace std;

namespace Helvetica
{
    CSPSolution::CSPSolution( CSP& problem, CSPSolver& solver ) :
        problem( &problem ), solver( &solver )
    {
        for( unsigned int i = 0; i < problem.variables.size(); i++ )
        {
            int j = problem.variables[ i ];
            allowable.push_back( bv_t( problem.domains[ j ] ) );
            assn.push_back( UNSET );
        }
    }

    CSPSolution::CSPSolution( CSP& problem, CSPSolver& solver, vector<int> assn ) :
        problem( &problem ), solver( &solver ), assn( assn )
    {
    }

    CSPSolution::CSPSolution( CSPSolution& solution, vector<int> assn ) :
        problem( solution.problem ), solver( solution.solver ), assn( assn )
    {
    }

    void CSPSolution::apply( Assignment& assn )
    {
        this->assn[ assn.first ] = assn.second;
    }

    /*
     * Check if any variables are unassigned
     */
    bool CSPSolution::isSolved()
    {
        for( unsigned int i = 0; i < problem->variables.size(); i++ )
        {
            if( assn[i] == UNSET ) return false;
        }

        return true;
    }
    
    ostream& operator<<( ostream& st, const CSPSolution& sol )
    {
        for( unsigned int i = 0; i < sol.assn.size(); i++ )
        {
            if( sol.assn[ i ] != UNSET ) 
                st << sol.assn[ i ] << " ";
            else
                st << "*" << " ";
        }
        
        return st;
    }

    Backtracker::Backtracker() 
    {}

    // Components
    void Backtracker::save( CSPSolution& sol, Assignment assn )
    {
        // Save the value
        memory.push( assn );
    }
    CSPSolution& Backtracker::backtrack( CSPSolution& sol )
    {
        if( memory.size() == 0 )
            throw runtime_error( "Unsolvable" ); 

        Assignment assn = memory.top(); memory.pop();
        int i = assn.first;
        int v = assn.second;

        // Cross off the domain option
        sol.allowable[ i ][ v ] = false;
        sol.assn[ i ] = UNSET;

        return sol;
    }

    Assignment ValueSelector::select( CSPSolution& sol )
    {
        // Find first unassigned index
        unsigned int i;
        for( i = 0; i < sol.assn.size() && sol.assn[ i ] != UNSET; i++ );

        // Assign first "acceptable" value
        unsigned int j;
        for( j = 0; j < sol.allowable[ i ].size(); j++ )
        {
            if ( sol.allowable[ i ][ j ] == false ) continue;
            sol.assn[ i ] = j;

            // Check with constraints
            bool satisfiable = true;
            vector<Constraint>::iterator it;
            for( it = sol.problem->constraints.begin(); it != sol.problem->constraints.end() && satisfiable; it++ )
            {
                // applicable => should pass test
                satisfiable &= ( ( !it->applicable( sol.assn ) ) || it->test( sol.assn ) );
            }

            if( satisfiable ) break;
        }
        // Undo any possible assignments
        sol.assn[ i ] = UNSET;

        if( j  == sol.allowable[ i ].size() )
        {
            j = UNSET;
        }

        return Assignment( i, j );
    }

    // CSPSolver
    CSPSolver::CSPSolver( Backtracker& backtracker, ValueSelector& valueSelector, Preprocessor& preprocessor )
        : backtracker( backtracker ), valueSelector( valueSelector ), preprocessor( preprocessor )
    {
    }

    CSPSolver::~CSPSolver( )
    {
        delete &backtracker;
        delete &valueSelector;
    }

    CSPSolver& CSPSolver::create( Settings settings )
    {
        Backtracker* bt = NULL;
        ValueSelector* vs = NULL;
        Preprocessor* pp = NULL;

        switch( settings.Backtracker )
        {
            case Settings::BT_NONE: 
                bt = new Backtracker();
                break;
            default:
                throw runtime_error( "Invalid option" );
        }

        switch( settings.ValueSelector )
        {
            case Settings::VS_NONE: 
                vs = new ValueSelector();
                break;
            default:
                throw runtime_error( "Invalid option" );
        }

        switch( settings.Preprocessor )
        {
            case Settings::PP_NONE: 
                pp = new Preprocessor();
                break;
            default:
                throw runtime_error( "Invalid option" );
        }

        CSPSolver* solver = new CSPSolver( *bt, *vs, *pp );

        return *solver;
    }

    CSPSolution& CSPSolver::solve( CSP& problem )
    {
        CSP& problem_ = preprocessor.preprocess( problem );
        CSPSolution* sol_ = new CSPSolution( problem_, *this );
        CSPSolution& sol = *sol_;

        while( !sol.isSolved() )
        {
            // Select a value, 
            Assignment assn = valueSelector.select( sol );
            if( assn.second == UNSET )
            {
                sol = backtracker.backtrack( sol );
            }
            else
            {
                sol.assn[ assn.first ] = assn.second;
                backtracker.save( sol, assn );
            }
        }

        return sol;
    }
};

