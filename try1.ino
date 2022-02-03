#include <Servo.h>
#include <ESP8266WiFi.h>
Servo automaticDoor;
int pos = 0;


#define gasSensor A0
#define emergencyLights D0
#define buzzer D1
#define exhaustFan D2

String deviceId = "v57B03D27FD0CD90";
const char* logServer = "api.pushingbox.com";

const char* ssid = "AJINKYA";
const char* password = "";

void sendNotification(String message){
  WiFiClient client;

  Serial.println("- connecting to pushing server: " + String(logServer));
  if (client.connect(logServer, 80)) {
    Serial.println("- succesfully connected");
    
    String postStr = "devid=";
    postStr += String(deviceId);
    postStr += "&message_parameter=";
    postStr += String(message);
    postStr += "\r\n\r\n";
    
    Serial.println("- sending data...");
    
    client.print("POST /pushingbox HTTP/1.1\n");
    client.print("Host: api.pushingbox.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  }
  client.stop();
  Serial.println("- stopping the client");
}


void openDoor(){
  if (pos <= 180){
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    automaticDoor.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                      // waits 15ms for the servo to reach the position
  }
  }
}


void setup() {
  
  automaticDoor.attach(D3);
  
  pinMode(gasSensor, INPUT);
  pinMode(emergencyLights, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(exhaustFan, OUTPUT);
  digitalWrite(exhaustFan, HIGH);
  delay(20000); //To let the sensor warm up

  Serial.begin(9600);
  Serial.println("- connecting to Home Router SID: " + String(ssid));
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("- succesfully connected");
  Serial.println("- starting client");
  

  
}

void loop() {
  
  int value = analogRead(gasSensor);
  int threshold_value = 600;
Serial.println(value);
  
  if(value>threshold_value){
    digitalWrite(emergencyLights, HIGH);
    tone(buzzer, 494, 0);
    digitalWrite(exhaustFan, LOW);
    openDoor();
    sendNotification("EMERGENCY");
  }
  
  else{
    digitalWrite(emergencyLights, LOW);
    digitalWrite(buzzer, LOW);
    digitalWrite(exhaustFan, HIGH);
  }
  delay(1000);
}
