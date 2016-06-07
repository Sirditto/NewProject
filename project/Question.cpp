#include "Question.h"


Question::Question(int id, string question, string correctAnswer, string ans2, string ans3, string ans4) : _id(id), _question(question)
{
	srand(time(NULL));
	_correctAnswerIndex = rand() % 4;
	_answers[_correctAnswerIndex] = correctAnswer;
	_answers[(_correctAnswerIndex + 1) % 4] = ans3;
	_answers[(_correctAnswerIndex + 2) % 4] = ans2;
	_answers[(_correctAnswerIndex + 3) % 4] = ans4;
}

string Question::getQuestion()
{
	return _question;
}

string * Question::getAnswers()
{
	return _answers;
}

int Question::getCorrectAnswerIndex()
{
	return _correctAnswerIndex;
}

int Question::getId()
{
	return _id;
}
