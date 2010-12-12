/**
* @file tuple.h
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench
*/

#ifndef TUPLE_H
#define TUPLE_H

#include <cassert>
#include <cstdarg>

template<class T>
struct tuple
{
    int len;
    T* vals;

    tuple(int len) 
        : len( len ), vals( new T[len] )
    {
    }

    tuple(int len, T* vals) 
        : len( len ), vals( new T[len] )
    {
        for( int i = 0; i < len; i++ )
            this->vals[ i ] = vals[ i ];
    }

    tuple(int len, T v1, ...)
        : len( len )
    {
        va_list lst;
        vals = new T[len];

        vals[0] = v1;
        va_start( lst, len );
        for( int i = 1; i < len; i++ )
        {
            vals[i] = va_arg( lst, T );
        }
        va_end( lst );
    }

    ~tuple()
    {
        delete vals;
    }
    T& operator[]( int i )
    {
        assert( i < len );

        return vals[ i ];
    }

};

template< class T >
bool operator<( const tuple<T>& t1, const tuple<T>& t2 )
{
    assert( t1.len == t2.len );

    for( int i = 0; i < t1.len; i++ )
    {
        if( t1.vals[i] < t2.vals[i] ) return true;
    }
    return false;
}
template< class T >
bool operator==( const tuple<T>& t1, const tuple<T>& t2 )
{
    assert( t1.len == t2.len );

    for( int i = 0; i < t1.len; i++ )
    {
        if( t1.vals[i] != t2.vals[i] ) return false;
    }
    return true;
}

#endif // TUPLE_H
