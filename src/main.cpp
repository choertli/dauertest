#include <Arduino.h>
#include "MotorDriver.h"
#include <Stepper.h>
#include "WebServer.h"
#include "HTTPClient.h"

#include <regex>
#include <iterator>
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

using namespace std;

#define SERIALCOM True
//#define HOME True
#define WORK True

/*Put your SSID & Password*/
#ifdef WORK
const char *ssid = "WLAN_BYOD";       // Enter SSID here
const char *password = "SegelSCHiff"; //Enter Password here
#endif

#ifdef HOME
   const char *ssid = "chris";        // Enter SSID here
   const char *password = "ab579042"; //Enter Password here
#endif
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

int counter = 0;
int Steps = 250;
int StepsRevers = -280;
WebServer server(80);

//Your Domain name with URL path or IP address with path
String serverName = "http://www.oertli.tech/dauertest/counter.php";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;
const int stepsPerRevolution = 200; // change this to fit the number of steps per revolution
// for your motor

int stepCount = 0; // number of steps the motor has taken

// Initialize the stepper library on the motor shield:
Stepper myStepper = Stepper(stepsPerRevolution, INA, INB, INA1, INB1);
MotorDriver Monster = MotorDriver(EN, INA, INB, PWM, CS);
MotorDriver Monster1 = MotorDriver(EN1, INA1, INB1, PWM1, CS1);

 
String SendHTML(float counter, float Steps)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>ESP32 Dauertest</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>Dauertest Report</h1>\n";

  ptr += "<p>Counter: ";
  ptr += (int)counter;
  ptr += "</p>";
  ptr += "<p>Steps: ";
  ptr += (int)Steps;
  ptr += "</p>";

  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

void handle_OnConnect()
{

  server.send(200, "text/html", SendHTML(counter, Steps));
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}

void setup()
{

  Serial.begin(115200);
  delay(100);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

#ifdef SERIALCOM
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());
#endif
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
#ifdef SERIALCOM
  Serial.println("HTTP server started");
#endif
  pinMode(2, OUTPUT);

  Monster.init();  // init Monster shield
  Monster1.init(); // init Monster shield

// Set the motor speed (RPMs):
  myStepper.setSpeed(70);
 

  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(serverName);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String payload = "{}";
  int httpResponseCode = http.GET();
#ifdef SERIALCOM
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
#endif
  payload = http.getString();
#ifdef SERIALCOM
  Serial.print("Data: ");
  Serial.print(payload);
#endif
  int len = payload.length();
#ifdef SERIALCOM
  Serial.print("len :");
  Serial.println(len);
#endif
  char buf[len + 1] = "0";
  payload.toCharArray(buf, len + 1);

  // re3gex  to search counter vartiable
  string s = buf; //" Counter: 129";
  regex words_regex("Counter:.([0-9]*)");
  smatch matches;

  if (regex_search(s, matches, words_regex))
  {
    cout << "Match found\n";
    for (size_t i = 0; i < matches.size(); ++i)
    {
      cout << i << ": '" << matches[i].str() << "'\n";
    }
  }
  else
  {
    cout << "Match not found\n";
  }

  // using the stringstream class to insert a string and
  // extract an int
  stringstream ss;
  ss << matches[1].str();
  ss >> counter;
}

void loop()
{

  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay)
  {
    //Check WiFi connection status

    if (WiFi.status() == WL_CONNECTED)
    {
      HTTPClient http;

      // Your Domain name with URL path or IP address with path
      http.begin(serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      //String httpRequestData = "sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";
      // Send HTTP POST request
      String data = "counter=";
      data += counter;
      data += "&steps=";
      data += Steps;

      int httpResponseCode = http.POST(data);


#ifdef SERIALCOM
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
#endif

      // Free resources
      http.end();
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  myStepper.step(Steps);
  delay(200);
  //Step on revolution in the other direction:
  myStepper.step(StepsRevers);
  digitalWrite(2, LOW);
  counter = counter + 1;

  server.handleClient();
#ifdef SERIALCOM
  Serial.print("counter:");
  Serial.println(counter);
#endif
  delay(4000);
}
