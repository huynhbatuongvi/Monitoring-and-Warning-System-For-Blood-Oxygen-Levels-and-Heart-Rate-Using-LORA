#include "Arduino.h"
#include "LoRa_E32.h"
#include <FirebaseESP32.h>
HardwareSerial mySerial(2);

// Create a LoRa_E32 object using the custom hardware serial
LoRa_E32 e32ttl(&mySerial, 5, 18, 19);
#include <WiFi.h> //TB

FirebaseData fbdo;

#define WIFI_SSID "Quanghuy"
#define WIFI_PASSWORD "quanghuy1402"

// #define WIFI_SSID "MOLCOFFEE"
// #define WIFI_PASSWORD "MOLCOFFEE"

const char* FIREBASE_HOST  = "https://heartrate-2cd32-default-rtdb.asia-southeast1.firebasedatabase.app/";
const char* FIREBASE_AUTH  = "3GGDdrNr0R0TlraTq6DEaHOKPa9WtQYYncJhXLTu";
#define ledPin 4
#define buzzer 2
// #define FIREBASE_HOST "https://cuoiky-94cd3-default-rtdb.firebaseio.com"
// #define FIREBASE_AUTH "AIzaSyAWr_z7vQe_H658lJ62Sf8YZTxFeI5IjJk"
// #define EMAIL "hollowill123@gmail.com";
// #define PASS "123456789";



void printParameters(struct Configuration configuration);
void printModuleInformation(struct ModuleInformation moduleInformation);
int SPO2_2 ;
    int HR_1 ;
    int SPO2_1 ;
    int HR_2 ;

  
void setup()
{

  pinMode(ledPin, OUTPUT);
   pinMode(buzzer, OUTPUT);

  Serial.begin(9600);
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("LoRa Receiver");
  while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB
    }
    
  delay(100);
  
  e32ttl.begin();

  ///////////////////
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH );
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(fbdo, 1000 * 60);
//  //tiny, small, medium, large and unlimited.
//  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(fbdo, "tiny");
  
 ResponseStructContainer c;
  c = e32ttl.getConfiguration();
  Configuration configuration = *(Configuration*)c.data;

  configuration.ADDL = 0x03;
	configuration.ADDH = 0x03;
	configuration.CHAN = 0x04;

  configuration.OPTION.fec = FEC_1_ON;
  configuration.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;
  configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
  configuration.OPTION.transmissionPower = POWER_14;       //2'b10 24dBm
  configuration.OPTION.wirelessWakeupTime = WAKE_UP_1000;  //3'b011 1000ms

  configuration.SPED.airDataRate = AIR_DATA_RATE_011_48;  //3'b011 4.8kbps
  configuration.SPED.uartBaudRate = UART_BPS_9600;
  configuration.SPED.uartParity = MODE_00_8N1;

  // Set configuration changed and set to not hold the configuration
  ResponseStatus rs = e32ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
 Serial.print("Lora Configuration: ");
 Serial.println(rs.getResponseDescription());
 printParameters(configuration);

  c.close();
  // ---------------------------
  Serial.println();
  Serial.println("Start listening!");
 

}

struct Message1 {
  
    byte heart_rate_1;
    byte spo2_1;
    byte temp_1;
    byte button_1;
};
struct Message2 {
    byte heart_rate;
    byte spo2;
    byte temp;
    byte button;
};

void loop() {

  if (e32ttl.available()  > 1){
		char type[5]; // first part of structure
		ResponseContainer rs = e32ttl.receiveInitialMessage(sizeof(type));
		String typeStr = rs.data;

		Serial.println(typeStr);
		if (typeStr=="B"){
			ResponseStructContainer rsc = e32ttl.receiveMessage(sizeof(Message1));
			struct Message1 message1 = *(Message1*) rsc.data;

			Serial.println(message1.heart_rate_1);
      Serial.println(message1.spo2_1);
      Serial.println(message1.temp_1);
      Serial.println(message1.button_1);

      int heartRate_1 = message1.heart_rate_1;
      int spo2_1 = message1.spo2_1;
       float temp_1 = message1.temp_1;
       int button_1  =  message1.button_1;

       if(heartRate_1 < 40 || heartRate_1 >120)
     {
        digitalWrite(ledPin, HIGH);
        digitalWrite(buzzer, HIGH);
     }
  //  else if(spo2_1 < 90)
  //    {
  //       digitalWrite(ledPin, HIGH);
  //       digitalWrite(buzzer, HIGH);
  //    }else if (temp_1 <=20 || temp_1 >= 38){
  //       digitalWrite(ledPin, HIGH);
  //       digitalWrite(buzzer, HIGH);
  //    }

      Firebase.setInt(fbdo, "/User/lora2/HeartRate", heartRate_1);
      Firebase.setInt(fbdo, "/User/lora2/Spo2", spo2_1);
     Firebase.setFloat(fbdo, "/User/lora2/Temp", temp_1);
     Firebase.setInt(fbdo, "/User/lora2/button", button_1);

  
    //  }else if(spo2_1 < 90)
    //  {
    //     digitalWrite(ledPin, HIGH);
    //     digitalWrite(buzzer, HIGH);
    //  }else if (temp_1 <=35 || temp_1 >= 38){
    //     digitalWrite(ledPin, HIGH);
    //     digitalWrite(buzzer, HIGH);
    //  }

      int buttonValue_1 = (int)message1.button_1;
      if (buttonValue_1 != 0) {
                digitalWrite(ledPin, HIGH);
                 digitalWrite(buzzer, HIGH);
                 delay(2000);
            } else {
                digitalWrite(ledPin, LOW);  // Turn off the LED
                digitalWrite(buzzer, LOW);
            }

			rsc.close();


		}else if (typeStr == "A"){
			ResponseStructContainer rsc = e32ttl.receiveMessage(sizeof(Message2));
			struct Message2 message2 = *(Message2*) rsc.data;

			Serial.println(message2.heart_rate);
      Serial.println(message2.spo2);
      Serial.println(message2.temp);
      Serial.println(message2.button);


       int heartRate = message2.heart_rate;
      int spo2 = message2.spo2;
       float temp = message2.temp;
       int button  =  message2.button;
      Firebase.setInt(fbdo, "/User/lora1/HeartRate", heartRate);
      Firebase.setInt(fbdo, "/User/lora1/Spo2", spo2);
     Firebase.setFloat(fbdo, "/User/lora1/Temp", temp);
     Firebase.setInt(fbdo, "/User/lora1/button", button);



      int buttonValue = (int)message2.button;
      if (buttonValue != 0) {
                digitalWrite(ledPin, HIGH);
                 digitalWrite(buzzer, HIGH);
                 delay(2000);
            } else {
                digitalWrite(ledPin, LOW);  // Turn off the LED
                digitalWrite(buzzer, LOW);
            }

			rsc.close();

		}else{
			Serial.println("Something goes wrong!!");
		}
	
      
}}



void printParameters(struct Configuration configuration) {
  Serial.println("----------------------------------------");

  Serial.print(F("HEAD : "));  Serial.print(configuration.HEAD, BIN);Serial.print(" ");Serial.print(configuration.HEAD, DEC);Serial.print(" ");Serial.println(configuration.HEAD, HEX);
  Serial.println(F(" "));
  Serial.print(F("AddH : "));  Serial.println(configuration.ADDH, DEC);
  Serial.print(F("AddL : "));  Serial.println(configuration.ADDL, DEC);
  Serial.print(F("Chan : "));  Serial.print(configuration.CHAN, DEC); Serial.print(" -> "); Serial.println(configuration.getChannelDescription());
  Serial.println(F(" "));
  Serial.print(F("SpeedParityBit     : "));  Serial.print(configuration.SPED.uartParity, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTParityDescription());
  Serial.print(F("SpeedUARTDatte  : "));  Serial.print(configuration.SPED.uartBaudRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTBaudRate());
  Serial.print(F("SpeedAirDataRate   : "));  Serial.print(configuration.SPED.airDataRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getAirDataRate());

  Serial.print(F("OptionTrans        : "));  Serial.print(configuration.OPTION.fixedTransmission, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getFixedTransmissionDescription());
  Serial.print(F("OptionPullup       : "));  Serial.print(configuration.OPTION.ioDriveMode, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getIODroveModeDescription());
Serial.print(F("OptionWakeup       : "));  Serial.print(configuration.OPTION.wirelessWakeupTime, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getWirelessWakeUPTimeDescription());
  Serial.print(F("OptionFEC          : "));  Serial.print(configuration.OPTION.fec, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getFECDescription());
  Serial.print(F("OptionPower        : "));  Serial.print(configuration.OPTION.transmissionPower, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getTransmissionPowerDescription());

  Serial.println("----------------------------------------");

}

void printModuleInformation(struct ModuleInformation moduleInformation) {
  Serial.println("----------------------------------------");
  Serial.print(F("HEAD BIN: "));
  Serial.print(moduleInformation.HEAD, BIN);
  Serial.print(" ");
  Serial.print(moduleInformation.HEAD, DEC);
  Serial.print(" ");
  Serial.println(moduleInformation.HEAD, HEX);

  Serial.print(F("Freq.: "));
  Serial.println(moduleInformation.frequency, HEX);
  Serial.print(F("Version  : "));
  Serial.println(moduleInformation.version, HEX);
  Serial.print(F("Features : "));
  Serial.println(moduleInformation.features, HEX);
  Serial.println("----------------------------------------");
}