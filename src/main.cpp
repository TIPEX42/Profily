#include <iostream>
#include <stdio.h>
#include "Application.h"

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cout << "The name of the program to profile must be given as the first argument" << std::endl;
		exit(1);
	}

	Application::Start(300, 300, argv[1]);
}