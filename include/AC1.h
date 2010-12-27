/**
* @file AC3.h
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench : AC3 lookahead implementation
*/

#ifndef AC3_H
#define AC3_H

#include <vector>
#include <stack>
#include <cassert>
#include "CSP.h"
#include "CSPSolver.h"
using namespace std;

namespace Helvetica 
{
    class AC1Backtracker : public Backtracker
    {
    public:
        /**
         * Save a variable assignment 
         *
         * @arg sol - the updated solution (after settings var_idx to val)
         * @arg var_idx - the variable index just modified
         * @arg val - the new value
         */
        virtual void save( CSPSolution& sol, Assignment assn );

        /**
         * Update the last backtrack point's disabled list to include
         * information for the assignment
         * @arg assn - the assignment to be
         */
        virtual void updateDisabled( Assignment assn );

        /**
         * Recover a variable assignment 
         *
         * @arg sol - the solution to revert
         */
        virtual CSPSolution& backtrack( CSPSolution& sol );

    private:
        // Store the assignment, and the bits disabled
        stack<pair< Assignment, vector<bv_t> > > memory;
    };

    class AC1ValueSelector : public ValueSelector
    {
    public:
        /**
         * Select a value 
         *
         * @arg sol - solution to update
         * @returns - (var_idx, val) 
         *            Should be UNSET if no suitable variable exists
         */
        virtual Assignment select( CSPSolution& sol );
    };

    class AC1Preprocessor : public Preprocessor
    {
    public:
        /**
         * Select a value 
         *
         * @arg sol - solution to update
         * @returns - (var_idx, val) 
         *            Should be UNSET if no suitable variable exists
         */
        void revise( CSPSolution& sol, Assignment& assn );
        virtual CSP& preprocess( CSP& problem );
        vector<bv_t>& getLastDisabled(); 

    private:
        void revise( CSPSolution& sol, Constraint& cnstr, vector<bv_t>& disabled  );
        vector<bv_t> last_disabled;
    };
};

#endif // AC3_H

