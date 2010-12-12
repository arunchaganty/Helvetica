/**
* @file tuples.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench
*/

#include <cstdlib>
#include <cassert>
#include <vector>
#include <set>
using namespace std;

void foo( set< vector< int > >& s, int n, int values[] )
{
    vector<int> v( n, -1 );
    for( int i = 0; i < n; i++ ) v[ i ] = values[ i ];
    s.insert( v );
}

int main( int argc, char* argv[] )
{
    set< vector< int > > s;
    int values[] = {1,2,3};
    vector<int> t;
    t.push_back( 1 ); t.push_back( 2 ); t.push_back( 3 );
    foo( s, 3, values );
    assert( s.find( t ) != s.end() );
    t.push_back( 4 ); 
    assert( s.find( t ) == s.end() );

    return EXIT_SUCCESS;
}

