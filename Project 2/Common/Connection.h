#pragma once

#include "network_headers.h"

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
