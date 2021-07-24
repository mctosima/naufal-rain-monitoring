#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "RTClib.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 2); // RX, TX

RTC_DS1307 rtc;


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4); //alamat I2C SDA SCL

void sendMessage( String msg ){ //kirim pesan ke nodemcu
  for( int i = 0 ; i < msg.length() ; i++ ){
    mySerial.write( msg.charAt(i) );
    delay(2);
  }
  mySerial.write('@');
}


void setup()
{
  //============================>
  Serial.begin(9600);
  mySerial.begin(9600);
  while(false){
    sendMessage("Hello");
    delay(100);
  }
  //============================>
  lcd.begin();
  //=============================>
  lcd.backlight();
  //=============================>
   if (! rtc.begin()) { //setting rtc
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
 
}
//==================================================>
String getJam(){ //ambil data jam rtc
  DateTime now = rtc.now();
  return String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
}

String getTanggal(){ //ambil data tanggal rtc
  DateTime now = rtc.now();
  return String(now.year()) + "-" + String(now.month()) + "-" + String(now.day());
}

//================================================>
float mmPerTip = 0.2 ;
float curahHujan = 0 ;
float counterCurahHujan = 0 ;
int pinTippingSensor = A0 ;

void readTippingBucket(){ //deteksi sensor
  bool sensorActive = false ;
  pinMode( pinTippingSensor , INPUT );
  unsigned long tmTipping = millis();
  counterCurahHujan = 0 ;
  //============================================>
  Serial.println();Serial.println();
  Serial.println("Mode Membaca Tipping Bucket-start");
  int counttip = 0 ;
  //============================================>
  while( (millis()-tmTipping) < 90000 ){
    if( digitalRead(A0) == sensorActive ){
      counterCurahHujan = counterCurahHujan + 1 ;

      //====================================>
      Serial.println("#");
      if( counttip > 15 ){
        Serial.println();
        counttip = 0 ;
      }else{
        counttip = counttip + 1 ;
      }
      delay(150);
      while( digitalRead(A0) == sensorActive ){
        delay(50);
      }
    }
    lcdShow();
  }
  counterCurahHujan;
  //===================================>
  Serial.println("Mode Membaca Tipping Bucket-selesai");
  //===================================> 
  curahHujan = ( counterCurahHujan * mmPerTip ) ; 
  //===============================>
  tmTipping = millis();
}

//=================================================>
unsigned long tmlcd = millis();

void lcdShow(){
  //=================================================>
  if( (millis()-tmlcd) > 1000 ){
    //=================================================>
    lcd.clear();
    //=================================================>
    lcd.setCursor(0,0);lcd.print("Tictoc : " +  String ( counterCurahHujan,0 ));
    lcd.setCursor(0,1);lcd.print("Curah Hujan : " +  String ( curahHujan ));
    lcd.setCursor(0,2);lcd.print("Tanggal : " +  getTanggal());
    lcd.setCursor(0,3);lcd.print("Jam : " +  getJam());
    

    tmlcd = millis();
  }
}
//=================================================>

unsigned long tmSend = millis();

void loop()
{
  //==============================================>
  readTippingBucket();
  //=================================================>
  lcdShow();
  //==============================================>
  if( (millis()-tmSend) > 1000 && curahHujan > 0 ){ //mengirim data curah hujan ke esp
    String data = String(curahHujan) + "," + getTanggal() + "," + getJam() + ",";
    sendMessage(data);
    Serial.println();Serial.println();
    Serial.println("=================");
    Serial.println(data);
    tmSend = millis();
  }
  
 
}
