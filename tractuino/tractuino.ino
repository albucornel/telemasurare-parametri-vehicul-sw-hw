#include <SoftwareSerial.h>
SoftwareSerial gprsSerial(3, 2);

const int nivelcombustibilpin = A6;         
const int oilpressurepin = A5;
const int watertemppin = A4;
const int waterlevelpin = A3;

void toSerial()
{
  while(gprsSerial.available()!=0)
  {
    Serial.write(gprsSerial.read());
  }
}

void sensorsRead(void) { 
    
  float watertempres = analogRead(watertemppin);//resistance value   (350.124-Resistance)/1.25  
  double watertempCalc = (watertempres-350.124) /147.33; // 
  
  float waterlevel = analogRead(waterlevelpin) ;   
  double waterLevelCalc = (411.12  - waterlevel) /-18.25; //max 15.7=1023
  
  float oilpressure = analogRead(oilpressurepin) ; 
  double oilpressCalc = ( 98.5 - oilpressure) /-6.098; // 70-210 bar
  
  float nivelcombustibil = analogRead(nivelcombustibilpin); //resistance value   (350.124-Resistance)/1.25  
  double nivelcombCalc = (237-nivelcombustibil) /-8.85; // max 98.4L=1023
  
  Serial.println("Water Level");
  delay(1000);  
  Serial.println(waterLevelCalc);
  delay(1000);  
//  Serial.println(waterlevel);
//  delay(1000);

  Serial.println("Water Temperature");
  delay(1000);  
  Serial.println(watertempCalc);
  delay(1000);  
//  Serial.println(watertempres);
//  delay(1000);

  Serial.println("Oil Pressure");
  delay(1000);  
  Serial.println(oilpressCalc);
  delay(1000);  
//  Serial.println(oilpressure);
//  delay(1000);

  Serial.println("Diesel Level");
  delay(1000);  
  Serial.println(nivelcombCalc);
  delay(1000);  
//  Serial.println(nivelcombustibil);
//  delay(1000);
  checkData(watertempCalc,waterLevelCalc, oilpressCalc, nivelcombCalc);
  dataToServer(watertempCalc,waterLevelCalc, oilpressCalc, nivelcombCalc); 

} 

void sendSms(String message){  
  gprsSerial.println("AT+CMGF=1"); // Configuring TEXT mode
  toSerial();
  gprsSerial.println("AT+CMGS=\"+40787665297\"");
  toSerial();
  gprsSerial.print(message); //text content
  toSerial();
  gprsSerial.write(26);
}

void gsmConfig(void)
{
  Serial.println("Con");
  delay(2000);
  Serial.println("Done!...");
  gprsSerial.flush();
  Serial.flush();

  // See if the SIM900 is ready
  gprsSerial.println("AT");
  delay(1000);
  toSerial();

  // SIM card inserted and unlocked?
  gprsSerial.println("AT+CPIN?");
  delay(1000);
  toSerial();

  // Is the SIM card registered?
  gprsSerial.println("AT+CREG?");
  delay(1000);
  toSerial();

  // Is GPRS attached?
  gprsSerial.println("AT+CGATT?");
  delay(1000);
  toSerial();

  // Check signal strength
  gprsSerial.println("AT+CSQ ");
  delay(1000);
  toSerial();

  // Set connection type to GPRS
  gprsSerial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  delay(2000);
  toSerial();

  // Set the APN
  gprsSerial.println("AT+SAPBR=3,1,\"APN\",\"net\"");
  delay(2000);
  toSerial();

  // Enable GPRS
  gprsSerial.println("AT+SAPBR=1,1");
  delay(10000);
  toSerial();

  // Check to see if connection is correct and get your IP address
  gprsSerial.println("AT+SAPBR=2,1");
  delay(2000);
  toSerial();
}

void dataToServer(double watertempCalc, double waterLevelCalc,double oilpressCalc, double nivelcombCalc){

 // initialize http service
   gprsSerial.println("AT+HTTPINIT");
   delay(2000); 
   toSerial();
      String temp_apa; // = String(watertempCalc);
      String nivel_apa; //=String(waterLevelCalc);
      String presiune_ulei; //=String(oilpressCalc);
      String niv_combustibil; //=String(nivelcombCalc);
      String device_id="t1690";

   gprsSerial.println("AT+HTTPPARA=URL,http://telemasurare1.000webhostapp.com/api.php?temp_apa=" + String(watertempCalc) + "&nivel_apa=" + String(waterLevelCalc) + "&presiune_ulei=" + String(oilpressCalc) + "&niv_combustibil=" + String(nivelcombCalc) + "&device_id=" + device_id);
   delay(6000);
   toSerial();

   // set http action type 0 = GET, 1 = POST, 2 = HEAD
   gprsSerial.println("AT+HTTPACTION=1");
   delay(6000);
   toSerial();

   // read server response
   gprsSerial.println("AT+HTTPREAD"); 
   delay(1000);
   toSerial();

   //gprsSerial.println("");
   gprsSerial.println("AT+HTTPTERM");
   toSerial();
   delay(300);

   gprsSerial.println("");
   delay(10000);
}

void checkData(double watertempCalc, double waterLevelCalc,double oilpressCalc, double nivelcombCalc){
  
  if(watertempCalc > 96){
    sendSms("Water temperature is too high");
  }
  else if(waterLevelCalc < 11){
    sendSms("Water level is low");
  }else if(oilpressCalc > 200){
    sendSms("Oil pressure is too high");
  }else if(nivelcombCalc < 15){
    sendSms("Diesel level is low");
  }else{
      Serial.println("The parameters have an optimal value");
  }
}

void setup()
{
  gprsSerial.begin(9600);
  Serial.begin(9600);

  gsmConfig();
}


void loop()
{
do { 
  sensorsRead();
  
} while(1); 
  delay(1000);  // update every 1000 = 1 sec 
}
