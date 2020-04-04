#include <Arduino.h>

#include "keyboard.h"
#include "serialmidi.h"

using namespace MIDIBoard;

void setup()
{
	SetupMIDI();
	PatchControl::Setup();
	Keyboard::Setup();
}

void loop()
{
	if (PatchControl::Scan())
		Keyboard::ApplyPatch(PatchControl::CurrentPatch());
	Keyboard::Scan();
	// drain the input
	if (usbMIDI.read()) {}
}
