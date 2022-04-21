# lufa_p2io
Barebones Arduino-based P2IO clone

Based on [LUFA_DualVirtualSerial](https://github.com/Palatis/Arduino-Lufa/tree/master/examples/LUFA_DualVirtualSerial).

This is not meant to be a finished implementation. It's just enough to get Python 2 games booting on a real PS2. You must implement other things such as I/O yourself.

An example I/O implementation using PsxNewLib to add support for all games using a Playstation 2 controller is included in `input_task.ino`. Replace this code with whatever implementation you wish for your input. Some individual devices like `thrilldrive_belt.cpp` and `icca.cpp` have places that you must also add special keybinds to for certain interactions like the card reader keypad and card insert/eject toggle.

Modify `config.h` to include dipswitch settings (or rewrite the code to use a real dipswitch!), dongle payloads, and card IDs as required. You must also switch the `GAME_TYPE` defined in `config.h` to change game configurations.

## Requirements
- Arduino-Lufa

