#include "Server.h"

#include <conio.h>

int main(int argc, char** argv)
{
	Server theServer;
	theServer.StartServer("4321");

	theServer.joinAuthenticator("127.0.0.1", "4567");

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
}