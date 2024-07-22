#include <Arduino.h>
#include "radio.h"

#define USE_WIFI   1   // set up a WiFi telnet server
#define DELAY_READ 100 // interval for realtime readouts

#if USE_WIFI
//provide your own wifi.h defining WIFI_SSID and WIFI_PASS
#include "wifi.h"
#include <WiFi.h>
#include <WiFiMulti.h>

static WiFiMulti wm;
static WiFiServer server(23);
static WiFiClient client;
#endif

#define LED PIN_P9

Stream *stream		  = NULL;
bool radioInitDone = false;
bool radioInitOk = false;
uint8_t buf[32];
String RadioStateDescr[] = {
	"RF_STATE_IDLE",
    "RF_STATE_RX_START",
    "RF_STATE_RX_WAIT",
    "RF_STATE_RX_DONE",
    "RF_STATE_RX_TIMEOUT",
    "RF_STATE_TX_START",
    "RF_STATE_TX_WAIT",
    "RF_STATE_TX_DONE",
    "RF_STATE_TX_TIMEOUT",
    "RF_STATE_ERROR"
};

unsigned long lastmillis;

void setup() {
	pinMode(LED, OUTPUT);
	digitalWrite(LED,LOW);
	Serial.begin(115200);
#if USE_WIFI
	wm.addAP(WIFI_SSID, WIFI_PASS);
	while (wm.run() != WL_CONNECTED) {
		Serial.println("WiFi connection failed, retrying in 5s");
		delay(5000);
	}
	server.begin();
	server.setNoDelay(true);
	lastmillis=millis();
#else
	stream	   = &Serial;
#endif
}

void loop() {
#if USE_WIFI
	while (wm.run() != WL_CONNECTED) {
		Serial.println("WiFi connection dropped, retrying in 5s");
		delay(5000);
	}
	if (!stream) {
		client = server.accept();
		if (client) {
			stream = &client;
		}
	} else if (!client.connected()) {
		stream = NULL;
		client.stop();
		client = WiFiClient();
	}
#endif


	if (stream) {
		lastmillis=millis();
		digitalWrite(LED,HIGH);
		int res;
		int radiostate;
		if (!radioInitDone) {
		    stream->print("Init radio: ");
		    res=RF_Init();
			radioInitOk = res ==0;
		    if (radioInitOk) 
				stream->println("ok");
			else
	  			stream->println("error");
			radioInitDone = true;
		}
		if (radioInitOk) {

			switch(RF_Process()) {
				case RF_ERROR:
					stream->println("ERROR");
					radioInitOk=false;
					break;
				case RF_RX_TIMEOUT:
				case RF_TX_TIMEOUT:
					stream->println("TIMEOUT");
					break;
				case RF_IDLE:
					RF_StartRx(buf,32,INFINITE);
					break;
				case RF_RX_DONE:
					for (int i=0; i<32; i++)
						stream->printf("%.2x ",buf[i]);
					stream->println("");
					break;
			}
		}
 	} else { //stream not available
		radioInitOk=false;
		radioInitDone=false;
		unsigned long newmillis=millis();
		unsigned long diff=newmillis-lastmillis;
		digitalWrite(LED,diff>250);
		if (diff>500) 
		   lastmillis=newmillis;
	}
}