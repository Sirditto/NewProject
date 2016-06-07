#include "User.h"


/*constructor, just initializes the fields.....*/
User::User(string username, SOCKET socket) : _sock(socket), _username(username), _currGame(NULL), _currRoom(NULL)
{
}

/*sends a message to the user*/
void User::send(string msg)
{
	//sending the message using Helper class.....
	Helper::sendData(_sock, msg);
}

/*Game setter, setting the Room pointer to NULL*/
void User::setGame(Game* gm)
{
	_currRoom = NULL;
	_currGame = gm;
}

/*disconnecting user from his game (doing so by setting the Game pointer to NULL*/
void User::clearGame()
{
	_currGame = NULL;
}

/*creates a new room, returns true if succeeded, else returns false*/
bool User::createRoom(int roomID, string roomName, int maxUsers, int questionsNum, int questionTime)
{
	//checking if the user is already in a room
	if (_currRoom)
	{
		//sending to user room creation fail message (message code 114)
		send(to_string(CREATE_ROOM_RESPONSE) + "1");
		return false; // <-- cause room creation failed
	}
	else // case user isn't already in a room
	{
		//creating a new room and linking it to the user
		_currRoom = new Room(roomID, this, roomName, maxUsers, questionsNum, questionTime);

		//sending to user room creation success message
		send(to_string(CREATE_ROOM_RESPONSE) + "0");
		return true; // <-- cause room creation succeeded
	}
}

/*joining a new room (the given room), return true if succeeded, else return false (basicly a room setter)*/
bool User::joinRoom(Room* newRoom)
{
	//checking if the user is already in a room
	if (_currRoom)
	{
		//returning false
		return false;
	}
	//if the user isn't already in a room
	else
	{
		//setting the new room and returning true
		_currRoom = newRoom;
		return true;
	}
}

/*leaves room (just setting the room pointer to NULL)*/
void User::leaveRoom()
{
	_currRoom->leaveRoom(this);
	_currRoom = NULL;
}

/*closes the room, returns the room number is succeeded, else returns -1*/
int User::closeRoom()
{
	//checking if the user is in the room
	if (_currRoom)
	{
		//deleting the room, setting the room pointer to NULL and returning the room number
		int roomNum = _currRoom->getId();
		if (_currRoom->closeRoom(this) == -1)
			return -1; // <-- case room closing didn't succeeded
		delete _currRoom;
		_currRoom = NULL;
		return roomNum;
	}
	//isn the user isn't in the room
	else
	{
		return -1;
	}
}

/*leaves game, returns true is successfuly left game, returns false if user is still connected to the game*/
bool User::leaveGame()
{
	//if the user is already inside a game
	if (_currGame)
	{
		if (!_currGame->leaveGame(this)) // leaving game
			return false; // case game leaving failed

		_currGame = NULL; // setting game pointer to NULL
		return true;
	}
	else
		return false;
}

/*setting the room pointer to NULL*/
void User::clearRoom()
{
	_currRoom = NULL;
}
