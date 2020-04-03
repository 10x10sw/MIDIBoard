#ifndef PRESET_H
#define PRESET_H

#include <vector>

// this is the Preset for a single button or key
struct Preset
{

	// MIDI note number
	char note = -1;
	char velocity = 100;
	bool hold = false; // i.e. not momentary

	char channel = 1;

	// sysex
	// other
};

using KeyboardPreset = std::vector<Preset>;

namespace PresetControl
{

void Setup();
bool Scan();
KeyboardPreset CurrentPreset();

} // namespace PresetControl

#endif // PRESET_H