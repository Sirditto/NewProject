#pragma once
#include <vector>
#include <map>
#include <algorithm>

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

private:
	vector<Question*> _questions;
	vector<User*> _players; 
	int _question_num;
	int _currQuestionIndex;
	DataBase& _db;
	map<string, int> _results;
	int _currTurnAnswers;
	int _id;

	bool insertGameToDB();
	void initQuestionsFromDB();
	void sendQuestionToAllUsers();
};

