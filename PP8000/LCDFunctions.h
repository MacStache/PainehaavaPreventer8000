#include <LiquidCrystal.h>

// Määritetään custom-merkit ääkkösille. Luodaan kaksiulotteinen taulukko, jossa on neljä riviä ja yhdellä rivillä on aina kahdeksan saraketta (LCD-näytössä on 8-riviä, joita nuo sarakkeet ohjaavat).
// Noihin kahdeksaan sarakkeeseen on määritelty kunkin ääkkösen kirjoitusasu siten, että 0 tarkoittaa LCD-näytöllä tyhjää pikseliä ja 1 tarkoittaa näytöllä täytettyä pikseliä.
// Kirjainten muodostamiseen käytetään vain 7 saraketta, koska kahdeksas ohjaa kirjaimen alapuolelle tulevaa lisäriviä, jota voisi käyttää esim. alleviivaukseen.
// Tässä koodissa custom-merkkitaulukon rivien sarakkeet on kirjoitettu allekkain koodin tulkinnan helpottamiseksi.
// Käytännössä kirjaimen määrittäminen onnistuisi myös (ja olisi koodirivien vähentämisenkin kannalta suotavaa) näin: {B01010,B00000,B01110,B00001,B01111,B10001,B01111}, joka vastaa ä-kirjainta.
// Voidaan valmiiseen koodiin korjata tarvittaessa tuohon muotoon.

byte customChars[4][8] ={
  // ä:
  {
    B01010,
    B00000,
    B01110,
    B00001,
    B01111,
    B10001,
    B01111
  },
  // ö:
  {
    B00000,
    B01010,
    B00000,
    B01110,
    B10001,
    B10001,
    B01110
  },
  // Ä:
  {
    B01010,
    B00000,
    B01110,
    B10001,
    B11111,
    B10001,
    B10001
  },
  // Ö:
  {
    B01010,
    B00000,
    B01110,
    B10001,
    B10001,
    B10001,
    B01110
  }
};

// Tässä customChar-taulukossa määritellyt ääkköset luodaan LiquidCrystal-kirjaston createChar()-funktiolla omiksi merkeiksi paikoille 1, 2, 3 ja 4, jotta ne voidaan kirjoittaa LCD-näytölle funktiolla lcd.write().

void createCustomChars(LiquidCrystal& lcd)
{
    lcd.createChar(1, customChars[0]); // ä
    lcd.createChar(2, customChars[1]); // ö
    lcd.createChar(3, customChars[2]); // Ä
    lcd.createChar(4, customChars[3]); // Ö
}

// lcd.write()-funktion ja lcd.print()-funktion yhtäaikaien käyttö kuitenkin osoittautui sekavaksi ja epäkäytännölliseksi.
// Tässä esimerkki miten ääkköset toimivat, mutta koodi oli sekavaa: lcd.print("K" + String((char)1) + "ynnistet" + String((char)1) + String((char)1) + "n...");
// Tuon pohjalta keksittiin, että replace()-funktiolla saadaan korvattua merkkijonon ääkköset itse määritellyillä merkeillä, jolloin ne saadaan tulostettua selkeämmin.
// Replace()-funktiolla pystytään korvaamaan yksittäisiä merkkejä/merkkijonoja toisilla merkeillä/merkkijonoilla.

void replaceCustomChars(String * printString) // Siirretään korvattavat merkit LCD-näytölle kirjoitettavaan viestiin pointterin avulla
{
    printString->replace("ä",String((char)1));
    printString->replace("ö",String((char)2));
    printString->replace("Ä",String((char)3));
    printString->replace("Ö",String((char)4));
}

void lcdFunc(LiquidCrystal& lcd, uint8_t col, uint8_t row, String message) // LCD-näytölle kirjoitettava teksti viedään message Stringiin
{
    if (col == 255 && row == 255) { //Sarake 255 ja Rivi 255 tyhjentää näytön
    lcd.clear();
    }
    else {
    lcd.setCursor(col,row); // Asetetaan kursori haluttuun sarakkeeseen ja riville
    }
    replaceCustomChars(&message); // Annetaan message-stringin osoite printString-pointterille, joka tuo ääkkösmuutokset stringiin
    lcd.print(message); // Tulostetaan lopullinen teksti LCD-näytölle korvattujen merkkien kanssa
}