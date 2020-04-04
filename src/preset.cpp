#include <Arduino.h>
#include <Bounce2.h>

#include "pins.h"
#include "preset.h"

namespace PresetControl
{
namespace
{
static const char kIncButtonPin = PREINCBTN;
static const char kDecButtonPin = PREDECBTN;
static const char kLedPins[] = {PRELED1, PRELED2, PRELED3, PRELED4};

// 0001 0010 0100 1000  0011 0110 1100 1001  1011 0111 1110 1101
static const char kDisplayPatterns[] = {1, 2, 4, 8, 3, 6, 0xc, 9, 0xb, 7, 0xe, 0xd};

static Bounce gIncDebouncer;
static Bounce gDecDebouncer;

static std::vector<KeyboardPreset> gPresets;
static char gCurrentPreset = 0;

static void DisplayPreset(size_t preset)
{
	char displayPattern = 0xf; // 1111
	if (preset < gPresets.size() && preset < sizeof(kDisplayPatterns))
		displayPattern = kDisplayPatterns[preset];
	digitalWrite(kLedPins[0], (displayPattern & 1) != 0);
	digitalWrite(kLedPins[1], (displayPattern & 2) != 0);
	digitalWrite(kLedPins[2], (displayPattern & 4) != 0);
	digitalWrite(kLedPins[3], (displayPattern & 8) != 0);
}

static void InstallPresets()
{
	KeyboardPreset keyboardPreset;
	Preset p;

	p.note = 64;
	keyboardPreset.push_back(p);
	p.note = 68;
	keyboardPreset.push_back(p);
	p.note = 71;
	keyboardPreset.push_back(p);

	for (int i = 0; i < 13; ++i) // [TODO] remove loop
		gPresets.push_back(keyboardPreset);

	gCurrentPreset = 0;
	DisplayPreset(gCurrentPreset);
}

static bool IncrementPreset()
{
	if (++gCurrentPreset >= gPresets.size())
		gCurrentPreset = 0;
	DisplayPreset(gCurrentPreset);
	return true;
}

static bool DecrementPreset()
{
	if (gCurrentPreset == 0)
		gCurrentPreset = gPresets.size();
	--gCurrentPreset;
	DisplayPreset(gCurrentPreset);
	return true;
}

} // anonymous namespace

void Setup()
{
	Serial.println("PresetControl::Setup");

	gIncDebouncer.attach(kIncButtonPin, INPUT_PULLUP);
	gIncDebouncer.interval(25);
	gDecDebouncer.attach(kDecButtonPin, INPUT_PULLUP);
	gDecDebouncer.interval(25);

	for (size_t i = 0; i < sizeof(kLedPins); ++i)
		pinMode(kLedPins[i], OUTPUT);

	InstallPresets();
}

bool Scan()
{
	gIncDebouncer.update();
	if (gIncDebouncer.fell())
	{
		return IncrementPreset();
	}
	else
	{
		// [TODO] is it OK to only update in the else case?
		gDecDebouncer.update();
		if (gDecDebouncer.fell())
		{
			return DecrementPreset();
		}
	}
	return false;
}

KeyboardPreset CurrentPreset()
{
	return gPresets[gCurrentPreset];
}

} // namespace PresetControl
