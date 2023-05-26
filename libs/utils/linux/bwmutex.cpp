#include <bwmutex.h>

/**
\brief Construtor.
*/
BWMutex::BWMutex() {
	int rc;

	m_locked = false;
	
	rc = pthread_mutex_init(&m_mutex, NULL);

	if (rc) {
		throw BWError(BASE_ERROR, "Erro ao iniciar mutex");
	}
}

/**
\brief Destrutor.
*/
BWMutex::~BWMutex() {
	pthread_mutex_destroy(&m_mutex);
}

/**
\brief Bloqueia uma sessao critica.
*/
void BWMutex::Lock() {
	int rc;

	rc = pthread_mutex_lock(&m_mutex);

	if (rc) {
		throw BWError(BASE_ERROR, "Erro ao obter lock da thread");
	}

	m_locked = true;
}

/**
\brief Desbloqueia uma sessao critica.
*/
void BWMutex::Unlock() {
	int rc;

	rc = pthread_mutex_unlock(&m_mutex);

	if (rc) {
		throw BWError(BASE_ERROR, "Erro ao liberar lock da thread");
	}

	m_locked = false;
}

/**
\brief Verifica se uma thread esta bloqueada
*/
bool BWMutex::IsLocked() {
	return m_locked;
}
