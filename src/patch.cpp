#include <Arduino.h>
#include <Bounce2.h>

#include "patch.h"
#include "pins.h"
#include "serialmidi.h"

namespace MIDIBoard
{

const int32_t Function::kKeyDown = 1;
const int32_t Function::kKeyUp = -1;

namespace PatchControl
{
namespace
{
static const uint8_t kIncButtonPin = INCBTN;
static const uint8_t kDecButtonPin = DECBTN;
static const uint8_t kLedPins[] = {PATCHLED1, PATCHLED2, PATCHLED3, PATCHLED4};

// 0001 0010 0100 1000  0011 0110 1100 1001  1011 0111 1110 1101
static const uint8_t kDisplayPatterns[] = {1, 2, 4, 8, 3, 6, 0xc, 9, 0xb, 7, 0xe, 0xd};

static Bounce gIncDebouncer;
static Bounce gDecDebouncer;

static std::vector<Patch> gPatches;
static uint8_t gCurrentPatch = 0;

static void DisplayPatch(uint8_t patch)
{
	uint8_t displayPattern = 0xf; // 1111
	if (patch < gPatches.size() && patch < sizeof(kDisplayPatterns))
		displayPattern = kDisplayPatterns[patch];
	digitalWrite(kLedPins[0], (displayPattern & 1) != 0);
	digitalWrite(kLedPins[1], (displayPattern & 2) != 0);
	digitalWrite(kLedPins[2], (displayPattern & 4) != 0);
	digitalWrite(kLedPins[3], (displayPattern & 8) != 0);
}

static bool SimpleSendNote(int32_t inKeyUpDown, uint8_t note, uint8_t velocity, uint8_t channel)
{
	if (inKeyUpDown == Function::kKeyDown)
		SendNoteOn(note, velocity, channel);
	else
		SendNoteOff(note, velocity, channel);
	return true;
}

static void InstallPreset()
{
	// preset 0 will play 8 notes from C0 to G0 with velocity 100 on channel 1
	Patch patch;
	Function p;
	for (uint8_t n = 48; n < 56; ++n)
	{
		p.send = [n](int32_t inKeyUpDown) -> bool {
			return SimpleSendNote(inKeyUpDown, n, 100, 1);
		};
		patch.keyboardFunctions.push_back(p);
	}
	RegisterPatch(patch);

	// preset 1 will play 8 notes from C0 to G0 with velocity 100 on channel 1 with latching (hold)
	for (auto &f : patch.keyboardFunctions)
		f.latch = true;
	RegisterPatch(patch);

	gCurrentPatch = 0;
	DisplayPatch(gCurrentPatch);
}

static bool IncrementPatch()
{
	if (++gCurrentPatch >= gPatches.size())
		gCurrentPatch = 0;
	DisplayPatch(gCurrentPatch);
	return true;
}

static bool DecrementPatch()
{
	if (gCurrentPatch == 0)
		gCurrentPatch = gPatches.size();
	--gCurrentPatch;
	DisplayPatch(gCurrentPatch);
	return true;
}

} // anonymous namespace

void Setup()
{
	gIncDebouncer.attach(kIncButtonPin, INPUT_PULLUP);
	gIncDebouncer.interval(25);
	gDecDebouncer.attach(kDecButtonPin, INPUT_PULLUP);
	gDecDebouncer.interval(25);

	for (uint8_t i = 0; i < sizeof(kLedPins); ++i)
		pinMode(kLedPins[i], OUTPUT);

	InstallPreset();
}

bool Scan()
{
	gIncDebouncer.update();
	if (gIncDebouncer.fell())
	{
		return IncrementPatch();
	}
	else
	{
		// [TODO] is it OK to only update in the else case?
		gDecDebouncer.update();
		if (gDecDebouncer.fell())
		{
			return DecrementPatch();
		}
	}
	return false;
}

void RegisterPatch(const Patch &patch)
{
	gPatches.push_back(patch);
}

Patch CurrentPatch()
{
	return gPatches[gCurrentPatch];
}

} // namespace PatchControl
} // namespace MIDIBoard