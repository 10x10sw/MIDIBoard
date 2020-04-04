#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "patch.h"

namespace MIDIBoard
{
namespace Keyboard
{

void Setup();
bool Scan();
void ApplyPatch(const Patch &patch);

} // namespace Keyboard
} // namespace MIDIBoard

#endif // KEYBOARD_H
