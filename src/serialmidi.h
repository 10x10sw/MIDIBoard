#ifndef SERIALMIDI_H
#define SERIALMIDI_H

#include <MIDI.h>

namespace MIDIBoard
{

using SerialMIDI = midi::MidiInterface<HardwareSerial>;
extern SerialMIDI gSerialMIDI;

// for consistency:
#define gUsbMIDI usbMIDI

} // namespace MIDIBoard

#endif // SERIALMIDI_H
