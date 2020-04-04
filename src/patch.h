#ifndef PATCH_H
#define PATCH_H

#include <vector>
#include <functional>

#define NO_MIDI_NOTE -1

struct Sysex
{
	const uint32_t length = 0;
	const uint8_t *data = NULL;
};

// bool is true on key down, false on key up
using SysexGenerator = std::function<Sysex(bool)>;

// this is the Preset for a single button or key
struct MIDIFunction
{
	bool hold = false; // i.e. not momentary

	uint8_t channel = 1;

	// MIDI note number 0-127; NO_MIDI_NOTE (-1) means don't send any note on/off
	int8_t note = NO_MIDI_NOTE;
	int8_t velocity = 100;

	// sysex
	SysexGenerator sysex = NULL;

	// other

	bool HasNote() const { return note != NO_MIDI_NOTE; }
	bool HasSysex() const { return sysex != NULL; }

};

using Patch = std::vector<MIDIFunction>;

namespace PatchControl
{

void Setup();
bool Scan();
void RegisterPatch(const Patch& patch);
Patch CurrentPatch();

} // namespace PresetControl

#endif // PATCH_H