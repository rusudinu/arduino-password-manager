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
    bool debuggingEnabled = false;
    String display = "LOCKED";
    String displayRow2 = "remote unlock";
} currentState;

void printDebugInfoMessage(const String &message) {
    if (currentState.debuggingEnabled) {
        Serial.println("[INFO] " + message);
    }
}

void printDebugWarningMessage(const String &message) {
    if (currentState.debuggingEnabled) {
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

void setState(bool isLocked, bool debuggingEnabled, const String &display, const String &displayRow2) {
    if (isLocked != currentState.isLocked) {
        currentState.isLocked = isLocked;
        currentState.stateChanged = true;
    }
    if (debuggingEnabled != currentState.debuggingEnabled) {
        currentState.debuggingEnabled = debuggingEnabled;
        currentState.stateChanged = true;
    }
    if (display.compareTo(currentState.display)) {
        currentState.display = display;
        currentState.stateChanged = true;
    }
    if (displayRow2.compareTo(currentState.displayRow2)) {
        currentState.displayRow2 = displayRow2;
        currentState.stateChanged = true;
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
        // TODO FIX THIS LATER
        if (irrecv.decodedIRData.decodedRawData != 0) {
            Serial.println(" ");
            Serial.print("Code: ");
            Serial.println(irrecv.decodedIRData.decodedRawData);
            Serial.println(" ");
        }
        irrecv.resume();
    }
    switch (irrecv.decodedIRData.decodedRawData) {
        case 3125149440 : {
            lcd.print("CH-");
            delay(1000);
            lcd.clear();
            break;
        }
        case 3108437760 : {
            lcd.print("CH");
            delay(1000);
            lcd.clear();
            break;
        }
        case 3091726080 : {
            lcd.print("CH+");
            delay(1000);
            lcd.clear();
            break;
        }
        case 3141861120 : {
            lcd.print("|<<");
            delay(1000);
            lcd.clear();
            break;
        }
        case 3208707840 : {
            lcd.print(">>|");
            delay(1000);
            lcd.clear();
            break;
        }
        case 3158572800 : {
            lcd.print(">||");
            delay(1000);
            lcd.clear();
            break;
        }
        case 3927310080 : {
            lcd.print("VOL+");
            delay(1000);
            lcd.clear();
            break;
        }
        case 4161273600: {
            lcd.print("VOL-");
            delay(1000);
            lcd.clear();
            break;
        }
        case 4127850240 : {
            lcd.print("EQ");
            delay(1000);
            lcd.clear();
            break;
        }
        case 3238126971 : {
            lcd.print("0");
            break;
        }
        case 3860463360 : {
            lcd.print("FOL-");
            delay(1000);
            lcd.clear();
            break;
        }
        case 4061003520 : {
            lcd.print("FOL+");
            delay(1000);
            lcd.clear();
            break;
        }
        case 4077715200 : {
            lcd.print("1");
            break;
        }
        case 3877175040 : {
            lcd.print("2");
            break;
        }
        case 2707357440 : {
            lcd.print("3");
            break;
        }
        case 4144561920 : {
            lcd.print("4");
            break;
        }
        case 3810328320 : {
            lcd.print("5");
            break;
        }
        case 2774204160 : {
            lcd.print("6");
            break;
        }
        case 3175284480 : {
            lcd.print("7");
            break;
        }
        case 2907897600 : {
            lcd.print("8");
            break;
        }
        case 3041591040 : {
            lcd.print("9");
            break;
        }
    }
}

