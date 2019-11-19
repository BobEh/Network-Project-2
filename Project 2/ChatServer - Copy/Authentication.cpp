#include "Authentication.h"

#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include <iostream>

std::string addIdToMessage(int id, const std::string &message)
{
	std::stringstream ss;
	ss << "[" << id << "] ";
	return ss.str() + message;
}

Authentication::Authentication(void) : _listenSocket(INVALID_SOCKET), _acceptSocket(INVALID_SOCKET), _connectionArray()
{
	_totalSockets = 0;
	FD_ZERO(&_readSet);

	WSAData		WSAData;
	int			iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (iResult != 0)
	{
		printf("Failed to load WinSock library! Error %d\n", WSAGetLastError());
	}
}

void Authentication::StopServer(void)
{
	closesocket(_listenSocket);
}

void Authentication::StartAuthenticator(const std::string& portString)
{
	// Convert the provided port as string to an int value
	int port = std::stoi(portString);

	if (Init(port) == false)
	{
		printf("[ERR] Failed to start the server!\n");
		return;
	}

	printf("Successfully started the server. listening on port: %d\n", port);
}

bool Authentication::Init(int port)
{
	SOCKADDR_IN	addr;
	int result;

	_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_listenSocket == INVALID_SOCKET) return false;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	result = bind(_listenSocket, (PSOCKADDR)&addr, sizeof(addr));
	if (result == SOCKET_ERROR) return false;

	result = listen(_listenSocket, 5);
	if (result == SOCKET_ERROR) return false;

	ULONG NonBlock = 1;
	result = ioctlsocket(_listenSocket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR) return false;

	return true;
}

std::vector<std::string> Authentication::Update(void)
{
	std::vector<std::string> emailAndPassword;
	int result;

	FD_ZERO(&_readSet);
	FD_SET(_listenSocket, &_readSet);

	for (unsigned int i = 0; i < _totalSockets; i++)
	{
		FD_SET(_connectionArray[i]->_socket, &_readSet);
	}

	int Total = select(0, &_readSet, NULL, NULL, NULL);

	if (Total == SOCKET_ERROR)
	{
		StopServer();
		return emailAndPassword;
	}

	if (CheckForNewClient())
	{
		Total--;
	}

	//std::cout << "This is about to go into the Update() function for loop\ncurrent socket: " << _connectionArray[0] << std::endl;
	//std::cout << "Total sockets " << _totalSockets << std::endl;
	//std::cout << "Which is socket: " << _connectionArray;
	for (int i = 0; i < _totalSockets; i++)
	{
		Connection* conn = _connectionArray[i];
		//std::cout << "Which is socket: " << conn->_socket << std::endl;
	}

	for (unsigned int i = 0; Total > 0 && i < _totalSockets; i++) {
		// get current socket information from socketarray
		Connection* conn = _connectionArray[i];
		//std::cout << "Current conn socket: " << conn->_socket << std::endl;

		// If this socket is in the ReadSet, read from the connection
		if (FD_ISSET(conn->_socket, &_readSet)) {
			Total--;
			std::cout << "Waiting for message on socket: " << conn->_socket << std::endl;
			std::cout << "Recieved message from server, attempting to read the message...\n";
			emailAndPassword = MessageFromServer(conn, i);
			return emailAndPassword;
		}
	}
	return emailAndPassword;
}

bool Authentication::CheckForNewClient(void)
{
	if (!FD_ISSET(_listenSocket, &_readSet)) return false;

	//std::cout << "listen socket: " << _listenSocket << std::endl;
	_acceptSocket = accept(_listenSocket, NULL, NULL);
	//std::cout << "accept socket: " << _acceptSocket << std::endl;
	if (_acceptSocket == INVALID_SOCKET)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			StopServer();
		}
	}
	else
	{
		ULONG NonBlock = 1;
		int result = ioctlsocket(_acceptSocket, FIONBIO, &NonBlock);
		if (result == SOCKET_ERROR)
		{
			StopServer();
		}
		else
		{
			CreateNewUser(_acceptSocket);
		}
	}

	return true;
}

void Authentication::CreateNewUser(SOCKET socket)
{
	Connection* conn = new Connection(socket);

	_connectionArray[_totalSockets] = conn;
	_totalSockets++;

	printf("[%d] Connected!\n", socket);
	SendMessageToServer(conn, "Master Server", "Welcome to the chat client! Join any room you want");
}

void Authentication::RemoveUser(int index)
{
	Connection* conn = _connectionArray[index];
	printf("[%d] Disconnected!\n", conn->_socket);

	RemoveUserFromAllRooms(conn);
	closesocket(conn->_socket);

	delete conn;

	for (unsigned int i = index; i < _totalSockets; i++)
	{
		_connectionArray[i] = _connectionArray[i + 1];
	}

	_totalSockets--;
}

void Authentication::RemoveUserFromAllRooms(Connection* conn)
{
	std::map<std::string, std::vector<Connection*>>::iterator roomIt;
	std::vector<Connection*>::iterator connIt;
	for (roomIt = _rooms.begin(); roomIt != _rooms.end(); roomIt++)
	{
		connIt = std::find(roomIt->second.begin(), roomIt->second.end(), conn);
		if (connIt != roomIt->second.end())
		{
			roomIt->second.erase(connIt);
		}
	}
}

std::vector<std::string> Authentication::MessageFromServer(Connection* conn, int index)
{
	//std::cout << "Inside MessageFromServer() function...\n";
	std::vector<std::string> emailAndPassword;
	int result = recv(conn->_socket, &((char&)(conn->protobuf[conn->_numBytes])), 512, 0);

	conn->_numBytes += result;
	if (conn->_numBytes >= 4 && conn->_length == 0)
	{
		//std::cout << "\tInside first if statement...\n";
		conn->_length |= conn->protobuf[0];
		conn->_length |= conn->protobuf[1] << 8;
		conn->_length |= conn->protobuf[2] << 16;
		conn->_length |= conn->protobuf[3] << 24;
		emailAndPassword = ProcessMessage(conn);
		conn->protobuf.Clear();
	}

	if (conn->_numBytes == conn->_length && conn->_numBytes != 0)
	{
		//std::cout << "\tInside second if statement...\n";
		conn->_length = 0;
		conn->_numBytes = 0;

		emailAndPassword = ProcessMessage(conn);
		conn->protobuf.Clear();
	}
	return emailAndPassword;
}

int Authentication::getId()
{
	return _connectionArray[0]->_socket;
}

std::vector<std::string> Authentication::ProcessMessage(Connection* conn)
{
	//std::cout << "Inside MessageFromServer() function...\n";
	std::vector<std::string> emailAndPassword;
	int packetSize = conn->protobuf.readFromBuffer32();
	//std::cout << "\tpacketSize: " << packetSize << std::endl;
	int messageId = conn->protobuf.readFromBuffer32();
	//std::cout << "\tmessageId: " << messageId << std::endl;

	switch (messageId)
	{
		case MessageType::AuthUser:
		{
			//std::cout << "\tInside AuthUser case...\n";
			emailAndPassword.push_back("AuthUser");
			int emailLength = conn->protobuf.readFromBuffer32();
			std::string email = conn->protobuf.readStringFromBuffer(emailLength);
			emailAndPassword.push_back(email);
			int passwordLength = conn->protobuf.readFromBuffer32();
			std::string password = conn->protobuf.readStringFromBuffer(passwordLength);
			emailAndPassword.push_back(password);



			//std::cout << "Recieved data:\n";
			//std::cout << "Data recieved is: " << email << ", " << password << std::endl;
			for (int i = 0; i < emailAndPassword.size(); i++)
			{
				std::cout << emailAndPassword.at(i) << std::endl;
			}

			break;
		}
		case MessageType::AddUser:
		{
			emailAndPassword.push_back("AddUser");
			int emailLength = conn->protobuf.readFromBuffer32();
			std::string email = conn->protobuf.readStringFromBuffer(emailLength);
			emailAndPassword.push_back(email);
			int passwordLength = conn->protobuf.readFromBuffer32();
			std::string password = conn->protobuf.readStringFromBuffer(passwordLength);
			emailAndPassword.push_back(password);

			//std::cout << "Recieved data:\n";
			for (int i = 0; i < emailAndPassword.size(); i++)
			{
				std::cout << emailAndPassword.at(i) << std::endl;
			}

			break;
		}
	}
	return emailAndPassword;
}

void Authentication::ServerJoinRoom(Connection* conn, std::string room)
{
	std::vector<Connection*>::iterator it = find(_rooms[room].begin(), _rooms[room].end(), conn);
	if (it != _rooms[room].end())
	{
		SendMessageToServer(conn, room, "Already in that room.");
		return;
	}

	SendMessageToServer(conn, room, "Successfully joined the room!");
	_rooms[room].push_back(conn);
}

void Authentication::SendMessageToServer(Connection* conn, std::string roomName, std::string message)
{
	int roomNameLength = roomName.length();
	int messageLength = message.length();
	int packetSize = HEADER_SIZE + sizeof(uint32_t) + roomNameLength + sizeof(uint32_t) + messageLength;

	conn->protobuf.Clear();
	conn->protobuf.writeToBuffer32(packetSize);
	conn->protobuf.writeToBuffer32(MessageType::ReceiveMessage);
	conn->protobuf.writeToBuffer32(roomName.length());
	conn->protobuf.writeStringToBuffer(roomName);
	conn->protobuf.writeToBuffer32(message.length());
	conn->protobuf.writeStringToBuffer(message);

	int result = send(conn->_socket, &((char&)conn->protobuf[0]), conn->protobuf._writeIndex, 0);

	if (result == 0)
	{
		printf("Result is 0");
	}
}