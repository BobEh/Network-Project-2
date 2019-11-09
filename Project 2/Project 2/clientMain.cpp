#include <conio.h>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#define DISABLE_NEWLINE_AUTO_RETURN  0x0008

#define ENTER 13
#define BACKSPACE 8
#define ESCAPE 27

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <WinSock2.h>

bool isOpen;

std::string userCommandString;
User user;

class Buffer
{
	uint8_t _readIndex;
	uint8_t _writeIndex;
	std::vector<uint8_t> _buffer;
	Buffer(int size)
	{
		_buffer.resize(size);
	}
	void writeToBuffer(uint32_t input)
	{
		_buffer.at(_writeIndex) = input;
		_buffer.at(_writeIndex + 1) = input >> 8;
		_buffer.at(_writeIndex + 2) = input >> 16;
		_buffer.at(_writeIndex + 3) = input >> 24;

		_writeIndex += 4;
	}
	void writeToBufferAtIndex(uint32_t input, uint8_t index)
	{

		_buffer[index] = input;
		_buffer[index + 1] = input >> 8;
		_buffer[index + 2] = input >> 16;
		_buffer[index + 3] = input >> 24;
	}
	void writeStringToBuffer(std::string input)
	{
		memcpy(&(_buffer.at(_writeIndex)), &(input.at(0)), input.length());

		int stringLength = input.length();
		_writeIndex += stringLength;
	}
	void writeStringToBufferAtIndes(std::string input, uint8_t index)
	{
		memcpy(&(_buffer.at(index)), &(input.at(0)), input.length());
	}
	uint32_t readFromBuffer()
	{
		uint32_t output = 0;

		output |= _buffer.at(_readIndex);
		output |= _buffer.at(_readIndex + 1) << 8;
		output |= _buffer.at(_readIndex + 2) << 16;
		output |= _buffer.at(_readIndex + 3) << 24;

		_readIndex += 4;

		return output;
	}
};

class Connection
{
	SOCKET _socket;
	int _numBytes;
	int _length;

	Connection(SOCKET socket) : protobuf(Buffer(512)), _numBytes(0), _length(0), _socket(socket)
	{
		_numBytes = 0;
		_length = 0;

	}
};

class User
{
	User()
	{
		WSAData WSAData;
		int result;

		result = WSAStartup(MAKEWORD(2, 2), &WSAData);
	}
	void joinServer(std::string address, std::string port)
	{
		struct sockaddr_in server;
		struct hostent* host;
		CHAR serverIp[128];
		int result;

		int portInt = std::stoi(port);
		strcpy_s(serverIp, sizeof(serverIp), address.c_str());

		
	}
};

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

	}
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

	while (isOpen)
	{
		if (_kbhit())
		{
			ch = _getch();

			if (ch == ENTER)
			{
				ProcessInput();
			}
		}
	}
}