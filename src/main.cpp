#include <Arduino.h>
#include "keyboard.h"

void setup()
{
	Serial.println("TeensyLC MIDI Control");
	Keyboard::Setup();
}

void loop()
{
	Keyboard::Scan();
}
