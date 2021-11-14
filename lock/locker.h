#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

/************************************************************************/
/*                     function notion                                  */
/*                                                                      */
/************************************************************************/
/*
int sem_init(sem_t* sem, int pshared, unsigned int value);

sem_init() initializes the unnamed semaphore at the address pointed to by sem.
The 'value' argument specifies the initial value for the semaphore.

The 'pshared' argument indicates whether this semaphore is to be shared 
    between the threads of a process, or between processes.
If pshared has the value 0, then the semaphore is shared between
       the threads of a process, and should be located at some address
       that is visible to all threads (e.g., a global variable, or a
       variable allocated dynamically on the heap).
If pshared is nonzero, then the semaphore is shared between
       processes, and should be located in a region of shared memory
       (see shm_open(3), mmap(2), and shmget(2)).  (Since a child
       created by fork(2) inherits its parent's memory mappings, it can
       also access the semaphore.)  Any process that can access the
       shared memory region can operate on the semaphore using
       sem_post(3), sem_wait(3), and so on.

Initializing a semaphore that has already been initialized
       results in undefined behavior.

sem_init() returns 0 on success; on error, -1 is returned, and
       errno is set to indicate the error.
 */



class sem{
    public:
        sem(){
            if(sem_init(&m_sem, 0, 0) != 0){
                throw std::exception();
            }
        }
        ~sem(){
            sem_destroy(&m_sem);
        }
        bool wait(){
            return sem_wait(&m_sem) == 0;
        }
        bool post(){
            return sem_post(&m_sem) == 0;
        }
    private:
        sem_t m_sem;
};

class locker{

    public:
        locker(){
            if(pthread_mutex_init(&m_mutex, NULL) != 0){
                throw std::exception();
            }
        }
        ~locker(){
            pthread_mutex_destroy(&m_mutex);
        }
        bool lock(){
            return pthread_mutex_lock(&m_mutex) == 0;
        }
        bool unlock(){
            return pthread_mutex_unlock(&m_mutex) == 0;
        }

    private:
        pthread_mutex_t m_mutex;
};

class cond{

    public:
        cond(){
            
            if(pthread_mutex_init(&m_mutex, NULL) != 0){
                throw std::exception();
            }
            if(pthread_cond_init(&m_cond, NULL) != 0){
                pthread_mutex_destroy(&m_mutex);
                throw std::exception();
            }
        }

        ~cond(){
            pthread_mutex_destroy(&m_mutex);
            pthread_cond_destroy(&m_cond);
        }

        bool wait(){
            int ret = 0;
            pthread_mutex_lock(&m_mutex);
            ret= pthread_cond_wait(&m_cond, &m_mutex);
            pthread_mutex_unlock(&m_mutex);
            return ret==0;
        }

        bool signal(){
            return pthread_cond_signal(&m_cond) == 0;
        }

    private:
        pthread_mutex_t m_mutex;
        pthread_cond_t m_cond;
};

#endif