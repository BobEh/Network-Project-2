#pragma once

#include <map>
#include <vector>
#include <string>

#include "network_headers.h"

class Authentication
{
public:
	bool Init(int port);
	bool CheckForNewClient(void);

	void CreateNewUser(SOCKET socket);
	void RemoveUser(int index);
	void RemoveUserFromAllRooms(Connection* conn);

	void AuthenticateWeb(std::string email, std::string password);
	void CreateAccountWeb(std::string email, std::string password);

	int getId();

	std::vector<std::string> MessageFromServer(Connection* client, int index);
	std::vector<std::string> ProcessMessage(Connection* client);

	void ServerJoinRoom(Connection* client, std::string room);
	void SendMessageToServer(Connection* conn, std::string roomName, std::string message);

	std::map<std::string, std::vector<Connection*>> _rooms;

	FD_SET _readSet;
	SOCKET _listenSocket;
	SOCKET _acceptSocket;

	Connection* _connectionArray[FD_SETSIZE];
	int _totalSockets;

	//void authSuccess();
	//void addSuccess();
	//void authFailed();
	//void addFailed();

	Authentication(void);

	void StartAuthenticator(const std::string& port);
	void StopServer(void);

	std::vector<std::string> Update(void);
};