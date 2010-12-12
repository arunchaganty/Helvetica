/**
* @file CSPSolver.h
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench : CSP Solver base class
*/

#include <vector>
#include <cassert>
using namespace std;

namespace Helvetica 
{
    // Forward declaration
    class CSPSolver;

    /**
     * Class to track intermediate state of the solution
     */
    struct CSPSolution
    {
        CSP* problem;
        CSPSolver* solver;
        vector<int> assn; // Intermediate variable assignments 

        CSPSolution( CSP& problem, CSPSolver& solver);
        CSPSolution( CSP& problem, CSPSolver& solver, vector<int> assn );
        CSPSolution( CSPSolution& solution, vector<int> assn );

        bool isSolved();
    };

    class Backtracker
    {
    public:
        /**
         * Save a variable assignment 
         *
         * @arg sol - the updated solution (after settings var_idx to val)
         * @arg var_idx - the variable index just modified
         * @arg val - the new value
         */
        virtual void save( CSPSolution& sol, int var_idx, val_t val ) = 0;
        /**
         * Recover a variable assignment 
         *
         * @arg sol - the solution to revert
         */
        virtual CSPSolution& backtrack( CSPSolution& sol ) = 0;
    };

    class ValueSelector
    {
    public:
        /**
         * Select a value 
         *
         * @arg sol - solution to update
         * @arg var_idx - Variable index that needs updating. 
         *                Should be UNSET if no suitable variable exists
         * @returns - value to be assigned to var_idx.
         *            Should be UNSET if no suitable variable exists
         */
        virtual val_t select( CSPSolution& sol, int& var_idx ) = 0;
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

            BacktrackerOptions Backtracker;
            ValueSelectorOptions ValueSelector;

            Settings( BacktrackerOptions bt_option = BT_NONE, ValueSelectorOptions vs_option = VS_NONE )
                : Backtracker( bt_option ), ValueSelector( vs_option ) {}
        };

        CSPSolver( Backtracker& backtracker, ValueSelector& valueSelector );
        CSPSolution& solve( CSP& problem );

        static CSPSolver& create( Settings settings );

    protected:
        Backtracker& backtracker;
        ValueSelector& valueSelector;
    };

};
