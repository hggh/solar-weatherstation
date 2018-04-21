#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <RFM69.h>
#include <SPI.h>
#include <DHT.h>

#include "config.h"

DHT dht(DHT22_PIN, DHTTYPE);
RFM69 radio;
char buffer[30] = "";

ISR(WDT_vect) {
  wdt_disable();
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

  ADCSRA = 0;
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
  
  sprintf(buffer, "%d;%s;%s", NODEID, Tstr, Hstr);
  if (DEBUG == 1) {
    Serial.println(buffer);
  }
  radio.sendWithRetry(GATEWAYID, buffer, strlen(buffer), 2);


  radio.sleep();
  for (uint8_t i = 1; i <= SLEEP_TIME_MIN * 8; i++) {
    wdt_enable(WDTO_8S);
    WDTCSR |= (1 << WDIE);

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    cli();
    sleep_enable();
    MCUCR = bit (BODS) | bit (BODSE);
    MCUCR = bit (BODS);
    sei();
    sleep_cpu();
    sleep_disable();
    sei();
  }

}
