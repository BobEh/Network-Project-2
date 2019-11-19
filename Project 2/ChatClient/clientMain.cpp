#include "user.h"

#include <conio.h>
#include <cctype>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#define DISABLE_NEWLINE_AUTO_RETURN  0x0008

#define ENTER 13
#define BACKSPACE 8
#define ESCAPE 27

std::string userCommandString;

User theUser;

bool running;

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

std::string sha256(const std::string str)
{
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str.c_str(), str.size());
	SHA256_Final(hash, &sha256);
	std::stringstream ss;
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	}
	return ss.str();
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
	else if (command == "/auth")
	{
		arg2 = sha256(arg2);
		theUser.ConfigureMessage(AuthUser, arg1, arg2, message);
	}
	else if (command == "/adduser")
	{
		arg2 = sha256(arg2);
		std::cout << "Adding a new user.\n";
		theUser.ConfigureMessage(AddUser, arg1, arg2, message);
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

int main(int argc, char** argv)
{
	// Erase the current printed console line in C:
	// https://stackoverflow.com/questions/1508490/erase-the-current-printed-console-line/31405089#31405089
	DWORD l_mode;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(hStdout, &l_mode);
	SetConsoleMode(hStdout, l_mode |
		ENABLE_VIRTUAL_TERMINAL_PROCESSING |
		DISABLE_NEWLINE_AUTO_RETURN);


	printf("Welcome to Super Chat!\n");
	printf("Type /help for a list of commands.\n");
	printf("Connect to the server with /connect 127.0.0.1 4321\n");

	running = true;
	int ch;

	PrintMessage();

	while (running)
	{ 
		if (_kbhit())
		{
			ch = _getch();

			switch (ch)
			{
			/**
			  * If the user presses Enter, this triggers the command recorded in
			  * the message string to be executed. 
			  */
			case ENTER:
				printf("\x1B[2K\r");
				ProcessInput();
				ClearMessage();
				PrintMessage();
				break;

			/*
				If the user presses the DEL key, then remove the last character
				that is in the message, and rewrite the message to the screen.
			*/
			case BACKSPACE:
				if (!userCommandString.empty())
				{
					userCommandString = userCommandString.substr(0, userCommandString.length() - 1);
				}
				PrintMessage();
				break;

			/*
				Add the actual character pressed to the message.
			*/
			default:
				userCommandString.push_back(ch);
				PrintMessage();
				break;
			}

		}

		theUser.Update();
		PrintMessage();
	}

	return 0;
}