// #include <Arduino.h>
//
//void setup() {
//    Serial.begin(9600);
//    Serial.println("ola");
//}
//
//int rx_index = 0;
//char rx_buf[20];
//bool blink = false;
//
//void loop() {
//    pinMode(2, INPUT_PULLUP);
//    if (blink) {
//        digitalWrite(13, HIGH);
//        delay(200);
//        digitalWrite(13, LOW);
//        delay(200);
//    }
//    if (Serial.available()) {
//        char a = Serial.read();
//        if (a == '\n') {
//            char buf[20];
//            Serial.println(rx_buf);
//            if (strcmp(rx_buf, "on") == 0) {
//                digitalWrite(13, HIGH);
//            }
//            if (strcmp(rx_buf, "off") == 0) {
//                digitalWrite(13, LOW);
//            }
//            if (strcmp(rx_buf, "blink") == 0) {
//                blink = !blink;
//            }
//            if (strcmp(rx_buf, "get") == 0) {
//                // read the status of a button
//                Serial.println(digitalRead(2));
//            }
//            rx_index = 0;
//            rx_buf[rx_index] = '\0';
//        } else {
//            rx_buf[rx_index] = a;
//            rx_index++;
//            rx_buf[rx_index] = '\0';
//        }
//    }
//}
//
//
///*2*/
//
//#include "usart.h"
//
///*
//   Functie de initializare a controllerului USART
//*/
//void USART0_init() {
//    /* seteaza baud rate la 9600 */
//    UBRR0 = 51;
//
//    /* porneste transmitatorul */
//    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
//
//    /* seteaza formatul frame-ului: 8 biti de date, 1 bit de stop, fara paritate */
//    UCSR0C &= ~(1 << USBS0);
//    UCSR0C |= (3 << UCSZ00);
//}
//
///*
//   Functie ce transmite un caracter prin USART
//
//   @param data - caracterul de transmis
//*/
//void USART0_transmit(char data) {
//    /* asteapta pana bufferul e gol */
//    while (!(UCSR0A & (1 << UDRE0)));
//
//    /* pune datele in buffer; transmisia va porni automat in urma scrierii */
//    UDR0 = data;
//}
//
///*
//   Functie ce primeste un caracter prin USART
//
//   @return - caracterul primit
//*/
//char USART0_receive() {
//    /* asteapta cat timp bufferul e gol */
//    while (!(UCSR0A & (1 << RXC0)));
//
//    /* returneaza datele din buffer */
//    return UDR0;
//}
//
///*
//   Functie ce transmite un sir de caractere prin USART
//
//   @param data - sirul (terminat cu '\0') de transmis
//*/
//void USART0_print(const char *data) {
//    while (*data != '\0')
//        USART0_transmit(*data++);
//}
//
//void setup() {
//    USART0_init();
//}
//
//bool wasPressed = false;
//
//void loop() {
//    pinMode(2, INPUT_PULLUP);
//    if (digitalRead(2) == 0) {
//        USART0_print("PD2 APASAT\0");
//        wasPressed = true;
//    } else if (wasPressed) {
//        USART0_print("PD2 LASAT\0");
//        wasPressed = false;
//    }
//}



#include "usart.h"

#define F_CPU 16000000

#include <util/delay.h>

/*
   Functie de initializare a controllerului USART
*/
void USART0_init() {
    /* seteaza baud rate la 9600 */
    UBRR0 = 103;

    /* porneste transmitatorul */
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);

    /* seteaza formatul frame-ului: 8 biti de date, 1 bit de stop, fara paritate */
    UCSR0C &= ~(1 << USBS0);
    UCSR0C |= (3 << UCSZ00);
}

/*
   Functie ce transmite un caracter prin USART

   @param data - caracterul de transmis
*/
void USART0_transmit(char data) {
    /* asteapta pana bufferul e gol */
    while (!(UCSR0A & (1 << UDRE0)));

    /* pune datele in buffer; transmisia va porni automat in urma scrierii */
    UDR0 = data;
}

/*
   Functie ce primeste un caracter prin USART

   @return - caracterul primit
*/
char USART0_receive() {
    /* asteapta cat timp bufferul e gol */
    while (!(UCSR0A & (1 << RXC0)));

    /* returneaza datele din buffer */
    return UDR0;
}

/*
   Functie ce transmite un sir de caractere prin USART

   @param data - sirul (terminat cu '\0') de transmis
*/
void USART0_print(const char *data) {
    while (*data != '\0')
        USART0_transmit(*data++);
}

void setup() {
    USART0_init();
}

int rx_index = 0;
char rx_buf[20];
bool blink = false;

void loop() {
    PORTD |= (1 << PD2);
    if (blink) {
        PORTB |= (1 << PB5);
        _delay_ms(200);
        PORTB &= ~(1 << PB5);
        _delay_ms(200);
    }
    if (!(UCSR0A & (1 << RXC0))) {
        char a = USART0_receive();
        if (a == '\n') {
            char buf[20];
            char resulta[40];
            strcat(resulta, rx_buf);
            strcat(resulta, "\n\0");
            USART0_print(resulta);
            if (strcmp(rx_buf, "on") == 0) {
                PORTB |= (1 << PB5);
            }
            if (strcmp(rx_buf, "off") == 0) {
                PORTB &= ~(1 << PB5);
            }
            if (strcmp(rx_buf, "blink") == 0) {
                blink = !blink;
            }
            if (strcmp(rx_buf, "get") == 0) {
                // read the status of a button
                int value = PIND & (1 << PD2);
                char result[40];
                strcat(result, value);
                strcat(result, "\n\0");
                USART0_print(result);
            }
            rx_index = 0;
            rx_buf[rx_index] = '\0';
        } else {
            rx_buf[rx_index] = a;
            rx_index++;
            rx_buf[rx_index] = '\0';
        }
    }
}