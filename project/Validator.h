#pragma once

#include <iostream>
#include <string>

using namespace std;

#define DIGITS "0123456789"
#define LOWER "abcdefghijklmnopqrstuvwxyz"
#define UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

class Validator
{
public:
	bool static isPasswordValid(string);
	bool static isUsernameValid(string);
};

