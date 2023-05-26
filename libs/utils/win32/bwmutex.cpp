#include <bwmutex.h>

/**
\brief Construtor.
*/
BWMutex::BWMutex() {
	m_locked = false;
	
	m_mutex = CreateMutex(NULL, false, NULL);

	if (!m_mutex) {
		throw BWError(BASE_ERROR, "Erro ao iniciar mutex");
	}
}

/**
\brief Destrutor.
*/
BWMutex::~BWMutex() {
	if (IsLocked()) {
		Unlock();
	}
	
	CloseHandle(m_mutex);
}

/**
\brief Bloqueia uma sessao critica.
*/
void BWMutex::Lock() {
	DWORD rc;
	
	rc = WaitForSingleObject(m_mutex, INFINITE);

	if (rc == WAIT_FAILED) {
		throw BWError(BASE_ERROR, "Erro ao obter lock da thread");
	}

	m_locked = true;
}

/**
\brief Desbloqueia uma sessao critica.
*/
void BWMutex::Unlock() {
	if (!ReleaseMutex(m_mutex)) {
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
