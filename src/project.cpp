#include <LiquidCrystal.h>
#include <IRremote.h>
#include <EEPROM.h>

#define CONTRAST 30

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);

struct state {
    bool stateChanged = true;
    bool isLocked = true;
    int32_t debuggingLevelEnabled = 1; // 0 -> no logs, 1 -> warning, 2 -> warning + info
    String display = "LOCKED";
    String displayRow2 = "remote unlock";
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

void appendWordToDisplay(const String &word, bool firstRow = true) {
    if (firstRow) {
        currentState.display = String(currentState.display + word + " ");
    } else {
        currentState.displayRow2 = String(currentState.displayRow2 + word + " ");
    }
    currentState.stateChanged = true;
}

void flushDisplay() {
    printDebugInfoMessage("Flushing display");
    if (currentState.stateChanged) {
        printDebugInfoMessage("Flushing display - currentState change found");
        lcd.setCursor(0, 0);
        lcd.print(currentState.display);
        lcd.setCursor(0, 1);
        lcd.print(currentState.displayRow2);
        currentState.stateChanged = false;
    } else {
        printDebugInfoMessage("Flushing display - no currentState change found");
    }
}

void setState(bool isLocked, int32_t debuggingEnabled, const String &display, const String &displayRow2) {
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

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    analogWrite(6, CONTRAST);
    flushDisplay();
    irrecv.enableIRIn();
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
    // create currentState logic

    // this should always be at the end of the loop
    flushDisplay();

    if (irrecv.decode()) {
        if (irrecv.decodedIRData.decodedRawData != 0) {
            String decodedValue = decodeRemoteCode(irrecv.decodedIRData.decodedRawData);
            printDebugInfoMessage("Received IR data: " + String(irrecv.decodedIRData.decodedRawData));
            printDebugInfoMessage("Decoded IR data: " + decodedValue);
            if (decodedValue.compareTo("UNKNOWN") != 0) {
                lcd.print(decodedValue);
            } else {
                printDebugWarningMessage("Unknown IR code received: " + String(irrecv.decodedIRData.decodedRawData));
            }
        }
        irrecv.resume();
    }
}

