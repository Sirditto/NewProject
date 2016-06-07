#pragma once

#include <iostream>
#include <datetimeapi.h>
#include <string>
#include <vector>
#include "sqlite3.h"
#include "Question.h"

using namespace std;

class DataBase
{
public:
	DataBase();
	~DataBase();

	bool isUserExists(string userName);
	bool addNewUser(string userName, string password, string email);
	bool isUserAndPassMatch(string userName, string password);
	vector<Question*> initQuestions(int questionsNo);
	vector<string> getBestScores(string username);
	vector<string> getPersonalStatus(string username);
	int insertNewGame();
	bool updateGameStatus(int gameNum);
	bool addAnswerToPlayer(int gameId, string userName, int questionId, string answer, bool isCorrect, int answerTime);

private:
	static int callbackCount(void* notUsed, int argc, char** argv, char** azCol);
	static int callbackQuestions(void* notUsed, int argc, char** argv, char** azCol);
	static int callbackBestScores(void* notUsed, int argc, char** argv, char** azCol);
	static int callbackPersonalStatus(void* notUsed, int argc, char** argv, char** azCol);
};

