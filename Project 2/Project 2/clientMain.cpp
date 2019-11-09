#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#define DISABLE_NEWLINE_AUTO_RETURN  0x0008

#define ENTER 13
#define BACKSPACE 8
#define ESCAPE 27

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Buffer.h"
#include "Connection.h"
#include "User.h"
#include "MessageTypes.h"

#include <conio.h>

bool isOpen;

std::string userCommandString;

User theUser;

void ParseInput(std::string& command, std::string& arg1, std::string& arg2, std::string& message)
{
	std::string word = "";
	std::string lowerWord;
	int inputNumber = 1;
	for (auto x : userCommandString)
	{

		char lowerChar = tolower(x);

		if (inputNumber < 4 && x == ' ')
		{
			inputNumber++;
			if (inputNumber != 4)
			{
				continue;
			}
		}
		if (inputNumber == 1)
		{
			command.push_back(lowerChar);
		}
		else if (inputNumber == 2)
		{
			arg1.push_back(lowerChar);

		}
		else if (inputNumber == 3)
		{
			arg2.push_back(lowerChar);
		}
		if (inputNumber == 4)
		{
			message.push_back(x);
		}
		
		if (x == ' ')
		{

		}
	}
}

void ProcessInput()
{
	std::string command;
	std::string arg1;
	std::string arg2;
	std::string message;

	ParseInput(command, arg1, arg2, message);

	if (command == "/connect")
	{
		theUser.joinServer(arg1, arg2);

	}
	else if (command == "/join")
	{
		theUser.ConfigureMessage(JoinRoom, arg1, arg2, message);
	}
	else if (command == "/send")
	{
		theUser.ConfigureMessage(MessageRoom, arg1, arg2, message);
	}
}

void ClearMessage()
{
	userCommandString = "";
}

void PrintMessage()
{
	printf("\x1B[2K\r$ %s", userCommandString.c_str());
}

int main()
{
	// Erase the current printed console line in C:
	// https://stackoverflow.com/questions/1508490/erase-the-current-printed-console-line/31405089#31405089
	DWORD l_mode;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(hStdout, &l_mode);
	SetConsoleMode(hStdout, l_mode |
		ENABLE_VIRTUAL_TERMINAL_PROCESSING |
		DISABLE_NEWLINE_AUTO_RETURN);

	std::cout << "Please type your command (/connect 127.0.0.1 5150 to connect to server): ";

	isOpen = true;
	int ch;

	PrintMessage();

	while (isOpen)
	{
		if (_kbhit())
		{
			ch = _getch();

			if (ch == ENTER)
			{
				ProcessInput();
				ClearMessage();
				PrintMessage();
			}
			else if (ch == BACKSPACE)
			{
				if (!userCommandString.empty())
				{
					userCommandString = userCommandString.substr(0, userCommandString.length() - 1);
					PrintMessage();
				}
			}
			else
			{
				userCommandString.push_back(ch);
				PrintMessage();
			}
		}

		theUser.Update();
		PrintMessage();
	}
}