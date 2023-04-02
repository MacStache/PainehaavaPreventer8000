#include <LiquidCrystal.h>
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pinnit:
const int HX711_dout = 10; //mcu > HX711 dout pinni
const int HX711_sck = 11; //mcu > HX711 sck pinni

//HX711 määrittely:
HX711_ADC LoadCell(HX711_dout, HX711_sck); //LoadCell() saa tietonsa HX711_dout ja _sck pinneistä

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
  lcd.print("Kaynnistetaan...");

  LoadCell.begin();
  //LoadCell.setReverseOutput(); //Kommentti pois jos halutaan muuttaa negatiivinen mittaustulos positiiviseksi
  unsigned long stabilizingtime = 2000; // Käynnistyksen jälkeistä tarkkuutta voidaan korottaa asettamalla pidempi stabilointiaika (nyt 2000ms/2s)
  boolean _tare = true; //Aseta tämä false asentoon jos et halua taarata

  //anturien taaraus
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag()) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Aikakatkaisu"); //jos pinnit on väärin asetettu niin laitteen yhteys aikakatkaistaan
    lcd.setCursor(0,1);
    lcd.print("tarkista johdot");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("ja pinnien");
    lcd.setCursor(0,1);
    lcd.println("asennot.");
    while (1);
  }
  else {
    LoadCell.setCalFactor(1.0); // Käyttäjän asetama kalibraatiomääre (float), oletusasetuksena 1.0 mutta voidaan muuttaa tarvittaessa
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("Kaynnistys");
    lcd.setCursor(0,1);
    lcd.println("valmis");
    delay(5000);
  }
  while (!LoadCell.update());
  calibrate(); //Aloita kalibrointi
}

void loop() {

  static boolean newDataReady = 0; 
  const int serialPrintInterval = 1000; //Syötteen tulostuksen nopeuden määritys. Korkeampi on hitaampi.

  // Tarkistetaan uusi data
  if (LoadCell.update()) newDataReady = true;

  // Haetaan pyöristetyt arvot datasetistä ja tulostetaan ne
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.getData();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.println("Painon maara: ");
      lcd.setCursor(0,1);
      lcd.println(i);
      newDataReady = 0;
      t = millis();
    }
  }

  // Komennot, jotka otetaan vastan serial monitorista. Tämä muutetaan napin/nappien taakse.
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay(); //taaraus käynnistetään syöttämällä t. Tämä muutetaan napin taakse
    else if (inByte == 'r') calibrate(); //kalibroi syöttämällä r. Laitetaan tämä napin taakse
  }

  // Tarkistetaan, että onko taaraus suoritettu
  if (LoadCell.getTareStatus() == true) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("Taaraus"); 
    lcd.setCursor(0,1);
    lcd.println("suoritettu.");
  }
}
  
/* TULEVAA KOODIA JOKA ON KOMMENTOITU, KOSKA SE EI OLE VALMIS
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
*/
  
void calibrate() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println("Aloita");
  lcd.setCursor(0,1);
  lcd.println("kalibrointi:");
  delay(4000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println("Aseta anturit");
  lcd.setCursor(0,1);
  lcd.println("tasaiselle.");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println("Poista paino");
  lcd.setCursor(0,1); 
  lcd.println("antureilta");
  delay(10000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println("Paina nappia");
  lcd.setCursor(0,1);
  lcd.println("kun olet valmis");

  boolean _resume = false; //Odotetaan, että käyttäjä painaa nappia
  while (_resume == false) {
    LoadCell.update();
    if (Serial.available() > 0) {
      if (Serial.available() > 0) {
        char inByte = Serial.read();
        if (inByte == 't') LoadCell.tareNoDelay(); //Napin virkaa toimittaa tässä koodissa serial monitoriin syötetty t-kirjain
      }
    }
    if (LoadCell.getTareStatus() == true) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.println("Taaraus valmis");
      delay(3000);
      _resume = true; //Jatketaan kalibrointia
    }
  }
//Taarataan käyttäjän tietämän painon mukaan
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.println("Aseta tiedetty");
  lcd.setCursor(0,1); 
  lcd.println("paino antureille");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println("ja syota paino");

//Mitataan tiedetty massa
  float known_mass = 0;
  _resume = false;
  while (_resume == false) {
    LoadCell.update();
    if (Serial.available() > 0) {
      known_mass = Serial.parseFloat();
      if (known_mass != 0) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.println("Syotetty massa:");
        lcd.setCursor(0,1);
        lcd.println(known_mass);
        _resume = true;
      }
    }
  }

  LoadCell.refreshDataSet(); //Päivitetään DataSetti, jotta varmistetaan, että paino on oikein
  float newCalibrationValue = LoadCell.getNewCalibration(known_mass); //Noudetaan uusi kalibrointiarvo

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println("Kalibrointiarvo: ");
  lcd.setCursor(0,1);
  lcd.println(newCalibrationValue);
  Serial.println(newCalibrationValue);
  delay(5000);
  lcd.setCursor(0,0);
  lcd.println("kayta tata arvoa");
  lcd.setCursor(0,1);
  lcd.println("projektissasi.");
  Serial.print(" Tallenna EEPROM osoite "); //Tässä kysytään, että tallennetaanko kalibrointi muistiin, mutta asetetaan tämä myöhemmin automaattiseksi
  Serial.print(calVal_eepromAdress);
  Serial.println("? y/n");

  _resume = false;
  while (_resume == false) {
    if (Serial.available() > 0) {
      char inByte = Serial.read(); //Tässä otetaan syöte serial monitorilta, mutta muutetaan tämä niin, että se toimii automaattisesti tai napin kautta.
      if (inByte == 'y') {
#if defined(ESP8266)|| defined(ESP32)
        EEPROM.begin(512);
#endif
        EEPROM.put(calVal_eepromAdress, newCalibrationValue);
#if defined(ESP8266)|| defined(ESP32)
        EEPROM.commit();
#endif
        EEPROM.get(calVal_eepromAdress, newCalibrationValue);
        Serial.print("Arvo ");
        Serial.print(newCalibrationValue);
        Serial.print(" tallennettu EEPROM osoitteeseen: ");
        Serial.println(calVal_eepromAdress);
        _resume = true;

      }
      else if (inByte == 'n') {
        Serial.println("Arvoa ei tallennettu EEPROM osoitteeseen");
        _resume = true;
      }
    }
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println("Kalibrointi valmis");
  delay(5000);
  lcd.clear();
  lcd.println("Kalibroidaksesi uudelleen");
  lcd.setCursor(0,1);
  lcd.println("Paina nappia");
  
}


  


