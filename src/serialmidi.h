#ifndef SERIALMIDI_H
#define SERIALMIDI_H

namespace MIDIBoard
{

void SetupMIDI();

// these functions send on both USB and Serial
void SendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel);
void SendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel);
void SendSysEx(uint32_t length, const uint8_t *data);

} // namespace MIDIBoard

#endif // SERIALMIDI_H
