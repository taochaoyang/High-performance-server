#ifndef _PTHREAD_POLL_H
#define _PTHREAD_POLL_H


#include "common.h"
#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "mutex.h"
#include "sem.h"

template< typename T >
class pthread_pool
{
public:
    pthread_pool( int thread_number = 8, int max_requests = 10000 );

    void init();

    ~pthread_pool();

    bool append( T&& request );

private:
    static void* worker( void* arg );

    void run();

private:
    int thread_number_;

    int max_requests_;

    pthread_t* threads_;

    std::list< T > workqueue_;

    mutex mutex_;//用于抢占任务的同步

    sem queuestat_;
    
    bool stop_;
};


template< typename T >
pthread_pool< T >::pthread_pool( int thread_number, int max_requests) :
        thread_number_( thread_number ), max_requests_( max_requests ), stop_( false ), threads_( NULL )
{
    if( ( thread_number <= 0 ) || ( max_requests <= 0 ) )
    {
        throw std::exception();
    }

    threads_ = new pthread_t[ thread_number_ ];
    if( ! threads_ )
    {
        throw std::exception();
    }
}

template< typename T >
void pthread_pool< T >::init() {
    for ( int i = 0; i < thread_number_; ++i )
    {
        printf( "create the %dth thread\n", i );
        if( pthread_create( threads_ + i, NULL, worker, this ) != 0 )
        {
            delete [] threads_;
            throw std::exception();
        }
        if( pthread_detach( threads_[i] ) )
        {
            delete [] threads_;
            throw std::exception();
        }
    }
}

template< typename T >
pthread_pool< T >::~pthread_pool()
{
    delete [] threads_;
    stop_ = true;
}

template< typename T >
bool pthread_pool< T >::append( T&& request )
{
    mutex_.lock();
    if ( workqueue_.size() > max_requests_ )
    {
        mutex_.unlock();
        return false;
    }
    workqueue_.push_back( request );
    mutex_.unlock();
    queuestat_.post();
    return true;
}

template< typename T >
void* pthread_pool< T >::worker( void* arg )
{
    pthread_pool* pool = ( pthread_pool* )arg;
    pool->run();
    return pool;
}

template< typename T >
void pthread_pool< T >::run()
{
    while ( ! stop_ )
    {
        queuestat_.wait();
        DBG("wake");
        mutex_.lock();
        if ( workqueue_.empty() )
        {
            mutex_.unlock();
            continue;
        }
        DBG("2");
        T request = workqueue_.front();
        workqueue_.pop_front();
        mutex_.unlock();
        DBG("333");
        request.call_back(&request);
        DBG("3");
    }
}



#endif