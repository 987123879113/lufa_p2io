# lufa_p2io
Barebones Arduino-based P2IO clone

Based on [LUFA_DualVirtualSerial](https://github.com/Palatis/Arduino-Lufa/tree/master/examples/LUFA_DualVirtualSerial).

This is not meant to be a finished implementation. It's just enough to get Python 2 games booting on a real PS2. You must implement other things such as I/O yourself.

An example I/O implementation using PsxNewLib to add support for a Playstation 2 controller in Dance Dance Revolution is included in `input_task.ino`. Replace this code with whatever implementation you wish for your input.