#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <RFM69.h>
#include <JeeLib.h>
#include <SPI.h>
#include <DHT.h>

#include "config.h"

DHT dht(DHT22_PIN, DHTTYPE);
RFM69 radio;
char buffer[32] = "";
static float volt_r1 = 56000.0;
static float volt_r2 = 10000.0;
static float voltage = 3.3;
byte ADCSRA_status;
uint8_t voltage_send = 1;

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void read_send_voltage() {
  ADCSRA = ADCSRA_status;
  digitalWrite(VOLTAGE_ENABLE_PIN, HIGH);
  float val = analogRead(VOLTAGE_READ_PIN);
  digitalWrite(VOLTAGE_ENABLE_PIN, LOW);
  ADCSRA &= ~(1 << 7);

  double vin = ((val * voltage) / 1024.0) / (volt_r2 / ( volt_r1 + volt_r2));
  char Vstr[10];

  dtostrf(vin, 3,2, Vstr);

  sprintf(buffer, "%d;V:%s", NODEID, Vstr);
  if (DEBUG == 1) {
    Serial.println(buffer);
    Serial.flush();
  }
  radio.sendWithRetry(GATEWAYID, buffer, strlen(buffer), 2);
}

void setup() {
  pinMode(DHT22_PIN, INPUT);

  if (DEBUG == 1) {
    Serial.begin(9600);
    Serial.println("Starting Solar Wetter");
  }
  else {
    power_usart0_disable();
  }

  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  radio.encrypt(ENCRYPTKEY);

  dht.begin();

  pinMode(VOLTAGE_READ_PIN, INPUT);
  pinMode(VOLTAGE_ENABLE_PIN, OUTPUT);
  digitalWrite(VOLTAGE_ENABLE_PIN, LOW);

  ADCSRA_status = ADCSRA;
  ADCSRA &= ~(1 << 7);

  power_twi_disable();
  power_timer1_disable();
  power_timer2_disable();
}

void loop() {

  double t = dht.readTemperature();
  double h = dht.readHumidity();
  
  char Tstr[10];
  char Hstr[10];

  if (isnan(t) || isnan(h)) {
    delay(10);
    return;
  }
  dtostrf(t, 3,2, Tstr);
  dtostrf(h, 3,2, Hstr);
  
  sprintf(buffer, "%d;T:%s;H:%s", NODEID, Tstr, Hstr);
  if (DEBUG == 1) {
    Serial.println(buffer);
    Serial.flush();
  }
  radio.sendWithRetry(GATEWAYID, buffer, strlen(buffer), 2);
  if (voltage_send % 2) {
    read_send_voltage();
  }
  voltage_send++;

  radio.sleep();

  for (uint8_t i = 1; i <= SLEEP_TIME_MIN; i++) {
    Sleepy::loseSomeTime(60000);
  }

}
