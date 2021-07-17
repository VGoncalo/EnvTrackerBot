/*  Terra Sintopica - EnvTrackerBot
 * 
 *  This Bot measures soil humidity, solar luminosity,
 *  env temperature and humidity in real time w/ module RTC(DS1302)
 *  Stores sensor data in a SD card w/ csv format
 *  The bot is feeded by solar energy in a 5v solar panel managed by a DFRobot board
 *  
 *  Libs:
 *    for DHT sensor - DHT
 *    for Power - LowPower;
 *    for SD card - SPI & SD
 *    for RTC - virtuabotixRTC
 */

#include "LowPower.h"
#include <virtuabotixRTC.h> 
#include <SPI.h>
#include <SD.h>
#include "DHT.h"

#define DHTPIN A5               // dht startup
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int pw_dht = 4;                 // dht sensor feeding pin
String dhtVals = "";            // to treat dht temperature and humidity values

#define soilhumPin A4           // Analog pin for dht
int soilhumVal;                 // soil humidity value
int pw_sh = 3;                  // soil humidity sensor feeding pin

#define lumnsPin A3             // Analog pin for solar intensidy
int lumnsVal;                   // lums value
int pw_lumns = 2;               // luminosity sensor feeding pin

int DFRobotPower = 5;           // feeding pin for RTC and microSD modules form DFRobot board
#define batteryLife A2          // Analog pin for battery life value
int batLife;                     // The Battery current life, updated from batCheck() function

File dataFile;                  
String data;                    // string bo be transformed - receive sensor data

virtuabotixRTC myRTC(8, 7, 6);  // startup Real Time Clock Object - Pins (SCLK, Data I/O, Chip Enabled)


void setup() {
  // Open Serial Communications
  Serial.begin(9600);
  while(!Serial){
    ; //for serial check
  }

  initSensores();               // Begins sensors
  batteryCheck();               // Verify baterry life
  //rtcSETUP();                   // ATENTION:Configure RTC time before replae in the field, comment this line after
  
  delay(500);
}

void loop() {
  // read analog sensor values
  dhtVals = getDHTVals(pw_dht);
  soilhumVal = getAnalogSensorReading(soilhumPin, pw_sh);
  lumnsVal = getAnalogSensorReading(lumnsPin, pw_lumns);
  delay(250);
  
  batteryCheck();
  
  // feed modules RTC and microSDcard
  digitalWrite(DFRobotPower, HIGH);
  delay(500);
  
  if (SD.begin(10)){
    // build string csv
    data = getCurrentTime()+","+batLife+","+dhtVals+","+soilhumVal+","+lumnsVal;
    // open file, convert data string to char, save datatemp and dlose file.
    openFile("spotdata.txt");
    char datatemp[50];
    data.toCharArray(datatemp,50);
    writeToFile( datatemp );
    closeFile();
  }else{
    Serial.println("sd card initialization failed");
  }
  
  //power off RTC and sdCard slaves
  digitalWrite(DFRobotPower, LOW);
  
  // sleeps 8 sec
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,SPI_OFF, USART0_OFF, TWI_OFF);  
}


// **** FUNCTIONS ****
void batteryCheck(){
  //Serial.print("Battery life ");
  batLife = analogRead(batteryLife);  
  //Serial.println(batLife);
}

String getDHTVals(int pwPin){
  String tempdhtVals = "";
  digitalWrite(pwPin,HIGH);
  delay(2000);
  
  float DHThum = dht.readHumidity();
  float DHTtemp = dht.readTemperature();

  if(isnan(DHThum) or isnan(DHTtemp)){
    Serial.println("Failed to read from dht");
    tempdhtVals = "NaN,NaN";
  }else{
    tempdhtVals = String(DHTtemp) +"," + String(DHThum);
  }
  
  digitalWrite(pwPin,LOW);
  
  return tempdhtVals;
}

int getAnalogSensorReading(int sPin, int pwPin){
  int tempval;
  digitalWrite(pwPin,HIGH);
  delay(2000);
  tempval = analogRead(sPin);
  delay(500);
  digitalWrite(pwPin,LOW);

  return tempval;
}

String getCurrentTime(){
  myRTC.updateTime();
  String tempdata = String(myRTC.dayofmonth)+":"+String(myRTC.dayofweek)+":"+String(myRTC.hours)+":"+String(myRTC.minutes)+":"+String(myRTC.seconds);
  Serial.println(tempdata);
  return tempdata;
}

void rtcSETUP(){
  digitalWrite(DFRobotPower, HIGH);
  
  // Set current date and time, comment this line ater fisrt set up
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(00, 13, 12, 3, 1, 7, 2020);
  String isOK = getCurrentTime();
  Serial.println(isOK);
  delay(500);
  digitalWrite(DFRobotPower, LOW);
}

void initSensores(){
  pinMode(pw_dht,OUTPUT);
  pinMode(pw_sh,OUTPUT);
  pinMode(pw_lumns,OUTPUT);
  // digital pin for comunicate with solar management board
  pinMode(DFRobotPower,OUTPUT);
  dht.begin();
}

int openFile(char filename[]){
  dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile){
    Serial.println("File opened with success!");
    return 1;
  }else{
    Serial.println("Error opening file...");
    return 0;
  }
}

void closeFile(){
  if(dataFile){
    dataFile.close();
    Serial.println("File closed");
  }
}

int writeToFile(char text[]){
  if (dataFile){
    dataFile.println(text);
    Serial.println("Writing to file ");
    Serial.println(text);
    return 1;
  }else{
    Serial.println("Couldn't write to file");
    return 0;
  }
}
