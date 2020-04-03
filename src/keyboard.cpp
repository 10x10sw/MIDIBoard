#include <Arduino.h>
#include <Bounce2.h>
#include <MIDI.h>

#include "keyboard.h"

namespace Keyboard
{

namespace
{

static const char kKeyButtonPins[] = {8, 12, 6};
static const char kKeyLedPins[] = {17, 19, 7};

struct KeyButton
{
	char ledPin;
	Bounce debouncer;
	Preset preset;
	bool noteIsOn = false;

	KeyButton(const int b, const int l, const Preset &p)
		: ledPin(l), preset(p)
	{
		pinMode(l, OUTPUT);
		digitalWrite(l, LOW);
		debouncer.attach(b, INPUT_PULLUP);
		debouncer.interval(25);
	}

	bool StartNote()
	{
		noteIsOn = true;
		digitalWrite(ledPin, HIGH);
		usbMIDI.sendNoteOn(preset.note, preset.velocity, preset.channel);
		return true;
	}

	bool StopNote()
	{
		noteIsOn = false;
		digitalWrite(ledPin, LOW);
		usbMIDI.sendNoteOff(preset.note, preset.velocity, preset.channel); // [TBD] note-off velocity?
		return true;
	}

	bool Scan()
	{
		debouncer.update();
		if (!preset.hold)
		{
			if (debouncer.rose())
			{
				return StopNote();
			}
			else if (debouncer.fell())
			{
				return StartNote();
			}
		}
		else
		{
			if (debouncer.fell())
			{
				if (noteIsOn)
				{
					return StopNote();
				}
				else
				{
					return StartNote();
				}
			}
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
		k->StopNote();
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
