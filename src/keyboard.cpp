#include <Arduino.h>
#include <Bounce2.h>
#include <memory>

#include "keyboard.h"
#include "pins.h"

namespace MIDIBoard
{
namespace Keyboard
{
namespace
{

static const uint8_t kKeyButtonPins[] = {KEYBTN1, KEYBTN2, KEYBTN3, KEYBTN4, KEYBTN5, KEYBTN6, KEYBTN7, KEYBTN8};
static const uint8_t kKeyLedPins[] = {KEYLED1, KEYLED2, KEYLED3, KEYLED4, KEYLED5, KEYLED6, KEYLED7, KEYLED8};

struct KeyButton
{
	uint8_t ledPin;
	Bounce debouncer;
	const Function function; // TBD reference?
	bool keyIsDown = false;

	KeyButton(const int b, const int l, const Function &f)
		: ledPin(l), function(f)
	{
		pinMode(l, OUTPUT);
		digitalWrite(l, LOW);
		debouncer.attach(b, INPUT_PULLUP);
		debouncer.interval(25);
	}

	bool KeyDown()
	{
		keyIsDown = true;
		digitalWrite(ledPin, HIGH);

		if (function.HasNote())
		{
			gUsbMIDI.sendNoteOn(function.note, function.velocity, function.channel);
			gSerialMIDI.sendNoteOn(function.note, function.velocity, function.channel);
		}
		if (function.HasSysex())
		{
			Sysex syx = function.sysex(kKeyUp);
			if (syx.length > 0)
			{
				gUsbMIDI.sendSysEx(syx.length, syx.data, true);
				gSerialMIDI.sendSysEx(syx.length, syx.data, true);
			}
		}

		return true;
	}

	bool KeyUp()
	{
		keyIsDown = false;
		digitalWrite(ledPin, LOW);

		if (function.HasNote())
		{
			gUsbMIDI.sendNoteOff(function.note, function.velocity, function.channel);	   // [TBD] note-off velocity?
			gSerialMIDI.sendNoteOff(function.note, function.velocity, function.channel); // [TBD] note-off velocity?
		}
		if (function.HasSysex())
		{
			Sysex syx = function.sysex(kKeyDown);
			if (syx.length > 0)
			{
				gUsbMIDI.sendSysEx(syx.length, syx.data, true);
				gSerialMIDI.sendSysEx(syx.length, syx.data, true);
			}
		}

		return true;
	}

	bool Scan()
	{
		debouncer.update();
		if (function.hold)
		{
			if (debouncer.fell())
			{
				if (keyIsDown)
					return KeyUp();
				else
					return KeyDown();
			}
		}
		else
		{
			if (debouncer.rose())
				return KeyUp();
			else if (debouncer.fell())
				return KeyDown();
		}
		return false;
	}
};

using KeyButtonPtr = std::shared_ptr<KeyButton>;
std::vector<KeyButtonPtr> gKeyButtons;

} // anonymous namespace

void Setup()
{
	ApplyPatch(PatchControl::CurrentPatch());
}

bool Scan()
{
	bool result = false;
	for (auto &c : gKeyButtons)
	{
		result &= c->Scan();
	}
	return result;
}

void ApplyPatch(const Patch &patch)
{
	for (auto &k : gKeyButtons)
	{
		k->KeyUp();
	}

	gKeyButtons.clear();

	uint8_t i = 0;
	for (const auto &p : patch.keyButtonFunctions)
	{
		if (i >= sizeof(kKeyButtonPins))
		{
			Serial.printf("error: patch too big for hardware: %d\n", i);
			return;
		}
		gKeyButtons.push_back(KeyButtonPtr(new KeyButton(kKeyButtonPins[i], kKeyLedPins[i], p)));
		i++;
	}
}

} // namespace Keyboard
} // namespace MIDIBoard