#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <JeeLib.h>
#include <RFM69.h>
#include <SPI.h>
#include <DHT.h>

#include "config.h"

DHT dht(DHT22_PIN, DHTTYPE);
RFM69 radio;
char buffer[30] = "";

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup() {
  pinMode(DHT22_PIN, INPUT);
  pinMode(SOLAR_POWER_PIN, INPUT);

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

  power_adc_disable();
  power_twi_disable();
  power_timer1_disable();
  power_timer2_disable();
}

double read_voltage() {
  power_adc_enable();
  float val = analogRead(SOLAR_POWER_PIN);

  double vin = ((val * voltage) / 1024.0) / (volt_r2 / ( volt_r1 + volt_r2));
  power_adc_disable();

  return vin;
}

void loop() {

  double t = dht.readTemperature();
  double h = dht.readHumidity();
  
  char Tstr[10];
  char Hstr[10];
  char Vstr[10];

  if (isnan(t) || isnan(h)) {
    delay(10);
    return;
  }
  double v = read_voltage();
  dtostrf(v, 3,2, Vstr);
  dtostrf(t, 3,2, Tstr);
  dtostrf(h, 3,2, Hstr);
  
  sprintf(buffer, "%d;%s;%s;%s", NODEID, Tstr, Hstr, Vstr);
  if (DEBUG == 1) {
    Serial.println(buffer);
  }
  radio.sendWithRetry(GATEWAYID, buffer, strlen(buffer), 2);


  radio.sleep();
  for (uint8_t i = 1; i <= SLEEP_TIME_MIN; i++) {
    Sleepy::loseSomeTime(60000);
  }

}
