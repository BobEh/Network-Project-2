#pragma once
#define HEADER_SIZE 8

enum MessageType
{
	JoinRoom,
	LeaveRoom,
	MessageRoom,
	ReceiveMessage,
	AuthUser
};