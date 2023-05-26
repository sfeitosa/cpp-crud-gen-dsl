#ifndef BWTHREAD_H
#define BWTHREAD_H

#include <pthread.h>
#include <bwerror.h>
#include <bwabstractthread.h>

class BWThread: public BWAbstractThread {
	public:
		BWThread();
		virtual ~BWThread();

		void Start();
		void Wait();
		void Abort();
		void Detach();

		unsigned long int GetId();

	protected:
		static void *entryPoint(void *pthis);
		virtual void run() = 0;

	private:
		unsigned long int m_thread_id;
};

#endif

