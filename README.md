# solar-wetterstation-rfm69

Sparsame "Wetterstation":

 * DHT22
 * RFM69 / 433Mhz
 * Atmega 328P (8Mhz, 3,3V)
 * 6V Solarpanel
 * 6V 5F SuperCap


## TODO
  * replace DHT22 with a BME280 (i2c?)
  * read the voltage of the solar panel before the z diode and the diode (+2 pins are used)
  * install the Lux sensor (i2c)
  * install the rain gauge INT0 pin (https://de.aliexpress.com/item/1-pcs-of-spare-part-for-weather-station-for-rain-meter-to-measure-the-rain-volume/32793367115.html) 


## read supercap voltage

  * NPN will enabled by a PIN = HIGH
  * NPN enables the PNP transistor
  * PNP transistor drives the volatage divider
  * amtel ADC read the output of the voltage divider
  * to save power disable the NPN bin by PIN = LOW
  
BAD: this method needs to pins on the atmel. One with a ADC and one to enable the NPN. but it will waste zero energy while sleeping.


## NTC 10k

  * NTC is directly attached to the supercap to read the temperature of the supercap, because my case is not perfekt
  * since the NTC wastes energy drive the 10k and NTC via a extra amtel pin.

BAD: two pins are used
  
