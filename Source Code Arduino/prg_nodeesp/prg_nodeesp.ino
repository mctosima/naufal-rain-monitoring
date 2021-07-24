#include <Wire.h>
//============================>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
SoftwareSerial ard;
//============================>
const char* ssid      = "rouuter"   ;
const char* password  = ""  ;
//===============> inisial server <====================//
WiFiServer server(80);
//============================>

//=====================>
void connectWifi(){ //koneksi
  //============================>
  WiFi.begin(ssid, password);
  //============================>
  Serial.println("Connecting..");
  int countFailed = 0 ;
  unsigned long tmOut = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if( countFailed > 10 ){
      countFailed = 0 ;
      Serial.println();
      WiFi.begin(ssid, password);
    }
    countFailed++ ;
  }
  server.begin();
  Serial.println("Server started");
  //============================>
  Serial.println("inisial hotspot finish");
  Serial.println(WiFi.localIP());
}

//=======================================>
String data = "12" ;
String tgl = "2020-12-02" ;
String jam = "12:45:11" ;
//=======================================>
String servername = "192.168.43.40/monitoringcurahhujan" ;
byte mode_uploaddata = 1 ;
byte mode_uploadlogdata = 2 ;

String createUrl( int mode ){

  String filephpname = "" ;
  if( mode == mode_uploaddata ){
    filephpname = "uploaddata.php" ;
  }
  if( mode == mode_uploadlogdata ){
    filephpname = "uploadlogdata.php" ;
  }
  return "http://"+servername+"/"+filephpname+"?data=" + data + "&tgl=" + tgl + "&jam=" + jam ;
}


//=====================>
String cmd = "";  
void sendReqCommand(String str){
  Serial.println(str);
  HTTPClient http;  
  http.begin(str);
  int httpCode= http.GET();    
  if(httpCode>0){  
      Serial.println("Output dari server");
      cmd = http.getString(); 
      for( int i = 0 ; i < cmd.length() ; i++ ){
        Serial.print(cmd.charAt(i));delay(0.1);
      }
      Serial.println("success");
 
  }else{
      Serial.println("No Connection");
  }
  http.end();   
}


//==========================================>
String getCompMessageStr(){ //
  String temps = "" ;
  while( Serial.available() ){
    char chr = char(Serial.read());
    temps += chr ;
    delay(4);
  }
  return temps ;
}

String getArdMessageStr(){ //mengambil pesan arduino
  String temps = "" ;
  while( true ){
    //============================================>
    char chr = ' ' ;
    unsigned long tms = millis();
    while((millis()-tms) < 1000 ){
      if( ard.available() ){
        chr = char(ard.read());
        break ;
      }
    }
    if( chr == '@' ) 
      break ;
    if( chr != ' ' ){
      temps += chr ;
    }
  }
  return temps ;
}

//==> data masuk "12,2020-12-02,13:15:14" 
String parseMessage( String msg ){ 
  String temp = "" ;
  int index = 0 ;
  for( int i = 0 ; i < msg.length() ; i++ ){
    char temps = msg.charAt(i);
    if( temps == ',' ){
      Serial.println("Num : " + String( index ) + ", Value : " + temp );
      if( index == 0 ){
        data = temp ;
      }
      else if( index == 1 ){
        tgl = temp ;
      }
      else if( index == 2 ){
        jam = temp ;
      }
      temp = "" ;
      index++ ;
    }else{
      temp += temps ;
    }
    delay(1);
  }
  return temp ;
}


//============================>
String msg = "" ; //software serial koneksi esp ke arduino
//============================>
void setup() {
  Serial.begin(9600); 
  ard.begin(9600, SWSERIAL_8N1, 14, 12, false, 100, 11);
  //================>
  while(false){
    String msg = getArdMessageStr();
    if( msg.length() > 0 ){
      Serial.println(msg);
    }
    delay(10);
  }
  //=========================> inisial wifi .
  connectWifi();
}

//===============================>

unsigned long tmSendLogData = millis();

void loop() {
/*  
 if( (millis()-tmSendData) > 2000 ){
  Serial.println("Kirim data curah hujan ...");
  sendReqCommand( createUrl( mode_uploaddata ) );
  tmSendData = millis();
 }
*/
 if( (millis()-tmSendLogData) > 30000){
  Serial.println("Kirim data log curah hujan ...");
  sendReqCommand( createUrl( mode_uploadlogdata ) );
  tmSendLogData = millis();
 }
 String dt = "" ;
 String dt_ard = getArdMessageStr();
 String dt_comp = getCompMessageStr();
 if( dt_ard.length() > 0 ){
  dt = dt_ard ;
 }else{
  dt = dt_comp ;
 }

 if( dt.length() > 1 ){ //membuka pesan ardino
    Serial.println("Data : " + dt );
    parseMessage( dt );
 }
 
}
