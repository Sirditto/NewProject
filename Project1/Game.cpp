#include "Game.h"


/*constructor, initializes the fields*/
Game::Game(const vector<User*>& players, int questionNum, DataBase& db) : _db(db), _players(players), _question_num(questionNum)
{
	//_db = *(new DataBase(db));
	try
	{
		//adding the game to the database
		_id = _db.insertNewGame();
	}
	catch (exception ex)
	{
		throw ex;
	}

	//initializing some fields.......
	_db.initQuestions(questionNum);
	_results.clear();

	//add all users (who were sent) to the game
	for (int i = 0; i < _players.size(); i++)
	{
		_players[i]->setGame(this);
	}
}

/*distructor*/
Game::~Game()
{
	//clearing the players and questions vectors
	_players.clear();
	_questions.clear();
}

/*send users first question*/
void Game::sendQuestionToAllUsers()
{
	_currTurnAnswers = 0;
	//poping first question from vector to send to users
	Question* first_q = _questions[_question_num];

	for (int i = 0; i < _players.size(); i++)
	{
		try
		{
			_players[i]->send("118" + to_string(first_q->getQuestion().size()) + first_q->getQuestion() + //[question len] + [question] 
				to_string(first_q->getAnswers()[0].size()) + first_q->getAnswers()[0] + // [answer1 len] [answer1]
				to_string(first_q->getAnswers()[1].size()) + first_q->getAnswers()[1] + // [answer2 len] [answer2]
				to_string(first_q->getAnswers()[2].size()) + first_q->getAnswers()[2] + // [answer3 len] [answer3]
				to_string(first_q->getAnswers()[3].size()) + first_q->getAnswers()[3]); // [answer4 len] [answer4]
		}
		catch (...){}
	}
}

/*sends the first question to users*/
void Game::sendFirstQuestion()
{
	sendQuestionToAllUsers();
}

/*finish game*/
void Game::handleFinishGame()
{
	_db.updateGameStatus(_id);
}

/*move to next turn, return true if game is still going, else return false*/
bool Game::handleNextTurn()
{
	//checking if the room is empty
	if (!_players.empty())
	{
		//checking if al the players have answered the question
		if (_currTurnAnswers == _players.size())
		{
			//checking if last question was the final one
			if (!_questions.empty())
			{
				_question_num++;
				sendQuestionToAllUsers();
				return true;
			}
			else
			{
				handleFinishGame();
				return false;
			}
		}
	}
	//case game is empty (no players)
	else
	{
		handleFinishGame();
		return false;
	}
}

/*handle answer from a player (update database.......), return true of game is still going, else return false*/
bool Game::handleAnswerFromUser(User* user, int answerNum, int time)
{
	_currTurnAnswers++;
	//if user answered correctly
	if (answerNum == _questions[_question_num]->getCorrectAnswerIndex())
	{
		_results.find(user->getUsername())->second++;

		if (_db.addAnswerToPlayer(_id, user->getUsername(), _questions[_question_num]->getId(), _questions[_question_num]->getAnswers()[answerNum], true, time))
		{
			//sending user result and attempting to move to next turn
			user->send("1201");
			return handleNextTurn();
		}
	}
	//case user didn't answer correctly
	else
	{
		user->send("1200");
		return handleNextTurn();
	}
}

/*removes a user from the game*/
bool Game::leaveGame(User* user)
{
	//deleting user from game
	_players.erase(std::remove(_players.begin(), _players.end(), user), _players.end());
	return handleNextTurn();
}
