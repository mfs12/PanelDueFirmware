#include "ProvelAction.hpp"

#include <cstring>

#define DEBUG 2
#include "Debug.hpp"

namespace Provel {

Action::Action()
{
}

Action::~Action()
{
}

int Action::Execute()
{
	dbg("Action\r\n");

	return 0;
}

ActionGcode::ActionGcode(const char *buffer) : buffer(buffer)
{
	dbg("buffer %s\r\n", buffer);
}

ActionGcode::~ActionGcode()
{
}

int ActionGcode::Execute()
{
	dbg("ActionGcode %s\r\n", buffer);
	if (!buffer)
	{
		dbg("gcode buffer missing\r\n");
		return -1;
	}

	size_t ret = SerialIo::Sendf(buffer);
	if (ret != strlen(buffer))
	{
		dbg("error sending gcode\r\n");
		return -1;
	}

	return 0;
}

}
