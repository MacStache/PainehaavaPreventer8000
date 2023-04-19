bool alarm = true; //asetetaan halytyksen alkuarvo (= tosi)
const int buzzerPin = 12; //summerin pinnin paikka Arduinossa, valittavissa

void SetupAlarm() {  //luodaan halytysfunktio
  int i = 0;
  if(alarm == true) //kun halytys saa arvon tosi
  do{               //mennaan do-while -looppiin, jossa
    i++;            // integer i:n arvoa kasvatetaan kunnes saavutetaan maaratty arvo 
    tone(buzzerPin, 600, 100); //maaritellaan aanen korkeus ja pituus
    delay(1000);
    noTone(buzzerPin); //maaritellaan tauko halytysten valiin  
    delay(1000);
  }while(i<3); //kun halytysten maaratty arvo on saavutettu poistutaan loopista
  alarm = false; // ja halytyksen arvoksi tulee (=epatosi), tama koodi siis halyttaa kolmesti
}

void startUpSound() {
  tone(buzzerPin, 1000, 100); 
  delay(100);
  tone(buzzerPin, 1250, 100); 
  delay(100);
  tone(buzzerPin, 1500, 100); 
  delay(100);
  tone(buzzerPin, 2000, 400);  
}

void errorSound() {
  tone(buzzerPin, 2000, 100); 
  delay(100);
  tone(buzzerPin, 1500, 100); 
  delay(100);
  tone(buzzerPin, 1250, 100); 
  delay(100);
  tone(buzzerPin, 1000, 400);
  delay(500);
  noTone(buzzerPin);
  tone(buzzerPin, 1000, 400);
  delay(500);
  noTone(buzzerPin);
  tone(buzzerPin, 1000, 400);
}