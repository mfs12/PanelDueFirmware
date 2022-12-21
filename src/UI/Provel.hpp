#ifndef UI_PROVEL_HPP
#define UI_PROVEL_HPP 1

#include "UI/ColourSchemes.hpp"
#include "UI/Display.hpp"
#include "UI/ProvelAction.hpp"
#include "UI/UserInterfaceConstants.hpp"

#include "Version.hpp"

namespace Provel {

class Screen;
class Element;

class Touch {
public:
	uint16_t x;
	uint16_t y;
	enum State {
		Released = 0,
		Pressed = 1,
		Repeated = 2
	} state;

	Touch(uint16_t x, uint16_t y, enum State state) :
		x(x), y(y), state(state) {};
};

// generic classes
class Provel {

private:
	Screen *screens;

	const ColourScheme *colours;

public:
	Provel();
	~Provel();

	// window stack functions
	Screen *Push(Screen *screen);
	Screen *Pop();
	Screen *Reset(Screen *screen);

	// event functions
	int Update();
	int ProcessTouch(Touch &event);
};

class Screen {
	MainWindow root;
	Element *elements;

public:
	Screen *next;
	Screen *prev;

	// add and delete ui elements from screen
	int Add(Element *element);
	int Delete(Element *element);

	Element *Find(int x, int y);

	// init or shutdown state machine
	virtual int Init(const ColourScheme *colours);
	virtual int Update();
	virtual int Shutdown();

	Screen() : root() {
		next = nullptr;
		prev = nullptr;
		elements = nullptr;
	};
	~Screen() {};
};

class Element {

public:
	Element *next;

	virtual int Init() { return 0; };
	virtual int Shutdown() { return 0; };

	virtual int ProcessTouch(Touch &event);
	virtual int Update() { return 0; };

	virtual DisplayField *Get() { return nullptr; };

	Element() {
		next = nullptr;
	};
	~Element();
};

class Button : public Element {
	TextButton button;
	Action *action;

public:
	Button(
		PixelNumber x, PixelNumber y,
		PixelNumber width, PixelNumber height,
		const char *text, event_t e, int param,
		Action *action
	) : button(y, x, width, height, text, e, param), action(action) {
		button.SetColours(UTFT::fromRGB(0xff, 0xff, 0xff), UTFT::fromRGB(0x8a, 0x8a, 0x8a));
	};
	Button(
		PixelNumber x, PixelNumber y,
		PixelNumber width, PixelNumber height,
		const char *text, event_t e, int param
	) : Button(x, y, width, height, text, e, param, nullptr) {};
	~Button() {};

	DisplayField *Get() { return &button; };
	int ProcessTouch(Touch &event);
};

class ButtonDouble : public Element {
	DisplayField group;
	TextButton button_left;
	TextButton button_right;

public:
	ButtonDouble(
			PixelNumber x=0, PixelNumber y=0,
			PixelNumber width=100, PixelNumber height=100,
			const char *text_left="btn: LEFT", const char *text_right="btn: RIGHT"
	);

	DisplayField *Get() { return &group; };
	int ProcessTouch(Touch &event);
};

class Text : public Element {
	StaticTextField text;

public:
	Text(
		PixelNumber x=0, PixelNumber y=0,
		PixelNumber width=100, PixelNumber height=100,
		const char *ptext="text");
	~Text() {};

	DisplayField *Get() { return &text; };
};

class Title : public Element {
	StaticTextField text;

public:
	Title(
		PixelNumber x=0, PixelNumber y=0,
		PixelNumber width=100, PixelNumber height=100,
		const char *title="title");
	~Title();

	DisplayField *Get() { return &text; };
};

class Status : public Element {
public:
	enum State {
		Unknown = UTFT::fromRGB(0x50, 0x50, 0x50), // #505050
		Normal = UTFT::fromRGB(0x00, 0xf9, 0x00), // #00f900
		Warning = UTFT::fromRGB(0xff, 0xfb, 0x00), // #fffb00
		Error = UTFT::fromRGB(0xe6, 0x24, 0x01), // #e62401
	};

private:
	ColourField status;

	enum State state;

public:
	Status(PixelNumber px, PixelNumber py, PixelNumber pw, PixelNumber ph, enum State state=Normal) :
		status(py, px, pw, ph), state(state) {
		status.SetColours(state, state);
	};
	~Status() {};

	void SetState(enum State val) { state = val; };
	enum State GetState() { return state; };

	DisplayField *Get() { return &status; }

	virtual int Init() { return 0; };
	virtual int Shutdown() { return 0; };

	virtual int Update() { return 0; };
};

}

#endif
