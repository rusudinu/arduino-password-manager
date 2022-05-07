#include <LiquidCrystal.h>
#include <IRremote.h>
#include <EEPROM.h>

#define CONTRAST 30
#define PASSWORD_COVER "*"

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);

struct state {
    bool stateChanged = true;
    bool isLocked = true;
    bool hidePassword = true;
    int32_t debuggingLevelEnabled = 2; // 0 -> no logs, 1 -> warning, 2 -> warning + info
    String display = "LOCKED ";
    String displayRow2 = "unlock: ";
    String input = "";
    String password = "";
    String secrets = "my awful secrets";
} currentState;

void printDebugInfoMessage(const String &message) {
    if (currentState.debuggingLevelEnabled > 1) {
        Serial.println("[INFO] " + message);
    }
}

void printDebugWarningMessage(const String &message) {
    if (currentState.debuggingLevelEnabled > 0) {
        Serial.println("[WARNING] " + message);
    }
}

void setState(bool isLocked, int32_t debuggingEnabled, const String &display, const String &displayRow2, const String &input = currentState.input, const String &password = currentState.password, const String &secrets = currentState.secrets) {
    printDebugWarningMessage("setState(" + String(isLocked) + ", " + String(debuggingEnabled) + ", " + display + ", " + displayRow2 + ", " + input + ", " + password + ", " + secrets + ")");
    if (isLocked != currentState.isLocked) {
        currentState.isLocked = isLocked;
        currentState.stateChanged = true;
    }
    if (debuggingEnabled != currentState.debuggingLevelEnabled) {
        currentState.debuggingLevelEnabled = debuggingEnabled;
        currentState.stateChanged = true;
    }
    if (display.compareTo(currentState.display) != 0) {
        currentState.display = display;
        currentState.stateChanged = true;
    }
    if (displayRow2.compareTo(currentState.displayRow2) != 0) {
        currentState.displayRow2 = displayRow2;
        currentState.stateChanged = true;
    }
    if (input.compareTo(currentState.input) != 0) {
        currentState.input = input;
        currentState.stateChanged = true;
    }
    if (password.compareTo(currentState.password) != 0) {
        currentState.password = password;
        currentState.stateChanged = true;
    }
    if (secrets.compareTo(currentState.secrets) != 0) {
        currentState.secrets = secrets;
        currentState.stateChanged = true;
    }
}

void writeToDisplay(const String &word, bool append = true, bool firstRow = true, bool appendSpace = true) {
    if (firstRow) {
        setState(currentState.isLocked, currentState.debuggingLevelEnabled, append ? String(currentState.display + word + (appendSpace ? " " : "")) : String(word + (appendSpace ? " " : "")), currentState.displayRow2);
    } else {
        setState(currentState.isLocked, currentState.debuggingLevelEnabled, currentState.display, append ? String(currentState.displayRow2 + word + (appendSpace ? " " : "")) : String(word + (appendSpace ? " " : "")));
    }
}

void clearLCDLine(int line) {
    lcd.setCursor(0, line);
    for (int n = 0; n < 16; ++n) {
        lcd.print(" ");
    }
}

void flushDisplay() {
    printDebugInfoMessage("Flushing display");
    if (currentState.stateChanged) {
        printDebugInfoMessage("Flushing display - currentState change found");
        clearLCDLine(0);
        lcd.setCursor(0, 0);
        lcd.print(currentState.display);
        clearLCDLine(1);
        lcd.setCursor(0, 1);
        lcd.print(currentState.displayRow2);
        currentState.stateChanged = false;
    } else {
        printDebugInfoMessage("Flushing display - no currentState change found");
    }
}

String decodeRemoteCode(uint32_t code) {
    switch (code) {
        case 3125149440 : {
            return "CH-";
        }
        case 3108437760 : {
            return "CH";
        }
        case 3091726080 : {
            return "CH+";
        }
        case 3141861120 : {
            return "|<<";
        }
        case 3208707840 : {
            return ">>|";
        }
        case 3158572800 : {
            return ">||";
        }
        case 3927310080 : {
            return "VOL+";
        }
        case 4161273600: {
            return "VOL-";
        }
        case 4127850240 : {
            return "EQ";
        }
        case 3910598400 : {
            return "0";
        }
        case 3860463360 : {
            return "FOL-";
        }
        case 4061003520 : {
            return "FOL+";
        }
        case 4077715200 : {
            return "1";
        }
        case 3877175040 : {
            return "2";
        }
        case 2707357440 : {
            return "3";
        }
        case 4144561920 : {
            return "4";
        }
        case 3810328320 : {
            return "5";
        }
        case 2774204160 : {
            return "6";
        }
        case 3175284480 : {
            return "7";
        }
        case 2907897600 : {
            return "8";
        }
        case 3041591040 : {
            return "9";
        }
        default: {
            return "UNKNOWN";
        }
    }
}

void generateFactoryPassword() {
    randomSeed(analogRead(0));
    String password = String(random(100000, 999999));
    currentState.password = password;
    writeToDisplay(password, true, false);
}

void initDisplay() {
    lcd.begin(16, 2);
    analogWrite(6, CONTRAST);
}

void initRemote() {
    irrecv.enableIRIn();
}

void setup() {
    Serial.begin(9600);
    initDisplay();
    initRemote();
    // TODO call this only if no default password is set in EEPROM
    generateFactoryPassword();
}

/*
 *
 * the program will have 4 states:
 * - [unlocked] lock system
 * - [unlocked] delete passwords (if any & will revert to a base password)
 * - [unlocked] add new password
 * - [locked] unlock system
 *
 * and also 2 modes:
 * locked / unlocked (bool)
 *
 */

void loop() {
    if (irrecv.decode()) {
        if (irrecv.decodedIRData.decodedRawData != 0) {
            String decodedValue = decodeRemoteCode(irrecv.decodedIRData.decodedRawData);
            printDebugInfoMessage("Received IR data: " + String(irrecv.decodedIRData.decodedRawData));
            printDebugInfoMessage("Decoded IR data: " + decodedValue);
            if (decodedValue.compareTo("UNKNOWN") != 0) {
                setState(currentState.isLocked, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2, String(currentState.input + decodedValue));
                if (currentState.isLocked) {
                    writeToDisplay(currentState.hidePassword ? PASSWORD_COVER : decodedValue, true, true, false);
                    setState(currentState.isLocked, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2);
                    if (currentState.input.length() > currentState.password.length()) {
                        printDebugInfoMessage("Password too long, resetting");
                        setState(currentState.isLocked, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2, "");
                    } else if (currentState.input.length() == currentState.password.length()) {
                        if (currentState.input.compareTo(currentState.password) == 0) {
                            printDebugInfoMessage("Unlocked");
                            writeToDisplay("UNLOCKED", false, true);
                            writeToDisplay(currentState.secrets, false, false);
                            setState(false, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2, "");
                        } else {
                            printDebugInfoMessage("Password incorrect, resetting");
                            setState(currentState.isLocked, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2, "");
                        }
                    }
                }
            } else {
                printDebugWarningMessage("Unknown IR code received: " + String(irrecv.decodedIRData.decodedRawData));
            }
        }
        irrecv.resume();
    }

    if (currentState.stateChanged) {
        flushDisplay();
    }
}