#pragma once
#include <iostream>
#include <time.h>

using namespace std;

class Question
{
private:
	string _question;
	string _answers[4];
	int _correctAnswerIndex;
	int _id;

public:
	Question(int id, string question, string correctAnswer, string ans2, string ans3, string ans4);
	string getQuestion();
	string* getAnswers();
	int getCorrectAnswerIndex();
	int getId();
};

