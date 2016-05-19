#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <cmath>
#include <EEPROM.h>

// GPIO settings
const int redPin_    = 15; //12
const int greenPin_  = 13; //15
const int bluePin_   = 12; //13
const int w1Pin_     = 14;
const int w2Pin_     = 4;
const int led1Pin_   = 5;
const int led2Pin_   = 1;
const int pwmRange_  = 1023;
const uint16_t calibrationAddress = 0;

// Index.html
// Minify, replace double quotes with single quotes, escape backlashes in regexes
//const char HTML_INDEX[] PROGMEM = "";
const char HTML_INDEX[] PROGMEM = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'><script src='https://cdn.linearicons.com/free/1.0.0/svgembedder.min.js'></script><style>.lnr{display: inline-block;fill: currentColor;width: 1em;height: 1em;vertical-align: -0.05em;font-size: 26px;}html{height: 100%;font-family: sans-serif;}body{background: #555555;margin: 0;height: 100%;padding: 8px;box-sizing: border-box;}.container{max-width: 800px;height: 100%;margin: 0 auto;}.layout-column, .layout-row{box-sizing: border-box;display: -webkit-flex;display: -ms-flexbox;display: flex;}.layout-row{-webkit-flex-direction: row;-ms-flex-direction: row;flex-direction: row;}.layout-row > *{margin-right: 8px;}.layout-row > *:last-child{margin-right: 0;}.layout-column{-webkit-flex-direction: column;-ms-flex-direction: column;flex-direction: column;}.layout-column > *{margin-bottom: 8px;}.layout-column > *:last-child{margin-bottom: 0;}.layout-align-center-center{-webkit-align-items: center;-ms-flex-align: center;align-items: center;-webkit-align-content: center;-ms-flex-line-pack: center;align-content: center;max-width: 100%;-webkit-justify-content: center;-ms-flex-pack: center;justify-content: center;}.flex{-webkit-flex: 1;-ms-flex: 1;flex: 1;box-sizing: border-box;}button{display: block;min-width: 48px; min-height: 48px;color: white;border: none;border-radius: 2px;}button:hover{cursor: pointer;}#settings-button{background-color: hsl(213, 100%, 60%);}#schedule-button{background-color: hsl(276, 100%, 40%);}#schedule-button.active{background-color: hsl(276, 100%, 70%);}#off-button{background-color: hsl(9, 100%, 25%);}#off-button.active{background-color: hsl(9, 100%, 60%);}#on-button{background-color: hsl(120, 100%, 20%);}#on-button.active{background-color: hsl(120, 100%, 50%);}.tile{background-color: rgba(0, 0, 0, 0.7);color: white;padding: 24px;border-radius: 2px;}.img{display: block;width: 100%;border-radius: 2px;position: relative;overflow: hidden;}.cursor{position: absolute;top: -10%; left: 50%;width: 6px; height: 6px;border-radius: 50%;transform: translate(-50%, -50%);background: black;}#dimming{width: 48px;background: linear-gradient(to bottom, #000000 5%, #ffffff 95%);}#dimming-cursor{background: red;}#color-temperature{background-repeat: repeat-x; background-size: contain; width: 48px;}#cie1976ucs{flex-shrink: 0;}#cie1976ucs-zoom{background-color: #333333;}#cie1976ucs-zoom > img{position: absolute;top: 50%; bottom: auto;left: 50%; right: auto;transform-origin: 0% 0%;transform: scale(2);}#settings{background-color: rgba(0, 0, 0, 0.54);position: absolute;top: 0; bottom: 0;left: 0; right: 0;}#settings.no-show{display: none;}#settings .tile{margin: 0 8px;}#settings h1{margin-top: 0;font-weight: normal;}#settings button{float: left; width: 50%;}#settings-save-button{background-color: hsl(213, 100%, 60%);border-top-left-radius: 0;border-bottom-left-radius: 0;}#settings-close-button{background-color: hsl(9, 100%, 25%);border-top-right-radius: 0;border-bottom-right-radius: 0;}#settings table{margin-bottom: 16px;}#settings tr.header th{padding-top: 16px;font-style: italic;font-weight: lighter;text-align: left;}#settings input[type='number']{max-width: 150px;width: 100%;border: none;border-radius: 2px;padding: 6px;box-sizing: border-box;}</style></head><body><div id='main-container' class='container layout-column'><div class='flex layout-row'><div id='dimming' class='img'><div id='dimming-cursor' class='cursor'></diV></div><div class='flex layout-column'><div id='cie1976ucs' class='img'><img class='img' src='https://raw.githubusercontent.com/jaakkopasanen/Tunable-White-LED/master/Matlab/img/CIE_1976_UCS_06_063_Full.jpg'><div id='cie1976ucs-cursor' class='cursor'></div></div><div id='cie1976ucs-zoom' class='img flex'><img id='cie1976ucs-zoom-image' class='img' src='https://raw.githubusercontent.com/jaakkopasanen/Tunable-White-LED/master/Matlab/img/CIE_1976_UCS_06_063_Full.jpg'></div></div><div id='color-temperature' class='img' style='background-image: url(https://raw.githubusercontent.com/jaakkopasanen/Tunable-White-LED/master/Matlab/img/cct_1000K_to_10000K_pow2_vertical.jpg);'><div id='color-temperature-cursor' class='cursor'></div></div></div><div class='layout-row'><button id='off-button' type='button' class='flex'><svg class='lnr'><use xlink:href='#lnr-moon'></use></svg></button><button id='schedule-button' type='button' class='flex'><svg class='lnr'><use xlink:href='#lnr-calendar-full'></use></svg></button><button id='settings-button' type='button' class='flex'><svg class='lnr'><use xlink:href='#lnr-cog'></use></svg></button><button id='on-button' type='button' class='flex'><svg class='lnr'><use xlink:href='#lnr-sun'></use></svg></button></div></div><div id='settings' class='layout-column layout-align-center-center no-show'><div class='tile'><table><tr><th></th><th>Red</th><th>Green</th><th>Blue</th></tr><tr class='header'><th colspan='4'>CIE 1976 UCS</th></tr><tr><th>u'</th><td><input id='red-u' type='number' step='0.0001' value=''></td><td><input id='green-u' type='number' step='0.0001' value=''></td><td><input id='blue-u' type='number' step='0.0001' value=''></td></tr><tr><th>v'</th><td><input id='red-v' type='number' step='0.0001' value=''></td><td><input id='green-v' type='number' step='0.0001' value=''></td><td><input id='blue-v' type='number' step='0.0001' value=''></td></tr><tr class='header'><th colspan='4'>Luminous flux</th></tr><tr><th>lm</th><td><input id='red-lum' type='number' step='0.0001' value=''></td><td><input id='green-lum' type='number' step='0.0001' value=''></td><td><input id='blue-lum' type='number' step='0.0001' value=''></td></tr><tr class='header'><th colspan='4'>Level by distance</th></tr><tr><th>p1</th><td><input id='red-p1' type='number' step='0.0001' value=''></td><td><input id='green-p1' type='number' step='0.0001' value=''></td><td><input id='blue-p1' type='number' step='0.0001' value=''></td></tr><tr><th>p2</th><td><input id='red-p2' type='number' step='0.0001' value=''></td><td><input id='green-p2' type='number' step='0.0001' value=''></td><td><input id='blue-p2' type='number' step='0.0001' value=''></td></tr><tr><th>q1</th><td><input id='red-q1' type='number' step='0.0001' value=''></td><td><input id='green-q1' type='number' step='0.0001' value=''></td><td><input id='blue-q1' type='number' step='0.0001' value=''></td></tr></table><button id='settings-close-button' type='button'><svg class='lnr'><use xlink:href='#lnr-cross'></use></svg></button><button id='settings-save-button' type='button'><svg class='lnr'><use xlink:href='#lnr-checkmark-circle'></use></svg></button></div></div></body><script type='text/javascript'>var zoomLevel=4;var debug='{{debug}}';var mainContainer=document.getElementById('main-container');var onButton=document.getElementById('on-button');var offButton=document.getElementById('off-button');var scheduleButton=document.getElementById('schedule-button');var dimming=document.getElementById('dimming');var dimmingCursor=document.getElementById('dimming-cursor');var cie1976Ucs=document.getElementById('cie1976ucs');var cie1976UcsCursor=document.getElementById('cie1976ucs-cursor');var cie1976UcsZoom=document.getElementById('cie1976ucs-zoom');var cie1976UcsZoomImage=document.getElementById('cie1976ucs-zoom-image');var colorTemperature=document.getElementById('color-temperature');var colorTemperatureCursor=document.getElementById('color-temperature-cursor');var settings=document.getElementById('settings');var settingsButton=document.getElementById('settings-button');var settingsCloseButton=document.getElementById('settings-close-button');var settingsSaveButton=document.getElementById('settings-save-button');var redU=document.getElementById('red-u');var greenU=document.getElementById('green-u');var blueU=document.getElementById('blue-u');var redV=document.getElementById('red-v');var greenV=document.getElementById('green-v');var blueV=document.getElementById('blue-v');var redLum=document.getElementById('red-lum');var greenLum=document.getElementById('green-lum');var blueLum=document.getElementById('blue-lum');var redP1=document.getElementById('red-p1');var greenP1=document.getElementById('green-p1');var blueP1=document.getElementById('blue-p1');var redP2=document.getElementById('red-p2');var greenP2=document.getElementById('green-p2');var blueP2=document.getElementById('blue-p2');var redQ1=document.getElementById('red-q1');var greenQ1=document.getElementById('green-q1');var blueQ1=document.getElementById('blue-q1');var ajax=function (url, callback){console.log('AJAX:', url);if (debug) return;var xhr=new XMLHttpRequest();xhr.open('GET', url, true);xhr.send();xhr.onreadystatechange=function (){if (xhr.readyState===4){if (xhr.status===200){if (callback){callback(xhr.responseText);}}else{console.log('Error:', xhr.responseText);}}};};var setOnOff=function (on, doAjax){if (on){onButton.className +=' active';offButton.className=offButton.className.replace( /(?:^|\\s)active(?!\\S)/g , '' );if (doAjax) ajax('/on');}else{offButton.className +=' active';onButton.className=onButton.className.replace( /(?:^|\\s)active(?!\\S)/g , '' );if (doAjax) ajax('/off');}};onButton.addEventListener('click', function (){setOnOff(true, true);});offButton.addEventListener('click', function (){setOnOff(false, true);});var openSettings=function (){settings.className=settings.className.replace( /(?:^|\\s)no-show(?!\\S)/g , '' );mainContainer.style['-webkit-filter']='blur(5px)';mainContainer.style['filter']='blur(5px)';};var saveSettings=function (){var url='/calibrate?';url +='redU=' + redU.value;url +='&redV=' + redV.value;url +='&greenU=' + greenU.value;url +='&greenV=' + greenV.value;url +='&blueU=' + blueU.value;url +='&blueV=' + blueV.value;url +='&redLum=' + redLum.value;url +='&greenLum=' + greenLum.value;url +='&blueLum=' + blueLum.value;url +='&redP1=' + redP1.value;url +='&redP2=' + redP2.value;url +='&redQ1=' + redQ1.value;url +='&greenP1=' + greenP1.value;url +='&greenP2=' + greenP2.value;url +='&greenQ1=' + greenQ1.value;url +='&blueP1=' + blueP1.value;url +='&blueP2=' + blueP2.value;url +='&blueQ1=' + blueQ1.value;ajax(url);};var closeSettings=function (){settings.className +=' no-show';mainContainer.style['-webkit-filter']='';mainContainer.style['filter']='';};settingsButton.addEventListener('click', openSettings);settingsSaveButton.addEventListener('click', saveSettings);settingsCloseButton.addEventListener('click', closeSettings);var scheduleOn=function (){if (scheduleButton.className.indexOf('active')===-1){scheduleButton.className +=' active';}};var scheduleOff=function (){scheduleButton.className=onButton.className.replace( /(?:^|\\s)active(?!\\S)/g , '' );};scheduleButton.addEventListener('click', function (ev){scheduleOn();});var setDimming=function (L, doAjax){L_=L;dimmingCursor.style.top=(L/100 * 90 + 5) + '%';if (doAjax){if (mode==='color'){ajax('/cie1976Ucs?L=' + L_ + '&u=' + u_ + '&v=' + v_);}else{ajax('/colorTemperature?L=' + L_ + '&T=' + T_);}}};dimming.addEventListener('click', function (ev){var y=(ev.pageY - dimming.offsetTop) / dimming.offsetHeight;y=(y - 0.05) / 0.9;y=Math.min(y, 1);y=Math.max(y, 0);var L=y * 100;setDimming(L, true);});var setCie1976Ucs=function (u, v, doAjax){u_=u;v_=v;mode='color';cie1976UcsCursor.style.left=(u / 0.63 * 100) + '%';cie1976UcsCursor.style.top=((1 - v / 0.6) * 100) + '%';x=-(u / 0.63) * cie1976Ucs.offsetWidth;y=-(1 - v / 0.6) * cie1976Ucs.offsetHeight;cie1976UcsZoomImage.style.transform='scale(' + zoomLevel + ') translate(' + x + 'px, ' + y + 'px)';unsetColorTemperature();scheduleOff();if (doAjax) ajax('/cie1976Ucs?L=' + L_ + '&u=' + u + '&v=' + v);};var unsetCie1976Ucs=function (){cie1976UcsCursor.style.left='-10%';cie1976UcsCursor.style.top='-10%';};cie1976Ucs.addEventListener('click', function (ev){var u=(ev.pageX - cie1976Ucs.offsetLeft) / cie1976Ucs.offsetWidth * 0.63;var v=(1 - (ev.pageY - cie1976Ucs.offsetTop) / cie1976Ucs.offsetHeight) * 0.6;setCie1976Ucs(u, v, true);});cie1976UcsZoom.addEventListener('click', function (ev){var dx=((ev.pageX - cie1976UcsZoom.offsetLeft) / cie1976UcsZoom.offsetWidth - 0.5);var dy=((ev.pageY - cie1976UcsZoom.offsetTop) / cie1976UcsZoom.offsetHeight - 0.5);var dxpx=dx * cie1976UcsZoom.offsetWidth;var dypx=dy * cie1976UcsZoom.offsetHeight;var du=dxpx / cie1976Ucs.offsetWidth * 0.63 / zoomLevel;var dv=dypx / cie1976Ucs.offsetHeight * 0.6 / zoomLevel;setCie1976Ucs(u_ + du, v_ - dv, true);});var setColorTemperature=function (T, doAjax){T_=T;mode='temperature';colorTemperatureCursor.style.top=((1 - Math.pow(((T - 1000) / 9000),0.5)) * 100) + '%';unsetCie1976Ucs();scheduleOff();if (doAjax) ajax('/colorTemperature?L=' + L_ + '&T=' + T_);};var unsetColorTemperature=function (){colorTemperatureCursor.style.top='-10%';};colorTemperature.addEventListener('click', function (ev){var y=(ev.pageY - colorTemperature.offsetTop) / colorTemperature.offsetHeight;var T=Math.pow((1-y), 2) * 9000 + 1000;setColorTemperature(T, true);});window.onload=function (){if (debug){var onOff_=true;var L_=75;var u_=0.199;var v_=0.471;var T_=-1;redU.value=0.5535;greenU.value=0.0373;blueU.value=0.1679;redV.value=0.5170;greenV.value=0.5856;blueV.value=0.1153;redLum.value=160;greenLum.value=320;blueLum.value=240;redP1.value=2.9658;greenP1.value=1.3587;blueP1.value=-0.2121;redP2.value=0.0;greenP2.value=0.0;blueP2.value=0.2121;redQ1.value=1.9658;greenQ1.value=0.3587;blueQ1.value=0.2121;}else{var onOff_='{{onOff}}';var L_='{{L}}';var u_='{{u}}';var v_='{{v}}';var T_='{{T}}';redU.value='{{redU}}';greenU.value='{{greenU}}';blueU.value='{{blueU}}';redV.value='{{redV}}';greenV.value='{{greenV}}';blueV.value='{{blueV}}';redLum.value='{{redLum}}';greenLum.value='{{greenLum}}';blueLum.value='{{blueLum}}';redP1.value='{{redP1}}';greenP1.value='{{greenP1}}';blueP1.value='{{blueP1}}';redP2.value='{{redP2}}';greenP2.value='{{greenP2}}';blueP2.value='{{blueP2}}';redQ1.value='{{redQ1}}';greenQ1.value='{{greenQ1}}';blueQ1.value='{{blueQ1}}';}var mode;if (T_ > 0){mode='temperature';setColorTemperature(T_, false);}else{mode='color';setCie1976Ucs(u_, v_, false);}setOnOff(onOff_, false);setDimming(L_, false);};</script></html>";

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

struct Calibration {
  // Luminous fluxes
  float redLum;
  float greenLum;
  float blueLum;
  float maxLum;
  
  // LED u', v' coordinates
  Luv redUv;
  Luv greenUv;
  Luv blueUv;
  
  // Mixing fit coefficients
  float redToGreenFit[3];
  float greenToBlueFit[3];
  float blueToRedFit[3];

  // Checksum
  uint32_t crc32;
};

// Is the light on?
bool onOff_;

// Raw PWM values
RGB raw_;

// CIE 1976 UCS color coordinates
Luv luv_;

// Correlated color temperature (in Kelvins)
int T_;

// Calibration data
Calibration cal_;

/**
 * Utility functions
 */

/**
 * Find coefficient for a LED
 */
float findCoefficient (const Luv PT, const Luv P0, const Luv P1, const Luv P2, const float rightHandFit[3], const float leftHandFit[3]) {

  float PTu = PT.u;
  float PTv = PT.v;
  float P0u = P0.u;
  float P0v = P0.v;
  float P1u = P1.u;
  float P1v = P1.v;
  float P2u = P2.u;
  float P2v = P2.v;
  float Rp1 = rightHandFit[0];
  float Rp2 = rightHandFit[1];
  float Rq1 = rightHandFit[2];
  float Lp1 = leftHandFit[0];
  float Lp2 = leftHandFit[1];
  float Lq1 = leftHandFit[2];

  float dR;
  if (Lp1 < 0) {
    dR = (sqrt((P0u*P0u)*(P1v*P1v)+(P1u*P1u)*(P0v*P0v)+(P0u*P0u)*(PTv*PTv)+(P0v*P0v)*(PTu*PTu)+(P1u*P1u)*(PTv*PTv)+(P1v*P1v)*(PTu*PTu)-Lp1*(P0u*P0u)*(P1v*P1v)*2.0-Lp1*(P1u*P1u)*(P0v*P0v)*2.0-Lp2*(P0u*P0u)*(P1v*P1v)*2.0-Lp2*(P1u*P1u)*(P0v*P0v)*2.0+Lq1*(P0u*P0u)*(P1v*P1v)*2.0+Lq1*(P1u*P1u)*(P0v*P0v)*2.0-Lp1*(P0u*P0u)*(PTv*PTv)*2.0-Lp1*(P0v*P0v)*(PTu*PTu)*2.0-Lp2*(P0u*P0u)*(PTv*PTv)*4.0-Lp2*(P0v*P0v)*(PTu*PTu)*4.0+Lq1*(P0u*P0u)*(PTv*PTv)*2.0+Lq1*(P0v*P0v)*(PTu*PTu)*2.0+Lq1*(P1u*P1u)*(PTv*PTv)*2.0+Lq1*(P1v*P1v)*(PTu*PTu)*2.0+(Lp1*Lp1)*(P0u*P0u)*(P1v*P1v)+(Lp1*Lp1)*(P1u*P1u)*(P0v*P0v)+(Lp2*Lp2)*(P0u*P0u)*(P1v*P1v)+(Lp2*Lp2)*(P1u*P1u)*(P0v*P0v)+(Lp1*Lp1)*(P1u*P1u)*(P2v*P2v)+(Lp1*Lp1)*(P2u*P2u)*(P1v*P1v)+(Lp2*Lp2)*(P0u*P0u)*(P2v*P2v)+(Lp2*Lp2)*(P2u*P2u)*(P0v*P0v)+(Lp2*Lp2)*(P1u*P1u)*(P2v*P2v)+(Lp2*Lp2)*(P2u*P2u)*(P1v*P1v)+(Lq1*Lq1)*(P0u*P0u)*(P1v*P1v)+(Lq1*Lq1)*(P1u*P1u)*(P0v*P0v)+(Lp1*Lp1)*(P0u*P0u)*(PTv*PTv)+(Lp1*Lp1)*(P0v*P0v)*(PTu*PTu)+(Lp1*Lp1)*(P2u*P2u)*(PTv*PTv)+(Lp1*Lp1)*(P2v*P2v)*(PTu*PTu)+(Lq1*Lq1)*(P0u*P0u)*(PTv*PTv)+(Lq1*Lq1)*(P0v*P0v)*(PTu*PTu)+(Lq1*Lq1)*(P1u*P1u)*(PTv*PTv)+(Lq1*Lq1)*(P1v*P1v)*(PTu*PTu)-P0u*P1u*(PTv*PTv)*2.0-P0u*(P1v*P1v)*PTu*2.0-P1u*(P0v*P0v)*PTu*2.0-P0v*P1v*(PTu*PTu)*2.0-(P0u*P0u)*P1v*PTv*2.0-(P1u*P1u)*P0v*PTv*2.0+Lp1*Lp2*(P0u*P0u)*(P1v*P1v)*2.0+Lp1*Lp2*(P1u*P1u)*(P0v*P0v)*2.0+Lp1*Lp2*(P1u*P1u)*(P2v*P2v)*2.0+Lp1*Lp2*(P2u*P2u)*(P1v*P1v)*2.0-Lp1*Lq1*(P0u*P0u)*(P1v*P1v)*2.0-Lp1*Lq1*(P1u*P1u)*(P0v*P0v)*2.0-Lp2*Lq1*(P0u*P0u)*(P1v*P1v)*2.0-Lp2*Lq1*(P1u*P1u)*(P0v*P0v)*2.0+Lp1*Lq1*(P0u*P0u)*(PTv*PTv)*2.0+Lp1*Lq1*(P0v*P0v)*(PTu*PTu)*2.0-(Lp1*Lp1)*P0u*P2u*(P1v*P1v)*2.0-(Lp2*Lp2)*P0u*P1u*(P2v*P2v)*2.0-(Lp2*Lp2)*P0u*P2u*(P1v*P1v)*2.0-(Lp2*Lp2)*P1u*P2u*(P0v*P0v)*2.0-(Lp1*Lp1)*(P1u*P1u)*P0v*P2v*2.0-(Lp2*Lp2)*(P0u*P0u)*P1v*P2v*2.0-(Lp2*Lp2)*(P1u*P1u)*P0v*P2v*2.0-(Lp2*Lp2)*(P2u*P2u)*P0v*P1v*2.0-(Lp1*Lp1)*P1u*(P0v*P0v)*PTu*2.0-(Lp1*Lp1)*P0u*P2u*(PTv*PTv)*2.0-(Lp1*Lp1)*P1u*(P2v*P2v)*PTu*2.0-(Lp1*Lp1)*(P0u*P0u)*P1v*PTv*2.0-(Lp1*Lp1)*P0v*P2v*(PTu*PTu)*2.0-(Lp1*Lp1)*(P2u*P2u)*P1v*PTv*2.0-(Lq1*Lq1)*P0u*P1u*(PTv*PTv)*2.0-(Lq1*Lq1)*P0u*(P1v*P1v)*PTu*2.0-(Lq1*Lq1)*P1u*(P0v*P0v)*PTu*2.0-(Lq1*Lq1)*P0v*P1v*(PTu*PTu)*2.0-(Lq1*Lq1)*(P0u*P0u)*P1v*PTv*2.0-(Lq1*Lq1)*(P1u*P1u)*P0v*PTv*2.0-P0u*P1u*P0v*P1v*2.0+P0u*P1u*P0v*PTv*2.0+P0u*P0v*P1v*PTu*2.0+P0u*P1u*P1v*PTv*2.0+P1u*P0v*P1v*PTu*2.0-P0u*P0v*PTu*PTv*2.0+P0u*P1v*PTu*PTv*2.0+P1u*P0v*PTu*PTv*2.0-P1u*P1v*PTu*PTv*2.0+Lp1*P0u*P2u*(P1v*P1v)*2.0+Lp2*P0u*P2u*(P1v*P1v)*2.0-Lp2*P1u*P2u*(P0v*P0v)*2.0+Lp1*(P1u*P1u)*P0v*P2v*2.0-Lp2*(P0u*P0u)*P1v*P2v*2.0+Lp2*(P1u*P1u)*P0v*P2v*2.0+Lp1*P0u*P1u*(PTv*PTv)*2.0+Lp1*P0u*(P1v*P1v)*PTu*2.0+Lp1*P1u*(P0v*P0v)*PTu*4.0+Lp1*P0u*P2u*(PTv*PTv)*2.0+Lp2*P0u*P1u*(PTv*PTv)*4.0+Lp2*P0u*(P1v*P1v)*PTu*2.0+Lp2*P1u*(P0v*P0v)*PTu*6.0-Lp1*P1u*P2u*(PTv*PTv)*2.0-Lp1*P2u*(P1v*P1v)*PTu*2.0+Lp2*P0u*P2u*(PTv*PTv)*4.0+Lp2*P2u*(P0v*P0v)*PTu*2.0-Lp2*P1u*P2u*(PTv*PTv)*4.0-Lp2*P2u*(P1v*P1v)*PTu*2.0+Lp1*P0v*P1v*(PTu*PTu)*2.0+Lp1*(P0u*P0u)*P1v*PTv*4.0+Lp1*(P1u*P1u)*P0v*PTv*2.0+Lp1*P0v*P2v*(PTu*PTu)*2.0+Lp2*P0v*P1v*(PTu*PTu)*4.0+Lp2*(P0u*P0u)*P1v*PTv*6.0+Lp2*(P1u*P1u)*P0v*PTv*2.0-Lp1*P1v*P2v*(PTu*PTu)*2.0-Lp1*(P1u*P1u)*P2v*PTv*2.0+Lp2*P0v*P2v*(PTu*PTu)*4.0+Lp2*(P0u*P0u)*P2v*PTv*2.0-Lp2*P1v*P2v*(PTu*PTu)*4.0-Lp2*(P1u*P1u)*P2v*PTv*2.0-Lq1*P0u*P1u*(PTv*PTv)*4.0-Lq1*P0u*(P1v*P1v)*PTu*4.0-Lq1*P1u*(P0v*P0v)*PTu*4.0-Lq1*P0v*P1v*(PTu*PTu)*4.0-Lq1*(P0u*P0u)*P1v*PTv*4.0-Lq1*(P1u*P1u)*P0v*PTv*4.0-Lp1*Lp2*P0u*P1u*(P2v*P2v)*2.0-Lp1*Lp2*P0u*P2u*(P1v*P1v)*4.0-Lp1*Lp2*P1u*P2u*(P0v*P0v)*2.0-Lp1*Lp2*(P0u*P0u)*P1v*P2v*2.0-Lp1*Lp2*(P1u*P1u)*P0v*P2v*4.0-Lp1*Lp2*(P2u*P2u)*P0v*P1v*2.0+Lp1*Lq1*P0u*P2u*(P1v*P1v)*2.0+Lp1*Lq1*P1u*P2u*(P0v*P0v)*4.0+Lp2*Lq1*P0u*P2u*(P1v*P1v)*2.0+Lp2*Lq1*P1u*P2u*(P0v*P0v)*2.0+Lp1*Lq1*(P0u*P0u)*P1v*P2v*4.0+Lp1*Lq1*(P1u*P1u)*P0v*P2v*2.0+Lp2*Lq1*(P0u*P0u)*P1v*P2v*2.0+Lp2*Lq1*(P1u*P1u)*P0v*P2v*2.0-Lp1*Lp2*P1u*(P0v*P0v)*PTu*2.0+Lp1*Lp2*P0u*(P2v*P2v)*PTu*2.0+Lp1*Lp2*P2u*(P0v*P0v)*PTu*2.0-Lp1*Lp2*P1u*(P2v*P2v)*PTu*2.0-Lp1*Lp2*(P0u*P0u)*P1v*PTv*2.0+Lp1*Lp2*(P0u*P0u)*P2v*PTv*2.0+Lp1*Lp2*(P2u*P2u)*P0v*PTv*2.0-Lp1*Lp2*(P2u*P2u)*P1v*PTv*2.0-Lp1*Lq1*P0u*P1u*(PTv*PTv)*2.0+Lp1*Lq1*P0u*(P1v*P1v)*PTu*2.0-Lp1*Lq1*P0u*P2u*(PTv*PTv)*2.0-Lp1*Lq1*P2u*(P0v*P0v)*PTu*4.0+Lp2*Lq1*P0u*(P1v*P1v)*PTu*2.0+Lp2*Lq1*P1u*(P0v*P0v)*PTu*2.0+Lp1*Lq1*P1u*P2u*(PTv*PTv)*2.0-Lp1*Lq1*P2u*(P1v*P1v)*PTu*2.0-Lp2*Lq1*P2u*(P0v*P0v)*PTu*2.0-Lp2*Lq1*P2u*(P1v*P1v)*PTu*2.0-Lp1*Lq1*P0v*P1v*(PTu*PTu)*2.0+Lp1*Lq1*(P1u*P1u)*P0v*PTv*2.0-Lp1*Lq1*P0v*P2v*(PTu*PTu)*2.0-Lp1*Lq1*(P0u*P0u)*P2v*PTv*4.0+Lp2*Lq1*(P0u*P0u)*P1v*PTv*2.0+Lp2*Lq1*(P1u*P1u)*P0v*PTv*2.0+Lp1*Lq1*P1v*P2v*(PTu*PTu)*2.0-Lp1*Lq1*(P1u*P1u)*P2v*PTv*2.0-Lp2*Lq1*(P0u*P0u)*P2v*PTv*2.0-Lp2*Lq1*(P1u*P1u)*P2v*PTv*2.0-(Lp1*Lp1)*P0u*P1u*P0v*P1v*2.0-(Lp2*Lp2)*P0u*P1u*P0v*P1v*2.0+(Lp1*Lp1)*P0u*P1u*P1v*P2v*2.0+(Lp1*Lp1)*P1u*P2u*P0v*P1v*2.0+(Lp2*Lp2)*P0u*P1u*P0v*P2v*2.0+(Lp2*Lp2)*P0u*P2u*P0v*P1v*2.0+(Lp2*Lp2)*P0u*P1u*P1v*P2v*2.0-(Lp2*Lp2)*P0u*P2u*P0v*P2v*2.0+(Lp2*Lp2)*P1u*P2u*P0v*P1v*2.0-(Lp1*Lp1)*P1u*P2u*P1v*P2v*2.0+(Lp2*Lp2)*P0u*P2u*P1v*P2v*2.0+(Lp2*Lp2)*P1u*P2u*P0v*P2v*2.0-(Lp2*Lp2)*P1u*P2u*P1v*P2v*2.0-(Lq1*Lq1)*P0u*P1u*P0v*P1v*2.0+(Lp1*Lp1)*P0u*P1u*P0v*PTv*2.0+(Lp1*Lp1)*P0u*P0v*P1v*PTu*2.0-(Lp1*Lp1)*P0u*P1u*P2v*PTv*2.0+(Lp1*Lp1)*P0u*P2u*P1v*PTv*4.0-(Lp1*Lp1)*P0u*P1v*P2v*PTu*2.0-(Lp1*Lp1)*P1u*P2u*P0v*PTv*2.0+(Lp1*Lp1)*P1u*P0v*P2v*PTu*4.0-(Lp1*Lp1)*P2u*P0v*P1v*PTu*2.0+(Lp1*Lp1)*P1u*P2u*P2v*PTv*2.0+(Lp1*Lp1)*P2u*P1v*P2v*PTu*2.0+(Lq1*Lq1)*P0u*P1u*P0v*PTv*2.0+(Lq1*Lq1)*P0u*P0v*P1v*PTu*2.0+(Lq1*Lq1)*P0u*P1u*P1v*PTv*2.0+(Lq1*Lq1)*P1u*P0v*P1v*PTu*2.0-(Lp1*Lp1)*P0u*P0v*PTu*PTv*2.0+(Lp1*Lp1)*P0u*P2v*PTu*PTv*2.0+(Lp1*Lp1)*P2u*P0v*PTu*PTv*2.0-(Lp1*Lp1)*P2u*P2v*PTu*PTv*2.0-(Lq1*Lq1)*P0u*P0v*PTu*PTv*2.0+(Lq1*Lq1)*P0u*P1v*PTu*PTv*2.0+(Lq1*Lq1)*P1u*P0v*PTu*PTv*2.0-(Lq1*Lq1)*P1u*P1v*PTu*PTv*2.0+Lp1*P0u*P1u*P0v*P1v*4.0+Lp2*P0u*P1u*P0v*P1v*4.0-Lp1*P0u*P1u*P1v*P2v*2.0-Lp1*P1u*P2u*P0v*P1v*2.0+Lp2*P0u*P1u*P0v*P2v*2.0+Lp2*P0u*P2u*P0v*P1v*2.0-Lp2*P0u*P1u*P1v*P2v*2.0-Lp2*P1u*P2u*P0v*P1v*2.0-Lq1*P0u*P1u*P0v*P1v*4.0-Lp1*P0u*P1u*P0v*PTv*4.0-Lp1*P0u*P0v*P1v*PTu*4.0-Lp1*P0u*P1u*P1v*PTv*2.0-Lp1*P1u*P0v*P1v*PTu*2.0-Lp2*P0u*P1u*P0v*PTv*6.0-Lp2*P0u*P0v*P1v*PTu*6.0+Lp1*P0u*P1u*P2v*PTv*2.0-Lp1*P0u*P2u*P1v*PTv*4.0+Lp1*P0u*P1v*P2v*PTu*2.0+Lp1*P1u*P2u*P0v*PTv*2.0-Lp1*P1u*P0v*P2v*PTu*4.0+Lp1*P2u*P0v*P1v*PTu*2.0-Lp2*P0u*P1u*P1v*PTv*2.0-Lp2*P0u*P2u*P0v*PTv*2.0-Lp2*P0u*P0v*P2v*PTu*2.0-Lp2*P1u*P0v*P1v*PTu*2.0+Lp1*P1u*P2u*P1v*PTv*2.0+Lp1*P1u*P1v*P2v*PTu*2.0-Lp2*P0u*P2u*P1v*PTv*6.0+Lp2*P0u*P1v*P2v*PTu*6.0+Lp2*P1u*P2u*P0v*PTv*6.0-Lp2*P1u*P0v*P2v*PTu*6.0+Lp2*P1u*P2u*P1v*PTv*2.0+Lp2*P1u*P1v*P2v*PTu*2.0+Lq1*P0u*P1u*P0v*PTv*4.0+Lq1*P0u*P0v*P1v*PTu*4.0+Lq1*P0u*P1u*P1v*PTv*4.0+Lq1*P1u*P0v*P1v*PTu*4.0+Lp1*P0u*P0v*PTu*PTv*4.0-Lp1*P0u*P1v*PTu*PTv*2.0-Lp1*P1u*P0v*PTu*PTv*2.0+Lp2*P0u*P0v*PTu*PTv*8.0-Lp1*P0u*P2v*PTu*PTv*2.0-Lp1*P2u*P0v*PTu*PTv*2.0-Lp2*P0u*P1v*PTu*PTv*4.0-Lp2*P1u*P0v*PTu*PTv*4.0+Lp1*P1u*P2v*PTu*PTv*2.0+Lp1*P2u*P1v*PTu*PTv*2.0-Lp2*P0u*P2v*PTu*PTv*4.0-Lp2*P2u*P0v*PTu*PTv*4.0+Lp2*P1u*P2v*PTu*PTv*4.0+Lp2*P2u*P1v*PTu*PTv*4.0-Lq1*P0u*P0v*PTu*PTv*4.0+Lq1*P0u*P1v*PTu*PTv*4.0+Lq1*P1u*P0v*PTu*PTv*4.0-Lq1*P1u*P1v*PTu*PTv*4.0-Lp1*Lp2*P0u*P1u*P0v*P1v*4.0+Lp1*Lp2*P0u*P1u*P0v*P2v*2.0+Lp1*Lp2*P0u*P2u*P0v*P1v*2.0+Lp1*Lp2*P0u*P1u*P1v*P2v*4.0+Lp1*Lp2*P1u*P2u*P0v*P1v*4.0+Lp1*Lp2*P0u*P2u*P1v*P2v*2.0+Lp1*Lp2*P1u*P2u*P0v*P2v*2.0-Lp1*Lp2*P1u*P2u*P1v*P2v*4.0+Lp1*Lq1*P0u*P1u*P0v*P1v*4.0-Lp1*Lq1*P0u*P1u*P0v*P2v*4.0-Lp1*Lq1*P0u*P2u*P0v*P1v*4.0+Lp2*Lq1*P0u*P1u*P0v*P1v*4.0-Lp1*Lq1*P0u*P1u*P1v*P2v*2.0-Lp1*Lq1*P1u*P2u*P0v*P1v*2.0-Lp2*Lq1*P0u*P1u*P0v*P2v*2.0-Lp2*Lq1*P0u*P2u*P0v*P1v*2.0-Lp2*Lq1*P0u*P1u*P1v*P2v*2.0-Lp2*Lq1*P1u*P2u*P0v*P1v*2.0+Lp1*Lp2*P0u*P1u*P0v*PTv*2.0+Lp1*Lp2*P0u*P0v*P1v*PTu*2.0-Lp1*Lp2*P0u*P2u*P0v*PTv*2.0-Lp1*Lp2*P0u*P0v*P2v*PTu*2.0-Lp1*Lp2*P0u*P1u*P2v*PTv*2.0+Lp1*Lp2*P0u*P2u*P1v*PTv*4.0-Lp1*Lp2*P0u*P1v*P2v*PTu*2.0-Lp1*Lp2*P1u*P2u*P0v*PTv*2.0+Lp1*Lp2*P1u*P0v*P2v*PTu*4.0-Lp1*Lp2*P2u*P0v*P1v*PTu*2.0-Lp1*Lp2*P0u*P2u*P2v*PTv*2.0-Lp1*Lp2*P2u*P0v*P2v*PTu*2.0+Lp1*Lp2*P1u*P2u*P2v*PTv*2.0+Lp1*Lp2*P2u*P1v*P2v*PTu*2.0-Lp1*Lq1*P0u*P1u*P1v*PTv*2.0+Lp1*Lq1*P0u*P2u*P0v*PTv*4.0+Lp1*Lq1*P0u*P0v*P2v*PTu*4.0-Lp1*Lq1*P1u*P0v*P1v*PTu*2.0-Lp2*Lq1*P0u*P1u*P0v*PTv*2.0-Lp2*Lq1*P0u*P0v*P1v*PTu*2.0+Lp1*Lq1*P0u*P1u*P2v*PTv*6.0-Lp1*Lq1*P0u*P1v*P2v*PTu*6.0-Lp1*Lq1*P1u*P2u*P0v*PTv*6.0+Lp1*Lq1*P2u*P0v*P1v*PTu*6.0-Lp2*Lq1*P0u*P1u*P1v*PTv*2.0+Lp2*Lq1*P0u*P2u*P0v*PTv*2.0+Lp2*Lq1*P0u*P0v*P2v*PTu*2.0-Lp2*Lq1*P1u*P0v*P1v*PTu*2.0+Lp1*Lq1*P1u*P2u*P1v*PTv*2.0+Lp1*Lq1*P1u*P1v*P2v*PTu*2.0+Lp2*Lq1*P0u*P1u*P2v*PTv*4.0-Lp2*Lq1*P0u*P2u*P1v*PTv*2.0-Lp2*Lq1*P0u*P1v*P2v*PTu*2.0-Lp2*Lq1*P1u*P2u*P0v*PTv*2.0-Lp2*Lq1*P1u*P0v*P2v*PTu*2.0+Lp2*Lq1*P2u*P0v*P1v*PTu*4.0+Lp2*Lq1*P1u*P2u*P1v*PTv*2.0+Lp2*Lq1*P1u*P1v*P2v*PTu*2.0-Lp1*Lq1*P0u*P0v*PTu*PTv*4.0+Lp1*Lq1*P0u*P1v*PTu*PTv*2.0+Lp1*Lq1*P1u*P0v*PTu*PTv*2.0+Lp1*Lq1*P0u*P2v*PTu*PTv*2.0+Lp1*Lq1*P2u*P0v*PTu*PTv*2.0-Lp1*Lq1*P1u*P2v*PTu*PTv*2.0-Lp1*Lq1*P2u*P1v*PTu*PTv*2.0)*(1.0/2.0)+P0u*P1v*(1.0/2.0)-P1u*P0v*(1.0/2.0)-P0u*PTv*(1.0/2.0)+P0v*PTu*(1.0/2.0)+P1u*PTv*(1.0/2.0)-P1v*PTu*(1.0/2.0)-Lp1*P0u*P1v*(1.0/2.0)+Lp1*P1u*P0v*(1.0/2.0)+Lp1*P0u*P2v-Lp1*P2u*P0v-Lp2*P0u*P1v*(1.0/2.0)+Lp2*P1u*P0v*(1.0/2.0)-Lp1*P1u*P2v*(1.0/2.0)+Lp1*P2u*P1v*(1.0/2.0)+Lp2*P0u*P2v*(1.0/2.0)-Lp2*P2u*P0v*(1.0/2.0)-Lp2*P1u*P2v*(1.0/2.0)+Lp2*P2u*P1v*(1.0/2.0)+Lq1*P0u*P1v*(1.0/2.0)-Lq1*P1u*P0v*(1.0/2.0)-Lp1*P0u*PTv*(1.0/2.0)+Lp1*P0v*PTu*(1.0/2.0)+Lp1*P2u*PTv*(1.0/2.0)-Lp1*P2v*PTu*(1.0/2.0)-Lq1*P0u*PTv*(1.0/2.0)+Lq1*P0v*PTu*(1.0/2.0)+Lq1*P1u*PTv*(1.0/2.0)-Lq1*P1v*PTu*(1.0/2.0))/(P0u*P1v-P1u*P0v-P0u*PTv+P0v*PTu+P1u*PTv-P1v*PTu-Lp1*P0u*P1v+Lp1*P1u*P0v+Lp1*P0u*P2v-Lp1*P2u*P0v-Lp1*P1u*P2v+Lp1*P2u*P1v);
  } else {
    dR = 1.0 - (sqrt(Lp1*Lp1*P0u*P0u*P2v*P2v - 2*Lp1*Lp1*P0u*P0u*P2v*PTv + Lp1*Lp1*P0u*P0u*PTv*PTv - 2*Lp1*Lp1*P0u*P2u*P0v*P2v + 2*Lp1*Lp1*P0u*P2u*P0v*PTv + 2*Lp1*Lp1*P0u*P2u*P2v*PTv - 2*Lp1*Lp1*P0u*P2u*PTv*PTv + 2*Lp1*Lp1*P0u*P0v*P2v*PTu - 2*Lp1*Lp1*P0u*P0v*PTu*PTv - 2*Lp1*Lp1*P0u*P2v*P2v*PTu + 2*Lp1*Lp1*P0u*P2v*PTu*PTv + Lp1*Lp1*P2u*P2u*P0v*P0v - 2*Lp1*Lp1*P2u*P2u*P0v*PTv + Lp1*Lp1*P2u*P2u*PTv*PTv - 2*Lp1*Lp1*P2u*P0v*P0v*PTu + 2*Lp1*Lp1*P2u*P0v*P2v*PTu + 2*Lp1*Lp1*P2u*P0v*PTu*PTv - 2*Lp1*Lp1*P2u*P2v*PTu*PTv + Lp1*Lp1*P0v*P0v*PTu*PTu - 2*Lp1*Lp1*P0v*P2v*PTu*PTu + Lp1*Lp1*P2v*P2v*PTu*PTu - 2*Lp1*Lp2*P0u*P0u*P1v*P2v + 2*Lp1*Lp2*P0u*P0u*P1v*PTv + 2*Lp1*Lp2*P0u*P0u*P2v*P2v - 2*Lp1*Lp2*P0u*P0u*P2v*PTv + 2*Lp1*Lp2*P0u*P1u*P0v*P2v - 2*Lp1*Lp2*P0u*P1u*P0v*PTv - 2*Lp1*Lp2*P0u*P1u*P2v*P2v + 2*Lp1*Lp2*P0u*P1u*P2v*PTv + 2*Lp1*Lp2*P0u*P2u*P0v*P1v - 4*Lp1*Lp2*P0u*P2u*P0v*P2v + 2*Lp1*Lp2*P0u*P2u*P0v*PTv + 2*Lp1*Lp2*P0u*P2u*P1v*P2v - 4*Lp1*Lp2*P0u*P2u*P1v*PTv + 2*Lp1*Lp2*P0u*P2u*P2v*PTv - 2*Lp1*Lp2*P0u*P0v*P1v*PTu + 2*Lp1*Lp2*P0u*P0v*P2v*PTu + 2*Lp1*Lp2*P0u*P1v*P2v*PTu - 2*Lp1*Lp2*P0u*P2v*P2v*PTu - 2*Lp1*Lp2*P1u*P2u*P0v*P0v + 2*Lp1*Lp2*P1u*P2u*P0v*P2v + 2*Lp1*Lp2*P1u*P2u*P0v*PTv - 2*Lp1*Lp2*P1u*P2u*P2v*PTv + 2*Lp1*Lp2*P1u*P0v*P0v*PTu - 4*Lp1*Lp2*P1u*P0v*P2v*PTu + 2*Lp1*Lp2*P1u*P2v*P2v*PTu + 2*Lp1*Lp2*P2u*P2u*P0v*P0v - 2*Lp1*Lp2*P2u*P2u*P0v*P1v - 2*Lp1*Lp2*P2u*P2u*P0v*PTv + 2*Lp1*Lp2*P2u*P2u*P1v*PTv - 2*Lp1*Lp2*P2u*P0v*P0v*PTu + 2*Lp1*Lp2*P2u*P0v*P1v*PTu + 2*Lp1*Lp2*P2u*P0v*P2v*PTu - 2*Lp1*Lp2*P2u*P1v*P2v*PTu - 2*Lp1*Lq1*P0u*P0u*P1v*P2v + 2*Lp1*Lq1*P0u*P0u*P1v*PTv + 2*Lp1*Lq1*P0u*P0u*P2v*PTv - 2*Lp1*Lq1*P0u*P0u*PTv*PTv + 2*Lp1*Lq1*P0u*P1u*P0v*P2v - 2*Lp1*Lq1*P0u*P1u*P0v*PTv - 2*Lp1*Lq1*P0u*P1u*P2v*PTv + 2*Lp1*Lq1*P0u*P1u*PTv*PTv + 2*Lp1*Lq1*P0u*P2u*P0v*P1v - 2*Lp1*Lq1*P0u*P2u*P0v*PTv - 2*Lp1*Lq1*P0u*P2u*P1v*PTv + 2*Lp1*Lq1*P0u*P2u*PTv*PTv - 2*Lp1*Lq1*P0u*P0v*P1v*PTu - 2*Lp1*Lq1*P0u*P0v*P2v*PTu + 4*Lp1*Lq1*P0u*P0v*PTu*PTv + 4*Lp1*Lq1*P0u*P1v*P2v*PTu - 2*Lp1*Lq1*P0u*P1v*PTu*PTv - 2*Lp1*Lq1*P0u*P2v*PTu*PTv - 2*Lp1*Lq1*P1u*P2u*P0v*P0v + 4*Lp1*Lq1*P1u*P2u*P0v*PTv - 2*Lp1*Lq1*P1u*P2u*PTv*PTv + 2*Lp1*Lq1*P1u*P0v*P0v*PTu - 2*Lp1*Lq1*P1u*P0v*P2v*PTu - 2*Lp1*Lq1*P1u*P0v*PTu*PTv + 2*Lp1*Lq1*P1u*P2v*PTu*PTv + 2*Lp1*Lq1*P2u*P0v*P0v*PTu - 2*Lp1*Lq1*P2u*P0v*P1v*PTu - 2*Lp1*Lq1*P2u*P0v*PTu*PTv + 2*Lp1*Lq1*P2u*P1v*PTu*PTv - 2*Lp1*Lq1*P0v*P0v*PTu*PTu + 2*Lp1*Lq1*P0v*P1v*PTu*PTu + 2*Lp1*Lq1*P0v*P2v*PTu*PTu - 2*Lp1*Lq1*P1v*P2v*PTu*PTu + Lp2*Lp2*P0u*P0u*P1v*P1v - 2*Lp2*Lp2*P0u*P0u*P1v*P2v + Lp2*Lp2*P0u*P0u*P2v*P2v - 2*Lp2*Lp2*P0u*P1u*P0v*P1v + 2*Lp2*Lp2*P0u*P1u*P0v*P2v + 2*Lp2*Lp2*P0u*P1u*P1v*P2v - 2*Lp2*Lp2*P0u*P1u*P2v*P2v + 2*Lp2*Lp2*P0u*P2u*P0v*P1v - 2*Lp2*Lp2*P0u*P2u*P0v*P2v - 2*Lp2*Lp2*P0u*P2u*P1v*P1v + 2*Lp2*Lp2*P0u*P2u*P1v*P2v + Lp2*Lp2*P1u*P1u*P0v*P0v - 2*Lp2*Lp2*P1u*P1u*P0v*P2v + Lp2*Lp2*P1u*P1u*P2v*P2v - 2*Lp2*Lp2*P1u*P2u*P0v*P0v + 2*Lp2*Lp2*P1u*P2u*P0v*P1v + 2*Lp2*Lp2*P1u*P2u*P0v*P2v - 2*Lp2*Lp2*P1u*P2u*P1v*P2v + Lp2*Lp2*P2u*P2u*P0v*P0v - 2*Lp2*Lp2*P2u*P2u*P0v*P1v + Lp2*Lp2*P2u*P2u*P1v*P1v - 2*Lp2*Lq1*P0u*P0u*P1v*P1v + 2*Lp2*Lq1*P0u*P0u*P1v*P2v + 2*Lp2*Lq1*P0u*P0u*P1v*PTv - 2*Lp2*Lq1*P0u*P0u*P2v*PTv + 4*Lp2*Lq1*P0u*P1u*P0v*P1v - 2*Lp2*Lq1*P0u*P1u*P0v*P2v - 2*Lp2*Lq1*P0u*P1u*P0v*PTv - 2*Lp2*Lq1*P0u*P1u*P1v*P2v - 2*Lp2*Lq1*P0u*P1u*P1v*PTv + 4*Lp2*Lq1*P0u*P1u*P2v*PTv - 2*Lp2*Lq1*P0u*P2u*P0v*P1v + 2*Lp2*Lq1*P0u*P2u*P0v*PTv + 2*Lp2*Lq1*P0u*P2u*P1v*P1v - 2*Lp2*Lq1*P0u*P2u*P1v*PTv - 2*Lp2*Lq1*P0u*P0v*P1v*PTu + 2*Lp2*Lq1*P0u*P0v*P2v*PTu + 2*Lp2*Lq1*P0u*P1v*P1v*PTu - 2*Lp2*Lq1*P0u*P1v*P2v*PTu - 2*Lp2*Lq1*P1u*P1u*P0v*P0v + 2*Lp2*Lq1*P1u*P1u*P0v*P2v + 2*Lp2*Lq1*P1u*P1u*P0v*PTv - 2*Lp2*Lq1*P1u*P1u*P2v*PTv + 2*Lp2*Lq1*P1u*P2u*P0v*P0v - 2*Lp2*Lq1*P1u*P2u*P0v*P1v - 2*Lp2*Lq1*P1u*P2u*P0v*PTv + 2*Lp2*Lq1*P1u*P2u*P1v*PTv + 2*Lp2*Lq1*P1u*P0v*P0v*PTu - 2*Lp2*Lq1*P1u*P0v*P1v*PTu - 2*Lp2*Lq1*P1u*P0v*P2v*PTu + 2*Lp2*Lq1*P1u*P1v*P2v*PTu - 2*Lp2*Lq1*P2u*P0v*P0v*PTu + 4*Lp2*Lq1*P2u*P0v*P1v*PTu - 2*Lp2*Lq1*P2u*P1v*P1v*PTu + 4*Lp2*P0u*P0u*P1v*P2v - 4*Lp2*P0u*P0u*P1v*PTv - 4*Lp2*P0u*P0u*P2v*PTv + 4*Lp2*P0u*P0u*PTv*PTv - 4*Lp2*P0u*P1u*P0v*P2v + 4*Lp2*P0u*P1u*P0v*PTv + 4*Lp2*P0u*P1u*P2v*PTv - 4*Lp2*P0u*P1u*PTv*PTv - 4*Lp2*P0u*P2u*P0v*P1v + 4*Lp2*P0u*P2u*P0v*PTv + 4*Lp2*P0u*P2u*P1v*PTv - 4*Lp2*P0u*P2u*PTv*PTv + 4*Lp2*P0u*P0v*P1v*PTu + 4*Lp2*P0u*P0v*P2v*PTu - 8*Lp2*P0u*P0v*PTu*PTv - 8*Lp2*P0u*P1v*P2v*PTu + 4*Lp2*P0u*P1v*PTu*PTv + 4*Lp2*P0u*P2v*PTu*PTv + 4*Lp2*P1u*P2u*P0v*P0v - 8*Lp2*P1u*P2u*P0v*PTv + 4*Lp2*P1u*P2u*PTv*PTv - 4*Lp2*P1u*P0v*P0v*PTu + 4*Lp2*P1u*P0v*P2v*PTu + 4*Lp2*P1u*P0v*PTu*PTv - 4*Lp2*P1u*P2v*PTu*PTv - 4*Lp2*P2u*P0v*P0v*PTu + 4*Lp2*P2u*P0v*P1v*PTu + 4*Lp2*P2u*P0v*PTu*PTv - 4*Lp2*P2u*P1v*PTu*PTv + 4*Lp2*P0v*P0v*PTu*PTu - 4*Lp2*P0v*P1v*PTu*PTu - 4*Lp2*P0v*P2v*PTu*PTu + 4*Lp2*P1v*P2v*PTu*PTu + Lq1*Lq1*P0u*P0u*P1v*P1v - 2*Lq1*Lq1*P0u*P0u*P1v*PTv + Lq1*Lq1*P0u*P0u*PTv*PTv - 2*Lq1*Lq1*P0u*P1u*P0v*P1v + 2*Lq1*Lq1*P0u*P1u*P0v*PTv + 2*Lq1*Lq1*P0u*P1u*P1v*PTv - 2*Lq1*Lq1*P0u*P1u*PTv*PTv + 2*Lq1*Lq1*P0u*P0v*P1v*PTu - 2*Lq1*Lq1*P0u*P0v*PTu*PTv - 2*Lq1*Lq1*P0u*P1v*P1v*PTu + 2*Lq1*Lq1*P0u*P1v*PTu*PTv + Lq1*Lq1*P1u*P1u*P0v*P0v - 2*Lq1*Lq1*P1u*P1u*P0v*PTv + Lq1*Lq1*P1u*P1u*PTv*PTv - 2*Lq1*Lq1*P1u*P0v*P0v*PTu + 2*Lq1*Lq1*P1u*P0v*P1v*PTu + 2*Lq1*Lq1*P1u*P0v*PTu*PTv - 2*Lq1*Lq1*P1u*P1v*PTu*PTv + Lq1*Lq1*P0v*P0v*PTu*PTu - 2*Lq1*Lq1*P0v*P1v*PTu*PTu + Lq1*Lq1*P1v*P1v*PTu*PTu) + 2*P0u*P1v - 2*P1u*P0v - 2*P0u*PTv + 2*P0v*PTu + 2*P1u*PTv - 2*P1v*PTu - 2*Lp1*P0u*P1v + 2*Lp1*P1u*P0v + Lp1*P0u*P2v - Lp1*P2u*P0v - Lp2*P0u*P1v + Lp2*P1u*P0v - 2*Lp1*P1u*P2v + 2*Lp1*P2u*P1v + Lp2*P0u*P2v - Lp2*P2u*P0v - Lp2*P1u*P2v + Lp2*P2u*P1v + Lq1*P0u*P1v - Lq1*P1u*P0v + Lp1*P0u*PTv - Lp1*P0v*PTu - Lp1*P2u*PTv + Lp1*P2v*PTu - Lq1*P0u*PTv + Lq1*P0v*PTu + Lq1*P1u*PTv - Lq1*P1v*PTu)/(2*(P0u*P1v - P1u*P0v - P0u*PTv + P0v*PTu + P1u*PTv - P1v*PTu - Lp1*P0u*P1v + Lp1*P1u*P0v + Lp1*P0u*P2v - Lp1*P2u*P0v - Lp1*P1u*P2v + Lp1*P2u*P1v));
  }

  float level;
  if (Rp1 < 0) {
    level = (Rp1*dR + Rp2) / (dR + Rq1);
  } else {
    level = (Rp1*(1 - dR) + Rp2) / ((1 - dR) + Rq1);
  }

  return level;
}

/**
 * CCIT CRC-32 (Autodin II) Polynomial
 */
uint32_t CRC32 (uint32_t crc, uint8_t *buf, uint16_t len) {
  while (len--) {
    crc = crc ^ *buf++;
    for (uint8_t i = 0; i < 8; ++i) {
      if (crc & 1) {
        crc = (crc >>1) ^ 0xEDB88320;
      } else {
        crc = crc >> 1;
      }
    }
  }
}

/**
 * Calculate CRC32 for calibration data
 */
uint32_t calibrationCRC32 (Calibration *cal) {
  // Non crc32 parts of calibration structure are 22 floats = 88 bytes
  const uint8_t len = 88;
  // Initial value for CRC
  uint32_t crc32 = 0xFFFFFFFF;
  // temporary data buffer
  uint8_t buf[len]; 
  
  // Copy calibration data to byte array
  memcpy(buf, cal, len);
  
  // Calculate CRC
  crc32 = CRC32(crc32, buf, len);

  return crc32;
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
  if (getOnOff()) {
    analogWrite(redPin_, pwmR);
    analogWrite(greenPin_, pwmG);
    analogWrite(bluePin_, pwmB);
  }

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

  // Convert to raw PWM values
  RGB raw;
  
  // Coefficients
  raw.R = findCoefficient(target, cal_.redUv, cal_.greenUv, cal_.blueUv, cal_.redToGreenFit, cal_.greenToBlueFit);
  raw.G = findCoefficient(target, cal_.greenUv, cal_.blueUv, cal_.redUv, cal_.greenToBlueFit, cal_.blueToRedFit);
  raw.B = findCoefficient(target, cal_.blueUv, cal_.redUv, cal_.greenUv, cal_.blueToRedFit, cal_.redToGreenFit);
  
  // Luma produced by the current raw values
  float Y = (raw.R * cal_.redLum + raw.G * cal_.greenLum + raw.B * cal_.blueLum) / cal_.maxLum;

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
 * Save calibration parameters
 */
void calibrate (const Luv redUv, const Luv greenUv, const Luv blueUv, const float redLum, const float greenLum,
const float blueLum, const float redToGreenFit[3], const float greenToBlueFit[3], const float blueToRedFit[3]) {
  // CIE 1976 UCS coordinates
  cal_.redUv.u = redUv.u; cal_.redUv.v = redUv.v;
  cal_.greenUv.u = greenUv.u; cal_.greenUv.v = greenUv.v;
  cal_.blueUv.u = blueUv.u; cal_.blueUv.v = blueUv.v;

  // Luminous fluxes
  cal_.redLum = redLum;
  cal_.greenLum = greenLum;
  cal_.blueLum = blueLum;

  // Fit functions
  for (uint8_t i = 0; i < 3; ++i) {
    cal_.redToGreenFit[i] = redToGreenFit[i];
    cal_.greenToBlueFit[i] = greenToBlueFit[i];
    cal_.blueToRedFit[i] = blueToRedFit[i];
  }

  // Calculate CRC32
  cal_.crc32 = calibrationCRC32(&cal_);

  // Save to EEPROM
  EEPROM.put(calibrationAddress, cal_);
  EEPROM.commit();
}

/**
 * HTTP API controllers
 */

/**
 * HTTP API for index.html
 */
void httpIndexController () {
  String html = FPSTR(HTML_INDEX);
  html.replace("'{{debug}}'", "false");
  html.replace("'{{onOff}}'", onOff_ ? "true" : "false");

  // L, u', v', T
  char strL[10], strU[10], strV[10], strT[10];
  Luv luv = getCie1976Ucs();
  dtostrf(luv.L, 6, 4, strL);
  dtostrf(luv.u, 6, 4, strU);
  dtostrf(luv.v, 6, 4, strV);
  dtostrf(getColorTemperature(), 6, 4, strT);
  html.replace("'{{L}}'", strL);
  html.replace("'{{u}}'", strU);
  html.replace("'{{v}}'", strV);
  html.replace("'{{T}}'", strT);

  // Calibration u', v' coordinates
  char strRedU[10], strRedV[10], strGreenU[10], strGreenV[10], strBlueU[10], strBlueV[10];
  dtostrf(cal_.redUv.u, 6, 4, strRedU);
  dtostrf(cal_.redUv.v, 6, 4, strRedV);
  dtostrf(cal_.greenUv.u, 6, 4, strGreenU);
  dtostrf(cal_.greenUv.v, 6, 4, strGreenV);
  dtostrf(cal_.blueUv.u, 6, 4, strBlueU);
  dtostrf(cal_.blueUv.v, 6, 4, strBlueV);
  html.replace("'{{redU}}'", strRedU);
  html.replace("'{{redV}}'", strRedV);
  html.replace("'{{greenU}}'", strGreenU);
  html.replace("'{{greenV}}'", strGreenV);
  html.replace("'{{blueU}}'", strBlueU);
  html.replace("'{{blueV}}'", strBlueV);

  // Calibration luminous fluxes
  char strRedLum[10], strGreenLum[10], strBlueLum[10];
  dtostrf(cal_.redLum, 6, 4, strRedLum);
  dtostrf(cal_.greenLum, 6, 4, strGreenLum);
  dtostrf(cal_.blueLum, 6, 4, strBlueLum);
  html.replace("'{{redLum}}'", strRedLum);
  html.replace("'{{greenLum}}'", strGreenLum);
  html.replace("'{{blueLum}}'", strBlueLum);
  
  // Calibration red to green fit
  char strRedP1[10], strRedP2[10], strRedQ1[10];
  dtostrf(cal_.redToGreenFit[0], 6, 4, strRedP1);
  dtostrf(cal_.redToGreenFit[1], 6, 4, strRedP2);
  dtostrf(cal_.redToGreenFit[2], 6, 4, strRedQ1);
  html.replace("'{{redP1}}'", strRedP1);
  html.replace("'{{redP2}}'", strRedP2);
  html.replace("'{{redQ1}}'", strRedQ1);
  
  // Calibration green to blue fit
  char strGreenP1[10], strGreenP2[10], strGreenQ1[10];
  dtostrf(cal_.greenToBlueFit[0], 6, 4, strGreenP1);
  dtostrf(cal_.greenToBlueFit[1], 6, 4, strGreenP2);
  dtostrf(cal_.greenToBlueFit[2], 6, 4, strGreenQ1);
  html.replace("'{{greenP1}}'", strGreenP1);
  html.replace("'{{greenP2}}'", strGreenP2);
  html.replace("'{{greenQ1}}'", strGreenQ1);
  
  // Calibration blue to red fit
  char strBlueP1[10], strBlueP2[10], strBlueQ1[10];
  dtostrf(cal_.blueToRedFit[0], 6, 4, strBlueP1);
  dtostrf(cal_.blueToRedFit[1], 6, 4, strBlueP2);
  dtostrf(cal_.blueToRedFit[2], 6, 4, strBlueQ1);
  html.replace("'{{blueP1}}'", strBlueP1);
  html.replace("'{{blueP2}}'", strBlueP2);
  html.replace("'{{blueQ1}}'", strBlueQ1);
  
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
 * HTTP API for setting calibration parameters
 */
void httpCalibrationController () {
  float redU = -1, redV = -1, greenU = -1, greenV = -1, blueU = -1, blueV = -1;
  float redLum = -1, greenLum = -1, blueLum = -1;
  float redP1 = -10000, redP2 = -10000, redQ1 = -10000;
  float greenP1 = -10000, greenP2 = -10000, greenQ1 = -10000;
  float blueP1 = -10000, blueP2 = -10000, blueQ1 = -10000;
  
  // Parse args
  for (uint8_t i = 0; i < server.args(); ++i) {
    if (server.argName(i) == "redU") {
      redU = server.arg(i).toFloat();
    } else if (server.argName(i) == "redV") {
      redV = server.arg(i).toFloat();
    } else if (server.argName(i) == "greenU") {
      greenU = server.arg(i).toFloat();
    } else if (server.argName(i) == "greenV") {
      greenV = server.arg(i).toFloat();
    } else if (server.argName(i) == "blueU") {
      blueU = server.arg(i).toFloat();
    } else if (server.argName(i) == "blueV") {
      blueV = server.arg(i).toFloat();
    } else if (server.argName(i) == "redLum") {
      redLum = server.arg(i).toFloat();
    } else if (server.argName(i) == "greenLum") {
      greenLum = server.arg(i).toFloat();
    } else if (server.argName(i) == "blueLum") {
      blueLum = server.arg(i).toFloat();
    } else if (server.argName(i) == "redP1") {
      redP1 = server.arg(i).toFloat();
    } else if (server.argName(i) == "redP2") {
      redP2 = server.arg(i).toFloat();
    } else if (server.argName(i) == "redQ1") {
      redQ1 = server.arg(i).toFloat();
    } else if (server.argName(i) == "greenP1") {
      greenP1 = server.arg(i).toFloat();
    } else if (server.argName(i) == "greenP2") {
      greenP2 = server.arg(i).toFloat();
    } else if (server.argName(i) == "greenQ1") {
      greenQ1 = server.arg(i).toFloat();
    } else if (server.argName(i) == "blueP1") {
      blueP1 = server.arg(i).toFloat();
    } else if (server.argName(i) == "blueP2") {
      blueP2 = server.arg(i).toFloat();
    } else if (server.argName(i) == "blueQ1") {
      blueQ1 = server.arg(i).toFloat();
    }
  }

  if (redU > 0 && redV > 0 && greenU > 0 && greenV > 0 && blueU > 0 && blueV > 0 && redLum > 0 &&
  greenLum > 0 && blueLum > 0 && redP1 > -10000 && redP2 > -10000 && redQ1 > -10000 && greenP1 > -10000 &&
  greenP2 > -10000 && greenQ1 > -10000 && blueP1 > -10000 && blueP2 > -10000 && blueQ1 > -10000) {
    Luv redUv = {100, redU, redV}, greenUv = {100, greenU, greenV}, blueUv = {100, blueU, blueV};
    float redToGreenFit[3] = {redP1, redP2, redQ1};
    float greenToBlueFit[3] = {greenP1, greenP2, greenQ1};
    float blueToRedFit[3] = {blueP1, blueP2, blueQ1};
    calibrate(redUv, greenUv, blueUv, redLum, greenLum, blueLum, redToGreenFit, greenToBlueFit, blueToRedFit);
  }

  // Send response
  server.send(200, "application/json", "");
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

  // Set onboard leds on as a sign for AP mode
  setLed1(true);
  setLed2(true);

  // Autoconnect to latest WiFi or create cofiguration portal if cannot connect as a client
  WiFiManager wiFiManager;
  //wiFiManager.resetSettings(); // For testing
  wiFiManager.autoConnect("chromasome", "chromasome");

  Serial.begin(115200);

  // Set onboard leds off as a sign for STA mode
  setLed1(false);
  setLed2(false);

  EEPROM.begin(128);
  // Read calibration data from EEPROM  
  Calibration cal;
  EEPROM.get(calibrationAddress, cal);
  
  if (cal.crc32 == calibrationCRC32(&cal)) {
    // EEPROM contents are valid, copy to calibration data
    memcpy(&cal_, &cal, sizeof(cal));
    
  } else {
    // EEPROM contains junk, use default values
    cal_.redLum = 50;
    cal_.greenLum = 100;
    cal_.blueLum = 75;
    cal_.maxLum = 225;
    cal_.redUv.L = 100;
    cal_.redUv.u = 0.5535;
    cal_.redUv.v = 0.5170;
    cal_.greenUv.L = 100;
    cal_.greenUv.u = 0.0373;
    cal_.greenUv.v = 0.5856;
    cal_.blueUv.L = 100;
    cal_.blueUv.u = 0.1679;
    cal_.blueUv.v = 0.1153;
    cal_.redToGreenFit[0] = 2.9658;
    cal_.redToGreenFit[1] = 0.0;
    cal_.redToGreenFit[2] = 1.9658;
    cal_.greenToBlueFit[0] = 1.3587;
    cal_.greenToBlueFit[1] = 0.0;
    cal_.greenToBlueFit[2] = 0.3587;
    cal_.blueToRedFit[0] = -0.2121;
    cal_.blueToRedFit[1] = 0.2121;
    cal_.blueToRedFit[2] = 0.2121;
    cal_.crc32 = 0;
  }

  // Set default to 1900K
  setOnOff(true);
  setColorTemperature(75, 1900);

  // Routes
  server.on("/", HTTP_GET, httpIndexController);
  server.on("/onboardLeds", HTTP_GET, httpOnboardLedsController);
  server.on("/on", HTTP_GET, httpOnController);
  server.on("/off", HTTP_GET, httpOffController);
  server.on("/raw", HTTP_GET, httpRawController);
  server.on("/cie1976Ucs", HTTP_GET, httpCie1976UcsController);
  server.on("/colorTemperature", HTTP_GET, httpColorTemperatureController);
  server.on("/calibrate", HTTP_GET, httpCalibrationController);

  // Not found
  server.onNotFound(httpNotFoundController);

  // Start listening
  server.begin();
}

void loop(void){
  server.handleClient();
}
