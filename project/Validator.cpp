#include "Validator.h"


bool Validator::isPasswordValid(string password)
{
	bool valid = true;
	if (password.length() < 4)
		valid = false;
	else if (password.find(" ") != -1)
		valid = false;
	else if (password.find_first_of(DIGITS) == -1)
		valid = false;
	else if (password.find_first_of(UPPER) == -1)
		valid = false;
	else if (password.find_first_of(LOWER) == -1)
		valid = false;
	return valid;
}
bool Validator::isUsernameValid(string username)
{
	bool valid = true;
	if (username.find_first_of(UPPER) != 0 && username.find_first_of(LOWER) != 0)
		valid = false;
	else if (username.find(" ") != -1)
		valid = false;
	else if (username.length() == 0)
		valid = false;
	return valid;
}
