#ifndef _SEM_H
#define _SEM_H



#include <semaphore.h>

class sem
{
public:
    sem();

    ~sem();

    int wait();

    int try_wait();

    int post();

private:
    sem_t sem_;
};



#endif