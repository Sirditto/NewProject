#pragma once

#include <WinSock2.h>
#include "User.h"

using namespace std;

class User;

class RecivedMessage
{
public:
	RecivedMessage(SOCKET sock, int messagecode);
	RecivedMessage(SOCKET sock, int messageCode, vector<string>& vals);

	SOCKET getSock();
	User* getUser();
	void setUser(User*);
	int getMessageCode();
	vector<string>& getValues();

private:
	SOCKET _sock;
	User* _user;
	int _messageCode;
	vector<string> _values;
};

