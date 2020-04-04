#include <MIDI.h>

#include "serialmidi.h"

namespace MIDIBoard
{

namespace
{
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, serialMIDI);
}

void SetupMIDI()
{
	serialMIDI.begin(MIDI_CHANNEL_OMNI);
}

void SendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel)
{
	usbMIDI.sendNoteOn(note, velocity, channel);
	serialMIDI.sendNoteOn(note, velocity, channel);
}

void SendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel)
{
	usbMIDI.sendNoteOff(note, velocity, channel);
	serialMIDI.sendNoteOff(note, velocity, channel);
}

void SendSysEx(uint32_t length, const uint8_t *data)
{
	usbMIDI.sendSysEx(length, data, true);
	serialMIDI.sendSysEx(length, data, true);
}

} // namespace MIDIBoard
