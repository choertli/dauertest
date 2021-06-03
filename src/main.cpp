#include <Arduino.h>
#include "MotorDriver.h"
#include <Stepper.h>
#include "WebServer.h"


/*Put your SSID & Password*/
const char* ssid = "chris";  // Enter SSID here
const char* password = "ab579042";  //Enter Password here


// Motor A
int INB = 12; // IN2
int INA = 14; // IN1
int EN = 2;   // ENA
int CS = 35;
int PWM = 16;

int INB1 = 13; // IN2
int INA1 = 17; // IN1
int EN1 = 4;   // ENA
int CS1 = 34;
int PWM1 = 27;



WebServer server(80);


const int stepsPerRevolution = 200; // change this to fit the number of steps per revolution
// for your motor

int stepCount = 0; // number of steps the motor has taken

// Initialize the stepper library on the motor shield:
Stepper myStepper = Stepper(stepsPerRevolution, INA, INB, INA1, INB1);
MotorDriver Monster = MotorDriver(EN, INA, INB, PWM, CS);
MotorDriver Monster1 = MotorDriver(EN1, INA1, INB1, PWM1, CS1);

String SendHTML(float Temperaturestat,float Humiditystat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP32 Weather Report</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP32 Weather Report</h1>\n";
  
  ptr +="<p>Temperature: ";
  ptr +=(int)Temperaturestat;
  ptr +="°C</p>";
  ptr +="<p>Humidity: ";
  ptr +=(int)Humiditystat;
  ptr +="%</p>";
  
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}


void handle_OnConnect() {

 float Temperature = 40;
  float Humidity = 10;
  server.send(200, "text/html", SendHTML(Temperature,Humidity)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}




void setup()
{

 Serial.begin(115200);
  delay(100);
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");


  pinMode(2, OUTPUT);
  Monster.init();   // init Monster shield
  Monster1.init();   // init Monster shield
 
  // Set the motor speed (RPMs):
  myStepper.setSpeed(60);
}

void loop()
{

  server.handleClient();

  digitalWrite(2, HIGH);

  // Step one revolution in one direction:
  myStepper.step(200);

  delay(2000);

  //Step on revolution in the other direction:
  myStepper.step(-200);
  digitalWrite(2, LOW);

  delay(2000);
}

