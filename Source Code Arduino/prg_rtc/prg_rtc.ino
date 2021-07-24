#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "RTClib.h"
//===========================================>
RTC_DS1307 rtc;
//===========================================>
LiquidCrystal_I2C lcd(0x27, 20, 4);
//===========================================>
void setup()
{
  //============================>
  Serial.begin(9600);
  //============================>
  lcd.begin();
  //=============================>
  lcd.backlight();
  //=============================>
   if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 
}
//==================================================>
String getJam(){
  DateTime now = rtc.now();
  return String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
}

String getTanggal(){
  DateTime now = rtc.now();
  return String(now.year()) + "-" + String(now.month()) + "-" + String(now.day());
}


//=================================================>
unsigned long tmlcd = millis();

void lcdShow(){
  //=================================================>
  if( (millis()-tmlcd) > 700 ){
    //=================================================>
    lcd.clear();
    //=================================================>
    lcd.setCursor(0,0);
    lcd.print(getJam()) ;
    lcd.setCursor(0,1);
    lcd.print(getTanggal());
    tmlcd = millis();
  }
}
//=================================================>

unsigned long tmSend = millis();

void loop()
{
  //=================================================>
  lcdShow();
  //==============================================>
  if( (millis()-tmSend) > 1000 ){
    String data = String( getTanggal() + "," + getJam() + "," );
    Serial.println();Serial.println();
    Serial.println("=================");
    Serial.println(data);
    tmSend = millis();
  }
  delay(10);
}
