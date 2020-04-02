#include <Arduino.h>
#include <Bounce2.h>
#include <MIDI.h>
#include <vector>

struct HoldModeButton
{
	int buttonPin;
	Bounce debouncer;
	bool hold = false;

	HoldModeButton(const int b)
		: buttonPin(b)
	{
		debouncer.attach(b, INPUT_PULLUP);
		debouncer.interval(25);
	}

	void Scan()
	{
		debouncer.update();
		if (debouncer.fell())
		{
			hold = !hold;
		}
	}
};

struct NoteButton
{
	int buttonPin;
	int ledPin;
	int note;
	Bounce debouncer;
	bool noteIsOn = false;
	const HoldModeButton &holdButton;

	NoteButton(const int b, const int l, const int n, const HoldModeButton &m)
		: buttonPin(b), ledPin(l), note(n), holdButton(m)
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
		usbMIDI.sendNoteOn(note, 110, 1);
	}

	void StopNote()
	{
		noteIsOn = false;
		digitalWrite(ledPin, LOW);
		usbMIDI.sendNoteOff(note, 110, 1);
	}

	void Scan()
	{
		debouncer.update();
		if (!holdButton.hold)
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

std::vector<NoteButton *> gNoteButtons;
HoldModeButton gHoldModeButton(4);

void setup()
{
	Serial.println("TeensyLC MIDI Control");
	Serial.printf("gHoldModeButton.hold:%d", gHoldModeButton.hold);
	gNoteButtons.push_back(new NoteButton(8, 17, 64, gHoldModeButton));
	gNoteButtons.push_back(new NoteButton(12, 20, 68, gHoldModeButton));
	gNoteButtons.push_back(new NoteButton(6, 7, 71, gHoldModeButton));
}

void loop()
{
	for (auto c : gNoteButtons)
	{
		c->Scan();
	}

	bool m = gHoldModeButton.hold;
	gHoldModeButton.Scan();
	if (gHoldModeButton.hold != m) // all notes off if hold mode changes
	{
	Serial.printf("gHoldModeButton.hold:%d", gHoldModeButton.hold);
		for (auto c : gNoteButtons)
		{
			c->StopNote();
		}
	}
}
