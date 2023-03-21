#include <LiquidCrystal.h>

LiquidCrystal lcd(1,2,3,4,5,6);

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.print("Kylkiasentoon!");
  delay(3000);

  lcd.setCursor(2,1);
  lcd.print("PP8000");
  delay(3000);

  lcd.clear();
}
