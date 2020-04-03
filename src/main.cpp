#include <Arduino.h>
#include "keyboard.h"

void setup()
{
	PresetControl::Setup();
	Keyboard::Setup();
}

void loop()
{
	PresetControl::Scan();
	Keyboard::Scan();
}
