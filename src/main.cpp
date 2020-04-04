#include <Arduino.h>
#include "keyboard.h"

namespace MIDIBoard
{
// i.e. MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, gSerialMIDI);
SerialMIDI gSerialMIDI(Serial1);
}
using namespace MIDIBoard;

void setup()
{
	gSerialMIDI.begin(MIDI_CHANNEL_OMNI);
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
