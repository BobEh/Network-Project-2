#include "Authentication.h"

#include <conio.h>
#include <mysql.h>
#include <iostream>
#include <string>
#include <sstream>

int qstate;

#define DEFAULT_BUFLEN 512
#define PORT "4567"

int main(int argc, char** argv)
{
	std::vector<std::string> emailAndPassword;
	MYSQL* sqlConn;
	MYSQL_ROW row;
	MYSQL_RES *res;
	sqlConn = mysql_init(0);

	sqlConn = mysql_real_connect(sqlConn, "localhost", "root", "root", "network_users", 3306, NULL, 0);

	if (sqlConn)
	{
		std::cout << "Connected to the database.\n";

		std::string query = "SELECT email, password FROM users";
		const char* q = query.c_str();
		qstate = mysql_query(sqlConn, q);
		if (!qstate)
		{
			res = mysql_store_result(sqlConn);
			int userCount = 1;
			while (row = mysql_fetch_row(res))
			{
				std::cout << "All users in the database: \n";
				std::cout << "User number: " << userCount << std::endl;
				//std::cout << "ID: " << row[0] << std::endl;
				std::cout << "Email: " << row[0] << std::endl;
				std::cout << "Password: ";
				for (int i = 0; i < sizeof(row[1]); i++)
				{
					std::cout << "*";
				}
				std::cout << " (" << row[1] << ")\n";
				//std::cout << "Last Login: " << row[3] << std::endl;
				//printf("ID: %d, Email: %s, Password: %s, Last Login: %s", row[0], row[1], row[2], row[3]);
				userCount++;
			}
		}
		else
		{
			std::cout << "Query failed: " << mysql_error(sqlConn) << std::endl;
		}
	}
	else
	{
		std::cout << "Database connection failed." << mysql_error(sqlConn);
	}

	Authentication theAuthenticator;
	theAuthenticator.StartAuthenticator("4567");

	//WSADATA wsaData;
	//int result;

	//// Initialize Winsock
	//result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//if (result != 0)
	//{
	//	// Something went wrong, tell the user the error id
	//	printf("WSAStartup failed with error: %d\n", result);
	//	return 1;
	//}
	//else
	//{
	//	printf("WSAStartup() was successful!\n");
	//}

	//// #1 Socket
	//SOCKET listenSocket = INVALID_SOCKET;
	//SOCKET acceptSocket = INVALID_SOCKET;

	//struct addrinfo* addrResult = NULL;
	//struct addrinfo hints;

	//// Define our connection address info 
	//ZeroMemory(&hints, sizeof(hints));
	//hints.ai_family = AF_INET;
	//hints.ai_socktype = SOCK_STREAM;
	//hints.ai_protocol = IPPROTO_TCP;
	//hints.ai_flags = AI_PASSIVE;

	//// Resolve the server address and port
	//result = getaddrinfo(NULL, PORT, &hints, &addrResult);
	//if (result != 0)
	//{
	//	printf("getaddrinfo() failed with error %d\n", result);
	//	WSACleanup();
	//	return 1;
	//}
	//else
	//{
	//	printf("getaddrinfo() is good!\n");
	//}

	//// Create a SOCKET for connecting to the server
	//listenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	//std::cout << "Listen socket set to: " << listenSocket << std::endl;
	//if (listenSocket == INVALID_SOCKET)
	//{
	//	// https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2
	//	printf("socket() failed with error %d\n", WSAGetLastError());
	//	freeaddrinfo(addrResult);
	//	WSACleanup();
	//	return 1;
	//}
	//else
	//{
	//	printf("socket() is created!\n");
	//}

	//// #2 Bind - Setup the TCP listening socket
	//result = bind(listenSocket,	addrResult->ai_addr, (int)addrResult->ai_addrlen);
	//if (result == SOCKET_ERROR)
	//{
	//	printf("bind failed with error: %d\n", WSAGetLastError());
	//	freeaddrinfo(addrResult);
	//	closesocket(listenSocket);
	//	WSACleanup();
	//	return 1;
	//}
	//else
	//{
	//	printf("bind() is good!\n");
	//}

	//// We don't need this anymore
	//freeaddrinfo(addrResult);

	//// #3 Listen
	//result = listen(listenSocket, SOMAXCONN);
	//if (result == SOCKET_ERROR)
	//{
	//	printf("listen() failed with error %d\n", WSAGetLastError());
	//	closesocket(listenSocket);
	//	WSACleanup();
	//	return 1;
	//}
	//else
	//{
	//	printf("listen() was successful!\n");
	//}

	//// #4 Accept		(Blocking call)
	//printf("Waiting for client to connect...\n");
	//acceptSocket = accept(listenSocket, NULL, NULL);
	//std::cout << "Accept socket set to: " << acceptSocket << std::endl;
	//if (acceptSocket == INVALID_SOCKET)
	//{
	//	printf("accept() failed with error: %d\n", WSAGetLastError());
	//	closesocket(listenSocket);
	//	WSACleanup();
	//	return 1;
	//}
	//else
	//{
	//	printf("accept() is OK!\n");
	//	printf("Accepted client on socket %d\n", acceptSocket);

	//	Connection* conn = new Connection(acceptSocket);

	//	printf("[%d] Connected!\n", acceptSocket);
	//	//TODO: MAKE THIS WORK!!
	//	//theAuthenticator->SendMessageToUser(conn, "Master Server", "Welcome to the chat client! Join any room you want");
	//}

	//// No longer need server socket
	//closesocket(listenSocket);

	//// #5 recv & send	(Blocking calls)
	//char recvbuf[DEFAULT_BUFLEN];
	//int recvbuflen = DEFAULT_BUFLEN;

	//int iSendResult;

	//do
	//{
	//	printf("Waiting to receive data from the client...\n");
	//	std::cout << "Waiting for server on socket: " << acceptSocket << std::endl;
	//	result = recv(acceptSocket, recvbuf, recvbuflen, 0);
	//	if (result > 0)
	//	{
	//		// We have received data successfully!
	//		// iResult is the number of bytes received
	//		printf("Bytes received: %d\n", result);

	//		// Send data to the client
	//		iSendResult = send(acceptSocket, recvbuf, result, 0);
	//		if (iSendResult == SOCKET_ERROR)
	//		{
	//			printf("send failed with error: %d\n", WSAGetLastError());
	//			closesocket(acceptSocket);
	//			WSACleanup();
	//			return 1;
	//		}
	//		printf("Bytes sent: %d\n", iSendResult);
	//	}
	//	else if (result < 0)
	//	{
	//		printf("recv failed with error: %d\n", WSAGetLastError());
	//		closesocket(acceptSocket);
	//		WSACleanup();
	//		return 1;
	//	}
	//	else // iResult == 0
	//	{
	//		printf("Connection closing...\n");
	//	}
	//} while (result > 0);

	//// #6 close
	//result = shutdown(acceptSocket, SD_SEND);
	//if (result == SOCKET_ERROR) {
	//	printf("shutdown failed with error: %d\n", WSAGetLastError());
	//	closesocket(acceptSocket);
	//	WSACleanup();
	//	return 1;
	//}

	//// cleanup
	//closesocket(acceptSocket);
	//WSACleanup();

	while (true)
	{
		if (_kbhit())
		{
			int ch = _getch();
			if (ch == 27) break;
		}

		//int result = recv(theAuthenticator->_socket, theAuthenticator->_protobuf, 512, 0);

		emailAndPassword = theAuthenticator.Update();
		std::cout << "Email and Password are:\n";
		if (emailAndPassword.size() > 0)
		{
			for (int i = 1; i < emailAndPassword.size(); i++)
			{
				std::cout << emailAndPassword.at(i) << std::endl;
			}
			if (emailAndPassword.at(0) == "AuthUser")
			{
				std::string email = emailAndPassword.at(1);
				std::string password = emailAndPassword.at(2);

				std::string getDateStatement = "SELECT CURRENT_DATE();";
				std::string loginDate = "";
				const char* q1 = getDateStatement.c_str();
				qstate = mysql_query(sqlConn, q1);
				if (!qstate)
				{
					res = mysql_store_result(sqlConn);
					row = mysql_fetch_row(res);
					loginDate = row[0];
				}
				std::string queryLogin = "UPDATE users SET last_login = '" + loginDate + "' WHERE email = '" + email + "' AND password = '" + password + "'";
				const char* q2 = queryLogin.c_str();
				qstate = mysql_query(sqlConn, q2);

				std::string query = "SELECT * FROM users WHERE email = '" + email + "' AND password = '" + password + "'";
				const char* q = query.c_str();
				qstate = mysql_query(sqlConn, q);
				if (!qstate)
				{
					res = mysql_store_result(sqlConn);
					row = mysql_fetch_row(res);
					if (row)
					{
						//success
						std::cout << "You have been authenticated!" << std::endl;
						//theAuthenticator.authSuccess();
					}
					else
					{
						//no user
						std::cout << "We couldn't find you in our system!" << std::endl;
						//theAuthenticator.authFailed();
					}
				}
				else
				{
					std::cout << "Query failed: " << mysql_error(sqlConn) << std::endl;
				}
			}
			if (emailAndPassword.at(0) == "AddUser")
			{
				int userId = theAuthenticator.getId();
				std::string email = emailAndPassword.at(1);
				std::string password = emailAndPassword.at(2);
				std::string getDateStatement = "SELECT CURRENT_DATE();";
				std::string currentDate = "";
				std::string loginDate = "";
				const char* q1 = getDateStatement.c_str();
				qstate = mysql_query(sqlConn, q1);
				if (!qstate)
				{
					res = mysql_store_result(sqlConn);
					row = mysql_fetch_row(res);
					currentDate = row[0];
					loginDate = row[0];
				}
				std::stringstream ss;
				ss << "INSERT INTO users (id, email, password, last_login, date_created) VALUES ('" << userId << "', '" << email << "', '" << password << "', '" << loginDate << "', '"<< currentDate << "')";
				std::string query = ss.str();
				const char* q = query.c_str();
				qstate = mysql_query(sqlConn, q);
				if (!qstate)
				{
					//success
					std::cout << "You have been added to the database!" << std::endl;
					//theAuthenticator.addSuccess();
				}
				else
				{
					std::cout << "Query failed: " << mysql_error(sqlConn) << std::endl;
					//theAuthenticator.addFailed();
				}
			}
		}

	}

	system("Pause");
}