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

Server::Server(void) : _listenSocket(INVALID_SOCKET), _acceptSocket(INVALID_SOCKET), _connectionArray()
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

void Server::StopServer(void)
{
	closesocket(_listenSocket);
}

void Server::StartServer(const std::string& portString)
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

bool Server::Init(int port)
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
void Server::Update(void)
{
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
		return;
	}

	if (CheckForNewClient())
	{
		Total--;
	}

	for (unsigned int i = 0; Total > 0 && i < _totalSockets; i++) {
		// get current socket information from socketarray
		Connection* conn = _connectionArray[i];

		// If this socket is in the ReadSet, read from the connection
		if (FD_ISSET(conn->_socket, &_readSet)) {
			Total--;
			MessageFromUser(conn, i);
		}
	}
}

bool Server::CheckForNewClient(void)
{
	if (!FD_ISSET(_listenSocket, &_readSet)) return false;

	_acceptSocket = accept(_listenSocket, NULL, NULL);
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

void Server::CreateNewUser(SOCKET socket)
{
	Connection* conn = new Connection(socket);

	_connectionArray[_totalSockets] = conn;
	_totalSockets++;

	printf("[%d] Connected!\n", socket);
	SendMessageToUser(conn, "Master Server", "Welcome to the chat client! Join any room you want");
}

void Server::RemoveUser(int index)
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

void Server::RemoveUserFromAllRooms(Connection* conn)
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

void Server::MessageFromUser(Connection* conn, int index)
{
	int result = recv(conn->_socket, &((char&)(conn->protobuf[conn->_numBytes])), 512, 0);

	if (result == SOCKET_ERROR)
	{
		int WSAErrorCode = WSAGetLastError();

		if (WSAErrorCode != WSAEWOULDBLOCK)
		{
			RemoveUser(index);
			return;
		}

		if (WSAErrorCode == WSAEHOSTUNREACH)
		{
			printf("Disconnected from client...\n");
		}
		return;
	}

	if (result == 0)
	{
		RemoveUser(index);
		return;
	}

	conn->_numBytes += result;
	if (conn->_numBytes >= 4 && conn->_length == 0)
	{
		conn->_length |= conn->protobuf[0];
		conn->_length |= conn->protobuf[1] << 8;
		conn->_length |= conn->protobuf[2] << 16;
		conn->_length |= conn->protobuf[3] << 24;
	}

	if (conn->_numBytes == conn->_length && conn->_numBytes != 0)
	{
		conn->_length = 0;
		conn->_numBytes = 0;

		ProcessMessage(conn);
		conn->protobuf.Clear();
	}
}

void Server::ProcessMessage(Connection* conn)
{
	int packetSize = conn->protobuf.readFromBuffer32();
	int messageId = conn->protobuf.readFromBuffer32();

	switch (messageId)
	{
		case MessageType::JoinRoom:
		{
			int roomLength = conn->protobuf.readFromBuffer32();
			std::string roomName = conn->protobuf.readStringFromBuffer(roomLength);
			UserJoinRoom(conn, roomName);
			break;

		}

		case MessageType::LeaveRoom:
		{
			int roomLength = conn->protobuf.readFromBuffer32();
			std::string roomName = conn->protobuf.readStringFromBuffer(roomLength);
			UserLeaveRoom(conn, roomName);
			break;
		}

		case  MessageType::MessageRoom:
		{
			int roomLength = conn->protobuf.readFromBuffer32();
			std::string roomName = conn->protobuf.readStringFromBuffer(roomLength);
			int messageLength = conn->protobuf.readFromBuffer32();
			std::string userMessage = conn->protobuf.readStringFromBuffer(messageLength);
			SendUserMessageToRoom(conn, roomName, addIdToMessage(conn->_socket, userMessage));
			break;
		}
	}
}

void Server::UserJoinRoom(Connection* conn, std::string room)
{
	std::vector<Connection*>::iterator it = find(_rooms[room].begin(), _rooms[room].end(), conn);
	if (it != _rooms[room].end())
	{
		SendMessageToUser(conn, room, "Already in that room.");
		return;
	}

	SendUserMessageToRoom(room, addIdToMessage(conn->_socket, " joined the room"));
	SendMessageToUser(conn, room, "Successfully joined the room!");
	_rooms[room].push_back(conn);
}

void Server::UserLeaveRoom(Connection* conn, std::string room)
{
	if (_rooms.find(room) == _rooms.end())
	{
		SendMessageToUser(conn, room, "You can only leave rooms that your are in.");
		return;
	}

	std::vector<Connection*>::iterator it = find(_rooms[room].begin(), _rooms[room].end(), conn);
	if (it == _rooms[room].end())
	{
		SendMessageToUser(conn, room, "You can only leave rooms that your are in.");
		return;
	}

	SendMessageToUser(conn, room, "You have left the room!");
	_rooms[room].erase(it);
	SendUserMessageToRoom(room, addIdToMessage(conn->_socket, "has left the room"));
}

void Server::SendUserMessageToRoom(Connection* conn, std::string room, std::string message)
{
	if (_rooms.find(room) == _rooms.end())
	{
		SendMessageToUser(conn, room, "Please join that room before sending this message.");
		return;
	}

	SendUserMessageToRoom(room, message.c_str());
}

void Server::SendUserMessageToRoom(std::string room, std::string message)
{
	if (_rooms.find(room) == _rooms.end())
	{
		printf("[ERR] Room '%s' does not exist!\n", room.c_str());
		return;
	}

	printf("[BROADCAST] [%s] %s\n", room.c_str(), message.c_str());

	std::vector<Connection*> clientsInRoom = _rooms[room];
	for (std::vector<Connection*>::iterator it = clientsInRoom.begin(); it != clientsInRoom.end(); it++)
	{
		SendMessageToUser(*it, room, message);
	}
}

void Server::SendMessageToUser(Connection* conn, std::string roomName, std::string message)
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