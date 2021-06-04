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



/*Put your SSID & Password*/
const char *ssid = "WLAN_BYOD";        // Enter SSID here
const char *password = "SegelSCHiff"; //Enter Password here

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
  ptr += "<title>ESP32 Weather Report</title>\n";
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
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

  pinMode(2, OUTPUT);
  Monster.init();  // init Monster shield
  Monster1.init(); // init Monster shield

  // Set the motor speed (RPMs):
  myStepper.setSpeed(80);


HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);

      // Specify content-type header
       http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String payload = "{}"; 
  int httpResponseCode = http.GET();
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  payload = http.getString();
  Serial.print("Data1: ");
  Serial.print(payload);
   int len=payload.length();
   Serial.print("len :");
   Serial.println(len);  
   char  buf[len + 1] = "0";
   payload.toCharArray(buf, len + 1);     
    Serial.println(buf); 
 
 std::string s = buf;//" Counter: 129";
 

  std::regex words_regex("Counter:.([0-9]*)");
 

  std::smatch matches  ;

  if(std::regex_search(s, matches, words_regex)) {
        std::cout << "Match found\n";
        for (size_t i = 0; i < matches.size(); ++i) {
            std::cout << i << ": '" << matches[i].str() << "'\n";

        }
    } else {
        std::cout << "Match not found\n";
  
}

 
 
  // using the stringstream class to insert a string and
  // extract an int
  stringstream ss;  
  ss <<  matches[1].str();  
  ss >> counter;  
  Serial.print("counter get: ");
  Serial.println(counter);
    

}

void loop()
{





  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status

   



    if(WiFi.status()== WL_CONNECTED){
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
      data += 250;

      int httpResponseCode = http.POST(data);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader("Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }


  delay(2000);

  //Step on revolution in the other direction:
  myStepper.step(-280);
  digitalWrite(2, LOW);
  counter = counter+1;
 
  server.handleClient();
  Serial.print("counter:");
  Serial.println(counter);

  
  
  delay(5000);
}
  