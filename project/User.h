#pragma once

#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <queue>

#include "Helper.h"
#include "Game.h"
#include "Protocol.h"
#include "Room.h"

class Room;

class User
{
public:
	User(string, SOCKET);
	void send(string);
	string getUsername();
	SOCKET getSocket();
	Room* getRoom();
	Game* getGame();
	void setGame(Game*);
	void clearRoom();
	void clearGame();
	bool createRoom(int, string, int, int, int);
	bool joinRoom(Room*);
	void leaveRoom();
	int closeRoom();
	bool leaveGame();


private:
	string _username;
	Room* _currRoom;
	Game* _currGame;
	SOCKET _sock;

};

