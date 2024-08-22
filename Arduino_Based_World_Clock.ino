#include <Wire.h>
#include <RTClib.h>
#include <LedControl.h>
//-----------------------------------------------------------------------------------------------------------pin no.s
// Constants for button pins
const int buttonDelhi = 2;
const int buttonParis = 3;
const int buttonTokyo = 4;
const int buttonNewYork = 5;
const int buttonDubai = 6;
const int buttonSet = 7;
const int buttonSnooze = 8;
const int buttonStop = 9;

// Constants for LED pins corresponding to city buttons
const int ledDelhi = 42;
const int ledParis = 43;
const int ledTokyo = 44;
const int ledNewYork = 45;
const int ledDubai = 41;

// Constants for 7-segment display using MAX7219
const int MAX7219_CLK = 52;
const int MAX7219_CS = 53;
const int MAX7219_DIN = 51;
const int NUM_DISPLAYS = 1; // Number of MAX7219 modules used
LedControl lc = LedControl(MAX7219_DIN, MAX7219_CLK, MAX7219_CS, NUM_DISPLAYS);

// Constants for buzzer pin
const int buzzerPin = 10;
//-----------------------------------------------------------------------------------------------------------------
// Variables for clock and alarm
RTC_DS3231 rtc;
DateTime currentTime;
DateTime alarmTime;
DateTime temp;
bool set_mode = false;
bool status = true;

struct times{
  int hour = 0;
  int minute = 0;
  int second = 0;
};
times curr;



int extrahr = 0;
int extramin = 0;
int category = 0;
bool blink = true;
bool alarmactive = false;
bool resmod = false;
int tin = 1;
int alarm_curr = 0;
int thanos = ledDelhi;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  rtc.begin();
  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  

  // Initialize MAX7219 Display
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  // Initialize button and LED pins
  pinMode(buttonDelhi, INPUT);
  pinMode(buttonParis, INPUT);
  pinMode(buttonTokyo, INPUT);
  pinMode(buttonNewYork, INPUT);
  pinMode(buttonDubai, INPUT);
  pinMode(buttonSet, INPUT);
  pinMode(buttonSnooze, INPUT);
  pinMode(buttonStop, INPUT);
  pinMode(ledDelhi, OUTPUT);
  pinMode(ledParis, OUTPUT);
  pinMode(ledTokyo, OUTPUT);
  pinMode(ledNewYork, OUTPUT);
  pinMode(ledDubai, OUTPUT);

  // Initialize buzzer pin
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // Read current time from RTC
  currentTime = rtc.now();
  // Check if alarm is active and trigger if time matches
  
  if (currentTime.hour() == alarmTime.hour() && currentTime.minute() == alarmTime.minute()) {
    alarmactive = true;
    activateAlarm();
  }
  else{
    alarmactive = false;
  }
  if(!alarmactive){
    for(int i = 41; i <=45; i++){
      if(i==thanos && status)digitalWrite(i, HIGH);
      else digitalWrite(i, LOW);
    }
  }
  // Check city buttons and update display accordingly
  updateCityButtons();

  // Check other buttons (set, snooze, stop)
  checkOtherButtons();

  // Update time on the display
  updateTimeDisplay(currentTime.hour(), currentTime.minute(), currentTime.second());
  
  // Delay to update display and button states
  delay(500);
}

void updateCityButtons() {
  if (digitalRead(buttonDelhi) == HIGH) {
    if(resmod){
      if(category == 0){
        curr.hour += 1;
        if(curr.hour == 24)curr.hour=0;
      }
      if(category == 1){
        curr.minute+=1;
        if(curr.minute == 60)curr.minute=0;
      }
      if(category == 2){
        curr.second+=1;
        if(curr.second == 60)curr.second=0;
      }
      return;
    }
    if(set_mode){
      if(category == 0){
        if(alarmTime.hour() == 23){
          alarmTime = DateTime(alarmTime.year(),alarmTime.month(),alarmTime.day(),0,alarmTime.minute(),alarmTime.second());
        }
        else{
          alarmTime = DateTime(alarmTime.year(),alarmTime.month(),alarmTime.day(),alarmTime.hour()+1,alarmTime.minute(),alarmTime.second());
        }
      }
      if(category == 1){
        if(alarmTime.minute() == 59){
          alarmTime = DateTime(alarmTime.year(),alarmTime.month(),alarmTime.day(),alarmTime.hour(),0,alarmTime.second());
        }
        else{
          alarmTime = DateTime(alarmTime.year(),alarmTime.month(),alarmTime.day(),alarmTime.hour(),alarmTime.minute()+1,alarmTime.second());
        }
      }
      if(category == 2){
        if(alarmTime.second() == 59){
          alarmTime = DateTime(alarmTime.year(),alarmTime.month(),alarmTime.day(),alarmTime.hour(),alarmTime.minute(),0);
        }
        else{
          alarmTime = DateTime(alarmTime.year(),alarmTime.month(),alarmTime.day(),alarmTime.hour(),alarmTime.minute(),alarmTime.second()+1);
        }
      }
      return;
    }
    extrahr=0;
    extramin=0;
    thanos = ledDelhi;
    displayCityTime("Delhi", 5, 30); // Set the city's time (e.g., Delhi's time)
    updateLED(ledDelhi);
  }
  else if (digitalRead(buttonParis) == HIGH) {
    if(resmod){
      if(category == 0){
        curr.hour -= 1;
        if(curr.hour == -1)curr.hour=23;
      }
      if(category == 1){
        curr.minute-=1;
        if(curr.minute == -1)curr.minute=59;
      }
      if(category == 2){
        curr.second-=1;
        if(curr.second == -1)curr.second=59;
      }
      return;
    }
    if(set_mode){
      if(category == 0){
        if(alarmTime.hour() == 0){
          alarmTime = DateTime(alarmTime.year(),alarmTime.month(),alarmTime.day(),23,alarmTime.minute(),alarmTime.second());
        }
        else{
          alarmTime = DateTime(alarmTime.year(),alarmTime.month(),alarmTime.day(),alarmTime.hour()-1,alarmTime.minute(),alarmTime.second());
        }
      }
      if(category == 1){
        if(alarmTime.minute() == 0){
          alarmTime = DateTime(alarmTime.year(),alarmTime.month(),alarmTime.day(),alarmTime.hour(),59,alarmTime.second());
        }
        else{
          alarmTime = DateTime(alarmTime.year(),alarmTime.month(),alarmTime.day(),alarmTime.hour(),alarmTime.minute()-1,alarmTime.second());
        }
      }
      if(category == 2){
        if(alarmTime.second() == 0){
          alarmTime = DateTime(alarmTime.year(),alarmTime.month(),alarmTime.day(),alarmTime.hour(),alarmTime.minute(),59);
        }
        else{
          alarmTime = DateTime(alarmTime.year(),alarmTime.month(),alarmTime.day(),alarmTime.hour(),alarmTime.minute(),alarmTime.second()-1);
        }
      }
      return;
    }
    extrahr=-3;
    extramin=-30;
    displayCityTime("Paris", 9, 45); // Set the city's time (e.g., Paris's time)
    thanos = ledParis;
    updateLED(ledParis);
  }
  else if (digitalRead(buttonTokyo) == HIGH) {
    if(set_mode || resmod){
      category+=1;
      if(category==3){
        category = 0;
      }
      return;
    }
    extrahr=3;
    extramin=30;
    displayCityTime("Tokyo", 14, 20); // Set the city's time (e.g., Tokyo's time)
    thanos = ledTokyo;
    updateLED(ledTokyo);
  }
  else if (digitalRead(buttonNewYork) == HIGH) {
    if(set_mode || resmod){
      category-=1;
      if(category==-1){
        category = 2;
      }
      return;
    }
    extrahr=-9;
    extramin=-30;
    displayCityTime("New York", 18, 10); // Set the city's time (e.g., New York's time)
    thanos = ledNewYork;
    updateLED(ledNewYork);
  }
  else if (digitalRead(buttonDubai) == HIGH) {
    extrahr=-1;
    extramin=-30;
    displayCityTime("Dubai", 20, 0); // Set the city's time (e.g., Dubai's time)
    thanos = ledDubai;
    updateLED(ledDubai);
  }
}

void updateTimeDisplay(int hour, int minute, int sec) {
  lc.clearDisplay(0); // Clear the display
  if (!status){
    return;
  }
  if(set_mode){
    resmod = false;
    hour = alarmTime.hour();
    minute = alarmTime.minute();
    sec = alarmTime.second();
  }
  if(resmod){
    hour = curr.hour;
    minute = curr.minute;
    sec = curr.second;
  }
  blink = !blink;
  if (minute + extramin >=60){
    hour+=1;
  }
  if (minute + extramin < 0)
  {
    hour-=1;
  }
  minute+=extramin;
  hour+=extrahr;
  if(minute < 0){
    minute+=60;
  }
  if(hour < 0){
    hour+=24;
  }
  hour = hour%24;
  minute = minute%60;
  
  lc.setDigit(0, 7, hour / 10, false); // Display hour (tens place)
  lc.setDigit(0, 6, hour % 10, false); // Display hour (ones place)
  lc.setDigit(0, 4, minute / 10, false); // Display minute (tens place)
  lc.setDigit(0, 3, minute % 10, false); // Display minute (ones place)
  lc.setDigit(0, 1, sec / 10, false); // Display minute (tens place)
  lc.setDigit(0, 0, sec % 10, false); // Display minute (ones place)
  if((set_mode || resmod) && blink){
    lc.clearDisplay(0);
    if(category == 0){
      lc.setDigit(0, 4, minute / 10, false); // Display minute (tens place)
      lc.setDigit(0, 3, minute % 10, false); // Display minute (ones place)
      lc.setDigit(0, 1, sec / 10, false); // Display minute (tens place)
      lc.setDigit(0, 0, sec % 10, false); 
    }
    if(category == 1){
      lc.setDigit(0, 7, hour / 10, false); // Display minute (tens place)
      lc.setDigit(0, 6, hour % 10, false); // Display minute (ones place)
      lc.setDigit(0, 1, sec / 10, false); // Display minute (tens place)
      lc.setDigit(0, 0, sec % 10, false); 
    }
    if(category == 2){
      lc.setDigit(0, 4, minute / 10, false); // Display minute (tens place)
      lc.setDigit(0, 3, minute % 10, false); // Display minute (ones place)
      lc.setDigit(0, 7, hour / 10, false); // Display minute (tens place)
      lc.setDigit(0, 6, hour % 10, false); 
    }
  }

}

void displayCityTime(String city, int hour, int minute) {
  Serial.println(city);
  Serial.println(hour);
}

void updateLED(int ledPin) {
  digitalWrite(ledDelhi, LOW);
  digitalWrite(ledParis, LOW);
  digitalWrite(ledTokyo, LOW);
  digitalWrite(ledNewYork, LOW);
  digitalWrite(ledDubai, LOW);
  digitalWrite(ledPin, HIGH);
}

void checkOtherButtons() {
  if (digitalRead(buttonSet) == HIGH) {
    set_mode = !set_mode;
  }

  if (digitalRead(buttonSnooze) == HIGH) {
    // Code to snooze alarm (delay by 5 minutes)
    if(!alarmactive){
      if(resmod){
        rtc.adjust(DateTime(currentTime.year(),currentTime.month(),currentTime.day(),curr.hour,curr.minute,curr.second));
      }
      resmod = !resmod;
      return;
    }
    int k = alarmTime.hour();
    int m = alarmTime.minute();
    m+=5;
    if(m>=60){
      k+=1;
    }
    alarmTime = DateTime(alarmTime.year(),alarmTime.month(),alarmTime.day(),k%24,m%60,alarmTime.second());
  }

  if (digitalRead(buttonStop) == HIGH) {
    // Code to stop the alarm
    if(!alarmactive){
      if(resmod){
        resmod=false;
        return;
      }
      status=!status;
      return;
    }
    alarmTime = DateTime(alarmTime.year(),alarmTime.month(),alarmTime.day(),0,0,0);
    
  }
}

void activateAlarm() {
  digitalWrite(alarm_curr%5 + 41, LOW);
  alarm_curr+=1;
  digitalWrite(alarm_curr%5 + 41, HIGH);
  tone(11,tin%3000,200);
  tin+=10;
}

void stopAlarm() {
  
}


