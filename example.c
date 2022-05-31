#include <unistd.h>

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
