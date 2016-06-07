#include "RecivedMessage.h"


/*constructor, initializes the fields.....*/
RecivedMessage::RecivedMessage(SOCKET sock, int messagecode) : _sock(sock), _messageCode(messagecode)
{
}

/*constructor, initializes the fields.....*/
RecivedMessage::RecivedMessage(SOCKET sock, int messageCode, vector<string>& vals) : _sock(sock), _messageCode(messageCode), _values(vals)
{
}

/*socket getter*/
SOCKET RecivedMessage::getSock()
{
	return _sock;
}

//user setter
void RecivedMessage::setUser(User* user)
{
	_user = user;
}

//message code getter
int RecivedMessage::getMessageCode()
{
	return _messageCode;
}

//values getter
vector<string>& RecivedMessage::getValues()
{
	return _values;
}

