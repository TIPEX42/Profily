#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

void __cyg_profile_func_enter(void *this_func, void *call_site)
__attribute__((no_instrument_function));
void __cyg_profile_func_exit(void *this_func, void *call_site)
__attribute__((no_instrument_function));

void __cyg_profile_func_enter(void *this_func, void *call_site)
{
	struct timeval time;
	gettimeofday(&time, NULL);
	printf("> %p %ld %d\n", this_func, time.tv_sec - 1653000000, time.tv_usec);
}

void __cyg_profile_func_exit(void *this_func, void *call_site)
{
	struct timeval time;
	gettimeofday(&time, NULL);
	printf("< %p %ld %d\n", this_func, time.tv_sec - 1653000000, time.tv_usec);
}

void f1()
{
	sleep(1);
}

void f2()
{
	f1();
	sleep(4);
	f1();
}

int main()
{
	f1();
	f2();
}
