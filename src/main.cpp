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
#include "AC1.h"
#include "Helvetica.h"

using namespace std;
using namespace Helvetica;

Log* g_Log;

extern int optind, opterr;

Options g_Options;
Stats g_Stats;

struct Settings
{
    enum BacktrackerOptions
    {
        BT_NONE,
        BT_AC1,
    };
    enum ValueSelectorOptions
    {
        VS_NONE,
        VS_AC1,
    };
    enum PreprocessorOptions
    {
        PP_NONE,
        PP_AC1,
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
            case( Options::AC1 ):
                return Settings( BT_AC1, VS_AC1, PP_AC1 );
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
        case Settings::BT_AC1: 
            bt = new AC1Backtracker();
            break;
        default:
            throw runtime_error( "Invalid option" );
    }

    switch( settings.ValueSelector )
    {
        case Settings::VS_NONE: 
            vs = new ValueSelector();
            break;
        case Settings::VS_AC1: 
            vs = new AC1ValueSelector();
            break;
        default:
            throw runtime_error( "Invalid option" );
    }

    switch( settings.Preprocessor )
    {
        case Settings::PP_NONE: 
            pp = new Preprocessor();
            break;
        case Settings::PP_AC1: 
            pp = new AC1Preprocessor();
            break;
        default:
            throw runtime_error( "Invalid option" );
    }

    CSPSolver* solver = new CSPSolver( *bt, *vs, *pp );

    return *solver;
}

static struct option options[] = {
    { "ac1", no_argument, NULL, Options::AC1 },
};

void print_help( FILE* file, char* argv[] )
{
    fprintf( file, "Usage: %s [options] <csp-file>\n", argv[ 0 ] );
    fprintf( file, "Options:\n" );
    fprintf( file, "\t-h \t--\t Print this message\n" );
    fprintf( file, "\t-v \t--\t Verbose\n" );
    fprintf( file, "\t--ac1 \t--\t Consistency - AC1\n" );
}

bool is_file( string fname )
{
    // Validate input
    struct stat st;
    return ( stat( fname.c_str(), &st ) == 0 && S_ISREG( st.st_mode ) );
}

ostream& print_stats( ostream& st, const Stats stats )
{
    st << "Backtracks: " << stats.backtracks;
    st << " Total Time: " << stats.run_time.getTicks() << "ms";
    st << " (" << (stats.avg_vs_time.getTicks() / (float) stats.vs_count ) << "ms)";

    return st;
}

int main( int argc, char* argv[] )
{
    int opt;

    g_Log = &Log::create( cerr, Log::DEBUG );
    int long_idx;

    // Parse command line options 
    while( ( opt = getopt_long( argc, argv, "vh", options, &long_idx ) ) != -1 )
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
            case Options::AC1:
                g_Options.plan = Options::AC1;
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
        g_Log->info( "Backtracks: %d Total Time: %ems (%ems)", 
                g_Stats.backtracks, g_Stats.run_time.getTicks(), (g_Stats.avg_vs_time.getTicks() / (float) g_Stats.vs_count ) );
    }
    else
    {
        print_help( stderr, argv );
        exit( EXIT_FAILURE );
    }


    return EXIT_SUCCESS;
}

