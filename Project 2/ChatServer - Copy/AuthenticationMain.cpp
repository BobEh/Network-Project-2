#include "Authentication.h"

#include <conio.h>
#include <mysql.h>
#include <iostream>

int qstate;

int main(int argc, char** argv)
{
	std::vector<std::string> emailAndPassword;
	MYSQL* conn;
	MYSQL_ROW row;
	MYSQL_RES *res;
	conn = mysql_init(0);

	conn = mysql_real_connect(conn, "localhost", "root", "root", "network_users", 3306, NULL, 0);

	if (conn)
	{
		std::cout << "Connected to the database.\n";

		std::string query = "SELECT * FROM users";
		const char* q = query.c_str();
		qstate = mysql_query(conn, q);
		if (!qstate)
		{
			res = mysql_store_result(conn);
			int userCount = 1;
			while (row = mysql_fetch_row(res))
			{
				std::cout << "All users in the database: \n";
				std::cout << "User number: " << userCount << std::endl;
				std::cout << "ID: " << row[0] << std::endl;
				std::cout << "Email: " << row[1] << std::endl;
				std::cout << "Password: ";
				for (int i = 0; i < sizeof(row[2]); i++)
				{
					std::cout << "*";
				}
				std::cout << " (" << row[2] << ")\n";
				std::cout << "Last Login: " << row[3] << std::endl;
				//printf("ID: %d, Email: %s, Password: %s, Last Login: %s", row[0], row[1], row[2], row[3]);
				userCount++;
			}
		}
		else
		{
			std::cout << "Query failed: " << mysql_error(conn) << std::endl;
		}
	}
	else
	{
		std::cout << "Database connection failed." << mysql_error(conn);
	}

	Authentication theAuthenticator;
	theAuthenticator.StartAuthenticator("1234");

	while (true)
	{
		if (_kbhit())
		{
			int ch = _getch();
			if (ch == 27) break;
		}

		emailAndPassword = theAuthenticator.Update();

	}

	system("Pause");
}