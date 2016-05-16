#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <LedEngine.h>

// GPIO settings
const int led1Pin = 5;
const int led2Pin = 1;
const int redPin = 15; //12
const int greenPin = 13; //15
const int bluePin = 12; //13
const int warmPin = 14;
const int coldPin = 4;
const int pwmRange = 1023;

// LED engine
LedEngine led(redPin, greenPin, bluePin, warmPin, coldPin, pwmRange);

// HTTP server
ESP8266WebServer server(80);

// Index.html
// Minify, replace double quotes with single quotes, escape backlashes in regexes
//const char HTML_INDEX[] PROGMEM = "";
const char HTML_INDEX[] PROGMEM = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'><script src='https://cdn.linearicons.com/free/1.0.0/svgembedder.min.js'></script><style>.lnr{display: inline-block;fill: currentColor;width: 1em;height: 1em;vertical-align: -0.05em;font-size: 26px;}html{height: 100%;font-family: sans-serif;}body{background: #555555;margin: 0;height: 100%;padding: 8px;box-sizing: border-box;}.container{max-width: 800px;height: 100%;margin: 0 auto;}.layout-column, .layout-row{box-sizing: border-box;display: -webkit-flex;display: -ms-flexbox;display: flex;}.layout-row{-webkit-flex-direction: row;-ms-flex-direction: row;flex-direction: row;}.layout-row > *{margin-right: 8px;}.layout-row > *:last-child{margin-right: 0;}.layout-column{-webkit-flex-direction: column;-ms-flex-direction: column;flex-direction: column;}.layout-column > *{margin-bottom: 8px;}.layout-column > *:last-child{margin-bottom: 0;}.layout-align-center-center{-webkit-align-items: center;-ms-flex-align: center;align-items: center;-webkit-align-content: center;-ms-flex-line-pack: center;align-content: center;max-width: 100%;-webkit-justify-content: center;-ms-flex-pack: center;justify-content: center;}.flex{-webkit-flex: 1;-ms-flex: 1;flex: 1;box-sizing: border-box;}button{display: block;min-width: 48px; min-height: 48px;color: white;border: none;border-radius: 2px;}button:hover{cursor: pointer;}#settings-button{background-color: hsl(213, 100%, 60%);}#schedule-button{background-color: hsl(276, 100%, 40%);}#schedule-button.active{background-color: hsl(276, 100%, 70%);}#off-button{background-color: hsl(9, 100%, 25%);}#off-button.active{background-color: hsl(9, 100%, 60%);}#on-button{background-color: hsl(120, 100%, 20%);}#on-button.active{background-color: hsl(120, 100%, 50%);}.tile{background-color: rgba(0, 0, 0, 0.7);color: white;padding: 24px;border-radius: 2px;}.img{display: block;width: 100%;border-radius: 2px;position: relative;overflow: hidden;}.cursor{position: absolute;top: -10%; left: 50%;width: 6px; height: 6px;border-radius: 50%;transform: translate(-50%, -50%);background: black;}#dimming{width: 48px;background: linear-gradient(to bottom, #000000 5%, #ffffff 95%);}#dimming-cursor{background: red;}#color-temperature{background-repeat: repeat-x; background-size: contain; width: 48px;}#cie1976ucs{flex-shrink: 0;}#cie1976ucs-zoom{background-color: #333333;}#cie1976ucs-zoom > img{position: absolute;top: 50%; bottom: auto;left: 50%; right: auto;transform-origin: 0% 0%;transform: scale(2);}#settings{background-color: rgba(0, 0, 0, 0.54);position: absolute;top: 0; bottom: 0;left: 0; right: 0;}#settings.no-show{display: none;}#settings .tile{margin: 0 8px;}#settings h1{margin-top: 0;font-weight: normal;}#settings button{float: left; width: 50%;}#settings-save-button{background-color: hsl(213, 100%, 60%);border-top-left-radius: 0;border-bottom-left-radius: 0;}#settings-close-button{background-color: hsl(9, 100%, 25%);border-top-right-radius: 0;border-bottom-right-radius: 0;}#settings table{margin-bottom: 16px;}#settings tr.header th{padding-top: 16px;font-style: italic;font-weight: lighter;text-align: left;}#settings input[type='number']{max-width: 150px;width: 100%;border: none;border-radius: 2px;padding: 6px;box-sizing: border-box;}</style></head><body><div id='main-container' class='container layout-column'><div class='flex layout-row'><div id='dimming' class='img'><div id='dimming-cursor' class='cursor'></diV></div><div class='flex layout-column'><div id='cie1976ucs' class='img'><img class='img' src='https://raw.githubusercontent.com/jaakkopasanen/Tunable-White-LED/master/Matlab/img/CIE_1976_UCS_06_063_Full.jpg'><div id='cie1976ucs-cursor' class='cursor'></div></div><div id='cie1976ucs-zoom' class='img flex'><img id='cie1976ucs-zoom-image' class='img' src='https://raw.githubusercontent.com/jaakkopasanen/Tunable-White-LED/master/Matlab/img/CIE_1976_UCS_06_063_Full.jpg'></div></div><div id='color-temperature' class='img' style='background-image: url(https://raw.githubusercontent.com/jaakkopasanen/Tunable-White-LED/master/Matlab/img/cct_1000K_to_10000K_pow2_vertical.jpg);'><div id='color-temperature-cursor' class='cursor'></div></div></div><div class='layout-row'><button id='off-button' type='button' class='flex'><svg class='lnr'><use xlink:href='#lnr-moon'></use></svg></button><button id='schedule-button' type='button' class='flex'><svg class='lnr'><use xlink:href='#lnr-calendar-full'></use></svg></button><button id='settings-button' type='button' class='flex'><svg class='lnr'><use xlink:href='#lnr-cog'></use></svg></button><button id='on-button' type='button' class='flex'><svg class='lnr'><use xlink:href='#lnr-sun'></use></svg></button></div></div><div id='settings' class='layout-column layout-align-center-center no-show'><div class='tile'><table><tr><th></th><th>Red</th><th>Green</th><th>Blue</th></tr><tr class='header'><th colspan='4'>CIE 1976 UCS</th></tr><tr><th>u'</th><td><input id='red-u' type='number' step='0.0001' value=''></td><td><input id='green-u' type='number' step='0.0001' value=''></td><td><input id='blue-u' type='number' step='0.0001' value=''></td></tr><tr><th>v'</th><td><input id='red-v' type='number' step='0.0001' value=''></td><td><input id='green-v' type='number' step='0.0001' value=''></td><td><input id='blue-v' type='number' step='0.0001' value=''></td></tr><tr class='header'><th colspan='4'>Luminous flux</th></tr><tr><th>lm</th><td><input id='red-lum' type='number' step='0.0001' value=''></td><td><input id='green-lum' type='number' step='0.0001' value=''></td><td><input id='blue-lum' type='number' step='0.0001' value=''></td></tr><tr class='header'><th colspan='4'>Level by distance</th></tr><tr><th>p1</th><td><input id='red-p1' type='number' step='0.0001' value=''></td><td><input id='green-p1' type='number' step='0.0001' value=''></td><td><input id='blue-p1' type='number' step='0.0001' value=''></td></tr><tr><th>p2</th><td><input id='red-p2' type='number' step='0.0001' value=''></td><td><input id='green-p2' type='number' step='0.0001' value=''></td><td><input id='blue-p2' type='number' step='0.0001' value=''></td></tr><tr><th>q1</th><td><input id='red-q1' type='number' step='0.0001' value=''></td><td><input id='green-q1' type='number' step='0.0001' value=''></td><td><input id='blue-q1' type='number' step='0.0001' value=''></td></tr></table><button id='settings-close-button' type='button'><svg class='lnr'><use xlink:href='#lnr-cross'></use></svg></button><button id='settings-save-button' type='button'><svg class='lnr'><use xlink:href='#lnr-checkmark-circle'></use></svg></button></div></div></body><script type='text/javascript'>var zoomLevel=4;var debug='{{debug}}';var mainContainer=document.getElementById('main-container');var onButton=document.getElementById('on-button');var offButton=document.getElementById('off-button');var scheduleButton=document.getElementById('schedule-button');var dimming=document.getElementById('dimming');var dimmingCursor=document.getElementById('dimming-cursor');var cie1976Ucs=document.getElementById('cie1976ucs');var cie1976UcsCursor=document.getElementById('cie1976ucs-cursor');var cie1976UcsZoom=document.getElementById('cie1976ucs-zoom');var cie1976UcsZoomImage=document.getElementById('cie1976ucs-zoom-image');var colorTemperature=document.getElementById('color-temperature');var colorTemperatureCursor=document.getElementById('color-temperature-cursor');var settings=document.getElementById('settings');var settingsButton=document.getElementById('settings-button');var settingsCloseButton=document.getElementById('settings-close-button');var settingsSaveButton=document.getElementById('settings-save-button');var redU=document.getElementById('red-u');var greenU=document.getElementById('green-u');var blueU=document.getElementById('blue-u');var redV=document.getElementById('red-v');var greenV=document.getElementById('green-v');var blueV=document.getElementById('blue-v');var redLum=document.getElementById('red-lum');var greenLum=document.getElementById('green-lum');var blueLum=document.getElementById('blue-lum');var redP1=document.getElementById('red-p1');var greenP1=document.getElementById('green-p1');var blueP1=document.getElementById('blue-p1');var redP2=document.getElementById('red-p2');var greenP2=document.getElementById('green-p2');var blueP2=document.getElementById('blue-p2');var redQ1=document.getElementById('red-q1');var greenQ1=document.getElementById('green-q1');var blueQ1=document.getElementById('blue-q1');var ajax=function (url, callback){console.log('AJAX:', url);if (debug) return;var xhr=new XMLHttpRequest();xhr.open('GET', url, true);xhr.send();xhr.onreadystatechange=function (){if (xhr.readyState===4){if (xhr.status===200){if (callback){callback(xhr.responseText);}}else{console.log('Error:', xhr.responseText);}}};};var setOnOff=function (on, doAjax){if (on){onButton.className +=' active';offButton.className=offButton.className.replace( /(?:^|\\s)active(?!\\S)/g , '' );if (doAjax) ajax('/on');}else{offButton.className +=' active';onButton.className=onButton.className.replace( /(?:^|\\s)active(?!\\S)/g , '' );if (doAjax) ajax('/off');}};onButton.addEventListener('click', function (){setOnOff(true, true);});offButton.addEventListener('click', function (){setOnOff(false, true);});var openSettings=function (){settings.className=settings.className.replace( /(?:^|\\s)no-show(?!\\S)/g , '' );mainContainer.style['-webkit-filter']='blur(5px)';mainContainer.style['filter']='blur(5px)';};var saveSettings=function (){var url='/calibrate?';url +='redU=' + redU.value;url +='&redV=' + redV.value;url +='&greenU=' + greenU.value;url +='&greenV=' + greenV.value;url +='&blueU=' + blueU.value;url +='&blueV=' + blueV.value;url +='&redLum=' + redLum.value;url +='&greenLum=' + greenLum.value;url +='&blueLum=' + blueLum.value;url +='&redP1=' + redP1.value;url +='&redP2=' + redP2.value;url +='&redQ1=' + redQ1.value;url +='&greenP1=' + greenP1.value;url +='&greenP2=' + greenP2.value;url +='&greenQ1=' + greenQ1.value;url +='&blueP1=' + blueP1.value;url +='&blueP2=' + blueP2.value;url +='&blueQ1=' + blueQ1.value;ajax(url);};var closeSettings=function (){settings.className +=' no-show';mainContainer.style['-webkit-filter']='';mainContainer.style['filter']='';};settingsButton.addEventListener('click', openSettings);settingsSaveButton.addEventListener('click', saveSettings);settingsCloseButton.addEventListener('click', closeSettings);var scheduleOn=function (){if (scheduleButton.className.indexOf('active')===-1){scheduleButton.className +=' active';}};var scheduleOff=function (){scheduleButton.className=onButton.className.replace( /(?:^|\\s)active(?!\\S)/g , '' );};scheduleButton.addEventListener('click', function (ev){scheduleOn();});var setDimming=function (L, doAjax){L_=L;dimmingCursor.style.top=(L/100 * 90 + 5) + '%';if (doAjax){if (mode==='color'){ajax('/cie1976Ucs?L=' + L_ + '&u=' + u_ + '&v=' + v_);}else{ajax('/colorTemperature?L=' + L_ + '&T=' + T_);}}};dimming.addEventListener('click', function (ev){var y=(ev.pageY - dimming.offsetTop) / dimming.offsetHeight;y=(y - 0.05) / 0.9;y=Math.min(y, 1);y=Math.max(y, 0);var L=y * 100;setDimming(L, true);});var setCie1976Ucs=function (u, v, doAjax){u_=u;v_=v;mode='color';cie1976UcsCursor.style.left=(u / 0.63 * 100) + '%';cie1976UcsCursor.style.top=((1 - v / 0.6) * 100) + '%';x=-(u / 0.63) * cie1976Ucs.offsetWidth;y=-(1 - v / 0.6) * cie1976Ucs.offsetHeight;cie1976UcsZoomImage.style.transform='scale(' + zoomLevel + ') translate(' + x + 'px, ' + y + 'px)';unsetColorTemperature();scheduleOff();if (doAjax) ajax('/cie1976Ucs?L=' + L_ + '&u=' + u + '&v=' + v);};var unsetCie1976Ucs=function (){cie1976UcsCursor.style.left='-10%';cie1976UcsCursor.style.top='-10%';};cie1976Ucs.addEventListener('click', function (ev){var u=(ev.pageX - cie1976Ucs.offsetLeft) / cie1976Ucs.offsetWidth * 0.63;var v=(1 - (ev.pageY - cie1976Ucs.offsetTop) / cie1976Ucs.offsetHeight) * 0.6;setCie1976Ucs(u, v, true);});cie1976UcsZoom.addEventListener('click', function (ev){var dx=((ev.pageX - cie1976UcsZoom.offsetLeft) / cie1976UcsZoom.offsetWidth - 0.5);var dy=((ev.pageY - cie1976UcsZoom.offsetTop) / cie1976UcsZoom.offsetHeight - 0.5);var dxpx=dx * cie1976UcsZoom.offsetWidth;var dypx=dy * cie1976UcsZoom.offsetHeight;var du=dxpx / cie1976Ucs.offsetWidth * 0.63 / zoomLevel;var dv=dypx / cie1976Ucs.offsetHeight * 0.6 / zoomLevel;setCie1976Ucs(u_ + du, v_ - dv, true);});var setColorTemperature=function (T, doAjax){T_=T;mode='temperature';colorTemperatureCursor.style.top=((1 - Math.pow(((T - 1000) / 9000),0.5)) * 100) + '%';unsetCie1976Ucs();scheduleOff();if (doAjax) ajax('/colorTemperature?L=' + L_ + '&T=' + T_);};var unsetColorTemperature=function (){colorTemperatureCursor.style.top='-10%';};colorTemperature.addEventListener('click', function (ev){var y=(ev.pageY - colorTemperature.offsetTop) / colorTemperature.offsetHeight;var T=Math.pow((1-y), 2) * 9000 + 1000;setColorTemperature(T, true);});window.onload=function (){if (debug){var onOff_=true;var L_=75;var u_=0.199;var v_=0.471;var T_=-1;redU.value=0.5535;greenU.value=0.0373;blueU.value=0.1679;redV.value=0.5170;greenV.value=0.5856;blueV.value=0.1153;redLum.value=160;greenLum.value=320;blueLum.value=240;redP1.value=2.9658;greenP1.value=1.3587;blueP1.value=-0.2121;redP2.value=0.0;greenP2.value=0.0;blueP2.value=0.2121;redQ1.value=1.9658;greenQ1.value=0.3587;blueQ1.value=0.2121;}else{var onOff_='{{onOff}}';var L_='{{L}}';var u_='{{u}}';var v_='{{v}}';var T_='{{T}}';redU.value='{{redU}}';greenU.value='{{greenU}}';blueU.value='{{blueU}}';redV.value='{{redV}}';greenV.value='{{greenV}}';blueV.value='{{blueV}}';redLum.value='{{redLum}}';greenLum.value='{{greenLum}}';blueLum.value='{{blueLum}}';redP1.value='{{redP1}}';greenP1.value='{{greenP1}}';blueP1.value='{{blueP1}}';redP2.value='{{redP2}}';greenP2.value='{{greenP2}}';blueP2.value='{{blueP2}}';redQ1.value='{{redQ1}}';greenQ1.value='{{greenQ1}}';blueQ1.value='{{blueQ1}}';}var mode;if (T_ > 0){mode='temperature';setColorTemperature(T_, false);}else{mode='color';setCie1976Ucs(u_, v_, false);}setOnOff(onOff_, false);setDimming(L_, false);};</script></html>";
/**
 * HTTP API for index.html
 */
void httpIndexController() {
	String html = FPSTR(HTML_INDEX);
	html.replace("'{{debug}}'", "false");
	html.replace("'{{onOff}}'", led.getOnOff() ? "true" : "false");

	// L, u', v', T
	char strL[10], strU[10], strV[10], strT[10];
	Luv luv = led.getCie1976Ucs();
	dtostrf(luv.L, 6, 4, strL);
	dtostrf(luv.u, 6, 4, strU);
	dtostrf(luv.v, 6, 4, strV);
	dtostrf(led.getColorTemperature(), 6, 4, strT);
	html.replace("'{{L}}'", strL);
	html.replace("'{{u}}'", strU);
	html.replace("'{{v}}'", strV);
	html.replace("'{{T}}'", strT);

	// Calibration u', v' coordinates
	Luv redUv = led.getRedUv();
	Luv greenUv = led.getGreenUv();
	Luv blueUv = led.getBlueUv();
	char strRedU[10], strRedV[10], strGreenU[10], strGreenV[10], strBlueU[10], strBlueV[10];
	dtostrf(redUv.u, 6, 4, strRedU);
	dtostrf(redUv.v, 6, 4, strRedV);
	dtostrf(greenUv.u, 6, 4, strGreenU);
	dtostrf(greenUv.v, 6, 4, strGreenV);
	dtostrf(blueUv.u, 6, 4, strBlueU);
	dtostrf(blueUv.v, 6, 4, strBlueV);
	html.replace("'{{redU}}'", strRedU);
	html.replace("'{{redV}}'", strRedV);
	html.replace("'{{greenU}}'", strGreenU);
	html.replace("'{{greenV}}'", strGreenV);
	html.replace("'{{blueU}}'", strBlueU);
	html.replace("'{{blueV}}'", strBlueV);

	// Calibration luminous fluxes
	char strRedLum[10], strGreenLum[10], strBlueLum[10];
	dtostrf(led.getRedLum(), 6, 4, strRedLum);
	dtostrf(led.getGreenLum(), 6, 4, strGreenLum);
	dtostrf(led.getBlueLum(), 6, 4, strBlueLum);
	html.replace("'{{redLum}}'", strRedLum);
	html.replace("'{{greenLum}}'", strGreenLum);
	html.replace("'{{blueLum}}'", strBlueLum);

	// Calibration red to green fit
	float * redToGreenFit = led.getRedToGreenFit();
	char strRedP1[10], strRedP2[10], strRedQ1[10];
	dtostrf(redToGreenFit[0], 6, 4, strRedP1);
	dtostrf(redToGreenFit[1], 6, 4, strRedP2);
	dtostrf(redToGreenFit[2], 6, 4, strRedQ1);
	html.replace("'{{redP1}}'", strRedP1);
	html.replace("'{{redP2}}'", strRedP2);
	html.replace("'{{redQ1}}'", strRedQ1);

	// Calibration green to blue fit
	float * greenToBlueFit = led.getGreenToBlueFit();
	char strGreenP1[10], strGreenP2[10], strGreenQ1[10];
	dtostrf(greenToBlueFit[0], 6, 4, strGreenP1);
	dtostrf(greenToBlueFit[1], 6, 4, strGreenP2);
	dtostrf(greenToBlueFit[2], 6, 4, strGreenQ1);
	html.replace("'{{greenP1}}'", strGreenP1);
	html.replace("'{{greenP2}}'", strGreenP2);
	html.replace("'{{greenQ1}}'", strGreenQ1);

	// Calibration blue to red fit
	float * blueToRedFit = led.getBlueToRedFit();
	char strBlueP1[10], strBlueP2[10], strBlueQ1[10];
	dtostrf(blueToRedFit[0], 6, 4, strBlueP1);
	dtostrf(blueToRedFit[1], 6, 4, strBlueP2);
	dtostrf(blueToRedFit[2], 6, 4, strBlueQ1);
	html.replace("'{{blueP1}}'", strBlueP1);
	html.replace("'{{blueP2}}'", strBlueP2);
	html.replace("'{{blueQ1}}'", strBlueQ1);

	server.send(200, "text/html", html);
}

/**
 * HTTP API for setting the light on
 */
void httpOnController() {
	led.setOnOff(true);

	// JSON response
	char response[40];
	sprintf(response, "{\n  \"onOff\": %s}", "true");

	// Send response
	server.send(200, "application/json", response);
}

/**
 * HTTP API for setting the light off
 */
void httpOffController() {
	led.setOnOff(false);

	// JSON response
	char response[40];
	sprintf(response, "{\n  \"onOff\": %s\n}", "false");

	// Send response
	server.send(200, "application/json", response);
}

/**
 * HTTP API for raw PWM values
 */
void httpRawController() {
	float R = -1, G = -1, B = -1;

	// Parse args
	for (uint8_t i = 0; i < server.args(); ++i) {
		if (server.argName(i) == "R") {
			R = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "G") {
			G = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "B") {
			B = server.arg(i).toFloat();
		}
	}

	int httpStatus = 200;

	// All parameters must be given for write
	if (R >= 0 && G >= 0 && B >= 0) {
		// All parameters given -> set new values
		RGB raw = { R, G, B };
		led.setRaw(raw);
	}

	// Read (updated) values
	RGB raw = led.getRaw();

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
void httpCie1976UcsController() {
	float L = -1, u = -1, v = -1;
	// Parse args
	for (uint8_t i = 0; i < server.args(); ++i) {
		if (server.argName(i) == "L") {
			L = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "u") {
			u = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "v") {
			v = server.arg(i).toFloat();
		}
	}

	// All or none of the parameters must be missing
	if (L >= 0 && u >= 0 && v >= 0) {
		// All parameters given -> set new color
		Luv luv = { L, u, v };
		led.setCie1976Ucs(luv);
	}

	// Read (updated) color
	Luv luv = led.getCie1976Ucs();
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
void httpColorTemperatureController() {
	int T = -1; float L = -1;
	// Parse args
	for (uint8_t i = 0; i < server.args(); ++i) {
		if (server.argName(i) == "T") {
			T = server.arg(i).toInt();
		}
		else if (server.argName(i) == "L") {
			L = server.arg(i).toFloat();
		}
	}

	// Set color temperature
	if (T >= 1000 && L >= 0) {
		led.setColorTemperature(L, T);
	}

	// Read (updated) color temperature
	T = led.getColorTemperature();

	// Read (updated) lightness
	Luv luv = led.getCie1976Ucs();
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
void httpCalibrationController() {
	float redU = -1, redV = -1, greenU = -1, greenV = -1, blueU = -1, blueV = -1;
	float redLum = -1, greenLum = -1, blueLum = -1;
	float redP1 = -10000, redP2 = -10000, redQ1 = -10000;
	float greenP1 = -10000, greenP2 = -10000, greenQ1 = -10000;
	float blueP1 = -10000, blueP2 = -10000, blueQ1 = -10000;

	// Parse args
	for (uint8_t i = 0; i < server.args(); ++i) {
		if (server.argName(i) == "redU") {
			redU = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "redV") {
			redV = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "greenU") {
			greenU = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "greenV") {
			greenV = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "blueU") {
			blueU = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "blueV") {
			blueV = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "redLum") {
			redLum = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "greenLum") {
			greenLum = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "blueLum") {
			blueLum = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "redP1") {
			redP1 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "redP2") {
			redP2 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "redQ1") {
			redQ1 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "greenP1") {
			greenP1 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "greenP2") {
			greenP2 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "greenQ1") {
			greenQ1 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "blueP1") {
			blueP1 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "blueP2") {
			blueP2 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "blueQ1") {
			blueQ1 = server.arg(i).toFloat();
		}
	}

	if (redU > 0 && redV > 0 && greenU > 0 && greenV > 0 && blueU > 0 && blueV > 0 && redLum > 0 &&
		greenLum > 0 && blueLum > 0 && redP1 > -10000 && redP2 > -10000 && redQ1 > -10000 && greenP1 > -10000 &&
		greenP2 > -10000 && greenQ1 > -10000 && blueP1 > -10000 && blueP2 > -10000 && blueQ1 > -10000) {
		Luv redUv = { 100, redU, redV }, greenUv = { 100, greenU, greenV }, blueUv = { 100, blueU, blueV };
		float redToGreenFit[3] = { redP1, redP2, redQ1 };
		float greenToBlueFit[3] = { greenP1, greenP2, greenQ1 };
		float blueToRedFit[3] = { blueP1, blueP2, blueQ1 };
		led.calibrate(redUv, greenUv, blueUv, redLum, greenLum, blueLum, redToGreenFit, greenToBlueFit, blueToRedFit);
	}

	// Send response
	server.send(200, "application/json", "");
}

/**
 * HTTP 404 - Not foudn
 */
void httpNotFoundController() {
	server.send(404, "text/plain", "404");
}



/**
 * Setup
 */
void setup(void) {

	pinMode(led1Pin, OUTPUT);
	pinMode(led2Pin, OUTPUT); ;

	// Set onboard leds on as a sign for AP mode
	digitalWrite(led1Pin, LOW);
	digitalWrite(led2Pin, LOW);

	// Autoconnect to latest WiFi or create cofiguration portal if cannot connect as a client
	WiFiManager wiFiManager;
	//wiFiManager.resetSettings(); // For testing
	wiFiManager.autoConnect("chromasome", "chromasome");

	//Serial.begin(115200);

	// Set onboard leds off as a sign for STA mode
	digitalWrite(led1Pin, HIGH);
	digitalWrite(led2Pin, HIGH);

	// Set default to 1900K
	led.setOnOff(true);
	led.setColorTemperature(75, 1900);

	// Routes
	server.on("/", HTTP_GET, httpIndexController);
	server.on("/on", HTTP_GET, httpOnController);
	server.on("/off", HTTP_GET, httpOffController);
	server.on("/raw", HTTP_GET, httpRawController);
	server.on("/cie1976Ucs", HTTP_GET, httpCie1976UcsController);
	server.on("/colorTemperature", HTTP_GET, httpColorTemperatureController);
	server.on("/calibrate", HTTP_GET, httpCalibrationController);
	server.onNotFound(httpNotFoundController);

	// Start listening
	server.begin();
}

void loop(void) {
	server.handleClient();

}
