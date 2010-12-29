/**
* @file GaschnigBacktracker.h
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench : Gaschnig's Backjumping
*/

#ifndef GASCHNIG_H
#define GASCHNIG_H

#include <vector>
#include <stack>
#include <cassert>
#include "CSP.h"
using namespace std;

namespace Helvetica 
{
    class GaschnigBacktracker : public Backtracker
    {
    public:
        GaschnigBacktracker();
        virtual ~GaschnigBacktracker() {}
        /**
         * Save a variable assignment 
         *
         * @arg sol - the updated solution (after settings var_idx to val)
         * @arg var_idx - the variable index just modified
         * @arg val - the new value
         */
        virtual void save( CSPSolution& sol, Assignment assn );
        /**
         * Recover a variable assignment 
         *
         * @arg sol - the solution to revert
         */
        virtual CSPSolution& backtrack( CSPSolution& sol );

        inline vector< int >& getCulprit() { return culprit; }

    private:
        vector< int > culprit;
    };

    class GaschnigValueSelector : public ValueSelector
    {
    public:
        /**
         * Select a value 
         *
         * @arg sol - solution to update
         * @returns - (var_idx, val) 
         *            Should be UNSET if no suitable variable exists
         */
        virtual bool valid( CSPSolution& sol, Assignment assn );
    };

};

#endif // GASCHNIG_H

