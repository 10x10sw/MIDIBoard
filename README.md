# MIDIBoard
A flexible MIDI controller implemented on the [Teensy](https://www.pjrc.com/teensy/) development board.
This code has been written specifically to support building a MIDI foot controller. Eight or more foot controller buttons are SPST switches (stomp box switches); the switches can be set to momentary operation (like a keyboard) or to hold (latch). They can send MIDI notes or sysex when pressed and/or released or perform sysex functions that vary over time. Two foot controllers are supported. Two switches are used to navigate through the patches that are held in memory. 

## Supported Hardware
The initial implementation supports [Teensy LC](https://www.pjrc.com/store/teensylc.html). MIDI can be sent through USB or old-school serial 5-pin DIN MIDI. Only MIDI out is supported at this time. For an example of how to wire up serial MIDI to a Teensy, see [this example](https://www.pjrc.com/teensy/td_libs_MIDI.html).

This is a [PlatformIO](https://platformio.org/) project developed in [Visual Studio Code](https://code.visualstudio.com).  
