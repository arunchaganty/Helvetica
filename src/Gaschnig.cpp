/**
* @file GaschnigBacktracker.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench
* Gaschnig's Backjumping implementation
*/

#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include <typeinfo>
#include <string>

#include "Helvetica.h"
#include "CSP.h"
#include "CSPSolver.h"
#include "Gaschnig.h"

using namespace std;

namespace Helvetica
{
    GaschnigBacktracker::GaschnigBacktracker()
    {
        culprit.push_back( 0 );
    }
    // Components
    void GaschnigBacktracker::save( CSPSolution& sol, Assignment assn )
    {
        Backtracker::save( sol, assn );
        unsigned int i = getAssignedCount( sol.assn );
        if( i >= culprit.size() )
            culprit.push_back( 0 );
        else
            culprit[ i ] = 0;
    }

    CSPSolution& GaschnigBacktracker::backtrack( CSPSolution& sol )
    {
        if( memory.size() == 0 )
            throw runtime_error( "Unsolvable" ); 
        
        // Pop till culprit
        unsigned int i = getAssignedCount( sol.assn );
        unsigned int popCount = getAssignedCount( sol.assn ) - culprit[ i ] + 1;
        for( unsigned int i = 0; i < popCount; i++ )
        {
            Backtracker::backtrack( sol );
        }

        return sol;
    }

    bool GaschnigValueSelector::valid( CSPSolution& sol, Assignment assn )
    {
        GaschnigBacktracker& bt = dynamic_cast< GaschnigBacktracker& >( sol.solver->getBacktracker() );

        int i = assn.first;
        int j = assn.second;
        int k = 0;

        bool satisfiable = true;
        sol.assn[ i ] = j;

        // Check for consistency at every level
        for( k = 0; k < i && satisfiable; k++ )
        {
            // Check with constraints
            vector<Constraint*>::iterator it;
            for( it = sol.problem->constraints.begin(); it != sol.problem->constraints.end() && satisfiable; it++ )
            {
                Constraint* cnstr = *it;
                // Only check constraints which have exactly one variable at level k
                {
                    vector<int>::iterator vt;
                    for( vt = cnstr->scope.begin(); vt != cnstr->scope.end(); vt++ )
                        if( (*vt) == k ) break;
                    if( vt == cnstr->scope.end() ) continue;
                }

                // applicable => should pass test
                satisfiable &= ( ( !cnstr->applicable( sol.assn ) ) || cnstr->test( sol.assn ) );
            }
        }
        if( k > bt.getCulprit()[ i ] )
        {
            bt.getCulprit()[ i ] = k;
        }

        return satisfiable;
    }

};

