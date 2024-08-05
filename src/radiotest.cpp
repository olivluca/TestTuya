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
    if (RF_Init()==0) {
		stream->println("RF_Init ok");
	} else {
	  	stream->println("RF_Init error");
		return;
	}

    CMT2300A_ConfigGpio (CMT2300A_GPIO1_SEL_INT1 | CMT2300A_GPIO2_SEL_INT2 | CMT2300A_GPIO3_SEL_DOUT);
	CMT2300A_ConfigInterrupt(CMT2300A_INT_SEL_TX_DONE, CMT2300A_INT_SEL_PKT_OK);
	CMT2300A_EnableInterrupt(CMT2300A_MASK_TX_DONE_EN | CMT2300A_MASK_PKT_DONE_EN);

	CMT2300A_WriteReg(CMT2300A_CUS_SYS2 , 0);

	CMT2300A_EnableFifoMerge(true);

	CMT2300A_WriteReg(CMT2300A_CUS_PKT29, 0x20); 

	CMT2300A_GoSleep();
	CMT2300A_GoStby();
	CMT2300A_ConfigGpio (CMT2300A_GPIO1_SEL_DCLK | CMT2300A_GPIO2_SEL_DOUT | CMT2300A_GPIO3_SEL_INT2);
	CMT2300A_ConfigInterrupt(CMT2300A_INT_SEL_SYNC_OK | CMT2300A_INT_SEL_SL_TMO, CMT2300A_INT_SEL_PKT_OK);
    CMT2300A_EnableFifoMerge(true);
	CMT2300A_ClearInterruptFlags();
	CMT2300A_ClearRxFifo();
	pinMode(CMT2300A_GPIO2_PIN, INPUT);
	
    CMT2300A_GoRx();

	//CMT2300_IsExist()
	int rssi=CMT2300A_GetRssiDBm();
	//stream->print("rssi dbm ");
	//stream->println(rssi);

	CMT2300A_ClearInterruptFlags();
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

    if (RF_Init()==0) {
		stream->println("RF_Init ok");
	} else {
	  	stream->println("RF_Init error");
		return;
	}

    int i=0;
	pinMode(CMT2300A_GPIO1_PIN, OUTPUT);
	//start with no tx
    int level=LOW;
	digitalWrite(CMT2300A_GPIO1_PIN, level);
    CMT2300A_WriteReg(CMT2300A_CUS_SYS2,0); //???? 
    CMT2300A_ConfigGpio(CMT2300A_GPIO1_SEL_DOUT | CMT2300A_GPIO3_SEL_DIN | CMT2300A_GPIO2_SEL_INT2);
	CMT2300A_EnableTxDin(true);    
	CMT2300A_ConfigTxDin(CMT2300A_TX_DIN_SEL_GPIO1);
	CMT2300A_EnableTxDinInvert(true);  //don't know why it's needed
	CMT2300A_GoSleep();
	CMT2300A_GoStby();


    #undef ESPHOME_DELAY
	if (CMT2300A_GoTx()) {
		stream->println("go tx ok");
		#ifdef ESPHOME_DELAY
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
		//delay(500);
		if (CMT2300A_GoStby())
		stream->println("go stby ok");
		else
		stream->println("go stdby error");  
	} else {
	  stream->println("go tx error");
	}
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