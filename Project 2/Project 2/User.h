#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <map>
#include <vector>
#include <string>
#include "MessageTypes.h"
#include "Connection.h"
#include <iostream>

using std::map;
using std::vector;
using std::string;

class User
{
public:
	User();

	void joinServer(std::string address, std::string port);

	void Update();

	void ConfigureMessage(MessageType type, string arg1, string arg2, string message);
	void SendBufferToServer(void);

private:
	void processMessage(Connection* server);
	void SendMessageToRoom(std::string room, std::string message);

	Connection* theServer;
};