#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "patch.h"

namespace Keyboard
{

void Setup();
bool Scan();
void ApplyPatch(const Patch &patch);

} // namespace Keyboard

#endif // KEYBOARD_H
