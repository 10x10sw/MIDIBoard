#ifndef PRESET_H
#define PRESET_H

#include <vector>

#define NO_MIDI_NOTE -1

// this is the Preset for a single button or key
struct Preset
{
	bool hold = false; // i.e. not momentary

	uint8_t channel = 1;

	// MIDI note number 0-127; NO_MIDI_NOTE (-1) means don't send any note on/off
	char note = NO_MIDI_NOTE;
	char velocity = 100;

	// sysex
	std::vector<uint8_t> keyDownSysex;
	std::vector<uint8_t> keyUpSysex;

	// other

	bool HasNote() const { return note != NO_MIDI_NOTE; }
	bool HasKeyDownSysex() const { return !keyDownSysex.empty(); }
	bool HasKeyUpSysex() const { return !keyUpSysex.empty(); }
};

using KeyboardPreset = std::vector<Preset>;

namespace PresetControl
{

void Setup();
bool Scan();
KeyboardPreset CurrentPreset();

} // namespace PresetControl

#endif // PRESET_H