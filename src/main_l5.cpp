// master code

/*
#include <SPI.h>

#define SS_PIN 7
char secret[] = "hello world";
int i;

void setup() {
    Serial.begin(115200);
    pinMode(SS_PIN, OUTPUT);
    digitalWrite(SS, OUTPUT);
    digitalWrite(SS, HIGH);
    digitalWrite(SS_PIN, HIGH);
    delay(1000);
    Serial.println("SPI master online!");
    SPI.begin();
    i = 0;
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    delay(100);
}


void loop() {
    if (i < strlen(secret)) {
    byte master_send, master_receive;
        digitalWrite(SS_PIN, LOW);
        master_send = secret[i++];
        master_receive = SPI.transfer(master_send);
        digitalWrite(SS_PIN, HIGH);
        Serial.print((char) master_receive);
    }else{
        i = 0;
    }
}
 */

// slave code
/*
#include <SPI.h>

volatile int i = 0;
#define MISO_PIN 12
char secret[] = "hello world from the slave";
bool received = false;
int8_t slave_received;

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("SPI slave online!");
    pinMode(MISO_PIN, OUTPUT);
    SPCR |= (1 << SPE);
    i = 0;
    received = false;
    SPI.attachInterrupt();
    delay(100);
}

ISR(SPI_STC_vect) {
    slave_received = SPDR;
    received = true;
}

void loop() {
    if (received) {
        Serial.print((char) slave_received);
        byte slave_send;
        slave_send = secret[i++];
        SPDR = slave_send;
        received = false;
        if (i >= strlen(secret)) {
            i = 0;
        }
    }
}

int8_t SPI_exchange(uint8_t send) {
    SPDR = send;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}
 */

#include <SPI.h>
#include <SD.h>

void setup() {
    File myFile;
    // read from the sd card
    Serial.begin(115200);
    Serial.println("SD card reader online.");
    if (!SD.begin(8)) {
        Serial.println("SD card initialization failed.");
        //return;
    }
    Serial.println("SD card initialized.");
    if (SD.exists("sd.txt")) {
        myFile = SD.open("sd.txt");
        if (myFile) {
            Serial.println("sd.txt:");
            while (myFile.available()) {
                Serial.write(myFile.read());
            }
            myFile.close();
        } else {
            Serial.println("error opening sd.txt");
        }
    } else {
        Serial.println("creating sd.txt");
        myFile = SD.open("sd.txt", FILE_WRITE);
        myFile.close();
    }
}

void loop() {

}