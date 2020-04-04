#include <Arduino.h>
#include "keyboard.h"

void setup()
{
	PatchControl::Setup();
	Keyboard::Setup();
}

void loop()
{
	if (PatchControl::Scan())
		Keyboard::ApplyPatch(PatchControl::CurrentPatch());
	Keyboard::Scan();
}
