#include "Timer.H"

#include <iostream>

void*
Timer::
_timeThread(void *arg)
{
        Timer *ptr = (Timer *)arg;
        while (ptr->_running == 1) {
                sleep(ptr->_time);
                pthread_cond_signal(&ptr->_cond);
        }
        return NULL;
}

void*
Timer::
_doJob(void *arg)
{
        Timer *ptr = (Timer *)arg;

        std::cout << __FUNCTION__ << std::endl;

        while (ptr->_running == 1) {
                pthread_mutex_lock(&ptr->_mutex);
                pthread_cond_wait(&ptr->_cond, &ptr->_mutex);
                ptr->_job(ptr->_jobArg);
                pthread_mutex_unlock(&ptr->_mutex);
        }

        return NULL;
}

void
Timer::
start()
{
        std::cout << __FUNCTION__ << std::endl;
        _running = 1;
        pthread_create(&_tidSleep, NULL, _timeThread, this); // sleep then send signal
        pthread_create(&_tidJob, NULL, _doJob, this); // do real job
}

void
Timer::
stop()
{
        std::cout << __FUNCTION__ << std::endl;
        _running = 0;
        pthread_join(_tidJob, NULL);
        pthread_join(_tidSleep, NULL);
}
