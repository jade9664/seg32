#include <SoftwareSerial.h>
SoftwareSerial esp8266(2,3);

String IP = ""; 
boolean No_IP = false;
String host = "www.heftytips.com";
const int clientID = 2;
int readingsToBeSent[16]; //format is 7,0,1,0,1.. = First item is clientID, then first appliance ON,2ndOFF,3rdON,4thOFF.. 
String readingsInStringFormat; //readingsToBeSent in string format for ease of sending 2 website

void setup() {
  Serial.begin(115200);
  esp8266.begin(115200);
  wifi_init();
  Serial.println("System Ready");
  delay(1000); // Let the module self-initialize
  //once initialization is completed, check status of all appliances connected 
  //and send 2 web server
  //sendStatusToRemoteServer();  
}

/////INITIALIZE AND GET IP
void wifi_init()
{
  connect_wifi("AT",100);
  connect_wifi("AT+CWMODE=1",100);
  connect_wifi("AT+CWQAP",100);
  connect_wifi("AT+RST",2000);
  check4IP(5000); //check 4previous WiFi binding & IP
  if(!No_IP)
  {
    Serial.println("Connecting Wifi....");
    connect_wifi("AT+CWJAP=\"oma\",\"omasilem\"",4000); //provide your WiFi username and password here    
  }
    Serial.println("WiFi Connected");
    get_ip();
    connect_wifi("AT+CIPMUX=1",100);
    connect_wifi("AT+CIPSERVER=1,80",100);
}

void connect_wifi(String cmd, int t)
{
  int temp=0,i=0;
  while(1)
  {
    Serial.println(cmd);
    esp8266.println(cmd);
    while(esp8266.available())
    {
    if(esp8266.find("OK"))
      i=8;
    }
  delay(t);
  if(i>5)
  break;
  i++;
  }
  if(i==8)
  Serial.println("OK");
  else
  Serial.println("Error");
}

void check4IP(int t1)
{
  int t2=millis();
  while(t2+t1>millis())
  {
    while(esp8266.available()>0)
    {
      if(esp8266.find("WIFI GOT IP"))
      {
        No_IP=true;
      }
    }
  }
}

void get_ip()
{
  IP="";
  char ch=0;
  while(1)
  {
    esp8266.println("AT+CIFSR");
    while(esp8266.available()>0)
    {
      if(esp8266.find("STAIP,"))
      {
        delay(1000);
        Serial.print("IP Address:");
        while(esp8266.available()>0)
        {
          ch=esp8266.read();
          if(ch=='+')
          break;
          IP+=ch;
        }
      }
      if(ch=='+')
      break;
    }
      if(ch=='+')
      break;
    delay(1000);
  }
  Serial.print(IP);
  Serial.print("Port:");
  Serial.println(80);
}

/////INITIALIZE AND GET IP - END

void sendStatusToRemoteServer() {
//This sends the intial status of all connected appliances to the remote server on power on

//get Status of appliances
getStatusOfAppliances();

//send to webserver
Serial.println("Sending to Application Status to website");    
sendMsg();
Serial.println("Sent to website");
delay(1000);    
}

void getStatusOfAppliances()
{
  String f = ",";
  readingsInStringFormat = "";
  readingsToBeSent[0] =  clientID;
  readingsInStringFormat += readingsToBeSent[0];  // add clientID   
  //assuming appliances are plugged in2 PINs 30 - 44
  for (int activePin = 30; activePin < 45; activePin++)
  {
    if (digitalRead(activePin) == LOW)
      readingsToBeSent[activePin-29] = 0;    //plus 1 bcos slot 0 is reserved for clientID
    else
      readingsToBeSent[activePin-29] = 1;

      readingsInStringFormat +=  f;   //add , 
      readingsInStringFormat +=  String(readingsToBeSent[activePin]); //now add reading
  delay(100);
  }  

  Serial.println("LENTH v1= " + readingsInStringFormat.length());

}

///send d msg 2 d server
void sendMsg()
{
String url = "/webcontroller/admin/acceptFromArduino.php?";
String postMessage;

esp8266.println("AT+CIPSTART=0,\"TCP\",\"" + host + "\",80");
if (esp8266.find("OK"))
{
  int len = 47;
  Serial.println("TCP Connection to Webserver Ready");
  //prepare the message 
     postMessage = "POST " + url + " HTTP/1.1\r\n" + 
    "HOST: " + host + ":80" + "\r\n" + "Accept: *" + "/" + "*\r\n" +
    "Content-Length: " + len + "\r\n" +
 "Content-Type: application/x-www-form-urlencoded\r\n" 
 "\r\n"+
    "ApplianceStatus=" + readingsInStringFormat;//4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";
    
  //send the message
    String sendCmd = "AT+CIPSEND=0,";
    esp8266.print(sendCmd);
    esp8266.println(postMessage.length());
    //Delay
    delay(500);
    if(esp8266.find(">")) 
    { 
      Serial.println("Sending..");
      Serial.println("Am about to send these. " + postMessage);
      esp8266.print(postMessage);
    }
  //confirm if message was successfully sent
    if(esp8266.find("SEND OK")) 
    { 
      Serial.println("Packet sent");
      // Read reply from server and print them to Serial
      Serial.println("Response from server: \n");
      while(esp8266.available())
      {
        String ServerResponse = esp8266.readString();
         Serial.print(ServerResponse);
       }
       //close the connection
       esp8266.println("AT+CIPCLOSE");
    } 
    }
    else
    {
      Serial.println("Connection to Server failed.");
      return;
    }
}

void loop() {
// actively listen 4 a response 4m Server
//then implement what ever is received 4m Server

char incoming [15];
char s[24];
String url2 = "/webcontroller/admin/userModifiedStatus.php?";

Serial.println("connecting...");
esp8266.println("AT+CIPSTART=0,\"TCP\",\"" + host + "\",80");
if (esp8266.find("OK"))
{
  int len2 = 10;
  Serial.println("TCP Connection to Webserver Ready");
  //prepare the message 
  delay(1000);     
  String respMessage = "POST " + url2 + " HTTP/1.1\r\n" + 
    "HOST: " + host + ":80" +"\r\n" + "Accept: *"+ "/"+ "*\r\n" +
    "Content-Length: " + len2 + "\r\n"+
    "Content-Type: application/x-www-form-urlencoded\r\n" 
    "\r\n"+
    "clientID=" + clientID; 
   //send the message
  String sendCmd2 = "AT+CIPSEND=0,";
    esp8266.print(sendCmd2);
    esp8266.println(respMessage.length());
    //Delay
    delay(500);
    if(esp8266.find(">")) 
    {   
      Serial.println("probing server 4 updates..");
      esp8266.print(respMessage);
      if (esp8266.find("SEND OK"))
      {
        Serial.println("Probe message sent successfully");
        //response from server
        delay(1000); //wait for server 2 respond...
    String m = "";
        while(esp8266.available())
        {
       m = esp8266.readStringUntil("x");        
    }
      m.toCharArray(incoming,15); //break d string received 4m server in2 15 characters
            
         
        
        //close the connection
        esp8266.println("AT+CIPCLOSE");
        } 
        //now  switch appliance ON/OFF based on response received 4m webserver 
        for (int i = 0; i <= sizeof(incoming); i++)
        {
          if (incoming[i] == NULL)
              continue;
          else
          {
            if (atoi(incoming[i]) == 1) 
            { 
              sprintf(s,"Application %i is now ON", i+1);
              Serial.println(s);        
              digitalWrite(i, HIGH);  //switch on
            } 
            else
            {
              sprintf(s,"Application %i is now OFF", i);
              Serial.println(s);        
              digitalWrite(i, LOW);   //switch off
            }
           }
          }
    }
    else 
    {
      // if you didn't get a connection to the server:
      Serial.println("Connected but could not send probes to WebServer!");
      }
}
else
      Serial.println("Could not connect To WebServer for probe!");

      delay(20000);
}
