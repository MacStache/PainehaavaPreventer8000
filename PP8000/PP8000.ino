#include <HX711_ADC.h> //HX711 vahvistimen kirjaston header
#include <LiquidCrystal.h>
#include "LCDFunctions.h" //LCD-funktioiden aliohjelmat
#include "AlarmFunctions.h" //Hälytinfunktioiden aliohjelmat

//#include <EEPROM.h> //EEPROM -kirjaston header Tätä ei välttämättä tarvita?!?

#define BREAKREMINDER 36000000 // Time break
#define WEIGHT_THRESHOLD 12000.0 //Threshold to count for person sitting

bool mittaus = false;
bool taaraus = true; //Aseta tämä false asentoon jos et halua taarata

unsigned long StartTime = 0; //Sitting timer
const unsigned long Interval = 18000; //no weight wait period

float humidity; //FIXME

//pinnit:
const int HX711_dout = 10; //mcu > HX711 dout pinni
const int HX711_sck = 11; //mcu > HX711 sck pinni

//HX711 määrittely:
HX711_ADC LoadCell(HX711_dout, HX711_sck); //LoadCell() saa tietonsa HX711_dout ja _sck pinneistä

const int calVal_eepromAdress = 0; //Asetetaan EEPROM-osoitteeksi 0. EEPROM = Electrically Erasable Programmable Read-Only Memory
unsigned long t = 0;

//Mittaukseen liittyvät muuttujat
float pressure = 9.81/(0.1*0.1)/133.322;  //paineen laskukaava elohopeamillimetreinä (10x10cm pinta-alalla)
float weight =80;  //käyttäjän paino muuttujana: saadaanko laitteesta asetettua tämä käyttäjän todellisen painon mukaan?
float calibrationValue = 22500;   //Kalibrointimuuttuja: säädä omaan tarpeeseen, jos ei toimi samalla arvolla, weight pitää nollata, jos tarvii kalibroida!


LiquidCrystal lcd(2,3,4,5,6,7); //määritellään käytettävät LCD-portit. 
                                //Portit 1-2 on tarkoitettu R/S (Register Select) ja E (Enable) porteille ja 3-6 porteille joista syötetään bittejä näytölle.
                                //R/S-portti on portti jonka kautta näytölle syötetään komentoja
                                //E-portti on portti joka avaa rekisterin kirjoitusta varten

void setup() {

lcd.begin(16,2); // Määritellään LCD-näytön koko
createCustomChars(lcd);

}

void loop() {

while (taaraus == true){  //Loopin alku rullataan läpi niin kauan kuin "taaraus" -kytkimen asento on true
                          //Siirsin tämän osan koodia setupista loopin alkuun.
  lcdFunc(lcd, 255,255,""); //lcdFunc tyhjentää näytön
  lcdFunc(lcd, 0,0, "Käynnistetään"); //lcdFunc kirjoittaa ensimmäisen sarakkeen ensimmäiseen riviin viestin
  lcdFunc(lcd, 0,1, "ÄÄ ÖÖ"); //lcdFunc kirjoittaa ensimmäisen sarakkeen toiselle riville viestin

  LoadCell.begin();
  //LoadCell.setReverseOutput(); //Kommentti pois jos halutaan muuttaa negatiivinen mittaustulos positiiviseksi, eli asettaa anturi toisin päin.
  unsigned long stabilizingtime = 2000; // Käynnistyksen jälkeistä tarkkuutta voidaan korottaa asettamalla pidempi stabilointiaika (nyt 2000ms/2s)
  boolean _tare = true;

  //anturien taaraus
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag()) {
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
    lcdFunc(lcd, 0, 1, "Valmis");
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
    if (millis() > t + serialPrintInterval) {

      if (LoadCell.getData() < 0){  //kun < 0, niin antaa vasemman pakaran paineen
        int i = (-weight/2 + LoadCell.getData()) * pressure;  //FIXME weight toteutettanee jotenkin järkevämmin. huom etumerkki, jotta saadaan positiivinen lukema.
        lcdFunc(lcd, 255,255,"");
        lcdFunc(lcd, 0, 0, "Vasen pakara");
        lcdFunc(lcd, 0, 1, "mmHg: "); //Vaihdoin tässä kokonaislukuna tulevan int i -muuttujan ja "mmHg" -tekstiosan paikat, jotta lcdFunc -toimii oikein ja mahdollisimman pienellä säädöllä
        lcd.print(-i);
        newDataReady = 0;
        t = millis();
      }        
      else {  //kun > 0, niin antaa oikean pakaran paineen
        int i = (weight/2 + LoadCell.getData()) * pressure; //FIXME weight toteutettanee jotenkin järkevämmin
        lcdFunc(lcd, 255,255,"");
        lcdFunc(lcd, 0, 0, "Oikea pakara");
        lcdFunc(lcd, 0, 1, "mmHg: ");
        lcd.print(i);
        newDataReady = 0;
        t = millis();
        }        
    }
  }

// 'enum' (enumeration) easy way to assign unique values/numbers to a bunch of names 
// where you don't really care which values they get as long as they are unique:
enum States {
  WAIT_FOR_WEIGHT, WAIT_FOR_ALARM, WAIT_FIRST_ALARM, WAIT_SEC_ALARM, WAIT_THIRD_ALARM, RESET_WAIT
} state = WAIT_FOR_WEIGHT;


  switch (state)
  {
    case WAIT_FOR_WEIGHT:
      if(weight > WEIGHT_THRESHOLD)
      {
        StartTime = millis();  // record the time
        state = WAIT_FIRST_ALARM; WAIT_SEC_ALARM; WAIT_THIRD_ALARM;
      }
      break;

    case WAIT_FIRST_ALARM:
      if(millis() - StartTime >= BREAKREMINDER)
      {
        alarm = true;
        SetupAlarm();
        StartTime = millis();  //record a new time to time against
        state = WAIT_SEC_ALARM;  // move to next state
      }
      break;

    case WAIT_SEC_ALARM:
      if((millis() - StartTime >= BREAKREMINDER) || (pressure >= 76800))//FIXME
      {
        alarm = true;
        SetupAlarm();
        StartTime = millis();  // get a new time to time against
        state = RESET_WAIT;
      }
      break;
    
    case WAIT_THIRD_ALARM:
      if((millis() - StartTime >= BREAKREMINDER) || (humidity>= 5000)) //FIXME
      {
        alarm = true;
        SetupAlarm();
        StartTime = millis(); //get a new time to time against
        state = RESET_WAIT;
      }
      break;

    case RESET_WAIT:
      if (millis() - StartTime > Interval) {
        state = WAIT_FOR_WEIGHT;  // reset the state to wait for the next weight
        // do anything else you want to do before you go around again.
      }
      break;
    }
  }
