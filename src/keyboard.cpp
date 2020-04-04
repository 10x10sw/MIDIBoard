#include <Arduino.h>
#include <Bounce2.h>
#include <MIDI.h>

#include "keyboard.h"
#include "pins.h"

namespace Keyboard
{
namespace
{
// Serial MIDI
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

static const uint8_t kKeyButtonPins[] = {KEYBTN1, KEYBTN2, KEYBTN3, KEYBTN4, KEYBTN5, KEYBTN6, KEYBTN7, KEYBTN8};
static const uint8_t kKeyLedPins[] = {KEYLED1, KEYLED2, KEYLED3, KEYLED4, KEYLED5, KEYLED6, KEYLED7, KEYLED8};

struct KeyButton
{
	uint8_t ledPin;
	Bounce debouncer;
	MIDIFunction midi;
	bool keyIsDown = false;

	KeyButton(const int b, const int l, const MIDIFunction &p)
		: ledPin(l), midi(p)
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

		if (midi.HasNote())
		{
			usbMIDI.sendNoteOn(midi.note, midi.velocity, midi.channel);
			MIDI.sendNoteOn(midi.note, midi.velocity, midi.channel);
		}
		if (midi.HasSysex())
		{
			Sysex syx = midi.sysex(true);
			if (syx.length > 0)
			{
				usbMIDI.sendSysEx(syx.length, syx.data, true);
				MIDI.sendSysEx(syx.length, syx.data, true);
			}
		}

		return true;
	}

	bool KeyUp()
	{
		keyIsDown = false;
		digitalWrite(ledPin, LOW);

		if (midi.HasNote())
		{
			usbMIDI.sendNoteOff(midi.note, midi.velocity, midi.channel); // [TBD] note-off velocity?
			MIDI.sendNoteOff(midi.note, midi.velocity, midi.channel);	 // [TBD] note-off velocity?
		}
		if (midi.HasSysex())
		{
			Sysex syx = midi.sysex(false);
			if (syx.length > 0)
			{
				usbMIDI.sendSysEx(syx.length, syx.data, true);
				MIDI.sendSysEx(syx.length, syx.data, true);
			}
		}

		return true;
	}

	bool Scan()
	{
		debouncer.update();
		if (midi.hold)
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

std::vector<KeyButton *> gKeyButtons;

} // anonymous namespace

void Setup()
{
	ApplyPatch(PatchControl::CurrentPatch());

	// Serial MIDI
	MIDI.begin(MIDI_CHANNEL_OMNI);
	Serial.begin(57600);
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
	for (const auto &p : patch)
	{
		if (i >= sizeof(kKeyButtonPins))
		{
			Serial.printf("error: patch too big for hardware: %d\n", i);
			return;
		}
		gKeyButtons.push_back(new KeyButton(kKeyButtonPins[i], kKeyLedPins[i], p));
		i++;
	}
}

} // namespace Keyboard
