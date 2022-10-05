#include <SoftwareSerial.h>

#define Button 9

SoftwareSerial SIM900A(7,8);

int State, ButtonState = 0;
String CellNumTemp, CellNum, CallNumTemp, CallNum;

void setup() {
  pinMode(Button,INPUT);
  SIM900A.begin(9600); // GSM Module Baud rate - communication speed
  delay(100);
  Serial.begin(9600);
  Serial.println ("Text Message Module Ready & Verified");
  ButtonState = 1;
}

void loop() {

  Serial.read();
  delay(1000);
  //Serial.print("ButtonState: ");
  //Serial.println(ButtonState);
  if (ButtonState == 1) {
    SIM900A.println("AT+CNMI=2,2,0,0,0"); // Receiving Mode Enabled
    delay(1000);
    ButtonState = 0;
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
      //SIM900A.println("AT+CLIP=1");
      CallResponse();
      if (tempo.indexOf("+CLIP: \"09") != -1) {
        CallNumTemp = tempo.substring(tempo.indexOf("09"));
        CallNum = CallNumTemp.substring(0,11);
        Serial.println(CallNum);
        SendMessage(CallNum);
        delay(1000);
        CallNumTemp = "";
        CallNum = "";
      }
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
  SIM900A.println("This is an auto-reply. Our staff is busy right now. Please email at apply@phrwe.com Thank you.");// Messsage content
  delay(1000);
  Serial.println("Done");
  SIM900A.println((char)26);//   delay(1000);
  Serial.println("Message sent succesfully");
}

void CallResponse() {
  SIM900A.println("ATA"); // Accepts incoming call
  //This is where the code for the voice kit module will be placed...
  delay(10000); // Call for 10 seconds
  SIM900A.println("ATH"); // Disconnects Call
}
