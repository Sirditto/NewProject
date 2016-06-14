#pragma once
#include <iostream>
#include <time.h>
#include <vector>

using namespace std;

class Question
{
private:
	string _question;
	vector<string> _answers;
	int _correctAnswerIndex;
	int _id;

public:
	Question(int id, string question, string correctAnswer, string ans2, string ans3, string ans4);
	string getQuestion();
	vector<string> getAnswers();
	int getCorrectAnswerIndex();
	int getId();
};

