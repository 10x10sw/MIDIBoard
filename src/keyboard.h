#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "preset.h"

namespace Keyboard
{

void Setup();
bool Scan();
void ApplyPreset(const KeyboardPreset &keyboardPreset);

} // namespace Keyboard

#endif // KEYBOARD_H
