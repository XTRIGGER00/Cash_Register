#include <iostream>
#include <mysql.h>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;
int qstate;
string username;
string password;
int products[100] = { 0 };

MYSQL* conn;
MYSQL_ROW row;
MYSQL_RES* res;


void registerProcedure() {
	string usr, pssw;
	
	do {
		cout << "Enter a new username: ";
		cin >> usr;
	} while (usr.length() <= 0);

	
	do {
		cout << "Enter a new password: ";
		cin >> pssw;
	} while (pssw.length() <= 0);

	//QUERY BELOW
	string query = "INSERT INTO `testschema`.`users` (`username`, `password`) VALUES ('" + usr + "', '" + pssw + "');";
	const char* q = query.c_str();
	qstate = mysql_query(conn, q);
	if (!qstate);
	else cout << "Query failed: " << mysql_error(conn) << endl;
}


bool isCredOK(string usr, string pssw) {
	bool ok = false;
	//QUERY BELOW
	string query = "SELECT PASSWORD FROM testschema.users WHERE username = '" + usr + "' ;";
	const char* q = query.c_str();
	qstate = mysql_query(conn, q);
	if (!qstate)
	{
		res = mysql_store_result(conn);
		while (row = mysql_fetch_row(res))
		{
			if (row[0] == pssw) {
				ok = true;
				cout << "\nYou're logged in successful!" << endl;
			}
			else cout << "Username or password are wrong" << endl;
		}
	}
	else cout << "Query failed: " << mysql_error(conn) << endl;
	return ok;
}


string getCode() {
	string code;
	cout << "\nEnter product code: ";
	cin >> code;
	return code;
}



bool isCodeExisting(string code) {
	res = NULL;
	row = NULL;
	bool exists = false;


	if (conn) {
		string query = "SELECT EXISTS(SELECT * from testschema.products WHERE code=" + code + ");";
		const char* q = query.c_str();
		qstate = mysql_query(conn, q);
		if (!qstate)
		{
			res = mysql_store_result(conn);
			while (row = mysql_fetch_row(res)) {
				if (*row[0] == '1') {
					exists = true;
				}
				else cout << "The product does not exixt" << endl;
			}
		}
		else
		{
			cout << "Query failed: " << mysql_error(conn) << endl;
		}
	}
	return exists;
}



void getProductDetails(string code) {
	res = NULL;
	row = NULL;


	if (conn) {
		string query = "SELECT name, brand, weight FROM testschema.products WHERE code = "+code+";";
		const char* q = query.c_str();
		qstate = mysql_query(conn, q);
		if (!qstate)
		{
			res = mysql_store_result(conn);
			while (row = mysql_fetch_row(res)) {
				cout << "\nProduct information:" << endl;
				cout << "    Name: " << row[0] << endl;
				cout << "    Brand: " << row[1] << endl;
				cout << "    Weight: " << row[2] << " g" << endl;
				cout << "    Price: " << row[3] << endl;
			}
		}
		else
		{
			cout << "Query failed: " << mysql_error(conn) << endl;
		}
	}
}



void codeLog(string code) {
	string tst;
	bool abort = false;

	res = NULL;
	row = NULL;

	if (conn) {
		//READ
		string query = "SELECT `products` FROM testschema.users WHERE username = '"+username+"';";
		const char* q = query.c_str();
		qstate = mysql_query(conn, q);
		if (!qstate)
		{
			res = mysql_store_result(conn);
			while (row = mysql_fetch_row(res))
			{
				tst = row[0];
				if (tst == "null") {
					tst = code;
					
				}
				else if (tst.find(code) != std::string::npos) {
					abort = true;
				} else {
					tst.erase(remove(tst.begin(), tst.end(), '['), tst.end());
					tst.erase(remove(tst.begin(), tst.end(), ']'), tst.end());
					tst = tst + ", " + code;
				}
			}
		}
		else
		{
			cout << "Query failed: " << mysql_error(conn) << endl;
			cout << "Username or password are wrong" << endl;
		}
		
		//WRITE
		if (!abort) {
			query = "UPDATE testschema.users SET products = JSON_ARRAY(" + tst + ") WHERE username = '" + username + "';";
			const char* p = query.c_str();
			qstate = mysql_query(conn, p);
			if (!qstate);
			else cout << "Query failed: " << mysql_error(conn) << endl;
		}
		

	}
}




int main() {
	

	conn = mysql_init(0);
	conn = mysql_real_connect(conn, "localhost", "root", "password", "testschema", 3306, NULL, 0);
	if (conn) {

		cout << "WELCOME. If you don't have an account type /register" << endl;

		int i = 0;
		do {
			username = "";
			password = "";
			while (username == "" || password == "") {
				if (i == 0) cout << "\n||LOGIN DIALOG||\n" << endl;
				else cout << "\nUsername or password are incorrect.\n" << endl;
				cout << "Username: ";
				cin >> username;
				if (username == "/register") {
					username = "";
					registerProcedure();
				}
				else {
					cout << "Password: ";
					cin >> password;
				}
			}
			i++;
		} while (!isCredOK(username, password));

		//REST OF THE CODE
		bool backtologin = false;
		while (true) {
			string code = getCode();
			if (isCodeExisting(code)) {
				codeLog(code);
				getProductDetails(code);
			}
			else cout << "The code does not exist." << endl;

		}

		} else {
			cout << "Error while connecting to the database" << endl;
		}
		
	
	

	

	/*---OLD-STUFF---
	MYSQL* conn;
	MYSQL_ROW row;
	MYSQL_RES *res;
	conn = mysql_init(0);
	conn = mysql_real_connect(conn, "localhost", "root", "password", "testschema", 3306, NULL, 0);
	if (conn)
	{
		cout << "Connected successfully!" << endl;
		string query = "SELECT * FROM testtable;";
		const char* q = query.c_str();
		qstate = mysql_query(conn, q);
		if (!qstate)
		{
			res = mysql_store_result(conn);
			while (row = mysql_fetch_row(res))
			{
				printf("Id: %s, Code: %s, Name: %s, Name of brand: %s\n", row[0], row[1], row[2], row[3]);
			}
		}
		else
		{
			cout << "Query failed: " << mysql_error(conn) << endl;
		}
	}
	else {
		cout << "Error while connecting to the database" << endl;
	}
	*/
	system("PAUSE");

}





