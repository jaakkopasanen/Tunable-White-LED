#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <cmath>
const int redPin_    = 15; //12
const int greenPin_  = 13; //15
const int bluePin_   = 12; //13
const int w1Pin_     = 14;
const int w2Pin_     = 4;
const int led1Pin_   = 5;
const int led2Pin_   = 1;
const int pwmRange_ = 1023;
ESP8266WebServer server(80);

/**
 * Data structure for CIE 1976 UCS color coordinates
 */
struct Cie1976Ucs {
  float L;
  float u;
  float v;
} luv_;

/**
 * Data structure for RGB color
 */
struct RGB {
  float R;
  float G;
  float B;
} raw_;

/**
 * Get raw pwm values
 * Values are normalized in the range 0..1
 */
RGB getRaw () {
  return raw_;
}

/**
 * Set raw pwm values
 * Values must be normalized to the range 0..1
 */
void setRaw (RGB raw) {

  // Limit values in the range 0..1
  if (raw.R < 0) raw.R = 0;
  if (raw.R > 1) raw.R = 1;
  if (raw.G < 0) raw.G = 0;
  if (raw.G > 1) raw.G = 1;
  if (raw.B < 0) raw.B = 0;
  if (raw.B > 1) raw.B = 1;
  
  // Convert input floats to integers in the pwm range
  int pwmR = static_cast<int>(raw.R * pwmRange_ + 0.5);
  int pwmG = static_cast<int>(raw.G * pwmRange_ + 0.5);
  int pwmB = static_cast<int>(raw.B * pwmRange_ + 0.5);

  // Write PWMs
  analogWrite(redPin_, pwmR);
  analogWrite(greenPin_, pwmG);
  analogWrite(bluePin_, pwmB);

  Serial.print("Wrote PWM: ");
  Serial.print(pwmR); Serial.print(", ");
  Serial.print(pwmG); Serial.print(", ");
  Serial.println(pwmB);

  // Save values
  raw_.R = static_cast<float>(pwmR) / pwmRange_;
  raw_.G = static_cast<float>(pwmG) / pwmRange_;
  raw_.B = static_cast<float>(pwmB) / pwmRange_;

  Serial.print("Saved raw: ");
  Serial.print(raw_.R); Serial.print(", ");
  Serial.print(raw_.G); Serial.print(", ");
  Serial.println(raw_.B);
}

/**
 * Get CIE 1976 UCS color coordinates
 */
Cie1976Ucs getCie1976Ucs () {
  return luv_;
}

/**
 * Set CIE 1976 UCS color coordinates
 */
void setCie1976Ucs (Cie1976Ucs luv) {

  Serial.print("Setting luv: ");
  Serial.print(luv.L); Serial.print(", ");
  Serial.print(luv.u); Serial.print(", ");
  Serial.println(luv.v);

  // Save values
  luv_.L = luv.L;
  luv_.u = luv.u;
  luv_.v = luv.v;

  // Convert to raw PWM values
  RGB raw;
  raw.R = 1 - 2 * sqrt( pow((luv.u - 0.5444), 2.0) + pow((luv.v - 0.5183), 2.0) );
  raw.G = 1 - 2 * sqrt( pow((luv.u - 0.0412), 2.0) + pow((luv.v - 0.5837), 2.0) );
  raw.B = 1 - 2 * sqrt( pow((luv.u - 0.1232), 2.0) + pow((luv.v - 0.1657), 2.0) );

  Serial.print("Converted to raw: ");
  Serial.print(raw.R); Serial.print(", ");
  Serial.print(raw.G); Serial.print(", ");
  Serial.println(raw.B);

  // Write PWMs
  setRaw(raw);
}

/**
 * Getters and setters for onboard leds
 */
bool getLed1 () { return !digitalRead(led1Pin_); }
void setLed1 (bool val) { digitalWrite(led1Pin_, !val); }
bool getLed2 () { return !digitalRead(led2Pin_); }
void setLed2 (bool val) { digitalWrite(led2Pin_, !val); }

/**
 * Responses with PWM, CIE1976UCS and sRGB values
 */
void httpIndexController () {
  server.send(200, "application/json", "Hello");
}

/**
 * HTTP API for onboard led control
 */
void httpOnboardLedsController () {
  int led1 = -1;
  int led2 = -1;

  // Parse args
  for (uint8_t i = 0; i < server.args(); ++i) {
    if (server.argName(i) == "led1") {
      led1 = server.arg(i).toInt();
    } else if ( server.argName(i) == "led2") {
      led2 = server.arg(i).toInt();
    }
  }

  // LED 1
  if (led1 == -1) led1 = getLed1();
  else setLed1(led1);

  // LED 2
  if (led2 == -1) led2 = getLed2();
  else setLed2(led2);

  // JSON response
  StaticJsonBuffer<JSON_OBJECT_SIZE(2)> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["led1"] = led1;
  json["led2"] = led2;
  String response;
  json.prettyPrintTo(response);

  server.send(200, "application/json", response);
}

/**
 * HTTP API for CIE 1976 UCS color coordinates
 */
void httpCie1976UcsController () {
  float L = -1, u = -1, v = -1;
  // Parse args
  for (uint8_t i = 0; i < server.args(); ++i) {
    if (server.argName(i) == "L") {
      L = server.arg(i).toFloat();
    } else if (server.argName(i) == "u") {
      u = server.arg(i).toFloat();
    } else if (server.argName(i) == "v") {
      v = server.arg(i).toFloat();
    }
  }

  int httpStatus = 200;

  // All or none of the parameters must be missing
  if (L > 0 && u > 0 && v > 0) {
    // All parameters given -> set new color
    Cie1976Ucs luv = {L, u, v};
    setCie1976Ucs(luv);
    
  } else if (L < 0 && u < 0 && v < 0) {
    // None of the parameters given -> read existing color
    Cie1976Ucs luv = getCie1976Ucs();
    L = luv.L;
    u = luv.u;
    v = luv.v;
    
  } else {
    // Some of the parameters missing -> invalid request
    httpStatus = 400;
  }

  // JSON response
  StaticJsonBuffer<60> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["L"] = L;
  json["u"] = u;
  json["v"] = v;
  String response;
  json.prettyPrintTo(response);

  // Send response
  server.send(httpStatus, "application/json", response);
}

/**
 * HTTP API for raw PWM values
 */
void httpRawController () {
  float R = -1, G = -1, B = -1;

  // Parse args
  for (uint8_t i = 0; i < server.args(); ++i) {
    if (server.argName(i) == "R") {
      R = server.arg(i).toFloat();
    } else if (server.argName(i) == "G") {
      G = server.arg(i).toFloat();
    } else if (server.argName(i) == "B") {
      B = server.arg(i).toFloat();
    }
  }

  int httpStatus = 200;

  // All or none of the parameters must be missing
  if (R > 0 && G > 0 && B > 0) {
    // All parameters given -> set new color
    RGB raw = {R, G, B};
    setRaw(raw);
    
  } else if (R < 0 && G < 0 && B < 0) {
    // None of the parameters given -> read existing color
    RGB raw = getRaw();
    R = raw.R;
    G = raw.G;
    B = raw.B;
    
  } else {
    // Some of the parameters missing -> invalid request
    httpStatus = 400;
  }

  // JSON response
  StaticJsonBuffer<60> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["R"] = R;
  json["G"] = G;
  json["B"] = B;
  String response;
  json.prettyPrintTo(response);

  // Send response
  server.send(httpStatus, "application/json", response);
}

void httpNotFoundController () {
  server.send(404, "text/plain", "404");
}

void setup(void){
  analogWriteRange(pwmRange_);
  
  pinMode(led1Pin_, OUTPUT); digitalWrite(led1Pin_, HIGH);
  pinMode(led2Pin_, OUTPUT); digitalWrite(led2Pin_, HIGH);
  pinMode(redPin_, OUTPUT); analogWrite(redPin_, 0);
  pinMode(greenPin_, OUTPUT); analogWrite(greenPin_, 0);
  pinMode(bluePin_, OUTPUT); analogWrite(bluePin_, 0);
  pinMode(w1Pin_, OUTPUT); analogWrite(w1Pin_, 0);
  pinMode(w2Pin_, OUTPUT); analogWrite(w2Pin_, 0);

  Serial.begin(115200);
  
  //WiFi.begin(ssid, password);
  
  /*while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }*/

  // Set onboard leds on as a sign for AP mode
  setLed1(true);
  setLed2(true);

  // Autoconnect to latest WiFi or create cofiguration portal if cannot connect as a client
  WiFiManager wiFiManager;
  //wiFiManager.resetSettings();
  wiFiManager.autoConnect("esp8277", "esp8266password");
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // Set onboard leds off as a sign for STA mode
  setLed1(false);
  setLed2(false);

  // Start Multicast domain name service
  //MDNS.begin("esp8266");

  // Routes
  server.on("/", httpIndexController);
  server.on("/onboardLeds", httpOnboardLedsController);
  server.on("/raw", httpRawController);
  server.on("/cie1976Ucs", httpCie1976UcsController);
  //server.on("/srgb", httpSrgbController);

  // Route not found
  server.onNotFound(httpNotFoundController);

  // Start listening
  server.begin();
}

void loop(void){
  server.handleClient();
}
