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
#include "TriviaServer.h"
#include "DataBase.h"
#include "Room.h"
#include "RecivedMessage.h"
#include "User.h"

class Game
{
public:
	Game(const vector<User*>&, int, DataBase&);
	~Game();
	void sendFirstQuestion();
	void handleFinishGame();
	bool handleNextTurn();
	bool handleAnswerFromUser(User*, int, int);
	bool leaveGame(User*);
	int getID();

private:
	vector<Question*> _questions;
	vector<User*> _players; 
	int _question_num;
	int _currQuestionIndex;
	DataBase& _db;
	map<string, int> _results;
	int _currTurnAnswers;

	bool insertGameToDB();
	void initQuestionsFromDB();
	void sendQuestionsRoAllUSers();
};

