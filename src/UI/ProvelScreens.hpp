#ifndef UI_PROVEL_SCREENS_HPP
#define UI_PROVEL_SCREENS_HPP 1

#include "Provel.hpp"
#include "ProvelAction.hpp"

namespace Provel {

class ScreenSplash : public Screen {
	Title *title;
	Title *version;
	int timeout; // in ms

public:
	ScreenSplash();
	~ScreenSplash();
};

class ScreenHoming : public Screen {
	Title *title;
	Button *stop;
	Status *status;

public:
	ScreenHoming();
	~ScreenHoming();
};

class ScreenLoading : public Screen {
	Title *title;
	Button *cancel;

public:
	ScreenLoading();
	~ScreenLoading();
};

class ScreenFileLoaded : public Screen {
	Title *title;
	Text *acme;
	Text *cup;
	Text *nozzle;
	Text *time;
	Text *copoly;
	ButtonDouble *startOrClear;
	Status *status;

public:
	ScreenFileLoaded();
	~ScreenFileLoaded();
};

class ScreenIdle : public Screen {
	Title *title;
	Button *load;
	Button *home;
	Button *printer;
	Button *purge;

public:
	ScreenIdle();
	~ScreenIdle();
};

class ScreenPrinting : public Screen {
	Title *title;
	Text *acme;
	Text *time;
	Status *status;

public:
	ScreenPrinting();
	~ScreenPrinting();
};

class ScreenHeating : public Screen {
	Title *title;
	Text *acme;
	Text *print;
	Text *set;
	Text *cup;
	Text *set2;
	Button *stop;
	Status *status;

public:
	ScreenHeating();
	~ScreenHeating();
};

class ScreenZCalibrate : public Screen {
	Title *title;
	Text *offset;
	ButtonDouble *plusMinus;
	ButtonDouble *enterExit;

public:
	ScreenZCalibrate();
	~ScreenZCalibrate();
};

class ScreenPurging : public Screen {
	Title *title;
	Text *temp;
	Text *set;
	Text *time;
	Button *stop;
	Status *status;

public:
	ScreenPurging();
	~ScreenPurging();
};

class ScreenPrinter : public Screen {
	Title *title;
	Text *temp;
	ButtonDouble *plusMinus;
	Button *zCalibrate;
	Button *indexCup;
	Button *access;
	ButtonDouble *enterExit;

public:
	ScreenPrinter();
	~ScreenPrinter();
};

class ScreenFault : public Screen {
	Title *title;
	Text *text;
	Button *clear;
	Status *status;

public:
	ScreenFault();
	~ScreenFault();
};

class ScreenWarning : public Screen {
	Title *title;
	Text *text;
	Status *status;

public:
	ScreenWarning();
	~ScreenWarning();
};

class ScreenGcode : public Screen {
	Title *title;
	Button *gcode;
	ActionGcode *action;

public:
	ScreenGcode();
	~ScreenGcode();
};

}
#endif
