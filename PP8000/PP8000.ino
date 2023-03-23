#include <LiquidCrystal.h>

LiquidCrystal lcd(1,2,3,4,5,6); //määritellään käytettävät LCD-portit. 
                                //Portit 1-2 on tarkoitettu R/S (Register Select) ja E (Enable) porteille ja 3-6 porteille joista syötetään bittejä näytölle.
                                //R/S-portti on portti jonka kautta näytölle syötetään komentoja
                                //E-portti on portti joka avaa rekisterin kirjoitusta varten

void setup() {

  Serial.begin(9600);
  lcd.begin(16,2); //määritellään LCD-näytön mitat (16x2 -merkkiä)
}

void loop() {
  
  lcd.print("Kylkiasentoon!"); //Tulostetaan näytölle teksti
  delay(3000); //Odotetaan 3 sekuntia

  lcd.setCursor(2,1);// Siirretään kursori sarakkeeseen 3 (lasketaan nollasta) ja riville 2 (lasketaan nollasta)
  lcd.print("PP8000"); //tulostetaan teksti
  delay(3000); //odotetaan 3 sekuntia

  lcd.clear(); //tyhjennetään LCD-näyttö

  float alkuArvo = analogRead(A0);
  float massa = (alkuArvo - 511)*125.0/6.0; // Ei mitään hajua mitä tässä lasketaan, otin youtubesta

  Serial.print("Massa: ");
  Serial.print(massa);  // Näyttää vielä mitä sattuu, pitää kalibroida
  Serial.print(" kg");
  Serial.print("	V: ");
  Serial.print(analogRead(A0)*5.0/1023.0);  // Lasketaan analogipinnin saama jännite
  Serial.println("V");
  delay(10);
}
