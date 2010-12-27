/**
* @file CSPSolver.h
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench : CSP Solver base class
*/

#ifndef CSPSOLVER_H
#define CSPSOLVER_H

#include <vector>
#include <stack>
#include <cassert>
#include "CSP.h"
using namespace std;

namespace Helvetica 
{
    // Forward declaration
    class CSPSolver;

    typedef pair< int, int > Assignment;

    /**
     * Class to track intermediate state of the solution
     */
    struct CSPSolution
    {
        CSP* problem;
        CSPSolver* solver;
        vector<bv_t> allowable; // Intermediate variable assignments 
        vector<int> assn; // Intermediate variable assignments 

        CSPSolution( CSP& problem, CSPSolver& solver);
        CSPSolution( CSP& problem, CSPSolver& solver, vector<int> assn );
        CSPSolution( CSPSolution& solution, vector<int> assn );

        void apply( Assignment& assn );
        bool isSolved();

    };

    ostream& operator<<( ostream& st, const CSPSolution& sol );

    class Backtracker
    {
    public:
        Backtracker();
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

    private:
        stack<Assignment> memory;
    };

    class ValueSelector
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

    class Preprocessor
    {
    public:
        /**
         * Select a value 
         *
         * @arg sol - solution to update
         * @returns - (var_idx, val) 
         *            Should be UNSET if no suitable variable exists
         */
        virtual CSP& preprocess( CSP& problem );
    };

    class CSPSolver
    {
    public:
        struct Settings
        {
            enum BacktrackerOptions
            {
                BT_NONE,
            };
            enum ValueSelectorOptions
            {
                VS_NONE,
            };
            enum PreprocessorOptions
            {
                PP_NONE,
            };

            BacktrackerOptions Backtracker;
            ValueSelectorOptions ValueSelector;
            PreprocessorOptions Preprocessor;

            Settings( BacktrackerOptions bt_option = BT_NONE, ValueSelectorOptions vs_option = VS_NONE, PreprocessorOptions pp_option = PP_NONE )
                : Backtracker( bt_option ), ValueSelector( vs_option ), Preprocessor( pp_option ) {}
        };

        CSPSolver( Backtracker& backtracker, ValueSelector& valueSelector, Preprocessor& preprocessor );
        virtual ~CSPSolver( );
        CSPSolution& solve( CSP& problem );

        static CSPSolver& create( Settings settings );

        Backtracker& getBacktracker();
        ValueSelector& getValueSelector();
        Preprocessor& getPreprocessor();


    protected:
        Backtracker& backtracker;
        ValueSelector& valueSelector;
        Preprocessor& preprocessor;
    };

};

#endif // CSPSOLVER_H

