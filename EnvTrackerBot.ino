/*  Terra Sintopica - EnvTrackerBot
 * 
 *  Este Bot mede a humidade do solo, luminosidade, 
 *  temperatura e umidade ambiente em tempo real com 
 *  o auxilio de um modulo RTC(DS1302) e guarda os dados 
 *  num formato csv em ficheiro txt no micro cartao SD acopulado.
 *  Estes modulos sao alimentados por placa de gestao de energia solar da DFRobot
 *  para painel de 5v
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

#define DHTPIN A5               // Inicializacao do sensor dht
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int pw_dht = 4;                 // Pino de alimentacao de sensor dht
String dhtVals = "";            // Para recever os valores de dht humidity e dht temperature

#define soilhumPin A4           // Pino Analogico para sensor de umidade
int soilhumVal;                 // valor da umidade do solo
int pw_sh = 3;                  // Pino de alimentacao de sensor de umidade do solo

#define lumnsPin A3             // Pino Analogico para sensor de luminosidade
int lumnsVal;                   // Valor de lums
int pw_lumns = 2;               // Pino de alimentacao de sensor de luminosidade

int DFRobotPower = 5;           // Pino para alimentar RTC e microSD modulos apartir da placa de gestao de energia solar
#define batteryLife A2          // Pino analogico para ler valor da bateria
int batLife;                     // The Battery current life, updated from batCheck() function

File dataFile;                  
String data;                    // string para armazenar a leitura dos sensores e transformar em char

virtuabotixRTC myRTC(8, 7, 6);  // Cria e inicia o Real Time Clock Object - Pins (SCLK, Data I/O, Chip Enabled)


void setup() {
  // Open Serial Communications
  Serial.begin(9600);
  while(!Serial){
    ; //for serial check
  }

  initSensores();               // Inicia Sensores
  batteryCheck();               // Verifica Estado da Bateria
  //rtcSETUP();                   // Configurar metodo para hora atual a guardar pelo sistema 
  
  delay(500);
}

void loop() {
  // le valores de sensores analogicos
  dhtVals = getDHTVals(pw_dht);
  soilhumVal = getAnalogSensorReading(soilhumPin, pw_sh);
  lumnsVal = getAnalogSensorReading(lumnsPin, pw_lumns);
  delay(250);
  
  batteryCheck();
  
  // alimenta os modulos RTC e microSDcard
  digitalWrite(DFRobotPower, HIGH);
  delay(500);
  
  if (SD.begin(10)){
    // constroi string em formato csv
    data = getCurrentTime()+","+batLife+","+dhtVals+","+soilhumVal+","+lumnsVal;
    // abre ficheiro, converte data string para char, grava datatemp e fecha o ficheiro.
    openFile("spotdata.txt");
    char datatemp[50];
    data.toCharArray(datatemp,50);
    writeToFile( datatemp );
    closeFile();
  }else{
    Serial.println("sd card initialization failed");
  }
  
  //corta alimentacao dos modulos RTC e sdCard
  digitalWrite(DFRobotPower, LOW);
  
  // dorme por 8 sec
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,SPI_OFF, USART0_OFF, TWI_OFF);  
}


// **** FUNCOES ****
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
  // pino digital para comunicar comunicar com a placa de gestao de energia solar
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
