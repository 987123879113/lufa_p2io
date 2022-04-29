# lufa_p2io
Barebones Arduino-based P2IO clone

Based on [LUFA_DualVirtualSerial](https://github.com/Palatis/Arduino-Lufa/tree/master/examples/LUFA_DualVirtualSerial).

This is not meant to be a finished implementation. It's just enough to get Python 2 games booting on a real PS2. You must implement other things such as I/O yourself.

Two examples of I/O have been provided: one using a USB Host Shield 2.0 (https://github.com/felis/USB_Host_Shield_2.0) to enable use of a PS4 controller, and another using PsxNewLib to provide support for PS1/PS2 controllers.

Modify `config_private.h` to include your game dongle information and card information.

A 1602 LCD Keypad Shield (https://www.dfrobot.com/product-51.html) can be used to provide an interactive interface to change the loaded game profile, perform card insertion/ejection and keypad operation, and change the dipswitch positions.
NOTE: For this keypad shield to work with the USB Host Shield 2.0, you must snip pin 9 that connects to the lower board and then bridge pin 11 to the "E" on the LCD board using a wire. This is because the USB Host Shield 2.0 uses pin 9 already. Optionally, you can use a breadboard or make your own shield to connect the two boards together without pin 9 conflicting.

## Requirements
- Arduino-Lufa
- USB Host Shield 2.0
- LiquidCrystal
