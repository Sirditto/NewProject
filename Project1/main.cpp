#include "TriviaServer.h"

using namespace std;

int main(void)
{
	TriviaServer *s = new TriviaServer();
	s->server();

	return 0;
}