#pragma once

#include <map>
#include <vector>
#include <string>

#include <openssl/sha.h>

#include "network_headers.h"

class User
{
public:
	User();

	void joinServer(std::string address, std::string port);
	void AuthenticateWeb(MessageType type, std::string email, std::string password);
	void CreateAccountWeb(MessageType type, std::string email, std::string password);
	void Update();

	void ConfigureMessage(MessageType type, std::string arg1, std::string arg2, std::string message);
	void SendBufferToServer(void);

private:
	void processMessage(Connection* server);
	void SendMessageToRoom(std::string room, std::string message);
	bool authenticated;

	Connection* theServer;
};