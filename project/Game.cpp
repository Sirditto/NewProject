#include "Game.h"


//constructor, initializes the fields
Game::Game(const vector<User*>& players, int questionNum, DataBase& db) : _db(db), _players(players), _question_num(questionNum)
{
	//
}


Game::~Game()
{
}
