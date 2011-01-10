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
    typedef pair<int, int> vars;
    
    class AC3Preprocessor : public Preprocessor
    {
        
    public:
        //	AC3Preprocessor(CSPsolution& sol, CSP& problem);
        /**
         * Select a value 
         *
         * @arg sol - solution to update
         * @returns - (var_idx, val) 
         *            Should be UNSET if no suitable variable exists
         */
        virtual CSPSolution& preprocess( CSPSolution& sol );

    private:
        bool revise( CSPSolution& sol, vars vartuple);  //revise function
        void print( CSPSolution& sol);
    };
};

#endif // AC3_H

