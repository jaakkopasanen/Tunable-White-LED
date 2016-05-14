#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
//#include <ESP8266mDNS.h>
#include <cmath>

// GPIO settings
const int redPin_    = 15; //12
const int greenPin_  = 13; //15
const int bluePin_   = 12; //13
const int w1Pin_     = 14;
const int w2Pin_     = 4;
const int led1Pin_   = 5;
const int led2Pin_   = 1;
const int pwmRange_  = 1023;

// Index.html
// Minify, replace double quotes with single quotes, escape backlashes in regexes
const char HTML_INDEX[] PROGMEM = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'><style>html{height: 100%;font-family: Arial;}body{background: #555555;margin: 0;height: 100%;padding: 8px;box-sizing: border-box;}.container{max-width: 800px;height: 100%;margin: 0 auto;}.layout-column, .layout-row{box-sizing: border-box;display: -webkit-flex;display: -ms-flexbox;display: flex;}.layout-row{-webkit-flex-direction: row;-ms-flex-direction: row;flex-direction: row;}.layout-row > *{margin-right: 8px;}.layout-row > *:last-child{margin-right: 0;}.layout-column{-webkit-flex-direction: column;-ms-flex-direction: column;flex-direction: column;}.layout-column > *{margin-bottom: 8px;}.layout-column > *:last-child{margin-bottom: 0;}.layout-align-center-center{-webkit-align-items: center;-ms-flex-align: center;align-items: center;-webkit-align-content: center;-ms-flex-line-pack: center;align-content: center;max-width: 100%;-webkit-justify-content: center;-ms-flex-pack: center;justify-content: center;}.flex{-webkit-flex: 1;-ms-flex: 1;flex: 1;box-sizing: border-box;}button{display: block;min-width: 48px; min-height: 48px;color: white;border: none;border-radius: 2px;}button:hover{cursor: pointer;}#settings-button{background-color: hsl(213, 100%, 60%);}#off-button{background-color: hsl(9, 100%, 25%);}#off-button.active{background-color: hsl(9, 100%, 60%);}#on-button{background-color: hsl(120, 100%, 20%);}#on-button.active{background-color: hsl(120, 100%, 50%);}.tile{background-color: rgba(0, 0, 0, 0.7);color: white;padding: 24px;border-radius: 2px;}.img{display: block;width: 100%;border-radius: 2px;position: relative;overflow: hidden;}.cursor{position: absolute;top: -10%; left: 50%;width: 6px; height: 6px;border-radius: 50%;transform: translate(-50%, -50%);background: black;}#dimming{width: 48px;background: linear-gradient(to bottom, #000000 5%, #ffffff 95%);}#dimming-cursor{background: red;}#color-temperature{background-repeat: repeat-x; background-size: contain; width: 48px;}#cie1976ucs{flex-shrink: 0;}#cie1976ucs-zoom{background-color: #333333;}#cie1976ucs-zoom > img{position: absolute;top: 50%; bottom: auto;left: 50%; right: auto;transform-origin: 0% 0%;transform: scale(2);}#settings{background-color: rgba(0, 0, 0, 0.54);position: absolute;top: 0; bottom: 0;left: 0; right: 0;}#settings.no-show{display: none;}#settings .tile{margin: 0 8px;}#settings h1{margin-top: 0;font-weight: normal;}#settings textarea{width: 100%;border: none;padding: 10px;border-radius: 2px;box-sizing: border-box;}#settings button{float: left; width: 50%;}#settings-save-button{background-color: hsl(213, 100%, 60%);border-top-left-radius: 0;border-bottom-left-radius: 0;}#settings-close-button{background-color: hsl(9, 100%, 25%);border-top-right-radius: 0;border-bottom-right-radius: 0;}</style></head><body><div id='main-container' class='container layout-column'><div class='flex layout-row'><div id='dimming' class='img'><div id='dimming-cursor' class='cursor'></diV></div><div class='flex layout-column'><div id='cie1976ucs' class='img'><img class='img' src='https://raw.githubusercontent.com/jaakkopasanen/Tunable-White-LED/master/Matlab/img/CIE_1976_UCS_06_063_Full.jpg'><div id='cie1976ucs-cursor' class='cursor'></div></div><div id='cie1976ucs-zoom' class='img flex'><img id='cie1976ucs-zoom-image' class='img' src='https://raw.githubusercontent.com/jaakkopasanen/Tunable-White-LED/master/Matlab/img/CIE_1976_UCS_06_063_Full.jpg'></div></div><div id='color-temperature' class='img' style='background-image: url(https://raw.githubusercontent.com/jaakkopasanen/Tunable-White-LED/master/Matlab/img/cct_1000K_to_10000K_pow2_vertical.jpg);'><div id='color-temperature-cursor' class='cursor'></div></div></div><div class='layout-row'><button id='off-button' type='button' class='flex'>OFF</button><button id='settings-button' type='button' class='flex'>SETTINGS</button><button id='on-button' type='button' class='flex'>ON</button></div></div><div id='settings' class='layout-column layout-align-center-center no-show'><div class='tile'><h1>CALIBRATE</h1><textarea rows='4'></textarea><button id='settings-close-button' type='button'>CLOSE</button><button id='settings-save-button' type='button'>SAVE</button></div></div></body><script type='text/javascript'>var onOff_='{{onOff_}}';var L_='{{L_}}';var u_='{{u_}}';var v_='{{v_}}';var T_='{{T_}}';var debug='{{debug}}';if (debug){onOff_=true;L_=75;u_=0.199;v_=0.471;T_=-1;}</script><script type='text/javascript'>var zoomLevel=4;var mainContainer=document.getElementById('main-container');var onButton=document.getElementById('on-button');var offButton=document.getElementById('off-button');var settings=document.getElementById('settings');var settingsButton=document.getElementById('settings-button');var settingsCloseButton=document.getElementById('settings-close-button');var settingsSaveButton=document.getElementById('settings-save-button');var dimming=document.getElementById('dimming');var dimmingCursor=document.getElementById('dimming-cursor');var cie1976Ucs=document.getElementById('cie1976ucs');var cie1976UcsCursor=document.getElementById('cie1976ucs-cursor');var cie1976UcsZoom=document.getElementById('cie1976ucs-zoom');var cie1976UcsZoomImage=document.getElementById('cie1976ucs-zoom-image');var colorTemperature=document.getElementById('color-temperature');var colorTemperatureCursor=document.getElementById('color-temperature-cursor');var mode='color';if (T_ && !L_) mode='temperature';var ajax=function (url){console.log('AJAX:', url);if (debug) return;var xhr=new XMLHttpRequest();xhr.open('GET', url, true);xhr.send();xhr.onreadystatechange=function (){if (xhr.readyState===4){if (xhr.status===200){console.log('Success:', url);}else{console.log('Error:', xhr.responseText);}}};};var setOnOff=function (on, doAjax){if (on){onButton.className +=' active';offButton.className=offButton.className.replace( /(?:^|\\s)active(?!\\S)/g , '' );if (doAjax) ajax('/on');}else{offButton.className +=' active';onButton.className=onButton.className.replace( /(?:^|\\s)active(?!\\S)/g , '' );if (doAjax) ajax('/off');}};setOnOff(onOff_);onButton.addEventListener('click', function (){setOnOff(true, true);});offButton.addEventListener('click', function (){setOnOff(false, true);});var openSettings=function (){settings.className=settings.className.replace( /(?:^|\\s)no-show(?!\\S)/g , '' );mainContainer.style['-webkit-filter']='blur(5px)';mainContainer.style['filter']='blur(5px)';};var saveSettings=function (){var data=settings.querySelector('textarea').value;data=data.replace(/\\s/g, '');ajax('/calibrate?data=' + data);};var closeSettings=function (){settings.className +=' no-show';mainContainer.style['-webkit-filter']='';mainContainer.style['filter']='';};settingsButton.addEventListener('click', openSettings);settingsSaveButton.addEventListener('click', saveSettings);settingsCloseButton.addEventListener('click', closeSettings);var setDimming=function (L, doAjax){L_=L;dimmingCursor.style.top=(L/100 * 90 + 5) + '%';if (doAjax){if (mode==='color'){ajax('/cie1976Ucs?L=' + L_ + '&u=' + u_ + '&v=' + v_);}else{ajax('/colorTemperature?L=' + L_ + '&T=' + T_);}}};dimming.addEventListener('click', function (ev){var y=(ev.pageY - dimming.offsetTop) / dimming.offsetHeight;y=(y - 0.05) / 0.9;y=Math.min(y, 1);y=Math.max(y, 0);var L=y * 100;setDimming(L, true);});var setCie1976Ucs=function (u, v, doAjax){u_=u;v_=v;mode='color';cie1976UcsCursor.style.left=(u / 0.63 * 100) + '%';cie1976UcsCursor.style.top=((1 - v / 0.6) * 100) + '%';x=-(u / 0.63) * cie1976Ucs.offsetWidth;y=-(1 - v / 0.6) * cie1976Ucs.offsetHeight;cie1976UcsZoomImage.style.transform='scale(' + zoomLevel + ') translate(' + x + 'px, ' + y + 'px)';unsetColorTemperature();if (doAjax) ajax('/cie1976Ucs?L=' + L_ + '&u=' + u + '&v=' + v);};var unsetCie1976Ucs=function (){cie1976UcsCursor.style.left='-10%';cie1976UcsCursor.style.top='-10%';};cie1976Ucs.addEventListener('click', function (ev){var u=(ev.pageX - cie1976Ucs.offsetLeft) / cie1976Ucs.offsetWidth * 0.63;var v=(1 - (ev.pageY - cie1976Ucs.offsetTop) / cie1976Ucs.offsetHeight) * 0.6;setCie1976Ucs(u, v, true);});cie1976UcsZoom.addEventListener('click', function (ev){var dx=((ev.pageX - cie1976UcsZoom.offsetLeft) / cie1976UcsZoom.offsetWidth - 0.5);var dy=((ev.pageY - cie1976UcsZoom.offsetTop) / cie1976UcsZoom.offsetHeight - 0.5);var dxpx=dx * cie1976UcsZoom.offsetWidth;var dypx=dy * cie1976UcsZoom.offsetHeight;var du=dxpx / cie1976Ucs.offsetWidth * 0.63 / zoomLevel;var dv=dypx / cie1976Ucs.offsetHeight * 0.6 / zoomLevel;setCie1976Ucs(u_ + du, v_ - dv, true);});var setColorTemperature=function (T, doAjax){T_=T;mode='temperature';colorTemperatureCursor.style.top=((1 - Math.pow(((T - 1000) / 9000),0.5)) * 100) + '%';unsetCie1976Ucs();if (doAjax) ajax('/colorTemperature?L=' + L_ + '&T=' + T_);};var unsetColorTemperature=function (){colorTemperatureCursor.style.top='-10%';};colorTemperature.addEventListener('click', function (ev){var y=(ev.pageY - colorTemperature.offsetTop) / colorTemperature.offsetHeight;var T=Math.pow((1-y), 2) * 9000 + 1000;setColorTemperature(T, true);});window.onload=function (){setCie1976Ucs(u_, v_, false);setDimming(L_, false);};</script></html>";

// HTTP server
ESP8266WebServer server(80);

// Data structure for CIE 1976 UCS color coordinates
struct Luv {
  float L;
  float u;
  float v;
};

// Data structure for RGB color
struct RGB {
  float R;
  float G;
  float B;
};

// Is the light on?
bool onOff_ = false;

// Raw PWM values
RGB raw_;

// CIE 1976 UCS color coordinates
Luv luv_;

// Correlated color temperature (in Kelvins)
int T_;


// Calibration coefficients with default values

// Luminous fluxes
float redLum_ = 0.5;
float greenLum_ = 1.0;
float blueLum_ = 0.75;
float maxLum_ = redLum_ + greenLum_ + blueLum_;

// LED u', v' coordinates
Luv redUv_ = {100, 0.5535, 0.5170};
Luv greenUv_ = {100, 0.0373, 0.5856};
Luv blueUv_ = {100, 0.1679, 0.1153};

// Mixing fit coefficients
float redToGreenFit_[] = {2.9658, 0.0, 1.9658};
float greenToBlueFit_[] = {1.3587, 0.0, 0.3587};
float blueToRedFit_[] = {-0.2121, 0.2121, 0.2121};



/**
 * Utility functions
 */

/**
 * Find coefficient for a LED
 */
float findCoefficient (const Luv PT, const Luv P0, const Luv P1, const Luv P2, const float rightHandFit[3], const float leftHandFit[3]) {

  double PTu = PT.u;
  double PTv = PT.v;
  double P0u = P0.u;
  double P0v = P0.v;
  double P1u = P1.u;
  double P1v = P1.v;
  double P2u = P2.u;
  double P2v = P2.v;
  double Rp1 = rightHandFit[0];
  double Rp2 = rightHandFit[1];
  double Rq1 = rightHandFit[2];
  double Lp1 = leftHandFit[0];
  double Lp2 = leftHandFit[1];
  double Lq1 = leftHandFit[2];

  double dR;
  if (Lp1 < 0) {
    dR = (sqrt((P0u*P0u)*(P1v*P1v)+(P1u*P1u)*(P0v*P0v)+(P0u*P0u)*(PTv*PTv)+(P0v*P0v)*(PTu*PTu)+(P1u*P1u)*(PTv*PTv)+(P1v*P1v)*(PTu*PTu)-Lp1*(P0u*P0u)*(P1v*P1v)*2.0-Lp1*(P1u*P1u)*(P0v*P0v)*2.0-Lp2*(P0u*P0u)*(P1v*P1v)*2.0-Lp2*(P1u*P1u)*(P0v*P0v)*2.0+Lq1*(P0u*P0u)*(P1v*P1v)*2.0+Lq1*(P1u*P1u)*(P0v*P0v)*2.0-Lp1*(P0u*P0u)*(PTv*PTv)*2.0-Lp1*(P0v*P0v)*(PTu*PTu)*2.0-Lp2*(P0u*P0u)*(PTv*PTv)*4.0-Lp2*(P0v*P0v)*(PTu*PTu)*4.0+Lq1*(P0u*P0u)*(PTv*PTv)*2.0+Lq1*(P0v*P0v)*(PTu*PTu)*2.0+Lq1*(P1u*P1u)*(PTv*PTv)*2.0+Lq1*(P1v*P1v)*(PTu*PTu)*2.0+(Lp1*Lp1)*(P0u*P0u)*(P1v*P1v)+(Lp1*Lp1)*(P1u*P1u)*(P0v*P0v)+(Lp2*Lp2)*(P0u*P0u)*(P1v*P1v)+(Lp2*Lp2)*(P1u*P1u)*(P0v*P0v)+(Lp1*Lp1)*(P1u*P1u)*(P2v*P2v)+(Lp1*Lp1)*(P2u*P2u)*(P1v*P1v)+(Lp2*Lp2)*(P0u*P0u)*(P2v*P2v)+(Lp2*Lp2)*(P2u*P2u)*(P0v*P0v)+(Lp2*Lp2)*(P1u*P1u)*(P2v*P2v)+(Lp2*Lp2)*(P2u*P2u)*(P1v*P1v)+(Lq1*Lq1)*(P0u*P0u)*(P1v*P1v)+(Lq1*Lq1)*(P1u*P1u)*(P0v*P0v)+(Lp1*Lp1)*(P0u*P0u)*(PTv*PTv)+(Lp1*Lp1)*(P0v*P0v)*(PTu*PTu)+(Lp1*Lp1)*(P2u*P2u)*(PTv*PTv)+(Lp1*Lp1)*(P2v*P2v)*(PTu*PTu)+(Lq1*Lq1)*(P0u*P0u)*(PTv*PTv)+(Lq1*Lq1)*(P0v*P0v)*(PTu*PTu)+(Lq1*Lq1)*(P1u*P1u)*(PTv*PTv)+(Lq1*Lq1)*(P1v*P1v)*(PTu*PTu)-P0u*P1u*(PTv*PTv)*2.0-P0u*(P1v*P1v)*PTu*2.0-P1u*(P0v*P0v)*PTu*2.0-P0v*P1v*(PTu*PTu)*2.0-(P0u*P0u)*P1v*PTv*2.0-(P1u*P1u)*P0v*PTv*2.0+Lp1*Lp2*(P0u*P0u)*(P1v*P1v)*2.0+Lp1*Lp2*(P1u*P1u)*(P0v*P0v)*2.0+Lp1*Lp2*(P1u*P1u)*(P2v*P2v)*2.0+Lp1*Lp2*(P2u*P2u)*(P1v*P1v)*2.0-Lp1*Lq1*(P0u*P0u)*(P1v*P1v)*2.0-Lp1*Lq1*(P1u*P1u)*(P0v*P0v)*2.0-Lp2*Lq1*(P0u*P0u)*(P1v*P1v)*2.0-Lp2*Lq1*(P1u*P1u)*(P0v*P0v)*2.0+Lp1*Lq1*(P0u*P0u)*(PTv*PTv)*2.0+Lp1*Lq1*(P0v*P0v)*(PTu*PTu)*2.0-(Lp1*Lp1)*P0u*P2u*(P1v*P1v)*2.0-(Lp2*Lp2)*P0u*P1u*(P2v*P2v)*2.0-(Lp2*Lp2)*P0u*P2u*(P1v*P1v)*2.0-(Lp2*Lp2)*P1u*P2u*(P0v*P0v)*2.0-(Lp1*Lp1)*(P1u*P1u)*P0v*P2v*2.0-(Lp2*Lp2)*(P0u*P0u)*P1v*P2v*2.0-(Lp2*Lp2)*(P1u*P1u)*P0v*P2v*2.0-(Lp2*Lp2)*(P2u*P2u)*P0v*P1v*2.0-(Lp1*Lp1)*P1u*(P0v*P0v)*PTu*2.0-(Lp1*Lp1)*P0u*P2u*(PTv*PTv)*2.0-(Lp1*Lp1)*P1u*(P2v*P2v)*PTu*2.0-(Lp1*Lp1)*(P0u*P0u)*P1v*PTv*2.0-(Lp1*Lp1)*P0v*P2v*(PTu*PTu)*2.0-(Lp1*Lp1)*(P2u*P2u)*P1v*PTv*2.0-(Lq1*Lq1)*P0u*P1u*(PTv*PTv)*2.0-(Lq1*Lq1)*P0u*(P1v*P1v)*PTu*2.0-(Lq1*Lq1)*P1u*(P0v*P0v)*PTu*2.0-(Lq1*Lq1)*P0v*P1v*(PTu*PTu)*2.0-(Lq1*Lq1)*(P0u*P0u)*P1v*PTv*2.0-(Lq1*Lq1)*(P1u*P1u)*P0v*PTv*2.0-P0u*P1u*P0v*P1v*2.0+P0u*P1u*P0v*PTv*2.0+P0u*P0v*P1v*PTu*2.0+P0u*P1u*P1v*PTv*2.0+P1u*P0v*P1v*PTu*2.0-P0u*P0v*PTu*PTv*2.0+P0u*P1v*PTu*PTv*2.0+P1u*P0v*PTu*PTv*2.0-P1u*P1v*PTu*PTv*2.0+Lp1*P0u*P2u*(P1v*P1v)*2.0+Lp2*P0u*P2u*(P1v*P1v)*2.0-Lp2*P1u*P2u*(P0v*P0v)*2.0+Lp1*(P1u*P1u)*P0v*P2v*2.0-Lp2*(P0u*P0u)*P1v*P2v*2.0+Lp2*(P1u*P1u)*P0v*P2v*2.0+Lp1*P0u*P1u*(PTv*PTv)*2.0+Lp1*P0u*(P1v*P1v)*PTu*2.0+Lp1*P1u*(P0v*P0v)*PTu*4.0+Lp1*P0u*P2u*(PTv*PTv)*2.0+Lp2*P0u*P1u*(PTv*PTv)*4.0+Lp2*P0u*(P1v*P1v)*PTu*2.0+Lp2*P1u*(P0v*P0v)*PTu*6.0-Lp1*P1u*P2u*(PTv*PTv)*2.0-Lp1*P2u*(P1v*P1v)*PTu*2.0+Lp2*P0u*P2u*(PTv*PTv)*4.0+Lp2*P2u*(P0v*P0v)*PTu*2.0-Lp2*P1u*P2u*(PTv*PTv)*4.0-Lp2*P2u*(P1v*P1v)*PTu*2.0+Lp1*P0v*P1v*(PTu*PTu)*2.0+Lp1*(P0u*P0u)*P1v*PTv*4.0+Lp1*(P1u*P1u)*P0v*PTv*2.0+Lp1*P0v*P2v*(PTu*PTu)*2.0+Lp2*P0v*P1v*(PTu*PTu)*4.0+Lp2*(P0u*P0u)*P1v*PTv*6.0+Lp2*(P1u*P1u)*P0v*PTv*2.0-Lp1*P1v*P2v*(PTu*PTu)*2.0-Lp1*(P1u*P1u)*P2v*PTv*2.0+Lp2*P0v*P2v*(PTu*PTu)*4.0+Lp2*(P0u*P0u)*P2v*PTv*2.0-Lp2*P1v*P2v*(PTu*PTu)*4.0-Lp2*(P1u*P1u)*P2v*PTv*2.0-Lq1*P0u*P1u*(PTv*PTv)*4.0-Lq1*P0u*(P1v*P1v)*PTu*4.0-Lq1*P1u*(P0v*P0v)*PTu*4.0-Lq1*P0v*P1v*(PTu*PTu)*4.0-Lq1*(P0u*P0u)*P1v*PTv*4.0-Lq1*(P1u*P1u)*P0v*PTv*4.0-Lp1*Lp2*P0u*P1u*(P2v*P2v)*2.0-Lp1*Lp2*P0u*P2u*(P1v*P1v)*4.0-Lp1*Lp2*P1u*P2u*(P0v*P0v)*2.0-Lp1*Lp2*(P0u*P0u)*P1v*P2v*2.0-Lp1*Lp2*(P1u*P1u)*P0v*P2v*4.0-Lp1*Lp2*(P2u*P2u)*P0v*P1v*2.0+Lp1*Lq1*P0u*P2u*(P1v*P1v)*2.0+Lp1*Lq1*P1u*P2u*(P0v*P0v)*4.0+Lp2*Lq1*P0u*P2u*(P1v*P1v)*2.0+Lp2*Lq1*P1u*P2u*(P0v*P0v)*2.0+Lp1*Lq1*(P0u*P0u)*P1v*P2v*4.0+Lp1*Lq1*(P1u*P1u)*P0v*P2v*2.0+Lp2*Lq1*(P0u*P0u)*P1v*P2v*2.0+Lp2*Lq1*(P1u*P1u)*P0v*P2v*2.0-Lp1*Lp2*P1u*(P0v*P0v)*PTu*2.0+Lp1*Lp2*P0u*(P2v*P2v)*PTu*2.0+Lp1*Lp2*P2u*(P0v*P0v)*PTu*2.0-Lp1*Lp2*P1u*(P2v*P2v)*PTu*2.0-Lp1*Lp2*(P0u*P0u)*P1v*PTv*2.0+Lp1*Lp2*(P0u*P0u)*P2v*PTv*2.0+Lp1*Lp2*(P2u*P2u)*P0v*PTv*2.0-Lp1*Lp2*(P2u*P2u)*P1v*PTv*2.0-Lp1*Lq1*P0u*P1u*(PTv*PTv)*2.0+Lp1*Lq1*P0u*(P1v*P1v)*PTu*2.0-Lp1*Lq1*P0u*P2u*(PTv*PTv)*2.0-Lp1*Lq1*P2u*(P0v*P0v)*PTu*4.0+Lp2*Lq1*P0u*(P1v*P1v)*PTu*2.0+Lp2*Lq1*P1u*(P0v*P0v)*PTu*2.0+Lp1*Lq1*P1u*P2u*(PTv*PTv)*2.0-Lp1*Lq1*P2u*(P1v*P1v)*PTu*2.0-Lp2*Lq1*P2u*(P0v*P0v)*PTu*2.0-Lp2*Lq1*P2u*(P1v*P1v)*PTu*2.0-Lp1*Lq1*P0v*P1v*(PTu*PTu)*2.0+Lp1*Lq1*(P1u*P1u)*P0v*PTv*2.0-Lp1*Lq1*P0v*P2v*(PTu*PTu)*2.0-Lp1*Lq1*(P0u*P0u)*P2v*PTv*4.0+Lp2*Lq1*(P0u*P0u)*P1v*PTv*2.0+Lp2*Lq1*(P1u*P1u)*P0v*PTv*2.0+Lp1*Lq1*P1v*P2v*(PTu*PTu)*2.0-Lp1*Lq1*(P1u*P1u)*P2v*PTv*2.0-Lp2*Lq1*(P0u*P0u)*P2v*PTv*2.0-Lp2*Lq1*(P1u*P1u)*P2v*PTv*2.0-(Lp1*Lp1)*P0u*P1u*P0v*P1v*2.0-(Lp2*Lp2)*P0u*P1u*P0v*P1v*2.0+(Lp1*Lp1)*P0u*P1u*P1v*P2v*2.0+(Lp1*Lp1)*P1u*P2u*P0v*P1v*2.0+(Lp2*Lp2)*P0u*P1u*P0v*P2v*2.0+(Lp2*Lp2)*P0u*P2u*P0v*P1v*2.0+(Lp2*Lp2)*P0u*P1u*P1v*P2v*2.0-(Lp2*Lp2)*P0u*P2u*P0v*P2v*2.0+(Lp2*Lp2)*P1u*P2u*P0v*P1v*2.0-(Lp1*Lp1)*P1u*P2u*P1v*P2v*2.0+(Lp2*Lp2)*P0u*P2u*P1v*P2v*2.0+(Lp2*Lp2)*P1u*P2u*P0v*P2v*2.0-(Lp2*Lp2)*P1u*P2u*P1v*P2v*2.0-(Lq1*Lq1)*P0u*P1u*P0v*P1v*2.0+(Lp1*Lp1)*P0u*P1u*P0v*PTv*2.0+(Lp1*Lp1)*P0u*P0v*P1v*PTu*2.0-(Lp1*Lp1)*P0u*P1u*P2v*PTv*2.0+(Lp1*Lp1)*P0u*P2u*P1v*PTv*4.0-(Lp1*Lp1)*P0u*P1v*P2v*PTu*2.0-(Lp1*Lp1)*P1u*P2u*P0v*PTv*2.0+(Lp1*Lp1)*P1u*P0v*P2v*PTu*4.0-(Lp1*Lp1)*P2u*P0v*P1v*PTu*2.0+(Lp1*Lp1)*P1u*P2u*P2v*PTv*2.0+(Lp1*Lp1)*P2u*P1v*P2v*PTu*2.0+(Lq1*Lq1)*P0u*P1u*P0v*PTv*2.0+(Lq1*Lq1)*P0u*P0v*P1v*PTu*2.0+(Lq1*Lq1)*P0u*P1u*P1v*PTv*2.0+(Lq1*Lq1)*P1u*P0v*P1v*PTu*2.0-(Lp1*Lp1)*P0u*P0v*PTu*PTv*2.0+(Lp1*Lp1)*P0u*P2v*PTu*PTv*2.0+(Lp1*Lp1)*P2u*P0v*PTu*PTv*2.0-(Lp1*Lp1)*P2u*P2v*PTu*PTv*2.0-(Lq1*Lq1)*P0u*P0v*PTu*PTv*2.0+(Lq1*Lq1)*P0u*P1v*PTu*PTv*2.0+(Lq1*Lq1)*P1u*P0v*PTu*PTv*2.0-(Lq1*Lq1)*P1u*P1v*PTu*PTv*2.0+Lp1*P0u*P1u*P0v*P1v*4.0+Lp2*P0u*P1u*P0v*P1v*4.0-Lp1*P0u*P1u*P1v*P2v*2.0-Lp1*P1u*P2u*P0v*P1v*2.0+Lp2*P0u*P1u*P0v*P2v*2.0+Lp2*P0u*P2u*P0v*P1v*2.0-Lp2*P0u*P1u*P1v*P2v*2.0-Lp2*P1u*P2u*P0v*P1v*2.0-Lq1*P0u*P1u*P0v*P1v*4.0-Lp1*P0u*P1u*P0v*PTv*4.0-Lp1*P0u*P0v*P1v*PTu*4.0-Lp1*P0u*P1u*P1v*PTv*2.0-Lp1*P1u*P0v*P1v*PTu*2.0-Lp2*P0u*P1u*P0v*PTv*6.0-Lp2*P0u*P0v*P1v*PTu*6.0+Lp1*P0u*P1u*P2v*PTv*2.0-Lp1*P0u*P2u*P1v*PTv*4.0+Lp1*P0u*P1v*P2v*PTu*2.0+Lp1*P1u*P2u*P0v*PTv*2.0-Lp1*P1u*P0v*P2v*PTu*4.0+Lp1*P2u*P0v*P1v*PTu*2.0-Lp2*P0u*P1u*P1v*PTv*2.0-Lp2*P0u*P2u*P0v*PTv*2.0-Lp2*P0u*P0v*P2v*PTu*2.0-Lp2*P1u*P0v*P1v*PTu*2.0+Lp1*P1u*P2u*P1v*PTv*2.0+Lp1*P1u*P1v*P2v*PTu*2.0-Lp2*P0u*P2u*P1v*PTv*6.0+Lp2*P0u*P1v*P2v*PTu*6.0+Lp2*P1u*P2u*P0v*PTv*6.0-Lp2*P1u*P0v*P2v*PTu*6.0+Lp2*P1u*P2u*P1v*PTv*2.0+Lp2*P1u*P1v*P2v*PTu*2.0+Lq1*P0u*P1u*P0v*PTv*4.0+Lq1*P0u*P0v*P1v*PTu*4.0+Lq1*P0u*P1u*P1v*PTv*4.0+Lq1*P1u*P0v*P1v*PTu*4.0+Lp1*P0u*P0v*PTu*PTv*4.0-Lp1*P0u*P1v*PTu*PTv*2.0-Lp1*P1u*P0v*PTu*PTv*2.0+Lp2*P0u*P0v*PTu*PTv*8.0-Lp1*P0u*P2v*PTu*PTv*2.0-Lp1*P2u*P0v*PTu*PTv*2.0-Lp2*P0u*P1v*PTu*PTv*4.0-Lp2*P1u*P0v*PTu*PTv*4.0+Lp1*P1u*P2v*PTu*PTv*2.0+Lp1*P2u*P1v*PTu*PTv*2.0-Lp2*P0u*P2v*PTu*PTv*4.0-Lp2*P2u*P0v*PTu*PTv*4.0+Lp2*P1u*P2v*PTu*PTv*4.0+Lp2*P2u*P1v*PTu*PTv*4.0-Lq1*P0u*P0v*PTu*PTv*4.0+Lq1*P0u*P1v*PTu*PTv*4.0+Lq1*P1u*P0v*PTu*PTv*4.0-Lq1*P1u*P1v*PTu*PTv*4.0-Lp1*Lp2*P0u*P1u*P0v*P1v*4.0+Lp1*Lp2*P0u*P1u*P0v*P2v*2.0+Lp1*Lp2*P0u*P2u*P0v*P1v*2.0+Lp1*Lp2*P0u*P1u*P1v*P2v*4.0+Lp1*Lp2*P1u*P2u*P0v*P1v*4.0+Lp1*Lp2*P0u*P2u*P1v*P2v*2.0+Lp1*Lp2*P1u*P2u*P0v*P2v*2.0-Lp1*Lp2*P1u*P2u*P1v*P2v*4.0+Lp1*Lq1*P0u*P1u*P0v*P1v*4.0-Lp1*Lq1*P0u*P1u*P0v*P2v*4.0-Lp1*Lq1*P0u*P2u*P0v*P1v*4.0+Lp2*Lq1*P0u*P1u*P0v*P1v*4.0-Lp1*Lq1*P0u*P1u*P1v*P2v*2.0-Lp1*Lq1*P1u*P2u*P0v*P1v*2.0-Lp2*Lq1*P0u*P1u*P0v*P2v*2.0-Lp2*Lq1*P0u*P2u*P0v*P1v*2.0-Lp2*Lq1*P0u*P1u*P1v*P2v*2.0-Lp2*Lq1*P1u*P2u*P0v*P1v*2.0+Lp1*Lp2*P0u*P1u*P0v*PTv*2.0+Lp1*Lp2*P0u*P0v*P1v*PTu*2.0-Lp1*Lp2*P0u*P2u*P0v*PTv*2.0-Lp1*Lp2*P0u*P0v*P2v*PTu*2.0-Lp1*Lp2*P0u*P1u*P2v*PTv*2.0+Lp1*Lp2*P0u*P2u*P1v*PTv*4.0-Lp1*Lp2*P0u*P1v*P2v*PTu*2.0-Lp1*Lp2*P1u*P2u*P0v*PTv*2.0+Lp1*Lp2*P1u*P0v*P2v*PTu*4.0-Lp1*Lp2*P2u*P0v*P1v*PTu*2.0-Lp1*Lp2*P0u*P2u*P2v*PTv*2.0-Lp1*Lp2*P2u*P0v*P2v*PTu*2.0+Lp1*Lp2*P1u*P2u*P2v*PTv*2.0+Lp1*Lp2*P2u*P1v*P2v*PTu*2.0-Lp1*Lq1*P0u*P1u*P1v*PTv*2.0+Lp1*Lq1*P0u*P2u*P0v*PTv*4.0+Lp1*Lq1*P0u*P0v*P2v*PTu*4.0-Lp1*Lq1*P1u*P0v*P1v*PTu*2.0-Lp2*Lq1*P0u*P1u*P0v*PTv*2.0-Lp2*Lq1*P0u*P0v*P1v*PTu*2.0+Lp1*Lq1*P0u*P1u*P2v*PTv*6.0-Lp1*Lq1*P0u*P1v*P2v*PTu*6.0-Lp1*Lq1*P1u*P2u*P0v*PTv*6.0+Lp1*Lq1*P2u*P0v*P1v*PTu*6.0-Lp2*Lq1*P0u*P1u*P1v*PTv*2.0+Lp2*Lq1*P0u*P2u*P0v*PTv*2.0+Lp2*Lq1*P0u*P0v*P2v*PTu*2.0-Lp2*Lq1*P1u*P0v*P1v*PTu*2.0+Lp1*Lq1*P1u*P2u*P1v*PTv*2.0+Lp1*Lq1*P1u*P1v*P2v*PTu*2.0+Lp2*Lq1*P0u*P1u*P2v*PTv*4.0-Lp2*Lq1*P0u*P2u*P1v*PTv*2.0-Lp2*Lq1*P0u*P1v*P2v*PTu*2.0-Lp2*Lq1*P1u*P2u*P0v*PTv*2.0-Lp2*Lq1*P1u*P0v*P2v*PTu*2.0+Lp2*Lq1*P2u*P0v*P1v*PTu*4.0+Lp2*Lq1*P1u*P2u*P1v*PTv*2.0+Lp2*Lq1*P1u*P1v*P2v*PTu*2.0-Lp1*Lq1*P0u*P0v*PTu*PTv*4.0+Lp1*Lq1*P0u*P1v*PTu*PTv*2.0+Lp1*Lq1*P1u*P0v*PTu*PTv*2.0+Lp1*Lq1*P0u*P2v*PTu*PTv*2.0+Lp1*Lq1*P2u*P0v*PTu*PTv*2.0-Lp1*Lq1*P1u*P2v*PTu*PTv*2.0-Lp1*Lq1*P2u*P1v*PTu*PTv*2.0)*(1.0/2.0)+P0u*P1v*(1.0/2.0)-P1u*P0v*(1.0/2.0)-P0u*PTv*(1.0/2.0)+P0v*PTu*(1.0/2.0)+P1u*PTv*(1.0/2.0)-P1v*PTu*(1.0/2.0)-Lp1*P0u*P1v*(1.0/2.0)+Lp1*P1u*P0v*(1.0/2.0)+Lp1*P0u*P2v-Lp1*P2u*P0v-Lp2*P0u*P1v*(1.0/2.0)+Lp2*P1u*P0v*(1.0/2.0)-Lp1*P1u*P2v*(1.0/2.0)+Lp1*P2u*P1v*(1.0/2.0)+Lp2*P0u*P2v*(1.0/2.0)-Lp2*P2u*P0v*(1.0/2.0)-Lp2*P1u*P2v*(1.0/2.0)+Lp2*P2u*P1v*(1.0/2.0)+Lq1*P0u*P1v*(1.0/2.0)-Lq1*P1u*P0v*(1.0/2.0)-Lp1*P0u*PTv*(1.0/2.0)+Lp1*P0v*PTu*(1.0/2.0)+Lp1*P2u*PTv*(1.0/2.0)-Lp1*P2v*PTu*(1.0/2.0)-Lq1*P0u*PTv*(1.0/2.0)+Lq1*P0v*PTu*(1.0/2.0)+Lq1*P1u*PTv*(1.0/2.0)-Lq1*P1v*PTu*(1.0/2.0))/(P0u*P1v-P1u*P0v-P0u*PTv+P0v*PTu+P1u*PTv-P1v*PTu-Lp1*P0u*P1v+Lp1*P1u*P0v+Lp1*P0u*P2v-Lp1*P2u*P0v-Lp1*P1u*P2v+Lp1*P2u*P1v);
  } else {
    dR = 1.0 - (sqrt(Lp1*Lp1*P0u*P0u*P2v*P2v - 2*Lp1*Lp1*P0u*P0u*P2v*PTv + Lp1*Lp1*P0u*P0u*PTv*PTv - 2*Lp1*Lp1*P0u*P2u*P0v*P2v + 2*Lp1*Lp1*P0u*P2u*P0v*PTv + 2*Lp1*Lp1*P0u*P2u*P2v*PTv - 2*Lp1*Lp1*P0u*P2u*PTv*PTv + 2*Lp1*Lp1*P0u*P0v*P2v*PTu - 2*Lp1*Lp1*P0u*P0v*PTu*PTv - 2*Lp1*Lp1*P0u*P2v*P2v*PTu + 2*Lp1*Lp1*P0u*P2v*PTu*PTv + Lp1*Lp1*P2u*P2u*P0v*P0v - 2*Lp1*Lp1*P2u*P2u*P0v*PTv + Lp1*Lp1*P2u*P2u*PTv*PTv - 2*Lp1*Lp1*P2u*P0v*P0v*PTu + 2*Lp1*Lp1*P2u*P0v*P2v*PTu + 2*Lp1*Lp1*P2u*P0v*PTu*PTv - 2*Lp1*Lp1*P2u*P2v*PTu*PTv + Lp1*Lp1*P0v*P0v*PTu*PTu - 2*Lp1*Lp1*P0v*P2v*PTu*PTu + Lp1*Lp1*P2v*P2v*PTu*PTu - 2*Lp1*Lp2*P0u*P0u*P1v*P2v + 2*Lp1*Lp2*P0u*P0u*P1v*PTv + 2*Lp1*Lp2*P0u*P0u*P2v*P2v - 2*Lp1*Lp2*P0u*P0u*P2v*PTv + 2*Lp1*Lp2*P0u*P1u*P0v*P2v - 2*Lp1*Lp2*P0u*P1u*P0v*PTv - 2*Lp1*Lp2*P0u*P1u*P2v*P2v + 2*Lp1*Lp2*P0u*P1u*P2v*PTv + 2*Lp1*Lp2*P0u*P2u*P0v*P1v - 4*Lp1*Lp2*P0u*P2u*P0v*P2v + 2*Lp1*Lp2*P0u*P2u*P0v*PTv + 2*Lp1*Lp2*P0u*P2u*P1v*P2v - 4*Lp1*Lp2*P0u*P2u*P1v*PTv + 2*Lp1*Lp2*P0u*P2u*P2v*PTv - 2*Lp1*Lp2*P0u*P0v*P1v*PTu + 2*Lp1*Lp2*P0u*P0v*P2v*PTu + 2*Lp1*Lp2*P0u*P1v*P2v*PTu - 2*Lp1*Lp2*P0u*P2v*P2v*PTu - 2*Lp1*Lp2*P1u*P2u*P0v*P0v + 2*Lp1*Lp2*P1u*P2u*P0v*P2v + 2*Lp1*Lp2*P1u*P2u*P0v*PTv - 2*Lp1*Lp2*P1u*P2u*P2v*PTv + 2*Lp1*Lp2*P1u*P0v*P0v*PTu - 4*Lp1*Lp2*P1u*P0v*P2v*PTu + 2*Lp1*Lp2*P1u*P2v*P2v*PTu + 2*Lp1*Lp2*P2u*P2u*P0v*P0v - 2*Lp1*Lp2*P2u*P2u*P0v*P1v - 2*Lp1*Lp2*P2u*P2u*P0v*PTv + 2*Lp1*Lp2*P2u*P2u*P1v*PTv - 2*Lp1*Lp2*P2u*P0v*P0v*PTu + 2*Lp1*Lp2*P2u*P0v*P1v*PTu + 2*Lp1*Lp2*P2u*P0v*P2v*PTu - 2*Lp1*Lp2*P2u*P1v*P2v*PTu - 2*Lp1*Lq1*P0u*P0u*P1v*P2v + 2*Lp1*Lq1*P0u*P0u*P1v*PTv + 2*Lp1*Lq1*P0u*P0u*P2v*PTv - 2*Lp1*Lq1*P0u*P0u*PTv*PTv + 2*Lp1*Lq1*P0u*P1u*P0v*P2v - 2*Lp1*Lq1*P0u*P1u*P0v*PTv - 2*Lp1*Lq1*P0u*P1u*P2v*PTv + 2*Lp1*Lq1*P0u*P1u*PTv*PTv + 2*Lp1*Lq1*P0u*P2u*P0v*P1v - 2*Lp1*Lq1*P0u*P2u*P0v*PTv - 2*Lp1*Lq1*P0u*P2u*P1v*PTv + 2*Lp1*Lq1*P0u*P2u*PTv*PTv - 2*Lp1*Lq1*P0u*P0v*P1v*PTu - 2*Lp1*Lq1*P0u*P0v*P2v*PTu + 4*Lp1*Lq1*P0u*P0v*PTu*PTv + 4*Lp1*Lq1*P0u*P1v*P2v*PTu - 2*Lp1*Lq1*P0u*P1v*PTu*PTv - 2*Lp1*Lq1*P0u*P2v*PTu*PTv - 2*Lp1*Lq1*P1u*P2u*P0v*P0v + 4*Lp1*Lq1*P1u*P2u*P0v*PTv - 2*Lp1*Lq1*P1u*P2u*PTv*PTv + 2*Lp1*Lq1*P1u*P0v*P0v*PTu - 2*Lp1*Lq1*P1u*P0v*P2v*PTu - 2*Lp1*Lq1*P1u*P0v*PTu*PTv + 2*Lp1*Lq1*P1u*P2v*PTu*PTv + 2*Lp1*Lq1*P2u*P0v*P0v*PTu - 2*Lp1*Lq1*P2u*P0v*P1v*PTu - 2*Lp1*Lq1*P2u*P0v*PTu*PTv + 2*Lp1*Lq1*P2u*P1v*PTu*PTv - 2*Lp1*Lq1*P0v*P0v*PTu*PTu + 2*Lp1*Lq1*P0v*P1v*PTu*PTu + 2*Lp1*Lq1*P0v*P2v*PTu*PTu - 2*Lp1*Lq1*P1v*P2v*PTu*PTu + Lp2*Lp2*P0u*P0u*P1v*P1v - 2*Lp2*Lp2*P0u*P0u*P1v*P2v + Lp2*Lp2*P0u*P0u*P2v*P2v - 2*Lp2*Lp2*P0u*P1u*P0v*P1v + 2*Lp2*Lp2*P0u*P1u*P0v*P2v + 2*Lp2*Lp2*P0u*P1u*P1v*P2v - 2*Lp2*Lp2*P0u*P1u*P2v*P2v + 2*Lp2*Lp2*P0u*P2u*P0v*P1v - 2*Lp2*Lp2*P0u*P2u*P0v*P2v - 2*Lp2*Lp2*P0u*P2u*P1v*P1v + 2*Lp2*Lp2*P0u*P2u*P1v*P2v + Lp2*Lp2*P1u*P1u*P0v*P0v - 2*Lp2*Lp2*P1u*P1u*P0v*P2v + Lp2*Lp2*P1u*P1u*P2v*P2v - 2*Lp2*Lp2*P1u*P2u*P0v*P0v + 2*Lp2*Lp2*P1u*P2u*P0v*P1v + 2*Lp2*Lp2*P1u*P2u*P0v*P2v - 2*Lp2*Lp2*P1u*P2u*P1v*P2v + Lp2*Lp2*P2u*P2u*P0v*P0v - 2*Lp2*Lp2*P2u*P2u*P0v*P1v + Lp2*Lp2*P2u*P2u*P1v*P1v - 2*Lp2*Lq1*P0u*P0u*P1v*P1v + 2*Lp2*Lq1*P0u*P0u*P1v*P2v + 2*Lp2*Lq1*P0u*P0u*P1v*PTv - 2*Lp2*Lq1*P0u*P0u*P2v*PTv + 4*Lp2*Lq1*P0u*P1u*P0v*P1v - 2*Lp2*Lq1*P0u*P1u*P0v*P2v - 2*Lp2*Lq1*P0u*P1u*P0v*PTv - 2*Lp2*Lq1*P0u*P1u*P1v*P2v - 2*Lp2*Lq1*P0u*P1u*P1v*PTv + 4*Lp2*Lq1*P0u*P1u*P2v*PTv - 2*Lp2*Lq1*P0u*P2u*P0v*P1v + 2*Lp2*Lq1*P0u*P2u*P0v*PTv + 2*Lp2*Lq1*P0u*P2u*P1v*P1v - 2*Lp2*Lq1*P0u*P2u*P1v*PTv - 2*Lp2*Lq1*P0u*P0v*P1v*PTu + 2*Lp2*Lq1*P0u*P0v*P2v*PTu + 2*Lp2*Lq1*P0u*P1v*P1v*PTu - 2*Lp2*Lq1*P0u*P1v*P2v*PTu - 2*Lp2*Lq1*P1u*P1u*P0v*P0v + 2*Lp2*Lq1*P1u*P1u*P0v*P2v + 2*Lp2*Lq1*P1u*P1u*P0v*PTv - 2*Lp2*Lq1*P1u*P1u*P2v*PTv + 2*Lp2*Lq1*P1u*P2u*P0v*P0v - 2*Lp2*Lq1*P1u*P2u*P0v*P1v - 2*Lp2*Lq1*P1u*P2u*P0v*PTv + 2*Lp2*Lq1*P1u*P2u*P1v*PTv + 2*Lp2*Lq1*P1u*P0v*P0v*PTu - 2*Lp2*Lq1*P1u*P0v*P1v*PTu - 2*Lp2*Lq1*P1u*P0v*P2v*PTu + 2*Lp2*Lq1*P1u*P1v*P2v*PTu - 2*Lp2*Lq1*P2u*P0v*P0v*PTu + 4*Lp2*Lq1*P2u*P0v*P1v*PTu - 2*Lp2*Lq1*P2u*P1v*P1v*PTu + 4*Lp2*P0u*P0u*P1v*P2v - 4*Lp2*P0u*P0u*P1v*PTv - 4*Lp2*P0u*P0u*P2v*PTv + 4*Lp2*P0u*P0u*PTv*PTv - 4*Lp2*P0u*P1u*P0v*P2v + 4*Lp2*P0u*P1u*P0v*PTv + 4*Lp2*P0u*P1u*P2v*PTv - 4*Lp2*P0u*P1u*PTv*PTv - 4*Lp2*P0u*P2u*P0v*P1v + 4*Lp2*P0u*P2u*P0v*PTv + 4*Lp2*P0u*P2u*P1v*PTv - 4*Lp2*P0u*P2u*PTv*PTv + 4*Lp2*P0u*P0v*P1v*PTu + 4*Lp2*P0u*P0v*P2v*PTu - 8*Lp2*P0u*P0v*PTu*PTv - 8*Lp2*P0u*P1v*P2v*PTu + 4*Lp2*P0u*P1v*PTu*PTv + 4*Lp2*P0u*P2v*PTu*PTv + 4*Lp2*P1u*P2u*P0v*P0v - 8*Lp2*P1u*P2u*P0v*PTv + 4*Lp2*P1u*P2u*PTv*PTv - 4*Lp2*P1u*P0v*P0v*PTu + 4*Lp2*P1u*P0v*P2v*PTu + 4*Lp2*P1u*P0v*PTu*PTv - 4*Lp2*P1u*P2v*PTu*PTv - 4*Lp2*P2u*P0v*P0v*PTu + 4*Lp2*P2u*P0v*P1v*PTu + 4*Lp2*P2u*P0v*PTu*PTv - 4*Lp2*P2u*P1v*PTu*PTv + 4*Lp2*P0v*P0v*PTu*PTu - 4*Lp2*P0v*P1v*PTu*PTu - 4*Lp2*P0v*P2v*PTu*PTu + 4*Lp2*P1v*P2v*PTu*PTu + Lq1*Lq1*P0u*P0u*P1v*P1v - 2*Lq1*Lq1*P0u*P0u*P1v*PTv + Lq1*Lq1*P0u*P0u*PTv*PTv - 2*Lq1*Lq1*P0u*P1u*P0v*P1v + 2*Lq1*Lq1*P0u*P1u*P0v*PTv + 2*Lq1*Lq1*P0u*P1u*P1v*PTv - 2*Lq1*Lq1*P0u*P1u*PTv*PTv + 2*Lq1*Lq1*P0u*P0v*P1v*PTu - 2*Lq1*Lq1*P0u*P0v*PTu*PTv - 2*Lq1*Lq1*P0u*P1v*P1v*PTu + 2*Lq1*Lq1*P0u*P1v*PTu*PTv + Lq1*Lq1*P1u*P1u*P0v*P0v - 2*Lq1*Lq1*P1u*P1u*P0v*PTv + Lq1*Lq1*P1u*P1u*PTv*PTv - 2*Lq1*Lq1*P1u*P0v*P0v*PTu + 2*Lq1*Lq1*P1u*P0v*P1v*PTu + 2*Lq1*Lq1*P1u*P0v*PTu*PTv - 2*Lq1*Lq1*P1u*P1v*PTu*PTv + Lq1*Lq1*P0v*P0v*PTu*PTu - 2*Lq1*Lq1*P0v*P1v*PTu*PTu + Lq1*Lq1*P1v*P1v*PTu*PTu) + 2*P0u*P1v - 2*P1u*P0v - 2*P0u*PTv + 2*P0v*PTu + 2*P1u*PTv - 2*P1v*PTu - 2*Lp1*P0u*P1v + 2*Lp1*P1u*P0v + Lp1*P0u*P2v - Lp1*P2u*P0v - Lp2*P0u*P1v + Lp2*P1u*P0v - 2*Lp1*P1u*P2v + 2*Lp1*P2u*P1v + Lp2*P0u*P2v - Lp2*P2u*P0v - Lp2*P1u*P2v + Lp2*P2u*P1v + Lq1*P0u*P1v - Lq1*P1u*P0v + Lp1*P0u*PTv - Lp1*P0v*PTu - Lp1*P2u*PTv + Lp1*P2v*PTu - Lq1*P0u*PTv + Lq1*P0v*PTu + Lq1*P1u*PTv - Lq1*P1v*PTu)/(2*(P0u*P1v - P1u*P0v - P0u*PTv + P0v*PTu + P1u*PTv - P1v*PTu - Lp1*P0u*P1v + Lp1*P1u*P0v + Lp1*P0u*P2v - Lp1*P2u*P0v - Lp1*P1u*P2v + Lp1*P2u*P1v));
  }

  double level;
  if (Rp1 < 0) {
    level = (Rp1*dR + Rp2) / (dR + Rq1);
  } else {
    level = (Rp1*(1 - dR) + Rp2) / ((1 - dR) + Rq1);
  }

  return level;
}



/**
 * Internal getters and setters
 */

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
Luv getCie1976Ucs () {
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

  // Save values
  raw_.R = static_cast<float>(pwmR) / pwmRange_;
  raw_.G = static_cast<float>(pwmG) / pwmRange_;
  raw_.B = static_cast<float>(pwmB) / pwmRange_;

  // Cannot be sure that current color is result of higher level color setter
  // unset luv_ and T_, respective setters will save the values afterwards
  luv_.L = -1.0;
  luv_.u = -1.0;
  luv_.v = -1.0;
  T_ = -1;
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
void setCie1976Ucs (Luv target) {

  // Off -> exit
  if (!getOnOff()) return;

  // Convert to raw PWM values
  RGB raw;
  
  // Coefficients
  unsigned long startTime = millis();
  raw.R = findCoefficient(target, redUv_, greenUv_, blueUv_, redToGreenFit_, greenToBlueFit_);
  raw.G = findCoefficient(target, greenUv_, blueUv_, redUv_, greenToBlueFit_, blueToRedFit_);
  raw.B = findCoefficient(target, blueUv_, redUv_, greenUv_, blueToRedFit_, redToGreenFit_);
  unsigned long endTime = millis();
  unsigned long deltaTime = endTime - startTime;
  Serial.print("Duration: "); Serial.println(deltaTime);
  
  // Luma produced by the current raw values
  float Y = (raw.R * redLum_ + raw.G * greenLum_ + raw.B * blueLum_) / maxLum_;

  // Luma level needed for requested lightness
  float Y_target = pow(((target.L + 16) / 116), 3);
  
  // Luma factor
  float C = Y_target / Y;

  // Scale raw values to produce target luma
  raw.R = raw.R * C;
  raw.G = raw.G * C;
  raw.B = raw.B * C;

  // Find max scaled raw value
  float maxRaw = raw.R;
  if (raw.G > maxRaw) maxRaw = raw.G;
  if (raw.B > maxRaw) maxRaw = raw.B;

  // Nothing can be more than at max power, limit coefficients to 1
  if (maxRaw > 1) {
    raw.R = raw.R * (1.0 / maxRaw);
    raw.G = raw.G * (1.0 / maxRaw);
    raw.B = raw.B * (1.0 / maxRaw);
  }

  // Write PWMs
  setRaw(raw);

  // Save values
  luv_.L = target.L;
  luv_.u = target.u;
  luv_.v = target.v;
}

/**
 * Sets ligth by color temperature
 */
void setColorTemperature (float L, int T) {
  // Off -> exit
  if (!getOnOff()) return;
  
  // These cryptic looking formulas are a result of least RMSE fit of
  // CIE1976UCS coordinates vs color temperature
  
  // Fit variable has been transformed to z-score in order to avoid floating point precision problems
  double x = (T-5500.0)/2599.0;
  double u = (-0.0001747*pow(x,3.0) + 0.1833*pow(x,2.0) + 0.872*x + 1.227) / (pow(x,2.0) + 4.813*x + 5.933);
  double v = (0.000311*pow(x,4.0) + 0.0009124*pow(x,3.0) + 0.3856*pow(x,2.0) + 1.873*x + 2.619) / (pow(x,2.0) + 4.323*x + 5.485);

  // Construct CIE 1976 UCS values from internal lightness and newly calculated u', v' coordinates
  Luv luv = {luv_.L, u, v};
  
  // Lightness is given and valid
  if (L > 0) luv.L = L;
  
  setCie1976Ucs(luv);

  // Save color temperature
  T_ = T;
}



/**
 * HTTP API controllers
 */

/**
 * HTTP API for index.html
 */
void httpIndexController () {
  
  String html = FPSTR(HTML_INDEX);
  html.replace("'{{onOff_}}'", onOff_ ? "true" : "false");
  html.replace("'{{L_}}'", String(luv_.L));
  html.replace("'{{u_}}'", String(luv_.u));
  html.replace("'{{v_}}'", String(luv_.v));
  html.replace("'{{T_}}'", String(T_));
  html.replace("'{{debug}}'", "false");
  
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
  char response[40];
  sprintf(response, "{\n  \"led1\": %d,\n  \"led2\":%d\n}", led1, led2);

  server.send(200, "application/json", response);
}

/**
 * HTTP API for setting the light on
 */
void httpOnController () {
  setOnOff(true);
  
  // JSON response
  char response[40];
  sprintf(response, "{\n  \"onOff\": %s}", "true");

  // Send response
  server.send(200, "application/json", response);
}

/**
 * HTTP API for setting the light off
 */
 void httpOffController () {
  setOnOff(false);
  
  // JSON response
  char response[40];
  sprintf(response, "{\n  \"onOff\": %s\n}", "false");

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

  // All parameters must be given for write
  if (R >= 0 && G >= 0 && B >= 0) {
    // All parameters given -> set new values
    RGB raw = {R, G, B};
    setRaw(raw);
  }
  
  // Read (updated) values
  RGB raw = getRaw();

  // JSON response
  char response[60];
  char strR[10]; dtostrf(raw.R, 6, 4, strR);
  char strG[10]; dtostrf(raw.G, 6, 4, strG);
  char strB[10]; dtostrf(raw.B, 6, 4, strB);
  sprintf(response, "{\n  \"R\": %s,\n  \"G\": %s,\n  \"B\": %s\n}", strR, strG, strB);

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

  // All or none of the parameters must be missing
  if (L >= 0 && u >= 0 && v >= 0) {
    // All parameters given -> set new color
    Luv luv = {L, u, v};
    setCie1976Ucs(luv);
  }
  
  // Read (updated) color
  Luv luv = getCie1976Ucs();
  L = luv.L;
  u = luv.u;
  v = luv.v;

  // JSON response
  char response[60];
  char strL[10]; dtostrf(L, 6, 4, strL);
  char strU[10]; dtostrf(u, 6, 4, strU);
  char strV[10]; dtostrf(v, 6, 4, strV);
  sprintf(response, "{\n  \"L\": %s,\n  \"u\": %s,\n  \"v\": %s\n}", strL, strU, strV);

  // Send response
  server.send(200, "application/json", response);
}

/**
 * HTTP API for color temperature
 */
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
  if (T >= 1000 && L >= 0) {
    setColorTemperature(L, T);
  }

  // Read (updated) color temperature
  T = getColorTemperature();

  // Read (updated) lightness
  Luv luv = getCie1976Ucs();
  L = luv.L;

  // JSON response
  char response[60];
  char strL[10]; dtostrf(L, 6, 4, strL);
  sprintf(response, "{\n  \"L\": %s,\n  \"T\": %d\n}", strL, T);

  // Send response
  server.send(200, "application/json", response);
}

/**
 * HTTP 404 - Not foudn
 */
void httpNotFoundController () {
  server.send(404, "text/plain", "404");
}



/**
 * Setup
 */
void setup(void){
  analogWriteRange(pwmRange_);
  
  pinMode(led1Pin_, OUTPUT); digitalWrite(led1Pin_, HIGH);
  pinMode(led2Pin_, OUTPUT); digitalWrite(led2Pin_, HIGH);
  pinMode(redPin_, OUTPUT); analogWrite(redPin_, 0);
  pinMode(greenPin_, OUTPUT); analogWrite(greenPin_, 0);
  pinMode(bluePin_, OUTPUT); analogWrite(bluePin_, 0);
  pinMode(w1Pin_, OUTPUT); analogWrite(w1Pin_, 0);
  pinMode(w2Pin_, OUTPUT); analogWrite(w2Pin_, 0);
  
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

  Serial.begin(115200);

  // Set onboard leds off as a sign for STA mode
  setLed1(false);
  setLed2(false);

  // Set default to 2700K
  setOnOff(true);
  setColorTemperature(75, 1900);

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
