#include "TriviaServer.h"

int pushRange(string str, vector<string>& vec, int first, int last);

/*constructor, initialize the main listening socket */
TriviaServer::TriviaServer() : _socket(INVALID_SOCKET)
{
	_db = *(new DataBase());
}


/*distructor, closes the socket and cleans upthe room and user lists*/
TriviaServer::~TriviaServer()
{
	//clearing the rook list
	_roomList.clear();
	//clearing the user list
	_connectedUser.clear();

	//closing the socket
	closesocket(_socket);
	WSACleanup();
}

/*main server function, contains the main server loop, accepts clients and call to all the other functions in the class*/
void TriviaServer::server()
{
	bindAndListen();
	thread t(&TriviaServer::handleRecivedMessages);
	t.detach();

	//main server loop
	while (true)
	{
		acceptClient();
	}

}
/*accept a new client*/
void TriviaServer::acceptClient()
{
	SOCKET curr_client_soc = INVALID_SOCKET;

	// Accept a client socket
	curr_client_soc = accept(_socket, NULL, NULL);
	if (curr_client_soc == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(_socket);
		WSACleanup();
	}

	thread t(&TriviaServer::clientHandler, curr_client_soc);
}

/*binds and listens t*/
void TriviaServer::bindAndListen()
{
	WSADATA wsaData;
	int iResult;

	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		
	}

	// Create a SOCKET for connecting to server
	_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (_socket == INVALID_SOCKET) {
		freeaddrinfo(result);
		WSACleanup();
	}

	// Setup the TCP listening socket
	iResult = bind(_socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		freeaddrinfo(result);
		WSACleanup();
	}

	//listen for connecting client
	if (listen(_socket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "an error occured while listening for connecting client, error: " + WSAGetLastError() << endl;
		WSACleanup();
	}

	freeaddrinfo(result);
}

/*handles the request from the messages queue*/
void TriviaServer::handleRecivedMessages()
{
	RecivedMessage* curr_msg = NULL;

	while (true)
	{
		//wait untill queue has requests in it
		while (!_queRcvMessages.size());

		//saving the first message in the queue
		curr_msg = _queRcvMessages.front();

		//locking the mutex
		unique_lock<mutex> lck(_mtxRecivedMessages);
		lck.lock();

		//poping the request
		_queRcvMessages.pop();

		//unlocking the mutex
		lck.unlock();

		//linking the username to the message
		curr_msg->setUser(getUserBySocket(curr_msg->getSock()));

	/**************************************************************************************************/

		// handle requests......
		switch (curr_msg->getMessageCode())
		{
		//case user requests to sign in
		case SIGN_IN_REQUEST:
			handleSignin(curr_msg);
			break;

		//case user requests to sign up
		case SIGN_UP_REQUEST:
			handleSignup(curr_msg);
			break;

		//case user requests to sign out
		case SIGN_OUT_REQUEST:
			handleSignout(curr_msg);

		//case user requests room list
		case ROOM_LIST_REQUEST:
			handleGetRooms(curr_msg);
			break;

		//case user requests list of users in room
		case USER_IN_ROOM_REQUEST:
			handleGetUserinRoom(curr_msg);
			break;

		//case user requests to join an existing room
		case JOIN_ROOM_REQUEST:
			handleJoinRoom(curr_msg);
			break;

		//case user requests to leave room
		case LEAVE_ROOM_REQUEST:
			handleLeaveRoom(curr_msg);
			break;

		//case user requests to create room
		case CREATE_ROOM_REQUEST:
			handleCreateRoom(curr_msg);
			break;

		//case user requests to close room
		case CLOSE_ROOM_REQUEST:
			handleCloseRoom(curr_msg);
			break;

		//case user requests to start game 
		case START_GAME_REQUEST:
			handleStartGame(curr_msg);
			break;

		//case user sent ansewr to question
		case ANSWER_REQUEST:
			handlePlayerAnswer(curr_msg);
			break;

		//case user requests to leave game
		case LEAVE_GAME_REQUEST:
			handleLeaveGame(curr_msg);
			break;

		//case user requests high scores
		case BEST_SCORES_REQUEST:
			handleGetBestScores(curr_msg);
			break;

		//case user requests his\her personal status
		case PERSONAL_STATUS_REQUEST:
			handleGetPersonalStatus(curr_msg);
			break; 

		//case user requests to quit
		case QUIT_REQUEST:
			safeDeleteUser(curr_msg);
			break;
		}
	}
}

/*return the room that belongs to the given id*/
Room* TriviaServer::getRoomByid(int roomid)
{
	return _roomList.find(roomid)->second;
}

/*returns user by its name*/
User* TriviaServer::getUserByName(string username)
{
	//going over the user map
	for (map<SOCKET, User*>::const_iterator i = _connectedUser.begin(); i != _connectedUser.end(); i++)
	{
		//if the checked user's username is equal to the given one, return the user
		if (i->second->getUsername() == username)
			return i->second;
	}
}

/*returns user by its socket*/
User* TriviaServer::getUserBySocket(SOCKET sock)
{
	return _connectedUser.find(sock)->second;
}

/*disconnects user in a safe way*/
void TriviaServer::safeDeleteUser(RecivedMessage* msg)
{
	handleSignout(msg); // calling the safe signout function
	closesocket(msg->getSock()); // closing the user's socket
}

/*handle sign in*/
User* TriviaServer::handleSignin(RecivedMessage* msg)
{
	//making sure that the user exists in the database
	if (_db.isUserExists(msg->getUser()->getUsername()))
	{
		//checking if the user's enteres password matchs its username
		if (_db.isUserAndPassMatch(msg->getValues()[1], msg->getValues()[2])) /// Note: potential problem in accessing username and password
		{
			//checking if the user is already connected
			if (getUserBySocket(msg->getSock()))
			{
				//sending user sign in fail message (1022)
				msg->getUser()->send(to_string(SIGN_IN_RESPONSE) + "2");
			}
			//if user isnt connected
			else
			{
				//sending user sign in success message
				msg->getUser()->send(to_string(SIGN_IN_RESPONSE) + "0");
				User newUser(msg->getUser()->getUsername(), msg->getUser()->getSocket());
				pair<SOCKET, User*> p(msg->getSock(), &newUser);
				_connectedUser.insert(p);
			}
		}
		//if username and password doesnt match
		else
		{
			//sending user sign in fail message (1021)
			msg->getUser()->send(to_string(SIGN_IN_RESPONSE) + "1");

		}
	}
}

/*handle sign up request*/
bool TriviaServer::handleSignup(RecivedMessage* msg)
{
	//checking that the password user sent is legal
	if (Validator::isPasswordValid(msg->getValues()[2])) /// this line is problematic because we cant be sure that this is the correct way to access the password.....  (203 ## username ## password ## email)
	{
		//checking that the username user sent is legal
		if (Validator::isUsernameValid(msg->getValues()[1])) /// again, same problem as above, we are not sure that this is the correct way to access username
		{
			//checking that the user isnt already listed in the database
			if (!_db.isUserExists(msg->getValues()[1])) /// as the above
			{
				//adding the user to the database
				if (_db.addNewUser(msg->getValues()[1], msg->getValues()[2], msg->getValues()[3]))/// again, probably not the correct way to access username password and this time even email
				{
					//sending user success message (1040)
					Helper::sendData(msg->getSock(), to_string(SIGN_UP_RESPONSE) + "0");
					return true;
				}
				//case failed to add user to database
				else
					//send user fail message (1044)
					Helper::sendData(msg->getSock(), to_string(SIGN_UP_RESPONSE) + "4");
					return false;
			}
			//case user already exist
			else
				//send fail message (1042)
				Helper::sendData(msg->getSock(), to_string(SIGN_UP_RESPONSE) + "2");
				return false;
		}
		//case username is illegal
		else
			//send fail message (1043)
			Helper::sendData(msg->getSock(), to_string(SIGN_UP_RESPONSE) + "3");
			return false;
	}
	//case password is illegal
	else
		//send fail message (1041)
		Helper::sendData(msg->getSock(), to_string(SIGN_UP_RESPONSE) + "1");
		return false;
}

/*handle game leave request*/
void TriviaServer::handleLeaveGame(RecivedMessage* msg)
{
	//removing user from game 
	if (msg->getUser()->leaveGame())
		delete msg->getUser()->getGame(); // if leave game returns true, it means the room should be closed, so we delete it
}

/*handle game start request*/
void TriviaServer::handleStartGame(RecivedMessage* msg)
{
	try
	{
		Game *g = new Game(msg->getUser()->getRoom()->getUsers(), msg->getUser()->getRoom()->getQuestionNum(), _db);
		msg->getUser()->setGame(g);

		//if no error has occured

		//delete room from available room list
		_roomList.erase(_roomList.find(msg->getUser()->getRoom()->getId())); /// might cause an error
		//send first question
		g->sendFirstQuestion();
	}
	catch (...)
	{
	}
}

/*handle recived answer*/
void TriviaServer::handlePlayerAnswer(RecivedMessage* msg)
{
	//just making sure that the game exist
	if (msg->getUser()->getGame())
		//passing the messages arguments to the game, the answer proccess will be dealt there
		if (!msg->getUser()->getGame()->handleAnswerFromUser(msg->getUser(), stoi(msg->getValues()[1]), stoi(msg->getValues()[2]))) /// probably not the correct way to access answer number and answer time 
			//if function returned false, game is over so we close it
			delete msg->getUser()->getGame();
}

/*handle room creation request*/
bool TriviaServer::handleCreateRoom(RecivedMessage* msg)
{
	//checking that a user is linked to the message
	if (msg->getUser())
	{
		_roomidSequence++;
		//creating new room
		if (msg->getUser()->createRoom(_roomidSequence, msg->getValues()[1], std::stoi(msg->getValues()[2]), std::stoi(msg->getValues()[3]), std::stoi(msg->getValues()[4]))) /// probably not the right way to access fields in message
		{
			//adding new room to room list
			pair<int, Room*> newRoom(msg->getUser()->getRoom()->getId(), msg->getUser()->getRoom());
			_roomList.insert(newRoom);
			return true;
		}
		else
		{
			_roomidSequence--;
			return false;
		}
	}
	else
		return false;
}

/*handle room closing request*/
bool TriviaServer::handleCloseRoom(RecivedMessage* msg)
{
	//checking that the user is in a room
	if (msg->getUser()->getRoom())
	{
		//closing the room and removing the room from the room list
		if (msg->getUser()->closeRoom() != -1)
		{
			_roomList.erase(_roomList.find(msg->getUser()->getRoom()->getId()));
			return true;
		}
	}
	//case user isn't in a room
	else
		return false;
}

/*handle room joining request*/
bool TriviaServer::handleJoinRoom(RecivedMessage* msg)
{
	//checking that the user is linked to the message
	if (msg->getUser())
	{
		msg->getUser()->joinRoom((_roomList.find(stoi(msg->getValues()[1]))->second)); /// possibly would cause an error (not the correct way to access these fields)
	}
	else
		return false;
}

/*handle room leaving request*/
bool TriviaServer::handleLeaveRoom(RecivedMessage* msg)
{
	//checking that the user is linked to the message
	if (msg->getUser())
	{
		//checking that the user is in a room
		if (msg->getUser()->getRoom())
		{
			msg->getUser()->leaveRoom();
			return true;
		}

		//case user isn't ain a room
		else
			return false;
	}
	//case user isn't linked to the message 
	else
		return false;
}

/*handle users in room request*/
void TriviaServer::handleGetUserinRoom(RecivedMessage* msg)
{
	//checking that the requesting user is in the room
	if (getRoomByid(stoi(msg->getValues()[1]))) /// probably not the correct way to access roomID
	{
		msg->getUser()->send( msg->getUser()->getRoom()->getUserListMessage()); /// critical line, might cause problems
	}
	//if room wasnt found, send fail message to user (1080)
	else
		msg->getUser()->send(to_string(USER_IN_ROOM_RESPONSE) + "0");
}

/*handle room list request*/
void TriviaServer::handleGetRooms(RecivedMessage* msg)
{
	//checking that there are rooms in the room list
	if (_roomList.size())
	{
		string message = to_string(ROOM_LIST_RESPONSE) + to_string(_roomList.size());

		for (map<int, Room*>::iterator i = _roomList.begin(); i != _roomList.end(); i++)
		{
			message += to_string(i->first); // adding current room's ID
			message += i->second->getName(); // aadding current room's name
		}

		msg->getUser()->send(message);
	}
}

/*handle best scores request*/
void TriviaServer::handleGetBestScores(RecivedMessage* msg)
{
	///Note: will be able to do after DataBase will be finished
}

/*handle personal status request*/
void TriviaServer::handleGetPersonalStatus(RecivedMessage* msg)
{

}

/*push message to message queue*/
void TriviaServer::addRecivedMessage(RecivedMessage* msg)
{
	unique_lock<mutex> lck(_mtxRecivedMessages);

	/***********lock mutex***********/
	lck.lock();

	_queRcvMessages.push(msg);

	/*********unlock mutex**********/
	lck.unlock();
}

/*building a new message*/
RecivedMessage* TriviaServer::buildRecivedMessage(SOCKET client_sock, int msgCode)
{
	string vals = Helper::getPartFromSocket(client_sock, DEFAULT_BUFLEN, 0);
	vector<string> splited_vals;
	int pos = 0;

	//adding the values
	switch (msgCode)
	{
	//case of sign in message
	case SIGN_IN_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); //message number
		pos = pushRange(vals, splited_vals, pos + 2, pos + 2 + (stoi(vals.substr(pos, 2)))); // username
		pos = pushRange(vals, splited_vals, pos + 2, pos + 2 + (stoi(vals.substr(pos, 2)))); // password
		break;
	//case of sign out message
	case SIGN_OUT_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); // message number
		break;
	//case of sign up message
	case SIGN_UP_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); // message number
		pos = pushRange(vals, splited_vals, pos + 2, pos + 2 + stoi(vals.substr(pos, 2))); //username
		pos = pushRange(vals, splited_vals, pos + 2, pos + 2 + stoi(vals.substr(pos, 2))); // password
		pos = pushRange(vals, splited_vals, pos + 2, pos + 2 + stoi(vals.substr(pos, 2))); // email
		break;
	//case of room list message
	case ROOM_LIST_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); // message number
		break;
	//case of user list in room message
	case USER_IN_ROOM_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); // message number
		pos = pushRange(vals, splited_vals, pos, pos + 3); // roomID
		break;
	//case of room join message
	case JOIN_ROOM_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); // message number
		pos = pushRange(vals, splited_vals, pos, pos + 3); // roomID
		break;
	//case of room leaving message
	case LEAVE_ROOM_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); // message number
		break;
	//case of room creation message
	case CREATE_ROOM_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); // message number
		pos = pushRange(vals, splited_vals, pos + 2, pos + 2 + stoi(vals.substr(pos, 2))); //room name
		pos = pushRange(vals, splited_vals, pos, pos); // number of players
		pos = pushRange(vals, splited_vals, pos, pos + 1); // number of question
		pos = pushRange(vals, splited_vals, pos, pos + 1); //time to answer question in seconds
		break;
	//room closing message
	case CLOSE_ROOM_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); // message number
		break;
	//case of game starting message
	case START_GAME_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); // message number
		break;
	//case of answer message
	case ANSWER_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); // message number
		pos = pushRange(vals, splited_vals, pos, pos); // answer number
		pos = pushRange(vals, splited_vals, pos, pos + 1); // answer time
		break;
	//case of game leaving message
	case LEAVE_GAME_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); // message number
		break;
	//case of top scores message
	case BEST_SCORES_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); // message number
		break;
	//case of personal status message
	case PERSONAL_STATUS_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); // message number
		break;
	//case of quit message
	case QUIT_REQUEST:
		pos = pushRange(vals, splited_vals, pos, 2); // message number 
		break;
	}
	RecivedMessage *msg = new RecivedMessage(client_sock, msgCode, splited_vals);
	return msg;
}

/*pushing values from string in the given range into the vector*/
int pushRange(string str, vector<string>& vec,  int first, int last)
{
	vec.push_back(str.substr(first, (last + 1) - (first + 1)));
	return last + 1;
}