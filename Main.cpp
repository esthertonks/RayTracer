#include "Application.h"

int main(int argc, char** args) {

	Application application;
	if (!application.Initialise())
		return 0;

	while (application.Run())
	{
	}

	return 0;
}
