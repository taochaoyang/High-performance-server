#include "mutex.h"

mutex::mutex()
{
    if( pthread_mutex_init( &mutex_, NULL ) != 0 )
    {
        throw std::exception();
    }
}

mutex::~mutex()
{
    pthread_mutex_destroy( &mutex_ );
}

int mutex::lock()
{
    return pthread_mutex_lock( &mutex_ ) == 0;
}

int mutex::trylock()
{
    return pthread_mutex_lock( &mutex_ ) == 0;
}

int mutex::unlock()
{
    return pthread_mutex_unlock( &mutex_ ) == 0;
}