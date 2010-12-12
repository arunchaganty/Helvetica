/**
* @file Helvetica.h
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench
*/

#ifndef HELVETICA_H
#define HELVETICA_H

#include "Log.h"

namespace Helvetica 
{
    const int UNSET = -1;

    enum Mode
    {
        NORMAL=0,
    };

    struct Options
    {
        Mode mode;
        bool isVerbose;

        Options()
        {
            mode = NORMAL;
            isVerbose = false;
        }

    };
};

#endif // HELVETICA_H

