#pragma once

#include <WinSock2.h>
#include "Buffer.h"

class Connection
{
public:
	Connection(SOCKET s);

	SOCKET _socket;
	Buffer protobuf;
	int _numBytes;
	int _length;
private:
	Connection();
};