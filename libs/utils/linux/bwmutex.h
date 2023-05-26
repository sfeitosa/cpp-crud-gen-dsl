#ifndef BWMUTEX_H
#define BWMUTEX_H

#include <pthread.h>
#include <bwerror.h>

class BWMutex {
	public:
		BWMutex();
		~BWMutex();

		void Lock();
		void Unlock();
		bool IsLocked();

	private:
		pthread_mutex_t m_mutex;
		bool m_locked;
};

#endif

