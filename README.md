# MikroProsjekt
AVR Moisture and lux sensor using I2C.
//Intro
School Project.
Uses two Arduino Uno's. One as master and other as slave. Master uses timer to poll data from slave(s)(multi slave not implementet) and displays the data on USART.
The slave uses ADC to read from two sensor and writes them to TWDR register for sending.
Communication is done with I2C/TWI.

//Requirements
Krav til prosjektet:
• Programmering av mikrokontrolleren i C (eventuelt C++). Alternativt også assembler i
tillegg.
• LCD/PC: Prosjektet må inneholde kode hvor LCD eller utskrift til PC benyttes.
• Timer: Prosjektet må inneholde kode som bruker en eller flere timer til å måle tid
og/eller lage signaler for styring av eksterne komponenter.
• ADC: Prosjektet må inneholde kode hvor ADC’en brukes med minst to forskjellige
analoge sensorer (temperatursensor, potentiometer, osv).
• Avbrudd: Prosjektet bruke minst ett avbrudd (interrupt).
• SPI/I2C: Prosjektet bør inneholde kode som benytter seg av SPI eller I2C.
• ESP32: Bruk gjerne ESP32 for kommunikasjon via WiFi.
• Versjonskontrollsystem: Ferdig kode må ligge Github (gratis for studenter). Link til
koden skal innleveres som en del av prosjektet.
