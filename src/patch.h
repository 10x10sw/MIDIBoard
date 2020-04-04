#ifndef PATCH_H
#define PATCH_H

#include <functional>
#include <memory>
#include <vector>

namespace MIDIBoard
{

// A patch function is a function that transmits some kind of MIDI on gUsbMIDI and gSerialMIDI when its send function is called.
// This might be one or more note on messages, note off messages, continuous controller, or system exclusive. 
// The contents of the MIDI transmission might vary randomly or over time. 
// The int32_t parameter represents:
// - key down (kKeyDown) and key up (kKeyUp) for buttons/switches
// - input for continuous controllers (INT_MIN to INT_MAX)

struct Function
{
	// set a send function to do something when called
	using Sender = std::function<bool(int32_t)>;
	Sender send = NULL;

	// for keys/switches/buttons: default to momentary, like a keyboard; non-latching
	bool latch = false; 

	static const int32_t kKeyDown;
	static const int32_t kKeyUp;
};

struct Patch
{
	std::vector<Function> keyboardFunctions;
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