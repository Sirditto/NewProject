#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <queue>

#include "Helper.h"
#include "DataBase.h"
#include "User.h"

class User;

class Game
{
public:
	Game(const vector<User*>& players, int questionNum, DataBase& db);
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

