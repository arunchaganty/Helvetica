
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

extern Log* g_Log;
extern Helvetica::Stats g_Stats;

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

        // Renable all-child options
        for( unsigned int j = i+1; j < sol.assn.size(); j++ )
        {
            int d = sol.problem->variables[ j ];
            sol.allowable[ j ] = bv_t( sol.problem->domains[ d ] );
        }
        
        // Cross off the domain option
        sol.allowable[ i ][ v ] = false;
        sol.assn[ i ] = UNSET;

        return sol;
    }

    Assignment ValueSelector::select( CSPSolution& sol )
    {
        // Find first unassigned index
        unsigned int i = getAssignedCount( sol.assn );

        // Assign first "acceptable" value
        unsigned int j;
        for( j = 0; j < sol.allowable[ i ].size(); j++ )
        {
            if ( sol.allowable[ i ][ j ] == false ) continue;
            if ( valid( sol, Assignment( i, j ) ) ) break;
        }
        // Undo any possible assignments
        sol.assn[ i ] = UNSET;

        if( j  == sol.allowable[ i ].size() )
        {
            j = UNSET;
        }

        return Assignment( i, j );
    }
    bool ValueSelector::valid( CSPSolution& sol, Assignment assn )
    {
        bool satisfiable = true;
        sol.assn[ assn.first ] = assn.second;

        // Check with constraints
        vector<Constraint*>::iterator it;
        for( it = sol.problem->constraints.begin(); it != sol.problem->constraints.end() && satisfiable; it++ )
        {
            // applicable => should pass test
            satisfiable &= ( ( !(*it)->applicable( sol.assn ) ) || (*it)->test( sol.assn ) );
        }

        return satisfiable;
    }

    CSPSolution& Preprocessor::preprocess( CSPSolution& sol )
    {
        return sol;
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
        delete &preprocessor;
    }

    Backtracker& CSPSolver::getBacktracker()
    {
        return backtracker;
    }
    ValueSelector& CSPSolver::getValueSelector()
    {
        return valueSelector;
    }
    Preprocessor& CSPSolver::getPreprocessor()
    {
        return preprocessor;
    }

    CSPSolution& CSPSolver::solve( CSP& problem )
    {
        CSPSolution* sol_ = new CSPSolution( problem, *this );
        CSPSolution& sol = preprocessor.preprocess( *sol_ );

        g_Stats.backtracks = 0;
        g_Stats.vs_count = 0;
        g_Stats.run_time.start();
        g_Stats.avg_vs_time.start();
        g_Stats.avg_vs_time.pause();

        while( !sol.isSolved() )
        {
            g_Stats.run_time.pause();
            g_Log->debug( "Backtracks: %d Total Time: %ems (%ems)", 
                    g_Stats.backtracks, g_Stats.run_time.getTicks(), (g_Stats.avg_vs_time.getTicks() / (float) g_Stats.vs_count ) );
            g_Stats.run_time.unpause();
            // Select a value, 
            g_Stats.avg_vs_time.unpause();
            Assignment assn = valueSelector.select( sol );
            g_Stats.avg_vs_time.pause();
            g_Stats.vs_count ++;

            if( assn.second == UNSET )
            {
                g_Stats.backtracks++;
                sol = backtracker.backtrack( sol );
            }
            else
            {
                sol.assn[ assn.first ] = assn.second;
                backtracker.save( sol, assn );
            }
        }
        g_Stats.run_time.pause();

        return sol;
    }
};

