#ifndef PATCH_H
#define PATCH_H

#include <vector>
#include <functional>

namespace MIDIBoard
{

#define kNoMIDINote -1
#define kKeyDown 1
#define kKeyUp -1

struct Sysex
{
	const uint32_t length = 0;
	const uint8_t *data = NULL;
};

// This int32_t parameter is used as input for continuous controllers and to indicate key down / key up.
// key down = kKeyDown (1), key up = kKeyUp (-1)
using SysexGenerator = std::function<Sysex(int32_t)>;

// this is the Preset for a single button, key, or continuous controller
struct Function
{

	uint8_t channel = 1;

	// MIDI note number 0-127; kNoMIDINote (-1) means don't send any note on/off
	bool hold = false; // for keys/switches/buttons: i.e. momentary, like a keyboard; non-latching
	int8_t note = kNoMIDINote;
	int8_t velocity = 100;

	// sysex
	SysexGenerator sysex = NULL;


	// helpers
	bool HasNote() const { return note != kNoMIDINote; }
	bool HasSysex() const { return sysex != NULL; }
};

struct Patch
{
	std::vector<Function> keyButtonFunctions;
	// TODO CC/foot controller functions
};

namespace PatchControl
{

void Setup();
bool Scan();
void RegisterPatch(const Patch& patch);
Patch CurrentPatch();

} // namespace PresetControl
} // namespace MIDIBoard

#endif // PATCH_H