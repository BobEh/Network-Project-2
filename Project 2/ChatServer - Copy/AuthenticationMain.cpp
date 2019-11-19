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
			//while (row = mysql_fetch_row(res))
			//{
			//	std::cout << "All users in the database: \n";
			//	std::cout << "User number: " << userCount << std::endl;
			//	//std::cout << "ID: " << row[0] << std::endl;
			//	std::cout << "Email: " << row[0] << std::endl;
			//	std::cout << "Password: ";
			//	for (int i = 0; i < sizeof(row[1]); i++)
			//	{
			//		std::cout << "*";
			//	}
			//	std::cout << " (" << row[1] << ")\n";
			//	//std::cout << "Last Login: " << row[3] << std::endl;
			//	//printf("ID: %d, Email: %s, Password: %s, Last Login: %s", row[0], row[1], row[2], row[3]);
			//	userCount++;
			//}
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