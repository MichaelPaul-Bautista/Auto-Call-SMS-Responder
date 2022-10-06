#include <SoftwareSerial.h>

#define voice 4

SoftwareSerial SIM900A(7,8);

int State = 0;
String CellNumTemp, CellNum;

void setup() {
  pinMode(voice, OUTPUT);
  SIM900A.begin(9600); // GSM Module Baud rate - communication speed
  delay(100);
  digitalWrite(voice, LOW);
  Serial.begin(9600);
  Serial.println ("Text Message Module Ready & Verified");
  State = 1;
}

void loop() {

  Serial.read();
  delay(1000);
  
  if (State == 1) {
    SIM900A.println("AT+CNMI=2,2,0,0,0"); // Receiving Mode Enabled
    delay(1000);
    State = 0;
  }
  
  if (SIM900A.available() > 0) {
    Serial.write(SIM900A.read());
    String tempo = SIM900A.readString();
    tempo.trim();
    Serial.println(tempo);
    if (tempo.indexOf("+CMT: \"+639") != -1) {
      CellNumTemp = tempo.substring(tempo.indexOf("+639"));
      CellNum = CellNumTemp.substring(0,13);
      Serial.println(CellNum);
      SendMessage(CellNum);
      delay(1000);
      CellNumTemp = "";
      CellNum = "";
    } else if (tempo.indexOf("RING") != -1) {
      CallResponse();
      //if (tempo.indexOf("+CLIP: \"09") != -1) {
        //CallNumTemp = tempo.substring(tempo.indexOf("09"));
        //CallNum = CallNumTemp.substring(0,11);
        //Serial.println(CallNum);
        //SendMessage(CallNum);
        //delay(1000);
        //CallNumTemp = "";
        //CallNum = "";
      //}
    }
    tempo = "";
  }
  delay(2000);
}

void SendMessage(String Num) {
  String send_command = "AT+CMGS=\"" + Num + "\"\r";
  Serial.println(send_command);
  Serial.println("Sending Message please wait....");
  SIM900A.println("AT+CMGF=1");    //Text Mode initialisation 
  delay(1000);
  Serial.println("Set SMS Number");
  SIM900A.println(send_command); // Send SMS command with Receiver's Mobile Number
  delay(1000);
  Serial.println("Set SMS Content");
  SIM900A.println("Hi, this is an auto-reply. Our staff is busy right now. For joblisting and how to apply, pls. visit cogentadsDOTinfo Please replace DOT with the point sign "."");// Messsage content
  delay(1000);
  Serial.println("Done");
  SIM900A.println((char)26);//   delay(1000);
  Serial.println("Message sent succesfully");
}

void CallResponse() {
  SIM900A.println("ATA"); // Accepts incoming call
  digitalWrite(voice, HIGH); // Plays Audio from Voice module
  delay(5000); // Call for 10 seconds
  SIM900A.println("ATH"); // Disconnects Call
  delay(1000);
  digitalWrite(voice, LOW); // Stops Audio from Voice module
}
