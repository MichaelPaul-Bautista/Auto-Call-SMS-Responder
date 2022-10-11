#include <SoftwareSerial.h>

#define voice 4 // Voice Module on Pin 4
#define button 9 // Button for GSM initialization on Pin 9

SoftwareSerial SIM900A(7,8); // Rx and Tx in ARDUINO Pin 7 and 8 respectively

int State = 0;
String CellNumTemp, CellNum, CallNumTemp, CallNum;

void setup() {
  pinMode(button, INPUT); // Initializing Button
  pinMode(voice, OUTPUT); // Initializing Voice Module as OUTPUT
  CellNum, CallNum = ""; // Initialize CellNum & CallNum
  SIM900A.begin(9600); // GSM Module Baud rate - communication speed
  delay(100);
  digitalWrite(voice, LOW); // Default state of Voice Module is LOW
  Serial.begin(9600);
  Serial.println ("Text Message Module Ready & Verified");
  //SIM900A.println("AT+CMGD=1,4\r"); // Deletes saved messages in SIM
  delay(100); 
  //State = 1; Initial State is set to 1 when button is not used
}

void loop() {

  Serial.read();
  delay(1000);

  State = digitalRead(button); // When State == 1 it initializes the GSM module. Additionally the button must be pressed around 2-5 secs
  
  if (State == 1) {
    //SIM900A.println("AT+CNMI=2,2,0,0,0"); Set the GSM into receiving SMS mode in TE flash memory
    SIM900A.println("AT+CNMI?"); 
    CellNum, CallNum = ""; // Re-initialize CellNum & CallNum
    State = 0; // Set the State to 0 after initialization
  }
  
  if (SIM900A.available() > 0) {
    String tempo = SIM900A.readString(); // Stores the Serial Output of SIM900A to a string variable
    tempo.trim();
    Serial.println(tempo);
    if (tempo.indexOf("+CMT: \"+639") != -1) {
      CellNumTemp = tempo.substring(tempo.indexOf("+639"));
      if (CellNum != CellNumTemp) {
        CellNum = CellNumTemp.substring(0,13); // Stores the SMS sender's number to string variable
        Serial.println(CellNum);
        SendMessage(CellNum);
        delay(1000);
        CellNumTemp = ""; // Resets value of CellNumTemp
      }
    } else if (tempo.indexOf("RING") != -1) {
      CallResponse();
      if (tempo.indexOf("+CLIP: \"09") != -1) {
        CallNumTemp = tempo.substring(tempo.indexOf("09"));
        if (CallNum != CallNumTemp) {
          CallNum = CallNumTemp.substring(0,11); // Stores the caller's number to string variable
          Serial.println(CallNum);
          SendMessage(CallNum);
          delay(1000);
          CallNumTemp = ""; // Resets value of CallNumTemp
        }
      }
    } 
    tempo = ""; // Resets value of tempo
  }
  delay(2000);
}

void SendMessage(String Num) {
  String send_command = "AT+CMGS=\"" + Num + "\"\r"; // SMS command
  Serial.println(send_command);
  Serial.println("Sending Message please wait....");
  SIM900A.println("AT+CMGF=1");    //Text Mode initialisation 
  delay(1000);
  Serial.println("Set SMS Number");
  SIM900A.println(send_command); // Send SMS command with Receiver's Mobile Number
  delay(1000);
  Serial.println("Set SMS Content");
  SIM900A.println("This is an auto-reply. Our staff is busy right now. For joblisting and how to apply, pls. visit cogentadsDOTinfo Please replace DOT with the point symbol \".\""); // Messsage content
  delay(1000);
  Serial.println("Done");
  SIM900A.println((char)26);//   delay(1000);
  Serial.println("Message sent succesfully");
}

void CallResponse() {
  SIM900A.println("ATA"); // Accepts incoming call
  delay(10);
  digitalWrite(voice, HIGH); // Plays Audio from Voice module
  delay(8000); // Duration of the Call. Currently calls for 8 seconds
  SIM900A.println("ATH"); // Disconnects Call
  delay(50);
  digitalWrite(voice, LOW); // Stops Audio from Voice module if the Audio is still playing after the call
}
