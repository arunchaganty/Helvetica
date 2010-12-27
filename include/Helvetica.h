/**
* @file Helvetica.h
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench
*/

#ifndef HELVETICA_H
#define HELVETICA_H

#include <sys/time.h>
#include "Log.h"
#include "Timer.h"

namespace Helvetica 
{
    const int UNSET = -1;

    struct Options
    {
        enum Mode
        {
            NORMAL=0,
        };
        enum Plan 
        {
            NONE,
            AC1
        };

        Mode mode;
        Plan plan;
        bool isVerbose;

        Options()
        {
            mode = NORMAL;
            plan = NONE;
            isVerbose = false;
        }
    };

    struct Stats
    {
        int backtracks;

        // Timing value selection
        Timer avg_vs_time;
        int vs_count;

        // Timing the entire process
        Timer run_time;
    };
};

#endif // HELVETICA_H

