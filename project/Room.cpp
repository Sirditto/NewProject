#include "Room.h"


//constructor, just initializes the fields.......
Room::Room(int id, User* admin, string name, int maxUsers, int questionNum, int questionTime) : _id(id), _admin(admin), _name(name), _maxUsers(maxUsers), _questionNum(questionNum), _questionTime(questionTime)
{
	_users.push_back(admin); // also adding the admin to the user list...
}

//returning the user names (splited with a ', ') as string in exeption of the "excludeUser"
string Room::getUsersAsString(vector<User*> userList, User* excludeUser)
{
	string names = "";
	//going over the user vector
	for (int i = 0; i < userList.size(); i++)
	{
		//enter here as long aas checked user isn't the excluded user
		if (userList[i] != excludeUser)
		{
			names += userList[i]->getUsername() + ", ";
		}
	}

	return names;
}

/*builds a user list message acording to the protocol*/
string Room::getUserListMessage()
{
	vector<string> names;
	string message = "";

	//saving up the names of the user inside a vector of strings
	for (int i = 0; i < _users.size(); i++)
	{
		//pushing the username into the vector
		names.push_back(_users[i]->getUsername());
	}

	//if the room is empty
	if (_users.size() == 0)
	{
		return to_string(USER_IN_ROOM_RESPONSE) + "0";
	}
	//is the room isn't empty
	else
	{
		//linking up all the names according to the protocol
		message = to_string(USER_IN_ROOM_RESPONSE) + to_string(names.size());
		for (int i = 0; i < names.size(); i++)
		{
			message += std::to_string(names[i].size()) + names[i]; // <name size in bytes><name>
		}
	}

	//returning complete message
	return message;
}

/*sends a message to all the users that are connected to the room in exeption to 'excludeUser'*/
void Room::sendMessage(User* excludeUser, string message)
{
	//going over all the connected users
	for (int i = 0; i < _users.size(); i++)
	{
		//checking that the checked user isnt the 'excludeUser'
		if (_users[i] != excludeUser)
		{
			try
			{
				//sending the message
				_users[i]->send(message);
			}
			catch (...)
			{
				cout << "an error occured while sending message to client: " + _users[i]->getUsername();
			}
		}
	}
}

/*adds the user to the room, if failed returns false, else returns true*/
bool Room::joinRoom(User* user)
{
	//checking if the room is full
	if (_users.size() == _maxUsers)
	{
		Helper::sendData(user->getSocket(), to_string(JOIN_ROOM_RESPONSE) + "2"); // sending room joinning fail message
		return false;
	}
	else
	{
		_users.push_back(user); // adding the new user to the room
		Helper::sendData(user->getSocket(), to_string(JOIN_ROOM_RESPONSE) + "0" + to_string(_questionNum) + to_string(_questionTime));
		sendMessage(NULL, getUserListMessage()); // sending to new and all other users the current user list in the room (108)
		return true;
	}
	
}

/*removes player from the room*/
void Room::leaveRoom(User* user)
{
	//going over the user vector
	for (int i = 0; i < _users.size(); i++)
	{
		//if user was found
		if (_users[i] == user)
		{
			//removing the user from the room
			_users.erase(_users.begin + i);

			//sending user leaving succes message
			user->send(to_string(LEAVE_ROOM_RESPONSE) + "0"); 

			//sending user list update message to all the connected users
			sendMessage(NULL, getUserListMessage());
		}
	}
}

/*closes the room, if succeeded returns the room ID, else returns -1*/
int Room::closeRoom(User* user)
{
	//checking if the user who requested the room closing is indeed the admin
	if (user == _admin)
	{
		sendMessage(NULL, to_string(CLOSE_ROOM_RESPONSE)); // sending room closing message to all the users (including the admin)

		for (int i = 0; i < _users.size(); i++)
		{
			//operate room clearing on all of the users but the admin
			if (user != _admin)
				_users[i]->clearRoom();
		}
		return _id;
	}
	//if the user isn't the admin
	else
		return -1;
}

