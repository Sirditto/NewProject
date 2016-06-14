#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>

#include "Helper.h"
#include "DataBase.h"
#include "RecivedMessage.h"
#include "Validator.h"

class RecivedMessage;

class DataBase;
class RecivedMessage;

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

	int _roomidSequence;

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

