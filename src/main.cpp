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
#include <getopt.h>
#include <string>
#include <stdexcept>
#include <iostream>

#include "CSP.h"
#include "CSPSolver.h"
#include "Helvetica.h"

using namespace std;
using namespace Helvetica;

Log* g_Log;

extern int optind, opterr;

Options g_Options;

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
    static Settings create( Options::Plan plan )
    {
        switch( plan )
        {
            case( Options::NONE ):
            default:
                return Settings( BT_NONE, VS_NONE, PP_NONE );
        }
    }
};

CSPSolver& create( Settings settings )
{
    Backtracker* bt = NULL;
    ValueSelector* vs = NULL;
    Preprocessor* pp = NULL;

    switch( settings.Backtracker )
    {
        case Settings::BT_NONE: 
            bt = new Backtracker();
            break;
        default:
            throw runtime_error( "Invalid option" );
    }

    switch( settings.ValueSelector )
    {
        case Settings::VS_NONE: 
            vs = new ValueSelector();
            break;
        default:
            throw runtime_error( "Invalid option" );
    }

    switch( settings.Preprocessor )
    {
        case Settings::PP_NONE: 
            pp = new Preprocessor();
            break;
        default:
            throw runtime_error( "Invalid option" );
    }

    CSPSolver* solver = new CSPSolver( *bt, *vs, *pp );

    return *solver;
}

void print_help( FILE* file, char* argv[] )
{
    fprintf( file, "Usage: %s [options] <csp-file>\n", argv[ 0 ] );
    fprintf( file, "Options:\n" );
    fprintf( file, "\t-h \t--\t Print this message\n" );
    fprintf( file, "\t-v \t--\t Verbose\n" );
}

bool is_file( string fname )
{
    // Validate input
    struct stat st;
    return ( stat( fname.c_str(), &st ) == 0 && S_ISREG( st.st_mode ) );
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
                print_help( stdout, argv );
                exit( EXIT_SUCCESS );
                break;
            default: /* '?' */
                print_help( stderr, argv );
                exit( EXIT_FAILURE );
        }
    }

    if( ( g_Options.mode == Options::NORMAL ) && ( argc - optind == 1 ) )
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
        CSPSolver solver = create( Settings::create( g_Options.plan ) );

        CSPSolution& sol = solver.solve( problem );
        // Print solution
        cout << "Answer: " << sol << endl;
    }
    else
    {
        print_help( stderr, argv );
        exit( EXIT_FAILURE );
    }


    return EXIT_SUCCESS;
}

