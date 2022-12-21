#ifndef PROVEL_ACTION_HPP
#define PROVEL_ACTION_HPP

#include <General/String.h>
#include <General/StringFunctions.h>

namespace Provel {

class Action {

public:
	Action();
	~Action();

	virtual int Execute();
};

class ActionGcode : public Action {
	const char *buffer;

public:
	ActionGcode(const char *buffer);
	~ActionGcode();

	virtual int Execute();
};

}

#endif
