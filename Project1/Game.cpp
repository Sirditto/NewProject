#include "Game.h"


//constructor, initializes the fields
Game::Game(const vector<User*>& players, int questionNum, DataBase& db) : _db(db), _players(players), _question_num(questionNum)
{
	
}


Game::~Game()
{
}

void Game::sendFirstQuestion()
{
}

void Game::handleFinishGame()
{
}

bool Game::handleNextTurn()
{
	return true;
}

bool Game::handleAnswerFromUser(User* user, int x, int y)
{
	return true;
}

bool Game::leaveGame(User* user)
{
	return true;
} 
