/*
  LiquidCrystal

  The circuit:
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)

*/

// include the library code:
#include <LiquidCrystal.h>
#include <IRremote.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int RECV_PIN = 7;  //ir sensor to pin 7
IRrecv irrecv(RECV_PIN);
decode_results results;


void setup() {
    // set up the LCD's number of columns and rows:
    Serial.begin(9600);
    lcd.begin(16, 2);
    irrecv.enableIRIn();
    lcd.print("Boot");
    // Print a message to the LCD.
}

void loop() {
    // set the cursor to column 0, lllllline 1
    // (note: line 1 is the second row, since counting begins with 0):
    //lcd.setCursor(0, 0);
    if (irrecv.decode(&results))// Returns 0 if no data ready, 1 if data ready.
    {
        int x = results.value;// Results of decoding are stored in result.value
        Serial.println(" ");
        Serial.print("Code: ");
        Serial.println(results.value); //prints the value a a button press
        Serial.println(" ");
        irrecv.resume(); // Restart the ISR state machine and Receive the next value
    }
    switch (results.value) {
        case 16580863 : {
            lcd.print("OFF");
            delay(1000);
            lcd.clear();
            break;
        }
        case 16613503 : {
            lcd.print("VOL+");
            delay(1000);
            lcd.clear();
            break;
        }
        case 16597183 : {
            lcd.print("FUNC/STOP");
            delay(1000);
            lcd.clear();
            break;
        }
        case 16589023 : {
            lcd.print("|<<");
            delay(1000);
            lcd.clear();
            break;
        }
        case 16621663 : {
            lcd.print(">||");
            delay(1000);
            lcd.clear();
            break;
        }
        case 16605343 : {
            lcd.print(">>|");
            delay(1000);
            lcd.clear();
            break;
        }
        case 16584943 : {
            lcd.print("DOWN");
            delay(1000);
            lcd.clear();
            break;
        }
        case 16617583 : {
            lcd.print("VOL-");
            delay(1000);
            lcd.clear();
            break;
        }
        case 16601263 : {
            lcd.print("UP");
            delay(1000);
            lcd.clear();
            break;
        }
        case 16593103 : {
            lcd.print("0");
            break;
        }
        case 16625743 : {
            lcd.print("EQ");
            delay(1000);
            lcd.clear();
            break;
        }
        case 16609423 : {
            lcd.print("ST/REPT");
            delay(1000);
            lcd.clear();
            break;
        }
        case 2534850111 : {
            lcd.print("1");
            break;
        }
        case 1033561079 : {
            lcd.print("2");
            break;
        }
        case 1635910171 : {
            lcd.print("3");
            break;
        }
        case 16716015 : {
            lcd.print("4");
            break;
        }
        case 1217346747 : {
            lcd.print("5");
            break;
        }
        case 71952287 : {
            lcd.print("6");
            break;
        }
        case 851901943 : {
            lcd.print("7");
            break;
        }
        case 4294967295 : {
            lcd.print("8");
            break;
        }
        case 1053031451 : {
            lcd.print("9");
            break;
        }
    }
}

