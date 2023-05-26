#ifndef BWABSTRACTTHREAD_H
#define BWABSTRACTTHREAD_H

#include <bwerror.h>

class BWAbstractThread {
	public:
		BWAbstractThread();
		virtual ~BWAbstractThread();

		virtual void Start() = 0;
		virtual void Wait() = 0;
		virtual void Abort() = 0;
		virtual void Detach() = 0;

		virtual unsigned long int GetId() = 0;

	protected:
		virtual void run() = 0;
};

#endif // BWABSTRACTTHREAD_H

