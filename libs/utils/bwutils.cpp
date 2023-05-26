#include <bwutils.h>

// TODO: Melhorar a disposicao destas funcoes
#if defined(linux) || defined(__linux__)
void exec_sleep(int sec) {
	struct timeval tv;

	tv.tv_sec = sec;
	tv.tv_usec = 0;

	select(0, NULL, NULL, NULL, &tv);
}

void exec_usleep(int usec) {
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = usec;

	select(0, NULL, NULL, NULL, &tv);
}

int exec_getcurpid(void) {
	return getpid();
}
#else
void exec_sleep(int sec) {
	Sleep(sec * 1000);
}

void exec_usleep(int usec) {
	Sleep(usec);
}

int exec_getcurpid(void) {
	return (int) GetCurrentProcessId();
}

#endif

BWString get_date(const BWString &mask) {
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];
	BWString curdate;
	
	time(&rawtime);

	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, mask.c_str(), timeinfo);

	curdate = buffer;

	return curdate;
}

BWString get_pc_name() {
    char name[4096];

    memset(name, 0, 4096);

#if defined(linux) || defined(__linux__)
    gethostname(name, 4095);
#else
    DWORD len = 4095;

    GetComputerName(name, &len);
#endif

    return name;
}


