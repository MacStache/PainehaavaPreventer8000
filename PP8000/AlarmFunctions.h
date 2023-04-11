bool alarm = true;
const int buzzerPin = 6;

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