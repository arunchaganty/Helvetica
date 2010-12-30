/**
* @file ForwardChecking.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench
* Forward Checking implementation
*/

#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include <typeinfo>
#include <string>

#include "Helvetica.h"
#include "CSP.h"
#include "CSPSolver.h"
#include "ForwardChecking.h"

using namespace std;

namespace Helvetica
{
    // Components
    void ForwardCheckingBacktracker::save( CSPSolution& sol, Assignment assn )
    {
        Backtracker::save( sol, assn );

        // Get the disabled bits from the preprocessor
        ForwardCheckingValueSelector& vs = dynamic_cast< ForwardCheckingValueSelector& >( sol.solver->getValueSelector() );
        vector<bv_t> disabled = vector<bv_t>( vs.getLastDisabled() );
        disabled_memory.push( disabled );
    }
    void ForwardCheckingBacktracker::updateDisabled( Assignment assn )
    {
        if( disabled_memory.size() > 0 )
        {
            vector<bv_t>& t = disabled_memory.top();
            t[ assn.first ][ assn.second ] = true;
        }
    }

    CSPSolution& ForwardCheckingBacktracker::backtrack( CSPSolution& sol )
    {
        if( memory.size() == 0 )
            throw runtime_error( "Unsolvable" ); 

        vector< bv_t > disabled = disabled_memory.top(); disabled_memory.pop();

        // Re-enable crossed-off options
        for( unsigned int i = 0; i < disabled.size(); i++ )
        {
            for( unsigned int j = 0; j < disabled[ i ].size(); j++ )
            {
                sol.allowable[ i ][ j ] = sol.allowable[ i ][ j ] | disabled[ i ][ j ];
            }
        }

        Assignment assn = memory.top();
        updateDisabled( assn );
        Backtracker::backtrack( sol );

        return sol;
    }

    Assignment ForwardCheckingValueSelector::select( CSPSolution& sol )
    {
        Assignment assn = make_pair( UNSET, UNSET );

        while( true )
        {
            assn = ValueSelector::select( sol );
            if( assn.second != UNSET )
            {
                revise( sol, assn );
                // assn could be set to UNSET
                if( assn.second != UNSET ) break;
            }
            else break;
        }

        return assn;
    }

    void ForwardCheckingValueSelector::revise( CSPSolution& sol, Constraint& cnstr, vector<bv_t>& disabled  )
    {
        int idx0 = cnstr.scope[ 0 ];
        int idx1 = cnstr.scope[ 1 ];

        if( cnstr.arity != 2 || !( (sol.assn[ idx0 ] == UNSET) ^ (sol.assn[ idx1 ] == UNSET) ) ) return;

        int unset_idx = (sol.assn[ idx0 ] == UNSET) ? idx0 : idx1;

        Domain& dom = sol.problem->domains[ sol.problem->variables[ unset_idx ] ];

        for( unsigned int i = 0; i < dom.size(); i++ )
        {
            if( !sol.allowable[ unset_idx ][ i ] ) continue;

            sol.assn[ unset_idx ] = i;
            disabled[ unset_idx ][ i ] = disabled[ unset_idx ][ i ] | !cnstr.test( sol.assn );
            sol.allowable[ unset_idx ][ i ] = sol.allowable[ unset_idx ][ i ] & cnstr.test( sol.assn );
        }
        sol.assn[ unset_idx ] = UNSET;
    }

    void ForwardCheckingValueSelector::revise( CSPSolution& sol, Assignment& assn )
    {
        // Reset last_disabled
        last_disabled.resize( sol.allowable.size() );
        for( unsigned int i = 0; i < sol.allowable.size(); i++ )
        {
            last_disabled[ i ].resize( 0 );
            last_disabled[ i ].resize( sol.allowable[ i ].size(), false );
        }

        sol.assn[ assn.first ] = assn.second;

        // For every constraint that is dependent on 2 or more values
        vector<Constraint*>::iterator it;
        for( it = sol.problem->constraints.begin(); it != sol.problem->constraints.end(); it++ )
        {
            Constraint& cnstr = *(*it);
            revise( sol, cnstr, last_disabled  );
        }

        // Check if any domains have become empty
        bool invalid = false;
        for( unsigned int i = 0; i < sol.allowable.size() && !invalid; i++ )
        {
            if( sol.assn[ i ] != UNSET ) continue;
            bool has_any = false;
            for( unsigned int j = 0; j < sol.allowable[i].size() && !has_any; j++ )
            {
                has_any |= sol.allowable[ i ][ j ];
            }

            invalid |= (!has_any);
        }
        if( invalid )
        {
            // Store this update in the previous backtrack-point
            ForwardCheckingBacktracker& bt = dynamic_cast< ForwardCheckingBacktracker& >( sol.solver->getBacktracker() );
            bt.updateDisabled( assn );
            sol.allowable[ assn.first ][ assn.second ] = false;

            assn.second = UNSET;
            // Re-enable crossed-off options
            for( unsigned int i = 0; i < last_disabled.size(); i++ )
            {
                for( unsigned int j = 0; j < last_disabled[ i ].size(); j++ )
                {
                    sol.allowable[ i ][ j ] = sol.allowable[ i ][ j ] | last_disabled[ i ][ j ];
                }
            }
        }

        sol.assn[ assn.first ] = UNSET;
    }

    vector<bv_t>& ForwardCheckingValueSelector::getLastDisabled()
    {
        return last_disabled;
    }

};

