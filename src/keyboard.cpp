#include <Arduino.h>
#include <Bounce2.h>
#include <MIDI.h>

#include "keyboard.h"

namespace Keyboard
{

namespace
{

static const char kKeyButtonPins[] = {8, 12, 6};
static const char kKeyLedPins[] = {17, 20, 7};

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

	void StartNote()
	{
		noteIsOn = true;
		digitalWrite(ledPin, HIGH);
		usbMIDI.sendNoteOn(preset.note, preset.velocity, preset.channel);
	}

	void StopNote()
	{
		noteIsOn = false;
		digitalWrite(ledPin, LOW);
		usbMIDI.sendNoteOff(preset.note, preset.velocity, preset.channel); // [TBD] note-off velocity?
	}

	void Scan()
	{
		debouncer.update();
		if (!preset.hold)
		{
			if (debouncer.rose())
			{
				StopNote();
			}
			else if (debouncer.fell())
			{
				StartNote();
			}
		}
		else
		{
			if (debouncer.fell())
			{
				if (noteIsOn)
				{
					StopNote();
				}
				else
				{
					StartNote();
				}
			}
		}
	}
};

std::vector<KeyButton *> gKeyButtons;

} // anonymous namespace

void Setup()
{
	Serial.println("Keyboard::Setup");
	ApplyPreset(InitialPreset());
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

void Scan()
{
	for (auto &c : gKeyButtons)
	{
		c->Scan();
	}
}

} // namespace Keyboard
