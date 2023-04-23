#include <HX711_ADC.h> //HX711 vahvistimen kirjaston header
#include <LiquidCrystal.h>
#include "LCDFunctions.h" //LCD-funktioiden aliohjelmat
#include "AlarmFunctions.h" //Hälytinfunktioiden aliohjelmat

//#include <EEPROM.h> //EEPROM -kirjaston header Tätä ei välttämättä tarvita?!?

#define BREAKREMINDER 7200000 // Time break //2h ajanjakso maaritellaan definessa koska se on muuttumaton 

const float weight = 100;  // Käyttäjän paino: muutetaan manuaalisesti käyttäjäkohtaisesti, koska anturit eivät pysty mittaamaan massaa tässä laitteessa näillä komponenteilla

bool mittaus = false;
bool taaraus = true; //Aseta tämä false asentoon jos et halua taarata

unsigned long StartTime = 0; //Sitting timer // Istumisajan laskuri, maaritellaan lahtemaan nollasta
const unsigned long Interval = 180000; //no weight wait period 3 min// aika jolloin asentoa muutetaan ja odotetaan painon laskeutuvan takaisin sensoreille 

float humidity; //TODO // koodi puuttuu

//pinnit:
const int HX711_dout = 10; //mcu > HX711 dout pinni
const int HX711_sck = 11; //mcu > HX711 sck pinni

//HX711 määrittely:
HX711_ADC LoadCell(HX711_dout, HX711_sck); //LoadCell() saa tietonsa HX711_dout ja _sck pinneistä

const int calVal_eepromAdress = 0; //Asetetaan EEPROM-osoitteeksi 0. EEPROM = Electrically Erasable Programmable Read-Only Memory
unsigned long t = 0;

//Mittaukseen liittyvät muuttujat
const float pressure = 9.81/(0.1*0.1)/133.322;  //paineen laskukaava elohopeamillimetreinä (10x10cm pinta-alalla)
const float calibrationValue = 22500;   //Kalibrointimuuttuja: säädä omaan tarpeeseen, jos ei toimi samalla arvolla, weight pitää nollata, jos tarvii kalibroida!

float leftPressure = 0.00; // Alustetaan muuttuja
float rightPressure = 0.00; // Alustetaan muuttuja
float WEIGHT_THRESHOLD = 0.00;

enum States {
  WAIT_FOR_WEIGHT, WAIT_FOR_ALARM, BUTT_TIMEOUT, RESET_WAIT
} state = WAIT_FOR_WEIGHT;

LiquidCrystal lcd(2,3,4,5,6,7); //määritellään käytettävät LCD-portit. 
                                //Portit 1-2 on tarkoitettu R/S (Register Select) ja E (Enable) porteille ja 3-6 porteille joista syötetään bittejä näytölle.
                                //R/S-portti on portti jonka kautta näytölle syötetään komentoja
                                //E-portti on portti joka avaa rekisterin kirjoitusta varten

void setup() {

//Serial.begin(9600); // DEBUG Poista tämä kun ei enää tarvita
lcd.begin(16,2); // Määritellään LCD-näytön koko
createCustomChars(lcd);
}

void loop() {

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
  const int serialPrintInterval = 1000; //Syötteen tulostuksen nopeuden määritys. Korkeampi on hitaampi.

  // Tarkistetaan uusi data
  if (LoadCell.update()) newDataReady = true;

  // Haetaan pyöristetyt arvot datasetistä ja tulostetaan ne
  if (newDataReady) {
    
    // Määritellään paine-muuttujat newDataReadyn jälkeen, jotta LoadCell.getData saa päivitetyt arvot
    leftPressure = (-weight/2 + LoadCell.getData()) * -pressure; // Vasemman puolen paine elohopeamillimetreinä (-weight ja -pressure, jotta saadaan tulostumaan positiivinen paine LCD-näytölle)
    rightPressure = (weight/2 + LoadCell.getData()) * pressure; // Oikean puolen paine elohopeamillimetreinä
    WEIGHT_THRESHOLD = (leftPressure + rightPressure) / 2 + 10; // Siirretty #definestä tähän, koska muuttuu käyttäjän painon mukaan. Viimeistä lukua muuttamalla voidaan säätää paineen huomioimisen aloitusrajaa.

    if (millis() > t + serialPrintInterval) {

      if (LoadCell.getData() < 0) {  //kun < 0, niin antaa vasemman pakaran paineen

        String paine = String(int(leftPressure)); //muunnetaan painelaskelma merkkijonoksi, jotta se saadaan tulostettua
        lcdFunc(lcd, 255,255,"");
        lcdFunc(lcd, 0, 0, "Vasen: " + paine + " mmHg"); //tulostetaan stringit näytölle
        lcdFunc(lcd, 0, 1, "Kosteus: " /*+ "FIXME" +*/ " %"); // FIXME kosteuden ilmaisin tähän
        
        //Serial.print("Oikea paine: "); // DEBUG Poista tämä kun ei enää tarvita
        //Serial.println(rightPressure); // DEBUG Poista tämä kun ei enää tarvita

        newDataReady = 0;
        t = millis();
      }        
      else {  //kun > 0, niin antaa oikean pakaran paineen

        String paine = String(int(rightPressure)); //muunnetaan painelaskelma merkkijonoksi, jotta se saadaan tulostettua
        lcdFunc(lcd, 255,255,"");
        lcdFunc(lcd, 0, 0, "Oikea: " + paine + " mmHg"); //tulostetaan stringit näytölle
        lcdFunc(lcd, 0, 1, "Kosteus: " /*+ "FIXME" +*/ " %"); // FIXME kosteuden ilmaisin tähän
        
        //Serial.print("Vasen paine: "); // DEBUG Poista tämä kun ei enää tarvita
        //Serial.println(leftPressure); // DEBUG Poista tämä kun ei enää tarvita

        newDataReady = 0;
        t = millis();
        }        
    }

if(leftPressure > WEIGHT_THRESHOLD || rightPressure > WEIGHT_THRESHOLD) {
      switch (state) {
        case WAIT_FOR_WEIGHT:
            StartTime = millis();  // timeri alkaa mitata ja tallentaa aikaa
            state = WAIT_FOR_ALARM; //odotellaan hälytystä
          break;

        case WAIT_FOR_ALARM:
          if(millis() - StartTime >= BREAKREMINDER) { //timeri ylittää 2 tunnin määräajan
            while(alarm) {
              setupAlarm(); //funktiota kutsutaan
              if (leftPressure < WEIGHT_THRESHOLD || rightPressure < WEIGHT_THRESHOLD) {
                alarm = false;
              }
            }
          }
          state = RESET_WAIT;  // odotetaan etta paine saadaan uudelleen sensoreille
          }
          if(rightPressure >= 760 || leftPressure>=760) {
            while(alarm) {
              setupAlarm(); //funktiota kutsutaan
              if (leftPressure < WEIGHT_THRESHOLD || rightPressure < WEIGHT_THRESHOLD) {
                alarm = false;
              }
            }
            state = BUTT_TIMEOUT;  
          }
          if(humidity >= 5000) { //TODO
            while(alarm) {
              setupAlarm(); //funktiota kutsutaan
              if (humidity <= 4999) { //TODO
                alarm = false;
              }
            } 
            state = RESET_WAIT; 
          }
          break;

        case BUTT_TIMEOUT:
          unassigned long butt_timer = 0;
          if(millis() - butt_timer >= 300000) {
            StartTime = 0;
            state = WAIT_FOR_ALARM;
          }
          break;

        case RESET_WAIT:
          if (millis() - StartTime > Interval) { //odotetaan 3 min ennen timerin uudelleen käynnistymistä
            StartTime = 0;
            alarm = true;
            state = WAIT_FOR_WEIGHT;  // resetoidaan tila ja odotetaan uutta painoa
          }
          break;
     }
  }
}

