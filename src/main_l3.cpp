const int buttonPin = 2;    // the number of the pushbutton pin
const int ledPin = 9;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
    Serial.begin(9600);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);

    // set initial LED state
    digitalWrite(ledPin, ledState);

    EICRA |= (1 << ISC01);
    EIMSK |= (1 << INT0);
    sei();
}

bool lastState = false;

ISR(INT0_vect)
        {
                Serial.println("clicked 0");
        if (lastState)
        {
            analogWrite(9, 0);
            analogWrite(10, 255);
            analogWrite(11, 255);
        }
        else {
            analogWrite(9, 255);
            analogWrite(10, 255);
            analogWrite(11, 0);
        }
        lastState = !lastState;
        }

void loop() {

}






void setup() {
    //Set the pins to analog output
    pinMode(8,OUTPUT);
    pinMode(9,OUTPUT);
    pinMode(10,OUTPUT);
}

void loop() {
    for(int hue=0;hue<360;hue++)
    {
        setLedColorHSV(hue,1,1);
        delay(10);
    }
}

void setLedColorHSV(int h, double s, double v) {
    double r=0;
    double g=0;
    double b=0;

    double hf=h/60.0;

    int i=(int)floor(h/60.0);
    double f = h/60.0 - i;
    double pv = v * (1 - s);
    double qv = v * (1 - s*f);
    double tv = v * (1 - s * (1 - f));

    switch (i)
    {
        case 0: //rojo dominante
            r = v;
            g = tv;
            b = pv;
            break;
        case 1: //verde
            r = qv;
            g = v;
            b = pv;
            break;
        case 2:
            r = pv;
            g = v;
            b = tv;
            break;
        case 3: //azul
            r = pv;
            g = qv;
            b = v;
            break;
        case 4:
            r = tv;
            g = pv;
            b = v;
            break;
        case 5: //rojo
            r = v;
            g = pv;
            b = qv;
            break;
    }

    setLedColor(constrain((int)255*r,0,255),constrain((int)255*g,0,255),constrain((int)255*b,0,255));
}

void setLedColor(int red, int green, int blue) {
    analogWrite(8,red);
    analogWrite(9,green);
    analogWrite(10,blue);
}








#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
    myservo.attach(5);  // attaches the servo on pin 9 to the servo object
}

void loop() {
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
    }
}




#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position



const int buttonPin = 2;    // the number of the pushbutton pin
const int ledPin = 9;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers

void setup() {
    Serial.begin(9600);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);

    // set initial LED state
    digitalWrite(ledPin, ledState);

    EICRA |= (1 << ISC01);
    EIMSK |= (1 << INT0);
    sei();

    myservo.attach(5);
}

int incr = 10;
ISR(INT0_vect)
        {
                if ((millis() - lastDebounceTime) > debounceDelay) {
                    Serial.println("clicked 0");
                    myservo.write(pos);
                    if (pos == 180) {
                        incr = -10;
                    }
                    else if (pos == 0){
                        incr = 10;
                    }
                    pos += incr;
                }

        }

void loop() {

}