#include "User.h"

#include <stdio.h>
#include <cstdlib>
#include <cstring>

#include <iostream>

User::User() : theServer(new Connection(INVALID_SOCKET))
{
	WSAData WSAData;
	int result;

	this->authenticated = false;

	result = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (result != 0)
	{
		printf("Failed to load WinSock library! Error %d\n", WSAGetLastError());
	}
}
void User::joinServer(std::string address, std::string port)
{
	struct sockaddr_in server;
	struct hostent* host;
	CHAR serverIp[128];
	int result;

	int portInt = std::stoi(port);
	strcpy_s(serverIp, sizeof(serverIp), address.c_str());

	theServer->_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	server.sin_family = AF_INET;
	server.sin_port = htons(portInt);
	server.sin_addr.s_addr = inet_addr(serverIp);
	if (server.sin_addr.s_addr == INADDR_NONE)
	{
		host = gethostbyname(serverIp);
		CopyMemory(&server.sin_addr, host->h_addr_list[0], host->h_length);
	}

	std::cout << "Connecting to the server\n";
	
	result = connect(theServer->_socket, (struct sockaddr*) & server, sizeof(server));
	if (result == SOCKET_ERROR)
	{
		
		pollfd poll;
		poll.fd = theServer->_socket;
		poll.events = POLLRDNORM | POLLWRNORM;

		int pollResult;
		pollResult = WSAPoll(&poll, 1, 5000);
		if (!pollResult)
		{
			theServer->_socket = INVALID_SOCKET;
			return;
		}

		else
		{
			theServer->_socket = INVALID_SOCKET;
			return;
		}
	}
	ULONG NonBlock = 1;
	if (ioctlsocket(theServer->_socket, FIONBIO, &NonBlock) == SOCKET_ERROR)
	{
		theServer->_socket = INVALID_SOCKET;
		return;
	}
	std::cout << "theServer socket: " << theServer->_socket << std::endl;
}
void User::AuthenticateWeb(MessageType type, std::string email, std::string password)
{

}
void User::CreateAccountWeb(MessageType type, std::string email, std::string password)
{

}

void User::Update()
{
	int result = recv(theServer->_socket, &((char&)(theServer->protobuf[theServer->_numBytes])), 512, 0);

	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return;
		}
		else
		{
			theServer->_socket = INVALID_SOCKET;
			return;
		}
	}
	if (result == 0)
	{
		closesocket(theServer->_socket);
		return;
	}
	theServer->_numBytes += result;
	if (theServer->_numBytes >= 4 && theServer->_length == 0)
	{
		theServer->_length |= theServer->protobuf[0];
		theServer->_length |= theServer->protobuf[1] << 8;
		theServer->_length |= theServer->protobuf[2] << 16;
		theServer->_length |= theServer->protobuf[3] << 24;
	}
	if (theServer->_numBytes == theServer->_length && theServer->_numBytes != 0)
	{
		theServer->_length = 0;
		theServer->_numBytes = 0;

		processMessage(theServer);

		theServer->protobuf.Clear();
	}
}
void User::processMessage(Connection* server)
{
	int packetSize = server->protobuf.readFromBuffer32();
	int messageId = server->protobuf.readFromBuffer32();

	if (messageId == ReceiveMessage)
	{
		int length = server->protobuf.readFromBuffer32();
		std::string name = server->protobuf.readStringFromBuffer(length);
		int messageLength = server->protobuf.readFromBuffer32();
		std::string message = server->protobuf.readStringFromBuffer(messageLength);
		SendMessageToRoom(name, message);
	}
}

void User::SendMessageToRoom(std::string room, std::string message)
{
	printf("\x1B[2K\r[%s] %s\n", room.c_str(), message.c_str());
}

void User::ConfigureMessage(MessageType type, std::string arg1, std::string arg2, std::string message)
{
	theServer->protobuf.Clear();
	theServer->protobuf.writeToBuffer32(0);
	theServer->protobuf.writeToBuffer32(type);

	bool passBuffer = true;

	if (type == JoinRoom)
	{
		if (this->authenticated == false)
		{
			std::cout << "Please log in before using the chat!\n";
			std::cout << "Command to log in is /auth <email> <password>\n";
			passBuffer = false;

		}
		else
		{
			passBuffer = true;
			theServer->protobuf.writeToBuffer32(arg1.length());
			theServer->protobuf.writeStringToBuffer(arg1);
		}
	}

	else if (type == LeaveRoom)
	{
		theServer->protobuf.writeToBuffer32(arg1.length());
		theServer->protobuf.writeStringToBuffer(arg1);
	}

	else if (type == MessageRoom)
	{
		theServer->protobuf.writeToBuffer32(arg1.length());
		theServer->protobuf.writeStringToBuffer(arg1);
		theServer->protobuf.writeToBuffer32(arg2.length());
		theServer->protobuf.writeStringToBuffer(arg2);
	}
	else if (type == AuthUser)
	{
		passBuffer = true;
		std::cout << "Writing information to theServer->protobuf\n";
		theServer->protobuf.writeToBuffer32(arg1.length());
		theServer->protobuf.writeStringToBuffer(arg1);
		theServer->protobuf.writeToBuffer32(arg2.length());
		theServer->protobuf.writeStringToBuffer(arg2);
	}
	else if (type == AddUser)
	{
		passBuffer = true;
		std::cout << "Writing information to theServer->protobuf\n";
		theServer->protobuf.writeToBuffer32(arg1.length());
		theServer->protobuf.writeStringToBuffer(arg1);
		theServer->protobuf.writeToBuffer32(arg2.length());
		theServer->protobuf.writeStringToBuffer(arg2);
	}
	if (passBuffer)
	{
		theServer->protobuf.writeToBufferAtIndex32(theServer->protobuf._writeIndex, 0);
		std::cout << "Sending buffer to theServer\ntheServer socket: " << theServer->_socket << std::endl;
		SendBufferToServer();
	}
}
void User::SendBufferToServer()
{
	std::cout << "Preparing to send buffer to server on socket (theServer->_socket): " << theServer->_socket << std::endl;
	int result = send(theServer->_socket, &(char&)(theServer->protobuf[0]), theServer->protobuf._writeIndex, 0);

	if (result == 0)
	{
		return;
	}
}