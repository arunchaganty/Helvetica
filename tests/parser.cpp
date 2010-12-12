/**
* @file parser.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench
*/

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <vector>
using namespace std;

#include "Helvetica.h"
#include "CSP.h"
#include "Log.h"
using namespace Helvetica;

#define TEST_DATA "data/test.xml"

Log* g_Log;

int main( int argc, char* argv[] )
{
    g_Log = &Log::create( cerr, Log::DEBUG );
    CSP& problem = CSP::parse( TEST_DATA );

    // Domains
    assert( problem.domains.size() == 3 );
    assert( problem.domains[0][0] && problem.domains[0][4] );
    assert( problem.domains[1][5] && !problem.domains[1][6] );
    assert( problem.domains[2][1] && problem.domains[2][13] && !problem.domains[2][5] );

    // Variables
    assert( problem.variables.size() == 5 );
    assert( problem.variables[0] == 0 );
    assert( problem.variables[1] == 0 );
    assert( problem.variables[2] == 1 );

    // Relations
    assert( problem.relations.size() == 4 );

    assert( !problem.relations[0].test(2, 0, 0 ) );
    assert( problem.relations[0].test(2, 0, 1 ) );

    assert( problem.relations[2].test(2, 5, 3 ) );
    assert( !problem.relations[2].test(2, 5, 4 ) );

    assert( problem.relations[3].test(3, 0, 1, 3 ) );
    assert( !problem.relations[3].test(3, 0, 1, 2 ) );

    // Constraints
    assert( problem.constraints.size() == 5 );

    assert( problem.constraints[0].rel == &problem.relations[0] );
    assert( problem.constraints[0].scope[0] == 0 && problem.constraints[0].scope[1] == 1 );

    assert( problem.constraints[1].rel == &problem.relations[1] );
    assert( problem.constraints[1].scope[0] == 3 && problem.constraints[1].scope[1] == 0 );

    return EXIT_SUCCESS;
}

