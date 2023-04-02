#include <LiquidCrystal.h>
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pinnit:
const int HX711_dout = 10; //mcu > HX711 dout pin
const int HX711_sck = 11; //mcu > HX711 sck pin

//HX711 määrittely:
HX711_ADC LoadCell(HX711_dout, HX711_sck); //LoadCell funktio saa tietonsa HX711_dout ja _sck pinneistä

const int calVal_eepromAdress = 0;
unsigned long t = 0;

//Mittaukseen liittyvät muuttujat
float alkuArvo = 0; //Mittauksen aloitusarvo
bool mittaus = false; //mittauskytkin
bool halytys = false; //hälytyskytkin
int ajastin; //ajastimen muuttuja

LiquidCrystal lcd(2,3,4,5,6,7); //määritellään käytettävät LCD-portit. 
                                //Portit 1-2 on tarkoitettu R/S (Register Select) ja E (Enable) porteille ja 3-6 porteille joista syötetään bittejä näytölle.
                                //R/S-portti on portti jonka kautta näytölle syötetään komentoja
                                //E-portti on portti joka avaa rekisterin kirjoitusta varten

void setup() {

  Serial.begin(57600); delay(10);
  lcd.begin(16,2); //määritellään LCD-näytön mitat (16x2 -merkkiä)

  //Anturien kalibrointi ja taaraus
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Käynnistetään...");

  LoadCell.begin();
  //LoadCell.setReverseOutput(); //Kommentti pois jos halutaan muuttaa negatiivinen mittaustulos positiiviseksi
  unsigned long stabilizingtime = 2000; // Käynnistyksen jälkeistä tarkkuutta voidaan korottaa asettamalla pidempi stabilointiaika (nyt 2000ms/2s)
  boolean _tare = true; //Aseta tämä false asentoon jos et halua taarata

  //anturien taaraus
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag()) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Aikakatkaisu, tarkista johdot"); //jos pinnit on väärin asetettu niin laitteen yhteys aikakatkaistaan
    lcd.setCursor(0,1);
    lcd.print("ja pinnien asennot.");
    while (1);
  }
  else {
    LoadCell.setCalFactor(1.0); // Käyttäjän asetama kalibraatiomääre (float), oletusasetuksena 1.0 mutta voidaan muuttaa tarvittaessa
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("Käynnistys valmis.");
  }
  //while (!LoadCell.update());
  //calibrate(); //Aloita kalibrointi
}

void loop() {

  //LCD-näytön esimerkkikoodia
  lcd.print("Kylkiasentoon!"); //Tulostetaan näytölle teksti
  delay(3000); //Odotetaan 3 sekuntia

  lcd.setCursor(2,1);// Siirretään kursori sarakkeeseen 3 (lasketaan nollasta) ja riville 2 (lasketaan nollasta)
  lcd.print("PP8000"); //tulostetaan teksti
  delay(3000); //odotetaan 3 sekuntia

  lcd.clear(); //tyhjennetään LCD-näyttö
  //LCD-näytön esimerkkikoodi loppuu


  /*
    if (ajastin >= 500000){ //Hälyttimen placeholder
    halytys = true;
  }
  */
  

  while (alkuArvo >= 0 && mittaus == false){ //Jos massa on suurempi kuin 0 ja jos mittaus ei ole jo käynnissä käynnistetään anturienLuku- ja LCD;n kirjoitusfunktio
    if (mittaus = false){ //Jos mittaus ei ole jo käynnissä
      mittaus = true; //Asetetaan mittaus alkaneeksi
      //int ajastin = 0; //Ajastimen placeholder, ei toiminnallinen. Startataan ajastin kun anturienluku() funktiota kutsutaan
     
      float massa = (alkuArvo - 511)*125.0/6.0; // Ei mitään hajua mitä tässä lasketaan, otin youtubesta Henkan kommentti: Tässä lasketaan luultavasti kalibrointi. 

      lcd.setCursor(0,0); //Siirretään kursori ensimmäisen rivin ensimmäisen merkin kohdalle
      lcd.print("Massa: "); //Kirjoitetaan näytölle, että mitä näytetään
      lcd.print(massa);  //Kirjoitetaan massa muuttujan arvo, joka näyttää vielä mitä sattuu, pitää kalibroida
      //Serial.print(" kg"); //En tiedä onko tarpeellistä lisätä yksikköä
      lcd.setCursor(0,1); //Siirretään kursori toisen rivin ensimmäisen merkin kohdalle
      //Näillä alemmillahan ei ole mitään merkitystä, mutta voidaan asettaa niihin toisen anturin tiedot, esim massa2 muuttuja ja toinen analogiportti
      Serial.print("	V: ");
      Serial.print(analogRead(A0)*5.0/1023.0);  // Lasketaan analogipinnin saama jännite
      Serial.println("V");
      delay(10);
      }
    else if (alkuArvo == 0 && mittaus == true){ //jos anturilta saatava lukema on 0 ja mittaus on käynnissä  
      mittaus = false; //asetetaan mittaus päättyneeksi
      lcd.clear(); //tyhjennetään LCD-näyttö
      }
    } 
}
  

  


