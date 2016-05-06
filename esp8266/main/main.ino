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

const char HTML_INDEX[] PROGMEM = "<!DOCTYPE html><html><head> <meta name='viewport' content='width=device-width, initial-scale=1.0'> <style>body{background: #333333; margin: 0;}.container{max-width: 500px; margin: 8px auto;}.layout-column, .layout-row{box-sizing: border-box; display: -webkit-flex; display: -ms-flexbox; display: flex;}.layout-row{-webkit-flex-direction: row; -ms-flex-direction: row; flex-direction: row;}.layout-column{-webkit-flex-direction: column; -ms-flex-direction: column; flex-direction: column; margin: 0 8px;}.layout-column > *:first-child{margin-bottom: 8px;}.flex{-webkit-flex: 1; -ms-flex: 1; flex: 1; box-sizing: border-box;}.button-group{width: 100%; height: 48px; margin-bottom: 8px;}button{display: block; width: 48px; height: 48px; color: white; border: none; border-radius: 2px;}button:hover{cursor: pointer;}#on-button{background-color: hsl(120, 75%, 20%);}#off-button{background-color: hsl(9, 100%, 25%);}#on-button.active{background-color: hsl(120, 75%, 50%);}#off-button.active{background-color: hsl(9, 100%, 60%);}.img{width: 100%; border-radius: 2px; position: relative; overflow: hidden;}.cursor{position: absolute; top: -10%; left: 50%; width: 6px; height: 6px; border-radius: 50%; transform: translate(-50%, -50%); background: black;}#dimming{width: 48px; background: linear-gradient(to bottom, #000000 5%, #ffffff 95%);}#dimming-cursor{background: red;}#color-temperature{background-repeat: repeat-x; background-size: contain; width: 48px;}#cie1976ucs{margin-bottom: 4px;}#cie1976ucs-zoom{height: 200px; background-color: #333333;}#cie1976ucs-zoom > img{position: absolute; top: 50%; bottom: auto; left: 50%; right: auto; transform-origin: 0% 0%; transform: scale(2);}</style></head><body> <div class='container layout-row'> <div class='layout-column'> <div id='dimming' class='img flex'><div id='dimming-cursor' class='cursor'></diV></div><button id='off-button' type='button'>OFF</button> </div><div class='flex layout-colum'> <div id='cie1976ucs' class='img flex'> <img class='img' src='https://raw.githubusercontent.com/jaakkopasanen/Tunable-White-LED/master/Matlab/img/CIE_1976_UCS_06_063_Full.jpg'> <div id='cie1976ucs-cursor' class='cursor'></div></div><div id='cie1976ucs-zoom' class='img'> <img id='cie1976ucs-zoom-image' class='img' src='https://raw.githubusercontent.com/jaakkopasanen/Tunable-White-LED/master/Matlab/img/CIE_1976_UCS_06_063_Full.jpg'> </div></div><div class='layout-column'> <div id='color-temperature' class='img flex' style='background-image: url(https://raw.githubusercontent.com/jaakkopasanen/Tunable-White-LED/master/Matlab/img/cct_1000K_to_10000K_pow2_vertical.jpg);'> <div id='color-temperature-cursor' class='cursor'></div></div><button id='on-button' type='button'>ON</button> </div></div></body><script type='text/javascript'>var onOff_={{onOff_}};var L_={{L_}};var u_={{u_}};var v_={{v_}};var T_={{T_}};debug=false;/*var onOff_=true;var L_=75;var u_=0.199;var v_=0.471;var T_=-1;var debug=true;*/</script><script type='text/javascript'> var onButton=document.getElementById('on-button'); var offButton=document.getElementById('off-button'); var dimming=document.getElementById('dimming'); var dimmingCursor=document.getElementById('dimming-cursor'); var cie1976Ucs=document.getElementById('cie1976ucs'); var cie1976UcsCursor=document.getElementById('cie1976ucs-cursor'); var cie1976UcsZoom=document.getElementById('cie1976ucs-zoom'); var cie1976UcsZoomImage=document.getElementById('cie1976ucs-zoom-image'); var colorTemperature=document.getElementById('color-temperature'); var colorTemperatureCursor=document.getElementById('color-temperature-cursor'); var mode='color'; if (T_ && !L_) mode='temperature'; var ajax=function (url){console.log('AJAX:', url); if (debug) return; var xhr=new XMLHttpRequest(); xhr.open('GET', url, true); xhr.send(); xhr.onreadystatechange=function (){if (xhr.readyState===4){if (xhr.status===200){console.log('Success:', url);}else{console.log('Error:', xhr.responseText);}}};}; var setOnOff=function (on, doAjax){if (on){onButton.className='active'; offButton.className=''; if (doAjax) ajax('/on');}else{offButton.className='active'; onButton.className=''; if (doAjax) ajax('/off');}}; setOnOff(onOff_); onButton.addEventListener('click', function (){setOnOff(true, true);}); offButton.addEventListener('click', function (){setOnOff(false, true);}); var setDimming=function (L, doAjax){L_=L; dimmingCursor.style.top=(L/100 * 90 + 5) + '%'; if (doAjax){if (mode==='color'){ajax('/cie1976Ucs?L=' + L_ + '&u=' + u_ + '&v=' + v_);}else{ajax('/colorTemperature?L=' + L_ + '&T=' + T_);}}}; dimming.addEventListener('click', function (ev){var y=(ev.pageY - dimming.offsetTop) / dimming.offsetHeight; y=(y - 0.05) / 0.9; y=Math.min(y, 1); y=Math.max(y, 0); var L=y * 100; setDimming(L, true);}); var setCie1976Ucs=function (u, v, doAjax){u_=u; v_=v; mode='color'; cie1976UcsCursor.style.left=(u / 0.63 * 100) + '%'; cie1976UcsCursor.style.top=((1 - v / 0.6) * 100) + '%'; x=-(u / 0.63) * cie1976Ucs.offsetWidth; y=-(1 - v / 0.6) * cie1976Ucs.offsetHeight; cie1976UcsZoomImage.style.transform='scale(4) translate(' + x + 'px, ' + y + 'px)'; unsetColorTemperature(); if (doAjax) ajax('/cie1976Ucs?L=' + L_ + '&u=' + u + '&v=' + v);}; var unsetCie1976Ucs=function (){cie1976UcsCursor.style.left='-10%'; cie1976UcsCursor.style.top='-10%';}; cie1976Ucs.addEventListener('click', function (ev){var u=(ev.pageX - cie1976Ucs.offsetLeft) / cie1976Ucs.offsetWidth * 0.63; var v=(1 - (ev.pageY - cie1976Ucs.offsetTop) / cie1976Ucs.offsetHeight) * 0.6; setCie1976Ucs(u, v, true);}); cie1976UcsZoom.addEventListener('click', function (ev){var dx=((ev.pageX - cie1976UcsZoom.offsetLeft) / cie1976UcsZoom.offsetWidth - 0.5); var dy=((ev.pageY - cie1976UcsZoom.offsetTop) / cie1976UcsZoom.offsetHeight - 0.5); var dxpx=dx * cie1976UcsZoom.offsetWidth; var dypx=dy * cie1976UcsZoom.offsetHeight; var du=dxpx / cie1976Ucs.offsetWidth * 0.63 / 4; var dv=dypx / cie1976Ucs.offsetHeight * 0.6 / 4; setCie1976Ucs(u_ + du, v_ - dv, true);}); var setColorTemperature=function (T, doAjax){T_=T; mode='temperature'; colorTemperatureCursor.style.top=((1 - Math.pow(((T - 1000) / 9000),0.5)) * 100) + '%'; unsetCie1976Ucs(); if (doAjax) ajax('/colorTemperature?L=' + L_ + '&T=' + T_);}; var unsetColorTemperature=function (){colorTemperatureCursor.style.top='-10%';}; colorTemperature.addEventListener('click', function (ev){var y=(ev.pageY - colorTemperature.offsetTop) / colorTemperature.offsetHeight; var T=Math.pow((1-y), 2) * 9000 + 1000; setColorTemperature(T, true);}); window.onload=function (){cie1976UcsZoom.style.height=cie1976Ucs.offsetWidth + 'px'; setCie1976Ucs(u_, v_, false); setDimming(L_, false);};</script></html>";

ESP8266WebServer server(80);

/**
 * Is the light on?
 */
bool onOff_ = false;

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
 * Color temperature (in Kelvins)
 */
int T_;

/**
 * Getters and setters for onboard leds
 */
bool getLed1 () { return !digitalRead(led1Pin_); }
void setLed1 (bool val) { digitalWrite(led1Pin_, !val); }
bool getLed2 () { return !digitalRead(led2Pin_); }
void setLed2 (bool val) { digitalWrite(led2Pin_, !val); }

/**
 * Get raw pwm values
 * Values are normalized in the range 0..1
 */
RGB getRaw () {
  return raw_;
}

/**
 * Getter for onOff
 */
bool getOnOff () {
  return onOff_;
}

/**
 * Get CIE 1976 UCS color coordinates
 */
Cie1976Ucs getCie1976Ucs () {
  return luv_;
}

/**
 * Getter for color temperature
 */
int getColorTemperature () {
  return T_;
}

/**
 * Set raw pwm values
 * Values must be normalized to the range 0..1
 */
void setRaw (RGB raw) {

  if (!getOnOff()) return;

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
 * Sets light on or off
 */
void setOnOff (bool onOff) {
  onOff_ = onOff;
  if (onOff) {
    setRaw(getRaw());
  } else {
    analogWrite(redPin_, 0);
    analogWrite(greenPin_, 0);
    analogWrite(bluePin_, 0);
  }
}

/**
 * Set CIE 1976 UCS color coordinates
 */
void setCie1976Ucs (Cie1976Ucs luv) {

  if (!getOnOff()) return;

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

  float m = raw.R;
  if (raw.G > m) m = raw.G;
  if (raw.B > m) m = raw.B;
  float Y = 100 * pow(((luv.L + 16) / 116), 3);
  raw.R = raw.R * (1 / m) * (Y / 100);
  raw.G = raw.G * (1 / m) * (Y / 100);
  raw.B = raw.B * (1 / m) * (Y / 100);

  Serial.print("Converted to raw: ");
  Serial.print(raw.R); Serial.print(", ");
  Serial.print(raw.G); Serial.print(", ");
  Serial.println(raw.B);

  // Cannot be sure that current color is result of color temperature setter
  // unset color temperature. Color temperature setter will save the color
  // temperature after calling this function
  T_ = -1;

  // Write PWMs
  setRaw(raw);
}

/**
 * Sets ligth by color temperature
 */
void setColorTemperature (float L, int T) {
  if (!getOnOff()) return;
  
  // These cryptic looking formulas are a result of least RMSE fit of
  // CIE1976UCS coordinates vs color temperature
  double x = (T-5500.0)/2599.0; // Transform to z-score to avoid floating point precision problems
  double u = (-0.0001747*pow(x,3.0) + 0.1833*pow(x,2.0) + 0.872*x + 1.227) / (pow(x,2.0) + 4.813*x + 5.933);
  double v = (0.000311*pow(x,4.0) + 0.0009124*pow(x,3.0) + 0.3856*pow(x,2.0) + 1.873*x + 2.619) / (pow(x,2.0) + 4.323*x + 5.485);
  
  Cie1976Ucs luv = {luv_.L, u, v};
  if (L > 0) luv.L = L;
  
  setCie1976Ucs(luv);

  // Save color temperature
  T_ = T;
}

/**
 * Responses with PWM, CIE1976UCS and sRGB values
 */
void httpIndexController () {
  
  String html = FPSTR(HTML_INDEX);
  html.replace("{onOff_}", onOff_ ? "true" : "false");
  html.replace("{L_}", String(luv_.L));
  html.replace("{u_}", String(luv_.u));
  html.replace("{v_}", String(luv_.v));
  html.replace("{T_}", String(T_));
  
  server.send(200, "text/html", html);
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
 * HTTP API for setting the light on
 */
void httpOnController () {
  setOnOff(true);
  
  // JSON response
  StaticJsonBuffer<20> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["onOff"] = true;
  String response;
  json.prettyPrintTo(response);

  // Send response
  server.send(200, "application/json", response);
}

/**
 * HTTP API for setting the light off
 */
 void httpOffController () {
  setOnOff(false);
  
  // JSON response
  StaticJsonBuffer<20> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["onOff"] = false;
  String response;
  json.prettyPrintTo(response);

  // Send response
  server.send(200, "application/json", response);
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

void httpColorTemperatureController () {
  int T = -1; float L = -1;
  // Parse args
  for (uint8_t i = 0; i < server.args(); ++i) {
    if (server.argName(i) == "T") {
      T = server.arg(i).toInt();
    } else if (server.argName(i) == "L") {
      L = server.arg(i).toFloat();
    }
  }

  // Set color temperature
  if (T > 0) {
    setColorTemperature(L, T);
  } else {
    T = getColorTemperature();
  }

  // Lightness not given -> read from luv
  if (L < 0) {
    Cie1976Ucs luv = getCie1976Ucs();
    L = luv.L;
  }

  // JSON response
  StaticJsonBuffer<20> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["T"] = T;
  json["L"] = L;
  String response;
  json.prettyPrintTo(response);

  // Send response
  server.send(200, "application/json", response);
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

  // Set default to 2700K
  setOnOff(true);
  setColorTemperature(75, 2700);

  // Start Multicast domain name service
  //MDNS.begin("esp8266");

  // Routes
  server.on("/", httpIndexController);
  server.on("/onboardLeds", httpOnboardLedsController);
  server.on("/on", httpOnController);
  server.on("/off", httpOffController);
  server.on("/raw", httpRawController);
  server.on("/cie1976Ucs", httpCie1976UcsController);
  server.on("/colorTemperature", httpColorTemperatureController);
  // Route not found
  server.onNotFound(httpNotFoundController);

  // Start listening
  server.begin();
}

void loop(void){
  server.handleClient();
}
