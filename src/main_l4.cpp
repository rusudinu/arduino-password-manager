
unsigned const int Button = 2;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers

void setup() {
    pinMode(Button, INPUT_PULLUP);
    Serial.begin(9600);

    ADMUX = 0;
    /* ADC1 - channel 1 */
    ADMUX |= (1 << MUX0);
    /* AVCC with external capacitor at AREF pin */
    ADMUX |= (1 << REFS0);

    ADCSRA = 0;
    /* set prescaler at 128 */
    ADCSRA |= (7 << ADPS0);
    /* enable ADC */
    ADCSRA |= (1 << ADEN);
    ADCSRA |= (1 << ADIE);
    ADCSRA |= (1 << ADATE);
    ADCSRB = 0;
    ADCSRB ^= (1 << ADTS2);
    ADCSRB ^= (1 << ADTS0);
    EICRA |= (1<<ISC01);
    EIMSK |= (1<<INT0);
    DDRD &= ~(1<<PD2);
    PORTD |= (1<<PD2);
    OCR1B = 31249;            // compare match register 16 MHz/256/2 Hz - 1
    TCCR1B |= (1 << WGM12);   // CTC mode
    TCCR1B |= (1 << CS12);
    TIMSK1 |= (1 << OCIE1B);
    sei();
}

ISR(ADC_vect) {
        int val = ADC;
        val = map(val, 0, 1023, 0, 100);
        Serial.println(val);
}

ISR(TIMER1_COMPB_vect) {
        // cod întrerupere de la Timer1
}

//ISR(INT0_vect)
//{
//  if ((millis() - lastDebounceTime) > debounceDelay) {}
//}

void loop() {
    //1a
//  int val1 = analogRead(A1);  // read the input pin
//  val1 = map(val1, 0, 1023, 0, 100);
//  Serial.println(val1);   // debug value
//  delay(10);
//  Serial.println();

    //1b
//  int val = analogRead(A0);  // read the input pin
//  float voltage = val * 5.0;
//  voltage /= 1024.0;
//  float temperatureC = (voltage - 0.5) * 100 ;
//  Serial.println(temperatureC);

    //2
}