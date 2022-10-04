#include <SoftwareSerial.h>

#define Button 9

SoftwareSerial SIM900A(7,8);

int State, ButtonState = 0;
String CellNumTemp, CellNum;

void setup() {
  pinMode(Button,INPUT);
  SIM900A.begin(9600); // GSM Module Baud rate - communication speed
  delay(100);
  Serial.begin(9600);
  Serial.println ("Text Message Module Ready & Verified");
}

void loop() {

  Serial.read();
  ButtonState = digitalRead(Button);
  delay(1000);
  Serial.print("ButtonState: ");
  Serial.println(ButtonState);
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
    }
    tempo = "";
  }
  delay(2000);
}

void SendMessage(String CellNum) {
  String send_command = "AT+CMGS=\"" + CellNum + "\"\r";
  Serial.println(send_command);
  Serial.println("Sending Message please wait....");
  SIM900A.println("AT+CMGF=1");    //Text Mode initialisation 
  delay(1000);
  Serial.println("Set SMS Number");
  SIM900A.println(send_command); // Send SMS command with Receiver's Mobile Number
  delay(1000);
  Serial.println("Set SMS Content");
  SIM900A.println("This is an auto-reply. Our staff is busy right now. Thank you.");// Messsage content
  delay(100);
  Serial.println("Done");
  SIM900A.println((char)26);//   delay(1000);
  Serial.println("Message sent succesfully");
}
