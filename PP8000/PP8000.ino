#include <HX711_ADC.h> //HX711 vahvistimen kirjasto
#include <LiquidCrystal.h> //LCD-näytön kirjasto
#include <SparkFun_HIH4030.h> //kosteusanturin kirjasto (ei HR202, mutta yhteensopiva)
#include <Wire.h> //kosteusanturin lämpötilakirjasto
#include "LCDFunctions.h" //LCD-funktioiden aliohjelmat
#include "AlarmFunctions.h" //Hälytinfunktioiden aliohjelmat
#define BREAKREMINDER 3600000 // Time break //1 h ajanjakso maaritellaan definessa koska se on muuttumaton

//kosteusanturin määrittelyt
#define HIH4030_OUT A0 //Kosteusanturin Analog IO pinni kytketään A0:aan
#define HIH4030_SUPPLY 5 //Paljonko virtaa sensori ottaa (volttia)

HIH4030 sensorSpecs(HIH4030_OUT, HIH4030_SUPPLY); //asetetaan edelliset arvot kirjaston käyttöön

float weight = 100.00;  // Käyttäjän paino: muutetaan manuaalisesti käyttäjäkohtaisesti, koska anturit eivät pysty mittaamaan massaa tässä laitteessa näillä komponenteilla
bool mittaus = false;
bool taaraus = true; //Aseta tämä false asentoon jos et halua taarata

unsigned long StartTime = 0; //Sitting timer // Istumisajan laskuri, maaritellaan lahtemaan nollasta
const unsigned long Interval = 300000; //no weight wait period 5 min// aika jolloin asentoa muutetaan ja odotetaan painon laskeutuvan takaisin sensoreille

//pinnit:
const int HX711_dout = 10; //mcu > HX711 dout pinni
const int HX711_sck = 11; //mcu > HX711 sck pinni
int sensorPin = A0; //kosteusanturin signaalipinni

//HX711 määrittely:
HX711_ADC LoadCell(HX711_dout, HX711_sck); //LoadCell() saa tietonsa HX711_dout ja _sck pinneistä

const int calVal_eepromAdress = 0; //Asetetaan EEPROM-osoitteeksi 0. EEPROM = Electrically Erasable Programmable Read-Only Memory
unsigned long t = 0;

//Mittaukseen liittyvät muuttujat
const float pressure = 9.81/(0.1*0.1)/133.322;  //paineen laskukaava elohopeamillimetreinä (10x10cm pinta-alalla)
const float calibrationValue = 22500.00;   //Kalibrointimuuttuja: säädä omaan tarpeeseen, jos ei toimi samalla arvolla, weight pitää nollata, jos tarvii kalibroida!

float leftPressure = 0.00; // Alustetaan muuttuja
float rightPressure = 0.00; // Alustetaan muuttuja
float WEIGHT_THRESHOLD_START = 0.00; // Alustetaan muuttuja
float WEIGHT_THRESHOLD_CHECK = 0.00; // Alustetaan muuttuja
int sensorValue = 0; // alusta kosteusanturin lukema

enum State_enum {
  WAIT_FOR_WEIGHT, WAIT_FOR_ALARM, BUTT_TIMEOUT, RESET_WAIT
} state = WAIT_FOR_WEIGHT;

LiquidCrystal lcd(2,3,4,5,6,7); //määritellään käytettävät LCD-portit. 
                                //Portit 1-2 on tarkoitettu R/S (Register Select) ja E (Enable) porteille ja 3-6 porteille joista syötetään bittejä näytölle.
                                //R/S-portti on portti jonka kautta näytölle syötetään komentoja
                                //E-portti on portti joka avaa rekisterin kirjoitusta varten

void setup() {

lcd.begin(16,2); // Määritellään LCD-näytön koko
createCustomChars(lcd); //luodaan ääkköset
Wire.begin(); //kosteusanturin lämpötilamittarin käynnistys
}

void loop() {
  humidityCalc(sensorSpecs, temp); //lähetetään laskurifunktiolle kosteusanturin lukemat

while (taaraus == true){  //Loopin alku rullataan läpi niin kauan kuin "taaraus" -kytkimen asento on true
                          //Siirsin tämän osan koodia setupista loopin alkuun.
  lcdFunc(lcd, 255,255,""); //lcdFunc tyhjentää näytön
  lcdFunc(lcd, 0, 0, "Käynnistetään"); //lcdFunc kirjoittaa ensimmäisen sarakkeen ensimmäiseen riviin viestin
  delay(1000);
  lcdFunc(lcd, 0, 1, "ÄLÄ ISTU"); //lcdFunc kirjoittaa ensimmäisen sarakkeen toiselle riville viestin

  LoadCell.begin();
  //LoadCell.setReverseOutput(); //Kommentti pois jos halutaan muuttaa negatiivinen mittaustulos positiiviseksi, eli asettaa anturi toisin päin.
  unsigned long stabilizingtime = 2000; // Käynnistyksen jälkeistä tarkkuutta voidaan korottaa asettamalla pidempi stabilointiaika (nyt 2000ms/2s)
  boolean _tare = true;

  //anturien taaraus
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag()) {
    errorSound();
    lcdFunc(lcd, 255,255,"");
    lcdFunc(lcd, 0, 0, "Aikakatkaisu");
    lcdFunc(lcd, 0, 1, "Tarkista johdot");
    delay(3000);
    lcdFunc(lcd, 255,255,"");
    lcdFunc(lcd, 0, 0, "ja pinnien");
    lcdFunc(lcd, 0, 1, "asennot");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue);
    lcdFunc(lcd, 255,255,"");
    lcdFunc(lcd, 0, 0, "Käynnistys");
    lcdFunc(lcd, 0, 1, "valmis");
    startUpSound();
    taaraus = false;
    delay(3000);
  }
}

  bool mittaus = true;
  static boolean newDataReady = 0; 
  const int serialPrintInterval = 500; //Syötteen tulostuksen nopeuden määritys. Korkeampi on hitaampi.

  // Tarkistetaan uusi data
  if (LoadCell.update()) newDataReady = true;

  // Haetaan pyöristetyt arvot datasetistä ja tulostetaan ne
  if (newDataReady) {

    // Määritellään paine-muuttujat newDataReadyn jälkeen, jotta LoadCell.getData saa päivitetyt arvot
    leftPressure = (-weight/2 + LoadCell.getData()) * -pressure; // Vasemman puolen paine elohopeamillimetreinä (-weight ja -pressure, jotta saadaan tulostumaan positiivinen paine LCD-näytölle)
    rightPressure = (weight/2 + LoadCell.getData()) * pressure; // Oikean puolen paine elohopeamillimetreinä
    WEIGHT_THRESHOLD_START = (leftPressure + rightPressure) / 2 + 10; // Paineraja paineen mittauksen käynnistämiseksi (oltava pienempi kuin CHECK, jotta pysytään tilakoneessa)
    WEIGHT_THRESHOLD_CHECK = WEIGHT_THRESHOLD_START + 20; // Paineraja paineen anturilta poistumisen tarkistamiseksi (oltava suurempi kuin START, jotta pysytään tilakoneessa)

    if (millis() > t + serialPrintInterval) {
      if (LoadCell.getData() < 0) {  //kun < 0, niin antaa vasemman pakaran paineen
        String paine = String(int(leftPressure)); //muunnetaan painelaskelma merkkijonoksi, jotta se saadaan tulostettua
        String kosteus = String(int(humidity)); //muunnetaan kosteuslaskelma merkkijonoksi, jotta se saadaan tulostettua
        lcdFunc(lcd, 255,255,"");
        lcdFunc(lcd, 0, 0, "Vasen: " + paine + " mmHg"); //tulostetaan stringit näytölle
        lcdFunc(lcd, 0, 1, "Kosteus: " + kosteus + " %"); //tulostetaan stringit näytölle
        newDataReady = 0; //anturidatan nollaus uutta dataa varten
        t = millis();
      }        
      else {  //kun > 0, niin antaa oikean pakaran paineen
        String paine = String(int(rightPressure)); //muunnetaan painelaskelma merkkijonoksi, jotta se saadaan tulostettua
        String kosteus = String(int(humidity)); //muunnetaan kosteuslaskelma merkkijonoksi, jotta se saadaan tulostettua
        lcdFunc(lcd, 255,255,"");
        lcdFunc(lcd, 0, 0, "Oikea: " + paine + " mmHg"); //tulostetaan stringit näytölle
        lcdFunc(lcd, 0, 1, "Kosteus: " + kosteus + " %"); //tulostetaan stringit näytölle
        newDataReady = 0; //anturidatan nollaus uutta dataa varten
        t = millis();
        }        
    }

if(leftPressure > WEIGHT_THRESHOLD_START || rightPressure > WEIGHT_THRESHOLD_START) {

  switch(state)
  {
    case WAIT_FOR_WEIGHT:
      StartTime = millis();  // timeri alkaa mitata ja tallentaa aikaa
      state = WAIT_FOR_ALARM; // odotellaan hälytystä
      break;
      
    case WAIT_FOR_ALARM:
      if(millis() - StartTime >= BREAKREMINDER) { // timeri ylittää 1 tunnin määräajan
        alarm = true;
        if(alarm == true) {
          setupAlarm(); //funktiota kutsutaan
        }
        if (leftPressure < WEIGHT_THRESHOLD_CHECK && rightPressure < WEIGHT_THRESHOLD_CHECK) {
          alarm = false;
          state = RESET_WAIT;
        }
      } 
      else if(rightPressure >= 760 || leftPressure >= 760) { // 760 mmHg asetettu ylärajaksi välittömälle hälytykselle
        alarm = true;
        if(alarm == true) {
          setupAlarm(); //funktiota kutsutaan
        }
        if (leftPressure < WEIGHT_THRESHOLD_CHECK && rightPressure < WEIGHT_THRESHOLD_CHECK) {
          alarm = false;
          state = BUTT_TIMEOUT;
        }
      }
      else if(humidity >= 40) { // Kosteushälytyksen raja 40 %
            alarm = true;
            if(alarm == true) {
              setupAlarm(); }
              if (humidity <= 35) { // Poistutaan hälytyksestä kun kosteus alle 35 %, jotta voidaan varmistua kosteuden lähteneen laskuun
                alarm = false;
                state = RESET_WAIT;
              }
      }              
      break;

    case RESET_WAIT:
      if (millis() - StartTime >= Interval) { //odotetaan 5 min ennen timerin uudelleen käynnistymistä
        StartTime = 0;
        alarm = true;
        state = WAIT_FOR_WEIGHT;  // resetoidaan tila ja odotetaan uutta painoa
      }
      break;
      
    case BUTT_TIMEOUT:       
      unsigned long butt_timer = 0;           
      if(millis() - butt_timer >= Interval) { // 5 min            
        StartTime = 0;          
        state = WAIT_FOR_ALARM;          
      }            
      break;
  }
}
}
}