#include "preset.h"

KeyboardPreset InitialPreset()
{
	KeyboardPreset keyboardPreset;
	Preset p;

	p.note = 64;
	keyboardPreset.push_back(p);
	p.note = 68;
	keyboardPreset.push_back(p);
	p.note = 71;
	keyboardPreset.push_back(p);

	return keyboardPreset;
}
