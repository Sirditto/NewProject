#pragma once

#define _WINSOCKAPI_

#include <windows.h>
#include <WinSock2.h>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include "User.h"
#include "DataBase.h"
#include "Validator.h"
#include "RecivedMessage.h"

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
	void acceptClient(); // name was changed from accept()
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

