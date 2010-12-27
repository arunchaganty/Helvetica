/*
 * Generic Timer class
 */

#include <ctime>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sys/time.h>
using namespace std;

#include "Timer.h"

Timer::Timer()
{
    m_isRunning = false;
}

void Timer::start()
{
    assert(!m_isRunning);
    m_isRunning = true;

    gettimeofday(&m_Base, NULL);
    timerclear(&m_Ticks);
}

void Timer::pause()
{
    assert(m_isRunning);
    m_isRunning = false;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    timersub(&tv, &m_Base, &tv);
    timeradd(&m_Ticks, &tv, &m_Ticks);
}

void Timer::unpause()
{
    assert(!m_isRunning);
    m_isRunning = true;

    gettimeofday(&m_Base, NULL);
}

long Timer::getSecs() const
{
    return  m_Ticks.tv_sec;
}

long Timer::getUSecs() const
{
    return  m_Ticks.tv_usec;
}

double Timer::getTicks() const
{
    return  m_Ticks.tv_sec + (double) m_Ticks.tv_usec/1000000;
}

