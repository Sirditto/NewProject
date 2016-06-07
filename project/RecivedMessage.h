#pragma once

#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <queue>

#include "TriviaServer.h"
#include "DataBase.h"
#include "Room.h"
#include "User.h"
#include "Game.h"

using namespace std;

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

