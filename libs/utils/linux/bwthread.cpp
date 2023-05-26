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
void *BWThread::entryPoint(void *pthis) {
	BWThread *ptr = static_cast<BWThread *>(pthis);
	ptr->run();

	return pthis;
}

/**
\brief Inicia uma nova thread.
*/
void BWThread::Start() {
	if (pthread_create(&m_thread_id, NULL, entryPoint, this) != 0) {
		// throw an error
		throw BWError(BASE_ERROR, "Erro ao criar nova thread");
	}
}

/**
\brief Espera ate a finalizacao da thread.
*/
void BWThread::Wait() {
    if (pthread_join(m_thread_id, NULL) != 0) {
        throw BWError(BASE_ERROR, "Impossivel fazer \"join\"");
    }
}

/**
\brief Envia um sinal solicitando o cancelamento de uma thread em execucao.
*/
void BWThread::Abort() {
    if (pthread_cancel(m_thread_id) != 0) {
        throw BWError(BASE_ERROR, "Impossivel enviar sinal de cancelamento para a thread");
    }
}

/**
\brief Marca a thread para desalocar seus recursos assim que a mesma termine.
*/
void BWThread::Detach() {
    if (pthread_detach(m_thread_id) != 0) {
        throw BWError(BASE_ERROR, "Impossivel desassociar recursos desta thread");
    }
}

/**
\brief Retorna o ID da thread.
*/
unsigned long int BWThread::GetId() {
	return m_thread_id;	
}

