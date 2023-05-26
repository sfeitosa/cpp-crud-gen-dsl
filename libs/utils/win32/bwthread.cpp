#include <bwthread.h>

/**
\brief Construtor.
*/
BWThread::BWThread(): BWAbstractThread() {
}

/**
\brief Destrutor.
*/
BWThread::~BWThread() {
}


/**
\brief Metodo estatico de callback invocado pela funcao pthread_create.
\param Ponteiro da instancia da classe de thread.
*/
DWORD WINAPI BWThread::entryPoint(void *pthis) {
	BWThread *ptr = static_cast<BWThread *>(pthis);
	ptr->run();

	return 0;
}

/**
\brief Inicia uma nova thread.
*/
void BWThread::Start() {
	m_thread_handler = CreateThread(NULL, 0, &entryPoint, this, 0, &m_thread_id);
}

/**
\brief Espera ate a finalizacao da thread.
*/
void BWThread::Wait() {
	WaitForSingleObject(m_thread_handler, INFINITE);
}

/**
\brief Envia um sinal solicitando o cancelamento de uma thread em execucao.
*/
void BWThread::Abort() {
	TerminateThread(m_thread_handler, 0);
}

/**
\brief Marca a thread para desalocar seus recursos assim que a mesma termine.
*/
void BWThread::Detach() {
	ExitThread(0);
}

/**
\brief Retorna o ID da thread.
*/
unsigned long int BWThread::GetId() {
	return m_thread_id;
}

HANDLE BWThread::GetHandler() {
	return m_thread_handler;
}

