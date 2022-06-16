# Arduino password manager

## About

A password manager (similar to a physical crypto wallet), that can be interacted with using a remote. The user can save multiple passwords by using the menus of the wallet. On the LCD are displayed the saved passwords and the status. Passwords can be added, deleted and saved to the EEPROM. It is intended to be used with highly sensitive passwords such as banking passwords (card pins, etc) that should never touch the internet.

## General description

![alt text](https://github.com/xrusu/ma-lab/blob/master/documentation/diagrams/software.png?raw=true)
![alt text](https://github.com/xrusu/ma-lab/blob/master/documentation/diagrams/hardware.png?raw=true)

## Hardware design

![alt text](https://github.com/xrusu/ma-lab/blob/master/documentation/diagrams/electric_schematic_visual.png?raw=true)
![alt text](https://github.com/xrusu/ma-lab/blob/master/documentation/diagrams/electric_schematic_wireframe.png?raw=true)

## Software design

Developed using CLion (+PlatformIO) and Arduino IDE (to upload the code to the Arduino). On boot, the wallet displays a seed, that is used in the companion app to retrieve the password. After the password is entered in the wallet, the user can scroll trough the passwords, edit or delete a password or save a new one. After performing edits on the passwords, they can be saved by pressing FOL+ while in ADD_PASSWORD mode (mode 4). This is done in order to prevent too many writes to the EEPROM (after
100000 writes, the EEPROM will become damaged, hence an 'autosave' feature is not provided).

Modularity and reusability were key while writing the code for the wallet, in order to allow for ease of expansion, and addition of new features.

In order to easily debug the code and to trim 'spammy' logs in the case that not all of them are necessary, 3 logging levels exist (No logs, warning, warning & info), and 2 type of logs (Info & Warning). Logging is handled by:
<code>void printDebugInfoMessage(const String &message)</code>
and
<code>void printDebugWarningMessage(const String &message)</code>

In order to define the logging level, debuggingLevelEnabled from currentState must be changed to one of: NO_LOGS, WARNINGS, INFO.

To decode the signal from the remote, the <code>String decodeRemoteCode(uint32_t code)</code> function is used, that contains a switch that returns the letter that was pressed. I did not use a HashMap in order to save memory.

Each remote code is defined at the top of the program, in order to easily update the code in case that a different remote is used. Also, all other constants such as pins, the password cover, and the logging levels are defined in the same way.

The wallet has a custom-built state management system, in order to not refresh the display every time loop() is ran. The state of the wallet is kept in a struct, and if any field in that struct is changed, a state changed is triggered, and the display is refreshed. The state is handled by:
<code>void setState(int32_t state, int32_t debuggingEnabled, const String &display, const String &displayRow2, const String &input = currentState.input, const String &password = currentState.password, const String &secrets = currentState.secrets)</code>

In order to write to the display, a custom function is used, that handles a few write modes, such as appending text and overwriting, and also the line on which the text is going to be written to. Writing to the display is handled by:
<code>void writeToDisplay(const String &word, bool append = true, bool firstRow = true, bool appendSpace = true)</code>

The companion app is a cross-platform app, written in Flutter (a framework for building cross-platform apps developed by Google, on top of Dart) that allows the user to get the wallet password and view quick tutorials about each mode of the wallet.

## Demo

The device works as expected, passwords are saved and retrieved from EEPROM, there seem to be no bugs in the software, neither for the Arduino nor for the companion app.

![alt text](https://github.com/xrusu/ma-lab/blob/master/documentation/demo/10.jpg?raw=true)
![alt text](https://github.com/xrusu/ma-lab/blob/master/documentation/demo/9.jpg?raw=true)
![alt text](https://github.com/xrusu/ma-lab/blob/master/documentation/demo/8.jpg?raw=true)
![alt text](https://github.com/xrusu/ma-lab/blob/master/documentation/demo/7.jpg?raw=true)
![alt text](https://github.com/xrusu/ma-lab/blob/master/documentation/demo/6.jpg?raw=true)
![alt text](https://github.com/xrusu/ma-lab/blob/master/documentation/demo/5.jpg?raw=true)
![alt text](https://github.com/xrusu/ma-lab/blob/master/documentation/demo/4.jpg?raw=true)
![alt text](https://github.com/xrusu/ma-lab/blob/master/documentation/demo/3.jpg?raw=true)
![alt text](https://github.com/xrusu/ma-lab/blob/master/documentation/demo/2.jpg?raw=true)

You can find the documentation in the 'documentation' directory.

## Useful links

[Companion App](https://github.com/xrusu/ma-lab-companion)