#include <LiquidCrystal.h> //LCD-näytön kirjaston header
#include <HX711_ADC.h> //HX711 vahvistimen kirjaston header
#if defined(ESP8266)|| defined(ESP32) || defined(AVR) //Eri mikrokontrollerityyppejä
#include <EEPROM.h> //EEPROM -kirjaston header
#endif
#define BREAKREMINDER 36000000 // Time break
#define WEIGHT_THRESHOLD 12000.0 //Threshold to count for person sitting
bool mittaus;
unsigned long StartTime = 0; //Sitting timer
const unsigned long Interval = 18000; //no weight wait period
const int buzzerPin = 6;
bool alarm = true;
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

void SetupAlarm() {
  int i = 0;
  if(alarm == true)
  do{
    i++;
    tone(buzzerPin, 600, 100);
    delay(1000);
    noTone(buzzerPin);
    delay(1000);
  }while(i<3);
  alarm = false;
}

// Ääkkös-aliohjelma. Siirretään myöhemmin omaan headeriin?
void createCustomChars() {
  byte AwithDots[8] = {
    B01010,
    B00000,
    B01110,
    B00001,
    B01111,
    B10001,
    B01111,
  };
  
  byte OwithDots[8] = {
    B01010,
    B00000,
    B01110,
    B10001,
    B10001,
    B10001,
    B01110,
  };
  
  byte CapitalAwithDots[8] = {
    B01010,
    B00000,
    B01110,
    B10001,
    B11111,
    B10001,
    B10001,
  };
  
  byte CapitalOwithDots[8] = {
    B01010,
    B00000,
    B01110,
    B10001,
    B10001,
    B10001,
    B01110,
  };

//Muutetaan numerot, jos 1-4 aiheuttaa ongelmia muun koodin kanssa

  lcd.createChar(1, AwithDots);         // ä
  lcd.createChar(2, OwithDots);         // ö
  lcd.createChar(3, CapitalAwithDots);  // Ä
  lcd.createChar(4, CapitalOwithDots);  // Ö
}



void setup() {

Serial.begin(9600); delay(10);
  lcd.begin(16,2); //määritellään LCD-näytön mitat (16x2 -merkkiä)
  createCustomChars();  // Ääkköset: 1=ä, 2=ö, 3=Ä, 4=Ö

  //Anturien kalibrointi ja taaraus aloitetaan
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("K");
  lcd.write(1); // ä
  lcd.print("ynnistet");
  lcd.write(1); // ä
  lcd.write(1); // ä
  lcd.print("n...");
  lcd.setCursor(0,1);
  lcd.write(3); // Ä
  lcd.print("L");
  lcd.write(3); // Ä
  lcd.print(" ISTU");

  LoadCell.begin();
  //LoadCell.setReverseOutput(); //Kommentti pois jos halutaan muuttaa negatiivinen mittaustulos positiiviseksi, eli asettaa anturi toisin päin.
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
    lcd.print("ja pinnien");
    lcd.setCursor(0,1);
    lcd.print("asennot.");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("K");
    lcd.write(1); // ä
    lcd.print("ynnistys");
    lcd.setCursor(0,1);
    lcd.print("valmis");
    delay(2000);
  }
}

void loop() {

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
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Vasen pakara: ");
        lcd.setCursor(0,1);
        lcd.print(-i);  // käännetään etumerkki, jotta näkyy positiivisena paineena
        lcd.print(" mmHg");
        newDataReady = 0;
        t = millis();
      }        
        else {  //kun > 0, niin antaa oikean pakaran paineen
          int i = (weight/2 + LoadCell.getData()) * pressure; //FIXME weight toteutettanee jotenkin järkevämmin
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Oikea pakara: ");
          lcd.setCursor(0,1);
          lcd.print(i);
          lcd.print(" mmHg");
          newDataReady = 0;
          t = millis();
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
}