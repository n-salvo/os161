/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Synchronization primitives.
 * The specifications of the functions are in synch.h.
 */

#include <types.h>
#include <lib.h>
#include <spinlock.h>
#include <wchan.h>
#include <thread.h>
#include <current.h>
#include <synch.h>

////////////////////////////////////////////////////////////
//
// Semaphore.

struct semaphore *
sem_create(const char *name, unsigned initial_count)
{
	struct semaphore *sem;

	sem = kmalloc(sizeof(*sem));
	if (sem == NULL) {
		return NULL;
	}

	sem->sem_name = kstrdup(name);
	if (sem->sem_name == NULL) {
		kfree(sem);
		return NULL;
	}

	sem->sem_wchan = wchan_create(sem->sem_name);
	if (sem->sem_wchan == NULL) {
		kfree(sem->sem_name);
		kfree(sem);
		return NULL;
	}

	spinlock_init(&sem->sem_lock);
	sem->sem_count = initial_count;

	return sem;
}

void
sem_destroy(struct semaphore *sem)
{
	KASSERT(sem != NULL);

	/* wchan_cleanup will assert if anyone's waiting on it */
	spinlock_cleanup(&sem->sem_lock);
	wchan_destroy(sem->sem_wchan);
	kfree(sem->sem_name);
	kfree(sem);
}

void
P(struct semaphore *sem)
{
	KASSERT(sem != NULL);

	/*
	 * May not block in an interrupt handler.
	 *
	 * For robustness, always check, even if we can actually
	 * complete the P without blocking.
	 */
	KASSERT(curthread->t_in_interrupt == false);

	/* Use the semaphore spinlock to protect the wchan as well. */
	spinlock_acquire(&sem->sem_lock);
	while (sem->sem_count == 0) {
		/*
		 *
		 * Note that we don't maintain strict FIFO ordering of
		 * threads going through the semaphore; that is, we
		 * might "get" it on the first try even if other
		 * threads are waiting. Apparently according to some
		 * textbooks semaphores must for some reason have
		 * strict ordering. Too bad. :-)
		 *
		 * Exercise: how would you implement strict FIFO
		 * ordering?
		 */
		wchan_sleep(sem->sem_wchan, &sem->sem_lock);
	}
	KASSERT(sem->sem_count > 0);
	sem->sem_count--;
	spinlock_release(&sem->sem_lock);
}

void
V(struct semaphore *sem)
{
	KASSERT(sem != NULL);

	spinlock_acquire(&sem->sem_lock);

	sem->sem_count++;
	KASSERT(sem->sem_count > 0);
	wchan_wakeone(sem->sem_wchan, &sem->sem_lock);

	spinlock_release(&sem->sem_lock);
}

////////////////////////////////////////////////////////////
//
// Lock.

struct lock *lock_create(const char *name) {
	struct lock *lock;

	lock = kmalloc(sizeof(*lock));
    // lock = kmalloc(sizeof(struct lock));
	if (lock == NULL) {
		return NULL;
	}

	lock->lk_name = kstrdup(name);
	if (lock->lk_name == NULL) {
		kfree(lock);
		return NULL;
	}

	HANGMAN_LOCKABLEINIT(&lock->lk_hangman, lock->lk_name);

	spinlock_init(&lock->lk_spinlock);
	lock->lk_thread = NULL;
	
	lock->lk_wchan = wchan_create(lock->lk_name);
	
	return lock;
}

void lock_destroy(struct lock *lock) {
	KASSERT(lock != NULL);
	if(lock->lk_thread != NULL) {
		panic("Attempting to destroy unreleased lock");
	}
	// add stuff here as needed
	wchan_destroy(lock->lk_wchan);
	kfree(lock->lk_name);
	spinlock_cleanup(&lock->lk_spinlock);
	//kfree(lock->lk_spinlock);
	kfree(lock);
}

void lock_acquire(struct lock *lock) {
	/* Call this (atomically) before waiting for a lock */
	HANGMAN_WAIT(&curthread->t_hangman, &lock->lk_hangman);

	// Write this

	// check wait channel
	// check current thread status
	// aquire lock

	spinlock_acquire(&lock->lk_spinlock);
	
	while(lock->lk_thread != NULL) {
		wchan_sleep(lock->lk_wchan, &lock->lk_spinlock);
	}
	lock->lk_thread = curthread;

	spinlock_release(&lock->lk_spinlock);



	// spinlock_acquire(&lock->lk_spinlock);
	// while(!lock_do_i_hold(lock)) {
	// 	if(lock->lk_thread == NULL) {
	// 		lock->lk_thread = curthread;
	// 		spinlock_release(&lock->lk_spinlock);
	// 	}
	// 	wchan_sleep(lock->lk_wchan, &lock->lk_spinlock);
	// }
	// spinlock_release(&lock->lk_spinlock);

	// hint check implementation of P()

	//(void)lock;  // suppress warning until code gets written

	/* Call this (atomically) once the lock is acquired */
	//HANGMAN_ACQUIRE(&curthread->t_hangman, &lock->lk_hangman);

}

void lock_release(struct lock *lock) {
	/* Call this (atomically) when the lock is released */
	HANGMAN_RELEASE(&curthread->t_hangman, &lock->lk_hangman);

	// Write this
	
	// check if u have the lock
	// if yes, then release it

	// check implementation of V()

	if(!lock_do_i_hold(lock)) {
		panic("Attempted to release unaquired lock");
	}
	spinlock_acquire(&lock->lk_spinlock);
	wchan_wakeone(lock->lk_wchan, &lock->lk_spinlock);
	lock->lk_thread = NULL;
	spinlock_release(&lock->lk_spinlock);

}

bool lock_do_i_hold(struct lock *lock) {
    //Write this
    return lock->lk_thread == curthread;
}

////////////////////////////////////////////////////////////
//
// CV


struct cv *
cv_create(const char *name)
{
	struct cv *cv;

	cv = kmalloc(sizeof(*cv));
	if (cv == NULL) {
		return NULL;
	}

	cv->cv_name = kstrdup(name);
	if (cv->cv_name==NULL) {
		kfree(cv);
		return NULL;
	}

	// add stuff here as needed

	return cv;
}

void
cv_destroy(struct cv *cv)
{
	KASSERT(cv != NULL);

	// add stuff here as needed

	kfree(cv->cv_name);
	kfree(cv);
}

void
cv_wait(struct cv *cv, struct lock *lock)
{
	// Write this
	(void)cv;    // suppress warning until code gets written
	(void)lock;  // suppress warning until code gets written
}

void
cv_signal(struct cv *cv, struct lock *lock)
{
	// Write this
	(void)cv;    // suppress warning until code gets written
	(void)lock;  // suppress warning until code gets written
}

void
cv_broadcast(struct cv *cv, struct lock *lock)
{
	// Write this
	(void)cv;    // suppress warning until code gets written
	(void)lock;  // suppress warning until code gets written
}



struct rwlock *rwlock_create(const char * rwlock_name) {
	struct rwlock *lock = kmalloc(sizeof(struct rwlock));
	lock->rwlock_name = kstrdup(rwlock_name);
	lock->mutex = sem_create("rwlock_mutex", 1);
	lock->turnstile = sem_create("rwlock_turnstile", 1);
	lock->roomEmpty = sem_create("rwlock_roomEmpty", 1);
	lock->readers = 0;
	return lock;
}

void rwlock_destroy(struct rwlock *rwlock) {
	sem_destroy(rwlock->roomEmpty);
	sem_destroy(rwlock->turnstile);
	sem_destroy(rwlock->mutex);
	kfree(rwlock->rwlock_name);
	kfree(rwlock);
}

void rwlock_acquire_read(struct rwlock *rwlock) {
	P(rwlock->turnstile);
	V(rwlock->turnstile);
	P(rwlock->mutex);
	rwlock->readers++;
	if(rwlock->readers == 1) {
		P(rwlock->roomEmpty);
	}
	V(rwlock->mutex);
}

void rwlock_release_read(struct rwlock *rwlock) {
	P(rwlock->mutex);
	rwlock->readers--;
	if(rwlock->readers == 0) {
		V(rwlock->roomEmpty);
	}
	V(rwlock->mutex);
}

void rwlock_acquire_write(struct rwlock *rwlock) {
	P(rwlock->turnstile);
	P(rwlock->roomEmpty);
}

void rwlock_release_write(struct rwlock *rwlock) {
	V(rwlock->turnstile);
	V(rwlock->roomEmpty);
}