#ifndef TIMER_H
#define TIMER_H

#include <unistd.h>
#include <pthread.h>

class Timer {
public:
        Timer(int i = 5, void* (*job)(void *arg) = NULL, void* jobArg = NULL)
                : _time(i), _job(job), _jobArg(jobArg),
                  _running(0),
                  _mutex(PTHREAD_MUTEX_INITIALIZER),
                  _cond(PTHREAD_COND_INITIALIZER) {}
        virtual ~Timer() = default;

        void start();
        void stop();
private:
        static void* _timeThread(void *arg);
        static void* _doJob(void *arg);
        pthread_mutex_t _mutex;
        pthread_cond_t _cond;
        pthread_t _tidSleep, _tidJob;
        int _time;
        void* (*_job)(void* arg);

        void* _jobArg;
        int _running;
};

#endif /* TIMER_H */
