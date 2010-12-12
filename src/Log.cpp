
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdarg>
#include <sys/stat.h>

#include <iostream>
#include <fstream>
using namespace std;

#include "Log.h"
#define STR_BUF_SIZE 1024

Log::Log( ostream& stream, Level level )
    : stream( &stream ), level( level )
{
}

Log& Log::create( string filename, Level level )
{
    ostream* stream = new fstream( filename.c_str(), fstream::out );
    Log* log = new Log( *stream, level );

    return *log;
}

Log& Log::create( ostream& stream, Level level )
{
    ostream* stream_ = &stream;
    Log* log = new Log( *stream_, level );

    return *log;
}

Log::Level Log::getLevel() 
{
    return level;
}

void Log::setLevel( Log::Level level ) 
{
    this->level = level;
}

char str_buf[STR_BUF_SIZE];
void Log::info( const char* msg, ... )
{
    if( level <= INFO )
    {
        va_list vl;
        va_start( vl, msg );
        vsnprintf( str_buf, STR_BUF_SIZE * sizeof( char ), msg, vl );
        (*stream) << "[INFO]: " << string( str_buf ) << endl;
    }
}

void Log::debug( const char* msg, ... )
{
    if( level <= DEBUG )
    {
        va_list vl;
        va_start( vl, msg );
        vsnprintf( str_buf, STR_BUF_SIZE * sizeof( char ), msg, vl );
        (*stream) << "[DEBUG]: " << str_buf << endl;
    }
}

void Log::error( const char* msg, ... )
{
    if( level <= ERROR )
    {
        va_list vl;
        va_start( vl, msg );
        vsnprintf( str_buf, STR_BUF_SIZE * sizeof( char ), msg, vl );
        (*stream) << "[ERROR]: " << str_buf << endl;
    }
}

