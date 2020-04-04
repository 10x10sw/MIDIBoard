#include <Arduino.h>
#include <Bounce2.h>

#include "pins.h"
#include "patch.h"

namespace MIDIBoard
{
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

static void InstallPatch()
{
	Patch patch;
	Function p;

	// p.sysex = [](bool isKeyDown) {
	// 	static Sysex syx;
	// 	return syx;
	// };

	p.note = 64;
	patch.keyButtonFunctions.push_back(p);
	p.note = 68;
	patch.keyButtonFunctions.push_back(p);
	p.note = 71;
	patch.keyButtonFunctions.push_back(p);

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

	InstallPatch();
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