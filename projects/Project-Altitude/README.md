# DE2 project - Altitude meter using pressure sensor and GPS

This project was made as an asigment for our Digital Electronics 2 lecture at Brno University of Technology.<br>

Authors:      Hana Stolarova,  Vladimir Lahoda<br>
                Dept. of Radio Electronics, Brno Univ. of Technology <br>
Created:     2019-11-26<br>
Platform:    ATmega328P, 16 MHz, AVR 8-bit Toolchain 3.6.2<br>
 



## Description
Using present pressure data from BMP280 sensor to calculate the accurated altitude 
      with reference pressure and current outside temperature.
      This parametr is being compared to GPS altitude data, which is read from the PA6H sensor.
      Then there is calculated the difference between BMP280 altitude data and PA6H data using absolute value. 


## Hardware

- Arduino GENUINO UNO r.3 clone
- Pressure sensor BOSCH BMP280
- GPS module PA6H
- Nokia 5110 Display module


## Bus 

- I2C (BMP280)
- SPI (Nokia 5110 display)
- UART (PA6H)

## Software

- Visual Studio Code 
- PuTTy
- Fritzing

## Our coding

### First session

(For starters we created our project file (main file, makefile).)
- At first we implemented library Nokia5110.h, we displayed static data.
We wrote: 
- function for BMP280 inicialisation with parameters which are reccomended for high resolution&precision
- function for read data registers from BMP280 sensor (0xFA - 0xFC, 0xF7 - 0xF9)
- composed registers based on LSB, MSB XLSB (pressure and temperature registers)

### Second session

- we tried calculating pressure&temperature values as manufacturer reccomendet id their datasheet, it was a special equation that can't be made up, and we thought we could re-write it to our own function; later we found out that we needed to use calibration data (16-bit value) from calibration registers (8-bit)
- this cost us a lot of time (at least two sessions), we were trying to figure this out, but we didn't, so we ended up not updating data on GitHub

### Next session

- implemented library for BMP280 sensor from [Jan Kasprzak](https://github.com/Yenya/avr-bmp280) and wew able to read pressure data though 
``` 
bmp280_measure(); 
```
and 
```
bmp280_getpressure();
```
and to get the real accurate pressure value, the data had to be corigated by arithmetical operations


## Next session
- we got working the temperature&pressure data and tried several equations to calculate the altitude
- we worked up the topic of pressure reference and used an equation, which includes temperature compensation, because the pressure and temperature are dependent on each other
- we tried to work out the GPS module, but we failed
- firstly we wanted to receive the data based on interrupt vector from the UART unit(USART_RX_vect)
- We ended up using recomendation from datasheet and with a little help from the teacher we wrote an algorithm which tests flag bit RXC0A set in the UCSR0A register
- the flag bit shows unread data from UART buffer
- if there are data present, we check the first bit for the $ sign, if it is present the rest of the data is written in a defined field
- then the function strstr() goes though this field and searches for for string "GPGGA" which contains desired data
- if the GPGGA string is found, then the fille field is rearanged into a new field, where thanks to function strtok we divide the field into seperate lines
- the particular data are separated by the symbol *"**,**"*
- the output parameter = altitude is singled into a new parameter, then rendered into double and displayed on Nokia5110 display

## Schema

<img src="https://github.com/xstola02/Digital-electronics-2/blob/master/projects/Project-Altitude/schema.jpg">

## Video
[online](https://youtu.be/bJNSbAfLO3k)


## References
- datasheet BOSCH BMP280, Digital Pressure Sensor, rev. 1.14, [[online](https://www.bosch-sensortec.com/media/boschsensortec/downloads/environmental_sensors_2/pressure_sensors_1/bmp280/bst-bmp280-ds001.pdf)]

- datasheet G.top PA6H, GPS module, Ver.V0A, [[online](https://cdn-shop.adafruit.com/datasheets/GlobalTop-FGPMMOPA6H-Datasheet-V0A.pdf)]

- datasheet Adafruit Industries, Nokia5110, Monochrome LCD, [[online](https://cdn-learn.adafruit.com/downloads/pdf/nokia-5110-3310-monochrome-lcd.pdf)]

- Equation for calculating current pressure, Pavel Janko, Ver 1.1, [[online](http://esp8266.fanson.cz/sensors/180131_ASP_1_0.pdf)]


