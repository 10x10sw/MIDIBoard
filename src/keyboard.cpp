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

static const char kKeyButtonPins[] = {KEYBTN1, KEYBTN2, KEYBTN3};
static const char kKeyLedPins[] = {KEYLED1, KEYLED2, KEYLED3};

struct KeyButton
{
	char ledPin;
	Bounce debouncer;
	Preset preset;
	bool keyIsDown = false;

	KeyButton(const int b, const int l, const Preset &p)
		: ledPin(l), preset(p)
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

		if (preset.HasNote())
		{
			usbMIDI.sendNoteOn(preset.note, preset.velocity, preset.channel);
			MIDI.sendNoteOn(preset.note, preset.velocity, preset.channel);
		}
		if (preset.HasKeyDownSysex())
		{
			usbMIDI.sendSysEx(preset.keyDownSysex.size(), preset.keyDownSysex.data(), true);
			MIDI.sendSysEx(preset.keyDownSysex.size(), preset.keyDownSysex.data(), true);
		}

		return true;
	}

	bool KeyUp()
	{
		keyIsDown = false;
		digitalWrite(ledPin, LOW);

		if (preset.HasNote())
		{
			usbMIDI.sendNoteOff(preset.note, preset.velocity, preset.channel); // [TBD] note-off velocity?
			MIDI.sendNoteOff(preset.note, preset.velocity, preset.channel); // [TBD] note-off velocity?
		}
		if (preset.HasKeyUpSysex())
		{
			usbMIDI.sendSysEx(preset.keyUpSysex.size(), preset.keyUpSysex.data(), true);
			MIDI.sendSysEx(preset.keyUpSysex.size(), preset.keyUpSysex.data(), true);
		}

		return true;
	}

	bool Scan()
	{
		debouncer.update();
		if (preset.hold)
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
	Serial.println("Keyboard::Setup");
	ApplyPreset(PresetControl::CurrentPreset());

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

void ApplyPreset(const KeyboardPreset &keyboardPreset)
{
	if (keyboardPreset.size() > sizeof(kKeyButtonPins))
	{
		Serial.println("error: preset too big for hardware");
		return;
	}

	Serial.println("Keyboard::ApplyPreset");
	for (auto &k : gKeyButtons)
	{
		k->KeyUp();
	}

	gKeyButtons.clear();

	size_t i = 0;
	for (const auto &p : keyboardPreset)
	{
		gKeyButtons.push_back(new KeyButton(kKeyButtonPins[i], kKeyLedPins[i], p));
		i++;
	}
}

} // namespace Keyboard
