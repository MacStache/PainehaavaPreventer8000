#include <LiquidCrystal.h>
//FIXME Fiksataan koko looppi. Pohja on valmiina, mutta ei jostain syystä pelitä (vaihda ääkkösiä Pasin koodin mukaisesti)
void createCustomChars(LiquidCrystal& lcd) {

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
    B00000,
    B01110,
    B10001,
    B11111,
    B10001,
    B10001,
    B10001,
  };
  
  byte CapitalOwithDots[8] = {
    B00000,
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B01110,
  };
  
  lcd.createChar(1, AwithDots);         // ä
  lcd.createChar(2, OwithDots);         // ö
  lcd.createChar(3, CapitalAwithDots);  // Ä
  lcd.createChar(4, CapitalOwithDots);  // Ö
}

createCustomChars(lcd);

void lcdFunc(LiquidCrystal& lcd, uint8_t col, uint8_t row, const char message[]) { //LCD funktioiden wrapperi. Annetaan sille kirjasto, sarakkeet, rivit ja viesti käsiteltäväksi.
  if (col == 255 && row == 255) { //Sarake 255 ja Rivi 255 tyhjentää näytön
    lcd.clear();
  }
  else {
    lcd.setCursor(col,row); // Asetetaan kursori haluttuun sarakkeeseen ja riville
  }
  for(int i = 0; i < strlen(message); i++){
  if (message[i] == 'ä'){
    lcd.write(1);
  }
  else if (message[i] == 'ö') {
    lcd.write(2);
  }
  else if (message[i] == 'Ä') {
    lcd.write(3); 
  }
  else if (message[i] == 'Ö') {
    lcd.write(4); 
  }
  else {
    lcd.print(message[i]); //Kirjoitetaan viesti
  }
}
}