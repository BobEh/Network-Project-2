#include "Server.h"

#include <conio.h>

int main(int argc, char** argv)
{
	Server theServer;
	theServer.StartServer("5150");

	while (true)
	{
		if (_kbhit())
		{
			int ch = _getch();
			if (ch == 27) break;
		}

		theServer.Update();
	}

	system("Pause");

	return 0;
}