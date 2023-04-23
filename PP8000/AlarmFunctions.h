#define NOTE_1 2000
#define NOTE_2 1500
#define NOTE_3 1250
#define NOTE_4 1000
#define NOTE_5 1100


bool alarm = true; //asetetaan halytyksen alkuarvo (= tosi)
const int buzzerPin = 12; //summerin pinnin paikka Arduinossa, valittavissa

void setupAlarm() {  //luodaan halytysfunktio
  int i = 0;
  if(alarm == true) //kun halytys saa arvon tosi
  do{               //mennaan do-while -looppiin, jossa
    i++;            // integer i:n arvoa kasvatetaan kunnes saavutetaan maaratty arvo 
    tone(buzzerPin, 600, 100); //maaritellaan aanen korkeus ja pituus
    delay(1000);
    noTone(buzzerPin); //maaritellaan tauko halytysten valiin  
    delay(1000);
  }while(i<3); //kun halytysten maaratty arvo on saavutettu poistutaan loopista
}

void startUpSound() { 
  int j = 0;
  int noteDurations[] ={4,4,4,4}; // 8=kahdeksasosanuotti, 4=neljäsosanuotti jne.
  int notes[]={
    NOTE_4, NOTE_3, NOTE_2, NOTE_1 //melodian nuottitaulukko, määritelty definessä
  };
  for (j= 0; j<4;j++) {
    int noteDuration = 800 / noteDurations[j];
    int pauseBetweenNotes = noteDuration*1.30; //nuottien välinen vähimmäistauko
    tone(12, notes[j], noteDuration);
    delay(pauseBetweenNotes);
  } 
}

void errorSound() {
  int k = 0;
  int noteDurations[] ={8,8,8,8,3,2}; // 8=kahdeksasosanuotti, 4=neljäsosanuotti jne.
  int notes[]={
    NOTE_1, NOTE_2, NOTE_3, NOTE_4, 0, NOTE_4 //melodian nuottitaulukko, määritelty definessä
  };
  for (k = 0; k<6;k++) {
    int noteDuration = 800 / noteDurations[k];
    int pauseBetweenNotes = noteDuration*1.30; //nuottien välinen vähimmäistauko
    tone(12, notes[k], noteDuration);
    delay(pauseBetweenNotes);
  }
}

