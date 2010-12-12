
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
#include "CSPSolver.h"
#include "Helvetica.h"

using namespace std;

namespace Helvetica
{
    CSPSolution::CSPSolution( CSP& problem, CSPSolver& solver ) :
        problem( &problem ), solver( &solver )
    {
        for( int i = 0; i < problem.V; i++ )
        {
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

    /*
     * Check if any variables are unassigned
     */
    bool CSPSolution::isSolved()
    {
        for( int i = 0; i < problem->V; i++ )
        {
            if( assn[i] == UNSET ) return false;
        }

        return true;
    }

    CSPSolution& CSPSolver::solve( CSP& problem )
    {
        CSPSolution* sol_ = new CSPSolution( problem, *this );
        CSPSolution& sol = *sol_;

        val_t val = UNSET;
        int var_idx = 0;

        while( !sol.isSolved() )
        {
            // Select a value, 
            val = valueSelector.select( sol, var_idx );
            if( val == UNSET )
            {
                sol = backtracker.backtrack( sol );
            }
            else
            {
                sol.assn[ var_idx ] = val;
                backtracker.save( sol, var_idx, val );
            }
        }

        return sol;
    }
};

