#pragma once

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <stdio.h>
#include <stdlib.h>

#include "Helper.h"
#include "Protocol.h"
#include "User.h"
#include "DataBase.h"
#include "Room.h"
#include "RecivedMessage.h"
#include "Validator.h"

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

class TriviaServer
{
public:
	TriviaServer();
	~TriviaServer();

	void server();

private:
	SOCKET _socket;
	map<SOCKET, User*> _connectedUser;
	DataBase _db;
	map<int, Room*> _roomList;
	
	mutex _mtxRecivedMessages;
	queue<RecivedMessage*> _queRcvMessages;

	int static _roomidSequence;

	void bindAndListen();
	void acceptClient(); // name was change from accept()
	void clientHandler(SOCKET);
	void safeDeleteUser(RecivedMessage*);
	
	User* handleSignin(RecivedMessage*);
	bool handleSignup(RecivedMessage*);
	void handleSignout(RecivedMessage*);

	void handleLeaveGame(RecivedMessage*);
	void handleStartGame(RecivedMessage*);
	void handlePlayerAnswer(RecivedMessage*);

	bool handleCreateRoom(RecivedMessage*);
	bool handleCloseRoom(RecivedMessage*);
	bool handleJoinRoom(RecivedMessage*);
	bool handleLeaveRoom(RecivedMessage*);
	void handleGetUserinRoom(RecivedMessage*);
	void handleGetRooms(RecivedMessage*);

	void handleGetBestScores(RecivedMessage*); 
	void handleGetPersonalStatus(RecivedMessage*);

	void handleRecivedMessages();
	void addRecivedMessage(RecivedMessage*);
	RecivedMessage* buildRecivedMessage(SOCKET, int);

	User* getUserByName(string);
	User* getUserBySocket(SOCKET);
	Room* getRoomByid(int);
};

