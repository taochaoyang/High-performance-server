#ifndef _MUTEX_H
#define _MUTEX_H


#include <pthread.h>
#include <exception>


class mutex
{
public:
    mutex();

    ~mutex();

    int lock();

    int trylock();
    
    int unlock();

private:
    pthread_mutex_t mutex_;
};



#endif