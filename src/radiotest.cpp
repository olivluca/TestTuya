#include <Arduino.h>
#include "radio.h"

#define USE_WIFI   1   // set up a WiFi telnet server

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

unsigned long lastmillis;

/* not working yet */
void receive() {
    if (StartRx()==0) {
		stream->println("RF_Init ok");
	} else {
	  	stream->println("RF_Init error");
		return;
	}

	int lastvalue=LOW;
	unsigned long lasttime=micros();
	unsigned long starttime=lasttime;

    while (true) {
		unsigned long now=micros();
		if (now-starttime>5000000) {
			break;
		}
		int v=digitalRead(CMT2300A_GPIO2_PIN);
		if (v!=lastvalue) {
			stream->printf("%d,%d,%d\n",v,now-lasttime,now);
			lastvalue=v;
			lasttime=now;
		}
	}
    CMT2300A_GoStby();
	stream->println("receive end");

}
void sendbutton(int b) {

    /* alternating sequences of pulse and gap obtained with original firmware and tinytuya 
	  https://github.com/jasonacox/tinytuya/blob/master/tinytuya/Contrib/RFRemoteControlDevice.py
	  
	*/
	int values[][70]={
		  /*play/pause*/ {6042,1106,391,1106,391,391,1106,333,1106,391,1106,391,1106,333,1106,391,1106,391,1106,1106,391,1106,391,391,1106,1106,391,1106,391,391,1106,1106,391,391,1106,1106,391,391,1106,1106,391,1106,391,1106,391,391,1106,391,1106,1106,391,391,1106,391,1106,391,1106,1106,333,1106,0},
		  /*1*/          {6750,1106,391,1106,391,391,1106,391,1106,391,1106,391,1106,391,1106,391,1106,391,1106,1106,332,1106,391,391,1106,1106,391,1106,391,391,1106,1106,391,391,1106,1106,332,391,1106,1106,391,1106,391,1106,391,1106,391,1106,391,391,1106,1106,391,391,1106,391,1106,391,1106,391,0},
		  /*2*/          {6013,1093,387,1093,341,387,1093,387,1093,387,1128,341,1128,387,1093,387,1093,387,1128,1093,387,1093,387,387,1093,1093,387,1093,387,387,1128,1093,387,387,1128,1093,387,387,1093,1128,387,1093,387,1093,387,1093,387,387,1093,387,1128,1093,387,387,1093,387,1093,387,1093,1093,0},
		  /*3*/          {6803,1105,379,1105,379,379,1105,379,1105,379,1105,379,1105,379,1105,379,1105,379,1105,1105,379,1105,379,379,1105,1105,379,1105,379,379,1105,1105,379,379,1105,1105,379,379,1105,1105,379,1105,379,1105,379,379,1105,1105,379,379,1105,1105,379,379,1105,379,1105,1105,379,379,0},
		  /*4*/          {6005,1105,379,1105,379,379,1105,379,1105,379,1105,379,1105,379,1105,379,1105,379,1105,1105,379,1105,379,379,1105,1105,379,1105,379,379,1105,1105,379,379,1105,1105,379,379,1105,1105,379,1105,379,1105,379,379,1105,379,1105,379,1105,1105,379,379,1105,379,1105,1105,379,1105,0},
		  /*5*/          {6816,1105,379,1105,379,379,1105,379,1105,379,1105,379,1105,379,1105,379,1105,379,1105,1105,379,1105,379,379,1105,1105,379,1105,379,379,1105,1105,379,379,1105,1105,379,379,1105,1105,379,1105,379,379,1105,1105,379,1105,379,379,1105,1105,379,379,1105,1105,379,379,1105,379,0},
		  /*6*/          {5933,1104,379,1104,379,379,1104,379,1104,379,1104,379,1104,379,1104,379,1104,379,1104,1104,379,1104,379,379,1104,1104,379,1104,379,379,1104,1104,379,379,1104,1104,379,379,1104,1104,379,1104,379,379,1104,1104,379,379,1104,379,1104,1104,379,379,1104,1104,379,379,1104,1104,0},
	};

    switch (StartTx()) {
		case 1:
		  stream->println("Error RF_Init");
		  return;
		case 2:
		  stream->println("Error go tx");
		  return;  
	}


    int i=0;
	//start with no tx
    int level=LOW;
    #define ESPHOME_DELAY
	#ifdef ESPHOME_DELAY
	stream->println("using esphome delay");
	uint32_t current_time = micros();
	#endif  
	while (values[b][i]!=0) {
		level=1-level;
		digitalWrite(CMT2300A_GPIO1_PIN, level);
		unsigned long delay=values[b][i];
		#ifdef ESPHOME_DELAY
		uint32_t target_time = current_time+delay;
		while (target_time > micros()) {
			//busy loop
		}
		current_time=target_time;
		#else
		delayMicroseconds(delay);
		#endif
		i++;
	}
	level=1-level;
	digitalWrite(CMT2300A_GPIO1_PIN,level);
	if (CMT2300A_GoStby())
		stream->println("go stby ok");
	else
		stream->println("go stdby error");  
	
}

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
		if (stream->available()) {
               char x=stream->read();
			   switch(x) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
  				  stream->print("sending button ");
				  stream->println(x);
				  sendbutton(x-'0');
				  break;
				case 'r':
				  stream->println("receiving");
				  receive();
				  break;  
			   }
		}
 	} else { //stream not available
		unsigned long newmillis=millis();
		unsigned long diff=newmillis-lastmillis;
		digitalWrite(LED,diff>250);
		if (diff>500) 
		   lastmillis=newmillis;
	}
}