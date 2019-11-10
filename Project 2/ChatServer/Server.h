#pragma once

#include <map>
#include <vector>
#include <string>

#include "network_headers.h"

class Server
{
	bool Init(int port);
	bool CheckForNewClient(void);

	void CreateNewUser(SOCKET socket);
	void RemoveUser(int index);
	void RemoveUserFromAllRooms(Connection* conn);

	void MessageFromUser(Connection* client, int index);
	void ProcessMessage(Connection* client);

	void UserJoinRoom(Connection* client, std::string room);
	void UserLeaveRoom(Connection* client, std::string room);
	void SendUserMessageToRoom(Connection* client, std::string room, std::string message);
	void SendUserMessageToRoom(std::string room, std::string message);
	void SendMessageToUser(Connection* client, std::string room, std::string message);

	std::map<std::string, std::vector<Connection*>> _rooms;

	FD_SET _readSet;
	SOCKET _listenSocket;
	SOCKET _acceptSocket;

	Connection* _connectionArray[FD_SETSIZE];
	int _totalSockets;

public:
	Server(void);

	void StartServer(const std::string& port);
	void StopServer(void);

	void Update(void);
};