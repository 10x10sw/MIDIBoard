#include <Arduino.h>
#include "keyboard.h"

void setup()
{
	PresetControl::Setup();
	Keyboard::Setup();
}

void loop()
{
	if (PresetControl::Scan())
		Keyboard::ApplyPreset(PresetControl::CurrentPreset());
	Keyboard::Scan();
}
