/**
* @file Log.h
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench
*/

#ifndef LOG_H
#define LOG_H

#include <string>
#include <fstream>
#include <iostream>
using namespace std;

class Log
{
    public:
        enum Level
        {
            DEBUG = 0,
            INFO = 1,
            ERROR = 2
        };

        static Log& create( string filename, Level level );
        static Log& create( ostream& stream, Level level );

        void setLevel( Level level ); 
        Level getLevel(); 

        void info( const char* msg, ...);
        void debug( const char* msg, ...);
        void error( const char* msg, ...);

    protected:
        Log( ostream& stream, Level level );
        ostream* stream;
        Level level;
};

#endif // LOG_H
