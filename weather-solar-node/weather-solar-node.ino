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
  power_adc_enable();
  ADCSRA = ADCSRA_status;
  digitalWrite(VOLTAGE_ENABLE_PIN, HIGH);
  float val = analogRead(VOLTAGE_READ_PIN);
  digitalWrite(VOLTAGE_ENABLE_PIN, LOW);
  ADCSRA &= ~(1 << 7);
  power_adc_disable();

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

void read_send_ntc() {
  float ntc_value;
  float resistor_value;

  power_adc_enable();
  ADCSRA = ADCSRA_status;
  digitalWrite(ENABLE_NTC, HIGH);

  ntc_value = analogRead(THERMISTORPIN);

  ADCSRA &= ~(1 << 7);
  power_adc_disable();

  // convert the value to resistance
  resistor_value = 1023 / ntc_value - 1;
  resistor_value = SERIESRESISTOR / resistor_value;

  float steinhart;
  steinhart = resistor_value / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C

  if (DEBUG == 1) {
    Serial.print("Temp/float: ");
    Serial.println(steinhart);
  }

  char Tempstr[10];
  dtostrf((double)steinhart, 3,2, Tempstr);

  sprintf(buffer, "%d;N:%s", NODEID, Tempstr);
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

  pinMode(THERMISTORPIN, INPUT);
  pinMode(ENABLE_NTC, OUTPUT);
  digitalWrite(ENABLE_NTC, LOW);

  analogReference(EXTERNAL);

  ADCSRA_status = ADCSRA;
  ADCSRA &= ~(1 << 7);
  power_adc_disable();

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
  read_send_ntc();
  if (voltage_send % 2) {
    read_send_voltage();
  }
  voltage_send++;

  radio.sleep();

  for (uint8_t i = 1; i <= SLEEP_TIME_MIN; i++) {
    Sleepy::loseSomeTime(60000);
  }

}
