#include "Authentication.h"

#include <conio.h>

int main(int argc, char** argv)
{
	Authentication theAuthenticator;
	theAuthenticator.StartServer("5150");

	while (true)
	{
		if (_kbhit())
		{
			int ch = _getch();
			if (ch == 27) break;
		}

		theAuthenticator.Update();
	}

	system("Pause");

	return 0;
}