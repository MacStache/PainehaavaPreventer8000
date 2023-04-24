# PainehaavaPreventer8000
TVT23KMO | Ryhmä 08 | Tietotekniikan sovellusprojekti | Painehaava Preventer 8000 

Painehaava Preventer 8000 on Oulun Ammattikorkeakoulun tieto- ja viestintätekniikan kevään 2023 vuosikurssin opiskelijaryhmän tietotekniikan sovellusprojekti. 

Painehaava Preventer (PP-8000) on Arduinosta, paineantureista ja vahvistimesta, kosteusanturista, LCD-näytöstä sekä hälytysäänisummerista koostuva laite, joka tarkkailee esimerkiksi liikerajoitteisen, muistamattoman tai tunnottoman potilaan kudokseen kohdistuvaa painetta tietyn ajan kuluessa sekä kudoksen kosteutta. Laitteessa on neljä paineanturia, joista pyritään muodostamaan kaksi Wheatstonen siltaa.

Anturit sijoitellaan siten, että ne mittaavat painevaihtelua kahden eri painehaavoille alttiin alueen välillä ja mahdollisesti käyttäjän valumista istuimella. Virta laitteelle otetaan paristoista niiden hyvän saatavuuden ja helppouden vuoksi. Laite voidaan asentaa esimerkiksi istuintyynyn sisälle pyörätuoliin.

/*******************************************************/

![IMG_20230417_105605](https://user-images.githubusercontent.com/123492715/233851884-a7526c14-1e0b-4e82-be59-94354e5d3ed7.jpg)

Painehaava Preventer 8000 -laite koostuu tässä repositorissa olevan koodin lisäksi seuraavista komponenteista:

1 x Arduino (Uno Rev3 tai JoyIt Mega2560R3) 
4 x kuormituskennoja (maks. 50 kilogramman kapasiteetti) 
1 x HX711 AD-moduuli 
1 x HR202 kosteusanturikortti 
1 x LM1125 LCD-näyttö 
1 x paristopesä 
1 x RUNCCI-YUN DC3V 6V 9V 12V 24V Elektroninen summeri 85 dB. 

/*******************************************************/

![IMG_20230418_202612](https://user-images.githubusercontent.com/123492715/233851900-965738f8-df4a-444f-9856-8bf79bca088e.jpg)

Projekti käyttää seuraavia kirjastoja:

HX711_ADC: https://github.com/olkal/HX711_ADC

LiquidCrystal: https://github.com/arduino-libraries/LiquidCrystal

SparkFun_HIH4030: https://github.com/sparkfun/SparkFun_HIH4030_Arduino_Library

/*******************************************************/

Kaikkien ryhmän jäsenten tehtävänä on ollut tuottaa siihen koodia Arduinon IDE-ympäristössä sekä toimia muissa projektin tehtävissä, joihin kuuluvat muun muassa dokumentointi, puheenjohtaja, sihteeri, ryhmän yleisjäsen sekä projektijulisteen työstäminen.

Projektiryhmään kuuluvat (aakkosjärjestyksessä):

Lotta Hautamaa, Pasi Honka, Henri Laukka, Sanna Mäki

