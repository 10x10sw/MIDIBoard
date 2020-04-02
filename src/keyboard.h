#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "preset.h"

namespace Keyboard
{

void Setup();
void ApplyPreset(const KeyboardPreset &keyboardPreset);
void Scan();

} // namespace Keyboard

#endif // KEYBOARD_H
