#ifndef BWMUTEX_H
#define BWMUTEX_H

#include <bwwindefs.h>
#include <bwerror.h>

class BWMutex {
	public:
		BWMutex();
		~BWMutex();

		void Lock();
		void Unlock();
		bool IsLocked();

	private:
		HANDLE m_mutex;
		bool m_locked;
};

#endif

