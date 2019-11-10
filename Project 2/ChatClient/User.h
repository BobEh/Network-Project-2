#pragma once

#include <map>
#include <vector>
#include <string>

#include "network_headers.h"

class User
{
public:
	User();

	void joinServer(std::string address, std::string port);
	void Authenticate(std::string email, std::string password);
	void Update();

	void ConfigureMessage(MessageType type, std::string arg1, std::string arg2, std::string message);
	void SendBufferToServer(void);

private:
	void processMessage(Connection* server);
	void SendMessageToRoom(std::string room, std::string message);

	Connection* theServer;
};