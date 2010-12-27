/*
 * Generic Timer class
 */

#ifndef TIMER_H
    #define TIMER_H

#include<ctime>

class Timer {

public:
    Timer();

    void start();
    void pause();
    void unpause();

    double getTicks() const; 
    long getSecs() const;
    long getUSecs() const;

protected:
    struct timeval m_Ticks;
    struct timeval m_Base;

    bool m_isRunning;
};

#endif // TIMER_H
