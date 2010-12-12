/**
* @file main.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench
*/

#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

#include "CSP.h"
#include "CSPSolver.h"
#include "Helvetica.h"

using namespace std;
using namespace Helvetica;

Log* g_Log;

extern int optind, opterr;

Options g_Options;

void print_help( FILE* file, char* argv[] )
{
    fprintf( file, "Usage: %s [options] <csp-file>\n", argv[ 0 ] );
    fprintf( file, "Options:\n" );
    fprintf( file, "\t-h \t--\t Print this message\n" );
    fprintf( file, "\t-v \t--\t Verbose (print all game states)\n" );
}

bool is_file( string fname )
{
    // Validate input
    struct stat st;
    if( stat( fname.c_str(), &st ) != 0 )
    {
        return S_ISREG( st.st_mode );
    }

    return false;
}

int main( int argc, char* argv[] )
{
    int opt;

    g_Log = &Log::create( cerr, Log::DEBUG );

    // Parse command line options 
    while( ( opt = getopt( argc, argv, "vh" ) ) != -1 )
    {
        switch( opt ) 
        {
            case 'v':
                g_Options.isVerbose = true;
                break;
            case 'h':
            default: /* '?' */
                print_help( stderr, argv );
                exit( EXIT_SUCCESS );
        }
    }

    if( ( g_Options.mode == NORMAL ) && ( argc - optind == 1 ) )
    {
        string filename = string( argv[ optind + 0 ] );
        // Validate input
        if( ! is_file( filename ) )
        {
            fprintf( stderr, "Error loading file: %s\n", filename.c_str() );
            exit( EXIT_FAILURE );
        }
        CSP problem = CSP::parse( filename );
        // TODO: Make a solver specific options file
        CSPSolver solver = CSPSolver::create( CSPSolver::Settings( ) );

        CSPSolution& sol = solver.solve( problem );
        // Print solution
    }


    return EXIT_SUCCESS;
}

