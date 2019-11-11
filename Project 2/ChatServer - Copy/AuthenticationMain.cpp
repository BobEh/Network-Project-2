#include "Authentication.h"

#include <conio.h>
#include <mysql.h>
#include <iostream>

int qstate;

int main(int argc, char** argv)
{
	MYSQL* conn;
	MYSQL_ROW row;
	MYSQL_RES *res;
	conn = mysql_init(0);

	conn = mysql_real_connect(conn, "localhost", "root", "", "network_users", 3306, NULL, 0);

	if (conn)
	{
		std::cout << "Connected to the database.\n";

		std::string query = "SELECT * FROM users";
		const char* q = query.c_str();
		qstate = mysql_query(conn, q);
		if (!qstate)
		{
			res = mysql_store_result(conn);
			while (row = mysql_fetch_row(res))
			{
				printf("ID: %d, Email: %s, Password: %s, Last Login: %s", row[0], row[1], row[2], row[3]);
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

	/*Authentication theAuthenticator;
	theAuthenticator.StartServer("5150");

	while (true)
	{
		if (_kbhit())
		{
			int ch = _getch();
			if (ch == 27) break;
		}

		theAuthenticator.Update();
	}*/

	system("Pause");

	return 0;
}