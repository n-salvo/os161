/*
 * All the contents of this file are overwritten during automated
 * testing. Please consider this before changing anything in this file.
 */

#include <types.h>
#include <lib.h>
#include <clock.h>
#include <thread.h>
#include <synch.h>
#include <test.h>
#include <kern/test161.h>
#include <spinlock.h>

#define PROBLEMS_MAX_YIELDER 16
#define PROBLEMS_MAX_SPINNER 8192

struct package {
	void *object1;
	void *object2;
};

/*
 * Use these stubs to test your reader-writer locks.
 */

int rwtest(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf("Test rwt1: creating and destroying 100 locks\n");

	int rwlockCount = 100;
	struct rwlock **rwlocks = kmalloc(sizeof(struct rwlock *) * rwlockCount);
	KASSERT(rwlocks != NULL);
	int i;
	for(i = 0; i < rwlockCount; i++) {
		rwlocks[i] = rwlock_create("lock " + i);
		if(rwlocks[i] == NULL) {
			success(TEST161_FAIL, SECRET, "failed to create lock " + i);
		}
		kprintf("lock %d created\n", i);
	}

	for(i = 0; i < rwlockCount; i++) {
		rwlocks[i] = rwlock_create("lock " + i);
		kprintf("lock %d destroyed\n", i);
	}
	
	kfree(rwlocks);

	success(TEST161_SUCCESS, SECRET, "rwt1");
	return 0;


	// kprintf_n("rwt2 unimplemented\n");
	// success(TEST161_FAIL, SECRET, "rwt2");

	// return 0;
}

int rwtest2(int nargs, char **args) {
	(void)nargs;
	(void)args;

	
	struct rwlock *rwlock = rwlock_create("rwtest2lock");
	struct semaphore *sem = sem_create("rwtest2_threadWatcher", 0);

	struct package *package = kmalloc(sizeof(struct package));
	package->object1 = (void*)rwlock;
	package->object2 = (void*)sem;

	int i, result, threads;
	threads = 40;
	for(i = 0; i < 10; i++) {
		result = thread_fork("rwtest2", NULL, test_simulated_write, (void*)package, (unsigned long)i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for(i = 10; i < 20; i++) {
		result = thread_fork("rwtest2", NULL, test_simulated_read, (void*)package, (unsigned long)i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for(i = 20; i < 30; i++) {
		result = thread_fork("rwtest2", NULL, test_simulated_write, (void*)package, (unsigned long)i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for(i = 30; i < 40; i++) {
		result = thread_fork("rwtest2", NULL, test_simulated_read, (void*)package, (unsigned long)i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n", strerror(result));
		}
	}

	//wait till all threads are done
	while(threads) {
		P(sem);
		threads--;
	}

	rwlock_destroy(rwlock);
	kfree(package);
	return 0;
}

int rwtest3(int nargs, char **args) {
	(void)nargs;
	(void)args;

	
	struct rwlock *rwlock = rwlock_create("rwtest3lock");
	struct semaphore *sem = sem_create("rwtest3_threadWatcher", 0);

	struct package *package = kmalloc(sizeof(struct package));
	package->object1 = (void*)rwlock;
	package->object2 = (void*)sem;

	int i, result, threads;
	threads = 500;
	for(i = 0; i < 200; i++) {
		result = thread_fork("rwtest3", NULL, test_simulated_write, (void*)package, (unsigned long)i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for(i = 200; i < 250; i++) {
		result = thread_fork("rwtest3", NULL, test_simulated_read, (void*)package, (unsigned long)i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for(i = 250; i < 350; i++) {
		result = thread_fork("rwtest3", NULL, test_simulated_write, (void*)package, (unsigned long)i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for(i = 350; i < 400; i++) {
		result = thread_fork("rwtest3", NULL, test_simulated_read, (void*)package, (unsigned long)i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for(i = 400; i < 500; i++) {
		result = thread_fork("rwtest3", NULL, test_simulated_write, (void*)package, (unsigned long)i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n", strerror(result));
		}
	}
	
	//wait till all threads are done
	while(threads) {
		P(sem);
		threads--;
	}

	rwlock_destroy(rwlock);
	kfree(package);

	kprintf_n("rwt3 write-heavy test completed\n");
	return 0;
}

int rwtest4(int nargs, char **args) {
	(void)nargs;
	(void)args;

	
	struct rwlock *rwlock = rwlock_create("rwtest4lock");
	struct semaphore *sem = sem_create("rwtest4_threadWatcher", 0);

	struct package *package = kmalloc(sizeof(struct package));
	package->object1 = (void*)rwlock;
	package->object2 = (void*)sem;

	int i, result, threads;
	threads = 500;
	for(i = 0; i < 200; i++) {
		result = thread_fork("rwtest4", NULL, test_simulated_read, (void*)package, (unsigned long)i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for(i = 200; i < 250; i++) {
		result = thread_fork("rwtest4", NULL, test_simulated_write, (void*)package, (unsigned long)i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for(i = 250; i < 350; i++) {
		result = thread_fork("rwtest4", NULL, test_simulated_read, (void*)package, (unsigned long)i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for(i = 350; i < 400; i++) {
		result = thread_fork("rwtest4", NULL, test_simulated_write, (void*)package, (unsigned long)i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for(i = 400; i < 500; i++) {
		result = thread_fork("rwtest4", NULL, test_simulated_read, (void*)package, (unsigned long)i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n", strerror(result));
		}
	}
	
	//wait till all threads are done
	while(threads) {
		P(sem);
		threads--;
	}

	rwlock_destroy(rwlock);
	kfree(package);

	kprintf_n("rwt4 read-heavy test completed\n");
	return 0;
}

int rwtest5(int nargs, char **args) {
	(void)nargs;
	(void)args;

	
	struct rwlock *rwlock = rwlock_create("rwtest4lock");
	struct semaphore *sem = sem_create("rwtest4_threadWatcher", 0);

	struct package *package = kmalloc(sizeof(struct package));
	package->object1 = (void*)rwlock;
	package->object2 = (void*)sem;

	int i, j, result, threads;
	threads = 900;
	for(i = 0; i < threads;) {
		for(j = 0; j < 25 && i < threads; j++) {
			result = thread_fork("rwtest4", NULL, test_simulated_read, (void*)package, (unsigned long)i);
			if (result) {
				panic("sem1: thread_fork failed: %s\n", strerror(result));
			}
			i++;
		}
		for(j = 0; j < 25 && i < threads; j++) {
			result = thread_fork("rwtest4", NULL, test_simulated_write, (void*)package, (unsigned long)i);
			if (result) {
				panic("sem1: thread_fork failed: %s\n", strerror(result));
			}
			i++;
		}
	}
	
	//wait till all threads are done
	while(threads) {
		P(sem);
		threads--;
	}

	rwlock_destroy(rwlock);
	kfree(package);

	kprintf_n("rwt5 thread-heavy test completed\n");
	return 0;
}


void test_simulated_read(void *package, unsigned long value) {
	struct package *pkg = (struct package*)package;
	struct rwlock *lock = (struct rwlock*)pkg->object1;
	struct semaphore *sem = (struct semaphore*)pkg->object2;
	random_yielder(PROBLEMS_MAX_YIELDER);
	rwlock_acquire_read(lock);
	kprintf("thread %lu starting read\n", value);
	random_yielder(PROBLEMS_MAX_YIELDER);
	random_spinner(PROBLEMS_MAX_SPINNER);
	kprintf("thread %lu reading\n", value);
	thread_yield();
	kprintf("thread %lu finishing read\n", value);
	rwlock_release_read(lock);
	V(sem);
}

void test_simulated_write(void *package, unsigned long value) {
	struct package *pkg = (struct package*)package;
	struct rwlock *lock = (struct rwlock*)pkg->object1;
	struct semaphore *sem = (struct semaphore*)pkg->object2;
	random_yielder(PROBLEMS_MAX_YIELDER);
	rwlock_acquire_write(lock);
	kprintf("thread %lu starting write\n", value);
	random_yielder(PROBLEMS_MAX_YIELDER);
	random_spinner(PROBLEMS_MAX_SPINNER);
	thread_yield();
	thread_yield();
	thread_yield();
	kprintf("thread %lu writing\n", value);
	thread_yield();
	thread_yield();
	thread_yield();
	kprintf("thread %lu finishing write\n", value);
	rwlock_release_write(lock);
	V(sem);
}

// void test_simulated_read(void *rwlock, unsigned long value) {
// 	struct rwlock *lock = (struct rwlock*)rwlock;
// 	rwlock_acquire_read(lock);
// 	kprintf("thread %lu starting read\n", value);
// 	random_yielder(PROBLEMS_MAX_YIELDER);
// 	random_spinner(PROBLEMS_MAX_SPINNER);
// 	kprintf("thread %lu reading\n", value);
// 	// thread_yield();
// 	kprintf("thread %lu finishing read\n", value);
// 	rwlock_release_read(lock);
// }

// void test_simulated_write(void *rwlock, unsigned long value) {
// 	struct rwlock *lock = (struct rwlock*)rwlock;
// 	rwlock_acquire_write(lock);
// 	kprintf("thread %lu starting write\n", value);
// 	random_yielder(PROBLEMS_MAX_YIELDER);
// 	random_spinner(PROBLEMS_MAX_SPINNER);
// 	// thread_yield();
// 	// thread_yield();
// 	// thread_yield();
// 	kprintf("thread %lu writing\n", value);
// 	// thread_yield();
// 	// thread_yield();
// 	// thread_yield();
// 	kprintf("thread %lu finishing write\n", value);
// 	rwlock_release_write(lock);
// }