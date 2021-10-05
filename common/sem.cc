#include "sem.h"
#include <exception>

sem::sem() {
    if (sem_init(&sem_, 0, 0) != 0) {
        throw std::exception();
    }
}

sem::~sem() {
    sem_destroy(&sem_);
}

int sem::wait() {
    return sem_wait(&sem_);
}

int sem::try_wait() {
    return sem_trywait(&sem_);
}

int sem::post() {
    return sem_post(&sem_);
}