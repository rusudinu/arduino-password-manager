#include <LiquidCrystal.h>
#include <IRremote.h>
#include <EEPROM.h>

#pragma region ARDUINO_CONSTANTS

#define BAUD_RATE 9600
#define IR_RECV_PIN 7

#pragma endregion

#pragma region LCD_CONSTANTS

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

#pragma endregion

#pragma region PASSWORD_CONSTANTS

#define PASSWORD_COVER "*"
#define INITIAL_PASSWORD_LOW_BOUND 100000
#define INITIAL_PASSWORD_UP_BOUND 999999

#pragma endregion

#pragma region LOG_LEVEL_CONSTANTS

#define NO_LOGS 0
#define WARNINGS 1
#define INFO 2

#pragma endregion

#pragma region STATE_CONSTANTS

#define LOCKED 0
#define UNLOCKED 1
#define DELETE_PASSWORD 2
#define VIEW_PASSWORDS 3
#define ADD_PASSWORD 4

#pragma endregion

#pragma region REMOTE_CONSTANTS

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

#pragma endregion

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D0, LCD_D1, LCD_D2, LCD_D3);
IRrecv irrecv(IR_RECV_PIN);

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

struct state {
    bool stateChanged = true;
    int32_t state = LOCKED; // 0 -> locked, 1 -> unlocked
    bool hidePassword = true;
    int32_t debuggingLevelEnabled = INFO; // 0 -> no logs, 1 -> warning, 2 -> warning + info
    String display = String(LOCKED) + " ";
    String displayRow2 = "seed: ";
    String input = "";
    String password = "";
    long seed = 0;
    String secrets = "";
} currentState;

int secretIndex = 0;

#pragma region Function declarations

void printDebugInfoMessage(const String &message);

void printDebugWarningMessage(const String &message);

void setState(int32_t state, int32_t debuggingEnabled, const String &display, const String &displayRow2, const String &input = currentState.input, const String &password = currentState.password, const String &secrets = currentState.secrets);

void writeToDisplay(const String &word, bool append = true, bool firstRow = true, bool appendSpace = true);

void clearLCDLine(int line);

void flushDisplay();

String decodeRemoteCode(uint32_t code);

void generatePasswordBasedOnSeed();

void initDisplay();

void initRemote();

void unlock();

void lock();

void wrongPassword(const String &reason);

void passwordTooLong();

bool isDigit(const String &word);

void deletePassword();

void addPassword();

void scrollPasswords(bool right);

void readPasswordsFromEEPROM();

void writePasswordsToEEPROM();

String readString(int address);

void writeString(int address, const String &data);

#pragma endregion

void setup() {
    Serial.begin(BAUD_RATE);
    initDisplay();
    initRemote();
    readPasswordsFromEEPROM();
    generatePasswordBasedOnSeed();
}

void loop() {
    if (irrecv.decode()) {
        if (irrecv.decodedIRData.decodedRawData != 0) {
            String decodedValue = decodeRemoteCode(irrecv.decodedIRData.decodedRawData);
            printDebugInfoMessage("Received IR data: " + String(irrecv.decodedIRData.decodedRawData));
            printDebugInfoMessage("Decoded IR data: " + decodedValue);
            if (decodedValue.compareTo("UNKNOWN") != 0) {
                bool isDigitV = isDigit(decodedValue);
                if (isDigitV) {
                    setState(currentState.state, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2, String(currentState.input + decodedValue));
                }
                if (currentState.state == LOCKED && isDigitV) {
                    writeToDisplay(currentState.hidePassword ? PASSWORD_COVER : decodedValue, true, true, false);
                    setState(currentState.state, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2);
                    if (currentState.input.length() > currentState.password.length()) {
                        wrongPassword("input too long");
                    } else if (currentState.input.length() == currentState.password.length()) {
                        printDebugInfoMessage(currentState.input + " == " + currentState.password);
                        if (currentState.input.compareTo(currentState.password) == 0) {
                            unlock();
                        } else {
                            wrongPassword("wrong password");
                        }
                    }
                } else if (currentState.state == ADD_PASSWORD && isDigitV) {
                    writeToDisplay(decodedValue, true, true, false);
                    setState(currentState.state, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2);
                    if (currentState.input.length() > 6) {
                        passwordTooLong();
                    }
                }
            } else {
                printDebugWarningMessage("Unknown IR code received: " + String(irrecv.decodedIRData.decodedRawData));
            }
        }
        irrecv.resume();
    }

    if (currentState.stateChanged) {
        printDebugInfoMessage("State change found");
        flushDisplay();
    }
}

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

void setState(int32_t state, int32_t debuggingEnabled, const String &display, const String &displayRow2, const String &input, const String &password, const String &secrets) {
    printDebugWarningMessage("setState(" + String(state) + ", " + String(debuggingEnabled) + ", " + display + ", " + displayRow2 + ", " + input + ", " + password + ", " + secrets + ")");
    if (state != currentState.state) {
        currentState.state = state;
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

void writeToDisplay(const String &word, bool append, bool firstRow, bool appendSpace) {
    if (firstRow) {
        setState(currentState.state, currentState.debuggingLevelEnabled, append ? String(currentState.display + word + (appendSpace ? " " : "")) : String(word + (appendSpace ? " " : "")), currentState.displayRow2);
    } else {
        setState(currentState.state, currentState.debuggingLevelEnabled, currentState.display, append ? String(currentState.displayRow2 + word + (appendSpace ? " " : "")) : String(word + (appendSpace ? " " : "")));
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
    clearLCDLine(0);
    lcd.setCursor(0, 0);
    lcd.print(currentState.display);
    clearLCDLine(1);
    lcd.setCursor(0, 1);
    lcd.print(currentState.displayRow2);
    currentState.stateChanged = false;
}

String decodeRemoteCode(uint32_t code) {
    switch (code) {
        case CH_M : {
            if (currentState.state > LOCKED) {
                writeToDisplay(String(DELETE_PASSWORD), false, true);
                setState(DELETE_PASSWORD, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2);
            }
            return "CH-";
        }
        case CH: {
            if (currentState.state > LOCKED) {
                writeToDisplay(String(VIEW_PASSWORDS), false, true);
                setState(VIEW_PASSWORDS, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2);
            }
            return "CH";
        }
        case CH_P: {
            if (currentState.state > LOCKED) {
                writeToDisplay(String(ADD_PASSWORD), false, true);
                setState(ADD_PASSWORD, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2);
            }
            return "CH+";
        }
        case PREV : {
            if (currentState.state > LOCKED) {
                scrollPasswords(false);
            }
            return "|<<";
        }
        case NEXT: {
            if (currentState.state > LOCKED) {
                scrollPasswords(true);
            }
            return ">>|";
        }
        case PLAY: {
            if (currentState.state > LOCKED) {
                lock();
            }
            return ">||";
        }
        case VOL_P : {
            if (currentState.state == ADD_PASSWORD) {
                addPassword();
            }
            return "VOL+";
        }
        case VOL_M: {
            if (currentState.state == DELETE_PASSWORD) {
                deletePassword();
            }
            return "VOL-";
        }
        case EQ: {
            flushDisplay();
            return "EQ";
        }
        case ZERO : {
            return "0";
        }
        case FOL_M: {
            return "FOL-";
        }
        case FOL_P: {
            if (currentState.state == ADD_PASSWORD) {
                writePasswordsToEEPROM();
            }
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
    long seedCopy = currentState.seed;
    long m, sum = 0;
    while (seedCopy > 0) {
        m = seedCopy % 10;
        sum = sum + m;
        seedCopy = seedCopy / 10;
    }
    sum = sum * sum + currentState.seed * sum;
    sum = sum % 100000 + currentState.seed;
    sum = sum % 100000;
    currentState.password = String(sum);
    Serial.println(sum);
    writeToDisplay(String(currentState.seed), true, false);
}

void initDisplay() {
    lcd.begin(LCD_ROWS, LCD_COLS);
    analogWrite(LCD_CONTRAST, CONTRAST);
}

void initRemote() {
    irrecv.enableIRIn();
}

void unlock() {
    printDebugInfoMessage("Unlocked");
    writeToDisplay(String(UNLOCKED), false, true, true);
    writeToDisplay(currentState.secrets.substring(secretIndex * 6 + secretIndex, (secretIndex + 1) * 6 + secretIndex), false, false);
    setState(UNLOCKED, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2, "");
}

void lock() {
    printDebugInfoMessage("Locked");
    writeToDisplay(String(LOCKED), false, true, true);
    writeToDisplay("seed:", false, false);
    generatePasswordBasedOnSeed();
    setState(LOCKED, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2, "");
}

void wrongPassword(const String &reason) {
    printDebugInfoMessage("Wrong password, resetting: " + reason);
    writeToDisplay(String(LOCKED), false, true, true);
    setState(currentState.state, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2, "");
}

void passwordTooLong() {
    printDebugInfoMessage("Password too long");
    writeToDisplay(String(ADD_PASSWORD), false, true, true);
    setState(currentState.state, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2, "");
}

bool isDigit(const String &word) {
    return word.length() == 1 && word.toInt() >= 0 && word.toInt() <= 9;
}

void deletePassword() {
    printDebugInfoMessage("Deleting password with index: " + String(secretIndex));
    printDebugInfoMessage("First part: " + currentState.secrets.substring(0, secretIndex * 6 + secretIndex));
    printDebugInfoMessage("Second part: " + currentState.secrets.substring((secretIndex + 1) * 6 + secretIndex + 1));
    currentState.secrets = currentState.secrets.substring(0, secretIndex * 6 + secretIndex) + currentState.secrets.substring((secretIndex + 1) * 6 + secretIndex + 1);
    printDebugInfoMessage("New secrets: " + currentState.secrets);
    writeToDisplay(currentState.secrets.substring(secretIndex * 6 + secretIndex, (secretIndex + 1) * 6 + secretIndex), false, false);
    setState(currentState.state, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2);
}

void addPassword() {
    if (currentState.input.length() < 6) {
        while (currentState.input.length() < 6) {
            currentState.input = "0" + currentState.input;
        }
    }
    if (currentState.secrets.length() == 0) {
        currentState.secrets = currentState.input;
    } else {
        currentState.secrets = currentState.secrets + " " + currentState.input;
    }
    writeToDisplay(String(VIEW_PASSWORDS), false, true, true);
    setState(currentState.state, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2, "");
}

int getNumberOfSecrets() {
    int count = 0;
    for (char secret: currentState.secrets)
        if (secret == ' ') count++;
    return count;
}

void scrollPasswords(bool right) {
    if (right) {
        printDebugInfoMessage("Scrolling passwords to the right.");
        if (secretIndex < getNumberOfSecrets()) {
            ++secretIndex;
            writeToDisplay(currentState.secrets.substring(secretIndex * 6 + secretIndex, (secretIndex + 1) * 6 + secretIndex), false, false);
            setState(currentState.state, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2);
            printDebugInfoMessage("New password index: " + String(secretIndex));
        } else {
            printDebugInfoMessage("No more passwords found to the right.");
        }
    } else {
        printDebugInfoMessage("Scrolling passwords to the left.");
        if (secretIndex > 0) {
            --secretIndex;
            writeToDisplay(currentState.secrets.substring(secretIndex * 6 + secretIndex, (secretIndex + 1) * 6 + secretIndex), false, false);
            setState(currentState.state, currentState.debuggingLevelEnabled, currentState.display, currentState.displayRow2);
            printDebugInfoMessage("New password index: " + String(secretIndex));
        } else {
            printDebugInfoMessage("No more passwords found to the left.");
        }
    }
}

void readPasswordsFromEEPROM() {
    currentState.secrets = readString(0);
    printDebugInfoMessage("EEPROM size: " + String(EEPROM.length()));
    printDebugInfoMessage("Read passwords from EEPROM: " + currentState.secrets);
}

void writePasswordsToEEPROM() {
    printDebugInfoMessage("Writing passwords to EEPROM: " + currentState.secrets);
    if (currentState.secrets.length() < EEPROM.length()) {
        writeString(0, currentState.secrets);
    } else {
        printDebugWarningMessage("Passwords too long for EEPROM");
    }
}

void writeString(int address, const String &data) {
    for (int i = 0; i < data.length(); i++) {
        EEPROM.write(address + i, data[i]);
    }
    EEPROM.write(address + data.length(), '\0');   //Add termination null character
}

String readString(int address) {
    char data[400]; //Max 100 Bytes
    int len = 0;
    unsigned char k;
    k = EEPROM.read(address);
    while (k != '\0' && len < 400) {
        k = EEPROM.read(address + len);
        data[len] = k;
        ++len;
    }
    return {data};
}