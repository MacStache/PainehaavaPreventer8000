#include <LiquidCrystal.h>
//FIXME Fiksataan koko looppi. Pohja on valmiina, mutta ei jostain syystä pelitä (vaihda ääkkösiä Pasin koodin mukaisesti)


void createCustomChars(LiquidCrystal& lcd) {
  byte AwithDots[] = {
  B00000,
  B00000,
  B01010,
  B11111,
  B00001,
  B11111,
  B10001,
  B11111
  };
  
  byte OwithDots[] = {
  B00000,
  B00000,
  B01010,
  B11111,
  B10001,
  B10001,
  B10001,
  B11111
  };
  
  byte CapitalAwithDots[] = {
   B01010,
   B00100,
   B01010,
   B01010,
   B01110,
   B11011,
   B10001,
   B10001
  };
  
  byte CapitalOwithDots[] = {
    B01010,
    B00000,
    B11111,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111
  };

  lcd.createChar(1, AwithDots);         // ä
  lcd.createChar(2, OwithDots);         // ö
  lcd.createChar(3, CapitalAwithDots);  // Ä
  lcd.createChar(4, CapitalOwithDots);  // Ö
}

void lcdFunc(LiquidCrystal& lcd, uint8_t col, uint8_t row, const char message[]) { //LCD funktioiden wrapperi. Annetaan sille kirjasto, sarakkeet, rivit ja viesti käsiteltäväksi.
  if (col == 255 && row == 255) { //Sarake 255 ja Rivi 255 tyhjentää näytön
    lcd.clear();
  }
  else {
    lcd.setCursor(col,row); // Asetetaan kursori haluttuun sarakkeeseen ja riville
  }
  for(int i = 0; i < strlen(message); i++){
  if (message[i] == 'ä'){
    lcd.write((uint8_t)1);
  }
  else if (message[i] == 'ö') {
    lcd.write((uint8_t)2);
  }
  else if (message[i] == 'Ä') {
    lcd.write((uint8_t)3); 
  }
  else if (message[i] == 'Ö') {
    lcd.write((uint8_t)4); 
  }
  else {
    lcd.print(message[i]); //Kirjoitetaan viesti
  }
}
}