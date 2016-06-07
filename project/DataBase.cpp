#include "DataBase.h"

sqlite3* db;
string Count;
int rc;
char* zErrMsg;
string errmsg;
string cmd;
vector<Question*> questions;
vector<string> strVec;


DataBase::DataBase()
{	// connection to the database
	rc = sqlite3_open("GameDB.db", &db);

	if (rc)
	{
		errmsg = "Can't open database: ";
		errmsg.append(sqlite3_errmsg(db));
		sqlite3_close(db);
		throw exception(errmsg.c_str());
	}

	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t_users(username TEXT NOT NULL PRIMARY KEY, password TEXT NOT NULL, email TEXT NOT NULL)", NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = "SQL error: ";
		errmsg.append(zErrMsg);
		sqlite3_free(zErrMsg);
		throw exception(errmsg.c_str());
	}

	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t_games(game_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, status INTEGER NOT NULL, start_time DATETIME NOT NULL, end_time DATETIME)", NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = "SQL error: ";
		errmsg.append(zErrMsg);
		sqlite3_free(zErrMsg);
		throw exception(errmsg.c_str());
	}

	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t_questions(question_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, question TEXT NOT NULL, correct_ans TEXT NOT NULL, ans2 TEXT NOT NULL, ans3 TEXT NOT NULL, ans4 TEXT NOT NULL)", NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = "SQL error: ";
		errmsg.append(zErrMsg);
		sqlite3_free(zErrMsg);
		throw exception(errmsg.c_str());
	}

	rc = sqlite3_exec(db, "PRAGMA foreign_keys = ON", NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = "SQL error: ";
		errmsg.append(zErrMsg);
		sqlite3_free(zErrMsg);
		throw exception(errmsg.c_str());
	}

	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t_players_answers(game_id INTEGER NOT NULL, username TEXT NOT NULL, question_id INTEGER NOT NULL, player_answer TEXT NOT NULL, is_correct INTEGER NOT NULL, answer_time INTEGER NOT NULL, PRIMARY KEY(game_id, username, question_id), FOREIGN KEY(game_id) REFERENCES t_games(game_id), FOREIGN KEY(username) REFERENCES t_users(username), FOREIGN KEY(question_id) REFERENCES t_questions(question_id))", NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = "SQL error: ";
		errmsg.append(zErrMsg);
		sqlite3_free(zErrMsg);
		throw exception(errmsg.c_str());
	}
}

DataBase::~DataBase()
{
	//closing the database
	sqlite3_close(db);
}

bool DataBase::isUserExists(string userName)
{
	cmd = "SELECT COUNT(*) FROM t_users WHERE username=";
	cmd.append(userName);

	rc = sqlite3_exec(db, cmd.c_str(), callbackCount, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = "SQL error: ";
		errmsg.append(zErrMsg);
		sqlite3_free(zErrMsg);
		throw exception(errmsg.c_str());
	}

	if (Count == "0")
		return false;
	else
		return true;
}

bool DataBase::addNewUser(string userName, string password, string email)
{
	cmd.clear();
	cmd = "INSERT INTO t_users VALUES(";
	cmd.append(userName);
	cmd.append(", ");
	cmd.append(password);
	cmd.append(", ");
	cmd.append(email);
	cmd.append(")");

	rc = sqlite3_exec(db, cmd.c_str(), NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

bool DataBase::isUserAndPassMatch(string userName, string password)
{
	cmd.clear();
	cmd = "SELECT password FROM t_users WHERE username=";
	cmd.append(userName);

	rc = sqlite3_exec(db, cmd.c_str(), callbackCount, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = "SQL error: ";
		errmsg.append(zErrMsg);
		sqlite3_free(zErrMsg);
		throw exception(errmsg.c_str());
	}
	if (Count == password)
		return true;
	else
		return false;
}

vector<Question*> DataBase::initQuestions(int questionsNo)
{
	questions.clear();

	cmd.clear();
	cmd = "SELECT * FROM t_questions ORDER BY RAND() LIMIT ";
	cmd.append(to_string(questionsNo));

	rc = sqlite3_exec(db, cmd.c_str(), callbackQuestions, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = "SQL error: ";
		errmsg.append(zErrMsg);
		sqlite3_free(zErrMsg);
		throw exception(errmsg.c_str());
	}

	return questions;
}

vector<string> DataBase::getBestScores(string username)
{
	strVec.clear();

	cmd.clear();
	cmd = "";

	rc = sqlite3_exec(db, cmd.c_str(), callbackBestScores, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = "SQL error: ";
		errmsg.append(zErrMsg);
		sqlite3_free(zErrMsg);
		throw exception(errmsg.c_str());
	}

	return strVec;
}

vector<string> DataBase::getPersonalStatus(string username)
{
	return vector<string>();
}

int DataBase::insertNewGame()
{
	cmd = "INSERT INTO t_games (status, start_time, end_time) VALUES (0, datetime('now'), NULL)";

	rc = sqlite3_exec(db, cmd.c_str(), NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = "SQL error: ";
		errmsg.append(zErrMsg);
		sqlite3_free(zErrMsg);
		throw exception(errmsg.c_str());
	}

	cmd = "SELECT MAX(game_id) AS max_id FROM t_games";

	rc = sqlite3_exec(db, cmd.c_str(), callbackCount, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = "SQL error: ";
		errmsg.append(zErrMsg);
		sqlite3_free(zErrMsg);
		throw exception(errmsg.c_str());
	}

	return atoi(Count.c_str());
}

bool DataBase::updateGameStatus(int gameNum)
{
	cmd = "UPDATE t_games SET status=1, end_time=datetime('now') WHERE game_id=";
	cmd.append(to_string(gameNum));

	rc = sqlite3_exec(db, cmd.c_str(), NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = "SQL error: ";
		errmsg.append(zErrMsg);
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

bool DataBase::addAnswerToPlayer(int gameId, string userName, int questionId, string answer, bool isCorrect, int answerTime)
{
	cmd = "INSERT INTO t_players_answers VALUES (";
	cmd.append(to_string(gameId) += ", ");
	cmd.append(userName += ", ");
	cmd.append(to_string(questionId) += ", ");
	cmd.append(answer += ", ");
	if (isCorrect)
		cmd.append(to_string(1) += ", ");
	else
		cmd.append(to_string(0) += ", ");
	cmd.append(to_string(answerTime) += ")");

	rc = sqlite3_exec(db, cmd.c_str(), NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = "SQL error: ";
		errmsg.append(zErrMsg);
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}



int DataBase::callbackCount(void * notUsed, int argc, char ** argv, char ** azCol)
{
	if (argc > 0)
		Count = argv[0];
	return 0;
}

int DataBase::callbackQuestions(void * notUsed, int argc, char ** argv, char ** azCol)
{
	Question* currQuestion;

	currQuestion = new Question(atoi(argv[0]), argv[1], argv[2], argv[3], argv[4], argv[5]);
	if (currQuestion)
	{
		questions.push_back(currQuestion);
		return 0;
	}
	return -1;
}

int DataBase::callbackBestScores(void * notUsed, int argc, char ** argv, char ** azCol)
{
	return 0;
}

int DataBase::callbackPersonalStatus(void * notUsed, int argc, char ** argv, char ** azCol)
{
	return 0;
}
