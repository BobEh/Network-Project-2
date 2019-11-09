#include "Connection.h"

Connection::Connection(SOCKET s) : protobuf(Buffer(512)), _numBytes(0), _length(0), _socket(s)
{

}