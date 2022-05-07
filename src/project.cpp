#include <LiquidCrystal.h>
#include <IRremote.h>
#include <EEPROM.h>
#include <math.h>

#define BAUD_RATE 9600

#define LCD_RS  12
#define LCD_EN  11
#define LCD_CONTRAST 6
#define LCD_D0  5
#define LCD_D1  4
#define LCD_D2  3
#define LCD_D3  2
#define LCD_ROWS 16
#define LCD_COLS 2
#define CONTRAST 30

#define IR_RECV_PIN 7

#define PASSWORD_COVER "*"
#define INITIAL_PASSWORD_LOW_BOUND 100000
#define INITIAL_PASSWORD_UP_BOUND 999999

#define NO_LOGS 0
#define WARNINGS 1
#define INFO 2

#define CH_M 3125149440
#define CH 3108437760
#define CH_P 3091726080
#define PREV 3141861120
#define NEXT 3208707840
#define PLAY 3158572800
#define VOL_P 3927310080
#define VOL_M 4161273600
#define EQ 4127850240
#define ZERO 3910598400
#define FOL_M 3860463360
#define FOL_P 4061003520
#define ONE  4077715200
#define TWO 3877175040
#define THREE 2707357440
#define FOUR 4144561920
#define FIVE 3810328320
#define SIX 2774204160
#define SEVEN 3175284480
#define EIGHT 2907897600
#define NINE 3041591040


LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D0, LCD_D1, LCD_D2, LCD_D3);
IRrecv irrecv(IR_RECV_PIN);

struct state {
    bool stateChanged = true;
    bool isLocked = true;
    bool hidePassword = true;
    int32_t debuggingLevelEnabled = INFO; // 0 -> no logs, 1 -> warning, 2 -> warning + info
    String display = "LOCKED ";
    String displayRow2 = "seed: ";
    String input = "";
    String password = "";
    long seed = 0;
    String secrets = "my secret";
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
    for (int n = 0; n < LCD_ROWS; ++n) {
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
        case CH_M : {
            return "CH-";
        }
        case CH: {
            return "CH";
        }
        case CH_P: {
            return "CH+";
        }
        case PREV : {
            return "|<<";
        }
        case NEXT: {
            return ">>|";
        }
        case PLAY: {
            return ">||";
        }
        case VOL_P : {
            return "VOL+";
        }
        case VOL_M: {
            return "VOL-";
        }
        case EQ: {
            return "EQ";
        }
        case ZERO : {
            return "0";
        }
        case FOL_M: {
            return "FOL-";
        }
        case FOL_P: {
            return "FOL+";
        }
        case ONE : {
            return "1";
        }
        case TWO : {
            return "2";
        }
        case THREE : {
            return "3";
        }
        case FOUR : {
            return "4";
        }
        case FIVE : {
            return "5";
        }
        case SIX : {
            return "6";
        }
        case SEVEN : {
            return "7";
        }
        case EIGHT : {
            return "8";
        }
        case NINE : {
            return "9";
        }
        default: {
            return "UNKNOWN";
        }
    }
}

void generatePasswordBasedOnSeed() {
    randomSeed(analogRead(0));
    currentState.seed = random(INITIAL_PASSWORD_LOW_BOUND, INITIAL_PASSWORD_UP_BOUND);
    currentState.password = String((fmod(pow(log(sqrt(currentState.seed) + 2), log10(sqrt(currentState.seed))), 100000)));
    currentState.password.replace(".", "7");
    Serial.println(currentState.password);
    writeToDisplay(String(currentState.seed), true, false);
}

void initDisplay() {
    lcd.begin(LCD_ROWS, LCD_COLS);
    analogWrite(LCD_CONTRAST, CONTRAST);
}

void initRemote() {
    irrecv.enableIRIn();
}

void setup() {
    Serial.begin(BAUD_RATE);
    initDisplay();
    initRemote();
    generatePasswordBasedOnSeed();
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