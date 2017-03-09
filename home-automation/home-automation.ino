#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#define DEBUG true

SoftwareSerial esp8266(2,3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3

String ssid = "B68A-57F8"
String key = "B2F66D4B"

String server="www.heftytips.com"
String url = "heftytips.com/microcontroller.php"

byte dat[5];
                             
int MaxVoltage; //overvoltage value
int MaxCurrent; //overcurrent value
int MinVoltage; //undervoltage value

int ActiveLoad[10]; //list of all active loads
int InactiveLoad[10]; //list of all inactive loads


float cVoltage; // voltage from sensor
float cCurrent; // current from sensor

char __loadProfile[] = "{'fan':1,'socket':2,'light':3}";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  esp.begin(115200);
  init();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void init(){
  // initialization procedures for the esp module
  reset();
  connectWifi()[
}

void reset(){
  esp.println("AT+RST");
  delay(1000);
  if (esp.find("OK")){
    Serial.println("Module Reset")
  }
}

void connectWifi(){
  String syn = "AT+CWJAP=\" + ssid +"\",""+key+"\"";
  esp.println(syn);
  delay(4000);
  if(esp.find("OK")){
    Serial.println("Connected to " + ssid +".");
  }
  else{
    connectWifi();
    Serial.println("Cannot connect to "+ ssid + ".");
  }
}

float getVoltage(){
  //placeholder
  //get voltage from voltage sensor
  }

  
float getCurrent(){
  //placeholder
  //get current from current sensor
  }

 void safetyCheck(){
   // **critical function**
   // anonymous function
   //check to see if values are within a safe range
   cCurrent = getCurrent();
   cVoltage = getVoltage();

   if (cCurrent >= MaxCurrent
   || cVoltage <= MinVoltage
   || cVoltage >= MaxVoltage){
    
    //cutoff power
    cutoffPower();
   }
  
 }


bool cutoffPower(){
  //**critical function**
  digitalWrite(mainPowerPin,LOW);
  return true
}


bool powerOnLoad(int id){
  //power on the load by its specified id
  digitalWrite(id,HIGH);
  return true;
}

bool powerOffLoad(int id){
  //power off the load by its specified id
  digitalWrite(id,LOW);
  return true;
}

void syncLoads(){
   // **critical function**
  //synchronize active and inactive loads
  for(int i=0; i<sizeof(ActiveLoad)-1;i++){
    if (powerOnLoad(ActiveLoad[i]) != true){
      powerOffLoad(ActiveLoad[i]);
    }
  }

  for(int i=0;i<sizeof(InactiveLoad)-1;i++){
    if (powerOffLoad(InactiveLoad[i]) != true){
      powerOnLoad(InactiveLoad[i]);
    }
  }
}


void hubSync(){
  //synchronize hub with server
  getServerStat();
  pushLocalStat();
  syncLoads();
}

char parseString(){
  //convert string response to list
}

String parseList(){
  // convert list to string
}

void getServerStat(){
  String request;
  String __data = __get(request); //pull from server
  char data = parseString(__data);
  cVoltage = data[0]
  cCurrent = data[1]
  ActiveLoad = data[2]
  InActiveLoad = data[3]
  
}

void pushLocalStat(){
  // push local storage value to server (json format)
  cVoltage = getVoltage();
  cCurrent = getCurrent();

//  char __data = [voltage,current,ActiveLoad,InactiveLoad];
    String data = parseList(__data);
    __post(data); //push to server
}

String __get(data){
  //helper function for get
 esp.println("AT-CIPSTART=\"TCP\",\""+server +"\",80"); //Start a TCP Connection
  if (esp.find("OK)){
   Serial.println("TCP Connection Ready");
  }
  delay(1000);

  String getRequest = "GET " + url + " HTTP/1.0\r\n" + "Host: " + server + "\r\n" + "Accept: *" + "/" + "*\r\n" + " Content-Length: " + data.length() + "'\r\n" + "Content-Type: application/x-www-form-urlencoded\r\n" + "\r\n" + data;
  String sendsyn = "AT+CIPSEND="; //determine the number of characters to be sent

  esp.print(sendsyn);
  esp.println(getRequest.length());
  delay(500);
  if(esp.find(">")){
  Serial.println("Sending...");
  esp.print(getRequest);
  if(esp.find("SEND OK")){
    Serial.println("Packet sent");
    while (esp.available()){
      String response = esp.readString();
      return response;
    }
    esp.println("AT+CIPCLOSE");
  }
}

String __post(data){
  //helper function for post
  esp.println("AT-CIPSTART=\"TCP\",\""+server +"\",80"); //Start a TCP Connection
  if (esp.find("OK)){
   Serial.println("TCP Connection Ready");
  }
  delay(1000);

  String postRequest = "POST " + url + " HTTP/1.0\r\n" + "Host: " + server + "\r\n" + "Accept: *" + "/" + "*\r\n" + " Content-Length: " + data.length() + "'\r\n" + "Content-Type: application/x-www-form-urlencoded\r\n" + "\r\n" + data;
  String sendsyn = "AT+CIPSEND="; //determine the number of characters to be sent

  esp.print(sendsyn);
  esp.println(postRequest.length());
  delay(500);
  if(esp.find(">")){
  Serial.println("Sending...");
  esp.print(postRequest);
  if(esp.find("SEND OK")){
    Serial.println("Packet sent");
    while (esp.available()){
      String response = esp.readString();
      return response;
    }
    esp.println("AT+CIPCLOSE");
  }
}


