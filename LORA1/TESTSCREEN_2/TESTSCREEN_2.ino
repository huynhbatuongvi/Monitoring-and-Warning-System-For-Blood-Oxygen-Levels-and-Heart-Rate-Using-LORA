#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "LoRa_E32.h"
#include <WiFi.h> //TB
#include <PubSubClient.h> //TB
#include <FirebaseESP32.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>
#include <HardwareSerial.h>            
#define MCU_SIM_BAUDRATE        115200
HardwareSerial simSerial(1);
HardwareSerial mySerial(2);
#define MCU_SIM_EN_PIN              15
FirebaseData fbdo;

#define WIFI_SSID "Quanghuy"
#define WIFI_PASSWORD "quanghuy1402"

const char* FIREBASE_HOST  = "https://heartrate-2cd32-default-rtdb.asia-southeast1.firebasedatabase.app/";
const char* FIREBASE_AUTH  = "3GGDdrNr0R0TlraTq6DEaHOKPa9WtQYYncJhXLTu";
// Please update number before test
// #define PHONE_NUMBER                "+84355198659"
String PHONE_NUMBER = "";


#define ENABLE_MAX30100 1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32

#define OLED_RESET     -1 // 4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32at
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#if ENABLE_MAX30100
#define REPORTING_PERIOD_MS  6000
#define buttonPinScreen 12
#define buttonWarn 14
#define ledPin 2
PulseOximeter pox;
#endif
uint32_t tsLastReport = 0;
int xPos = 0;
LoRa_E32 e32ttl(&mySerial,5,19,18); // Arduino RX <-- e32 TX, Arduino TX --> e32 RX

Adafruit_MLX90614 mlx = Adafruit_MLX90614();



void sim_at_wait()
{

    while (simSerial.available()) {
        Serial.write(simSerial.read());
    }
}

bool sim_at_cmd(String cmd){
    simSerial.println(cmd);
    sim_at_wait();
    return true;
}

bool sim_at_send(char c){
    simSerial.write(c);
    return true;
}

void sent_sms()
{
    sim_at_cmd("AT+CMGF=1");
    String temp = "AT+CMGS=\"";
    temp += (String)PHONE_NUMBER;
    temp += "\"";
    sim_at_cmd(temp);
    sim_at_cmd("Health Warning");

    // End charactor for SMS
    sim_at_send(0x1A);
}

void call()
{
    String temp = "ATD";
    temp += PHONE_NUMBER;
    temp += ";";
    sim_at_cmd(temp); 

   

    // Hang up
    sim_at_cmd("ATH"); 
}


void onBeatDetected()
{
  Serial.println("Beat!");
  heart_beat(&xPos);
}

void setupMLX90614() {
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX90614");
    while (1);
  }
  Serial.println("MLX90614 connected");
}

void setupScreen(){
   Serial.println("SSD1306 128x64 OLED TEST");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  //display.display();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 18);
  // Display static text
  display.print("Pulse OxiMeter");
  int temp1 = 0;
  int temp2 = 40;
  int temp3 = 80;
  heart_beat(&temp1);
  heart_beat(&temp2);
  heart_beat(&temp3);
  xPos = 0;
  display.display();
  //delay(2000); // Pause for 2 seconds
  display.cp437(true);
  display.clearDisplay();
  Serial.print("Initializing pulse oximeter..");
#if ENABLE_MAX30100
  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }
  // The default current for the IR LED is 50mA and it could be changed
  //   by uncommenting the following line. Check MAX30100_Registers.h for all the
  //   available options.
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  // Register a callback for the beat detection
pox.setOnBeatDetectedCallback(onBeatDetected);
  display_data(0, 0);
#endif
}

void setupLora(){
   e32ttl.begin();
  ResponseStructContainer c;
  c = e32ttl.getConfiguration();
  Configuration configuration = *(Configuration*)c.data;
    configuration.ADDL = 0x01;
    configuration.ADDH = 0x01;
    configuration.CHAN = 0x02;

    configuration.OPTION.fec = FEC_1_ON;
    configuration.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;
    configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
    configuration.OPTION.transmissionPower = POWER_14;       //2'b10 24dBm
    configuration.OPTION.wirelessWakeupTime = WAKE_UP_1000;  //3'b011 1000ms

    configuration.SPED.airDataRate = AIR_DATA_RATE_011_48;  //3'b011 4.8kbps
    configuration.SPED.uartBaudRate = UART_BPS_9600;
    configuration.SPED.uartParity = MODE_00_8N1;

    ResponseStatus rs = e32ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
  
//     Serial.print("Lora Configuration: ");
//  Serial.println(rs.getResponseDescription());
//  printParameters(configuration);
  c.close();
}

void setupWifi(){
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
}

void setup()
{
Serial.begin(9600);
mySerial.begin(9600, SERIAL_8N1, 16, 17);
  setupLora();
  setupWifi();
  setupScreen();
  setupMLX90614();
  
  // initialize the pushbutton pin as an input:
   pinMode(buttonPinScreen, INPUT_PULLUP);
   pinMode(buttonWarn, INPUT_PULLUP);

   /*  Power enable  */
    pinMode(MCU_SIM_EN_PIN,OUTPUT); 
    digitalWrite(MCU_SIM_EN_PIN,LOW);

    Serial.println("\n\n\n\n-----------------------\nSystem started!!!!");

    // Delay 8s for power on
  
    simSerial.begin(MCU_SIM_BAUDRATE, SERIAL_8N1, 4, 2);

    // Check AT Command
    sim_at_cmd("AT");

    // Product infor
    sim_at_cmd("ATI");

    // Check SIM Slot
    sim_at_cmd("AT+CPIN?");

    // Check Signal Quality
    sim_at_cmd("AT+CSQ");

    sim_at_cmd("AT+CIMI");

    pinMode(2,OUTPUT); 
    digitalWrite(2,HIGH);

    // pinMode(btn_1,INPUT_PULLUP); 
    // pinMode(btn_2,INPUT_PULLUP); 
      if (Firebase.getString(fbdo, "/User/lora1/Thông tin/Phone Number")) {
    if (fbdo.dataType() == "string") {
      PHONE_NUMBER = fbdo.stringData();
      Serial.println("Phone Number: " + PHONE_NUMBER);
    }
  } else { 
    Serial.println("Failed to get phone number from Firebase");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}


 

int  i =0;
//  // Variable to count button presses
// unsigned long lastButtonPress = 0;
// unsigned long buttonPressStart = 0;  // Variable to track button press start time
// const unsigned long buttonHoldTime = 50; 
// bool onSecondScreen = false;
// int buttonStateScreen = 0;



int buttonState = 0;
int buttonPressCount_warn = 0; // Variable to count button presses
unsigned long lastButtonPress_warn = 0;
unsigned long buttonPressStart_warn = 0;  // Variable to track button press start time
const unsigned long buttonHoldTime_warn = 1000; 


const unsigned long buttonHoldTime = 500; // Example hold time in milliseconds
const unsigned long debounceDelay = 50; // Debounce time in milliseconds

unsigned long buttonPressStart = 0;
unsigned long lastDebounceTime = 0;
bool onSecondScreen = false;
bool lastButtonState = HIGH; // Initial state for a pull-up resistor
bool buttonStateScreen;

void loop(){

  if (Serial.available()){
        char c = Serial.read();
        simSerial.write(c);
    }
    sim_at_wait();

  #if ENABLE_MAX30100
  pox.update();
  int bpm =0;
  int spo2 =0;
  // buttonStateLora = digitalRead(buttonPinLora);
   int reading = digitalRead(buttonPinScreen);
  unsigned long currentMillis = millis();

  // Debounce handling
  if (reading != lastButtonState) {
    lastDebounceTime = currentMillis; // Reset the debounce timer
  }

  if ((currentMillis - lastDebounceTime) > debounceDelay) {
    buttonStateScreen = reading;

    // Xử lý nút bấm chuyển màn hình
    if (buttonStateScreen == LOW) { // Button is pressed
      if (buttonPressStart == 0) {
        buttonPressStart = currentMillis;
      } else if (currentMillis - buttonPressStart >= buttonHoldTime) {
        onSecondScreen = !onSecondScreen;

        buttonPressStart = 0;
        display.clearDisplay();
        display.display(); // Xóa màn hình mỗi khi chuyển đổi
      }
    } else { // Button is not pressed
      buttonPressStart = 0;
    }
  }
  lastButtonState = reading; // Update the button state


buttonState = digitalRead(buttonWarn );

 if (buttonState == LOW) {
        if (buttonPressStart_warn == 0) {
            buttonPressStart_warn = currentMillis;  // Start timing the button press
        } else if (currentMillis - buttonPressStart_warn >= buttonHoldTime_warn) {
            // Button has been held for the required time
            buttonPressCount_warn++;
            if(buttonPressCount_warn == 1)
            {
               call();
              // sent_sms();
              
            }
            buttonPressStart_warn = 0;  // Reset the button press start time
            lastButtonPress_warn = currentMillis;  // Update last button press time for debounce
        }
    } else {
        buttonPressStart_warn = 0;  // Reset the button press start time if button is not pressed
    }
  //  Serial.print(buttonPressCount_warn);



 float objectTemp = mlx.readObjectTempC();

  if (!onSecondScreen) {
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      bpm = pox.getHeartRate();
      spo2 = pox.getSpO2();
      display_data(bpm, spo2);
      tsLastReport = millis();

      
      struct Message {
        char type[5] = "B";
        byte hear_rate;
        byte sp02;
        byte object_temp;;
        byte button_1;
        
      
      } message;
      message.hear_rate = bpm;
      message.sp02 = spo2;
      message.object_temp = objectTemp;
      message.button_1 = buttonPressCount_warn;
     
      ResponseStatus rs = e32ttl.sendFixedMessage(0x03,0x03,0x04, &message, sizeof(Message));
      Serial.println(rs.getResponseDescription());
    
      buttonPressCount_warn = 0;

    }
    drawLine(&xPos);
  } else {

    
    displayTemperature();
  }

#endif
  // drawLine(&xPos);
}


void display_data(int bpm, int spo2) {
  display.fillRect(0, 18, 127, 15, SSD1306_BLACK);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 18);
  // Display static text
  display.print("BPM ");
  display.print(bpm);
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(64, 18);
  // Display static text
  display.print("Spo2% ");
  display.println(spo2);
  display.display();

}
void drawLine(int *x_pos) {
  // Draw a single pixel in white
  display.drawPixel(*x_pos, 8, SSD1306_WHITE);
  display.drawPixel((*x_pos)++, 8, SSD1306_WHITE);
  display.drawPixel((*x_pos)++, 8, SSD1306_WHITE);
  display.drawPixel((*x_pos)++, 8, SSD1306_WHITE);
  display.drawPixel((*x_pos), 8, BLACK);  // -----
  //Serial.println(*x_pos);
  display.fillRect(*x_pos, 0, 31, 16, SSD1306_BLACK);
  display.display();
  
  if (*x_pos >= SCREEN_WIDTH) {
    *x_pos = 0;
  }
}
void heart_beat(int *x_pos) {
  /************************************************/
  //display.clearDisplay();
  display.fillRect(*x_pos, 0, 30, 15, SSD1306_BLACK);
  // Draw a single pixel in white
  display.drawPixel(*x_pos + 0, 8, SSD1306_WHITE);
  display.drawPixel(*x_pos + 1, 8, SSD1306_WHITE);
  display.drawPixel(*x_pos + 2, 8, SSD1306_WHITE);
  display.drawPixel(*x_pos + 3, 8, SSD1306_WHITE);
  display.drawPixel(*x_pos + 4, 8, BLACK); // -----
  //display.display();
  delay(1);
  display.drawPixel(*x_pos + 5, 7, SSD1306_WHITE);
  display.drawPixel(*x_pos + 6, 6, SSD1306_WHITE);
  display.drawPixel(*x_pos + 7, 7, SSD1306_WHITE); // .~.
  //display.display();
  delay(1);
  display.drawPixel(*x_pos + 8, 8, SSD1306_WHITE);
  display.drawPixel(*x_pos + 9, 8, SSD1306_WHITE); // --
  //display.display();
  delay(1);
  /******************************************/
  display.drawPixel(*x_pos + 10, 8, SSD1306_WHITE);
  display.drawPixel(*x_pos + 10, 9, SSD1306_WHITE);
  display.drawPixel(*x_pos + 11, 10, SSD1306_WHITE);
display.drawPixel(*x_pos + 11, 11, SSD1306_WHITE);
  //display.display();
  delay(1);
  /******************************************/
  display.drawPixel(*x_pos + 12, 10, SSD1306_WHITE);
  display.drawPixel(*x_pos + 12, 9, SSD1306_WHITE);
  display.drawPixel(*x_pos + 12, 8, SSD1306_WHITE);
  display.drawPixel(*x_pos + 12, 7, SSD1306_WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 13, 6, SSD1306_WHITE);
  display.drawPixel(*x_pos + 13, 5, SSD1306_WHITE);
  display.drawPixel(*x_pos + 13, 4, SSD1306_WHITE);
  display.drawPixel(*x_pos + 13, 3, SSD1306_WHITE);
  //display.display();
  delay(1);
  display.drawPixel(*x_pos + 14, 2, SSD1306_WHITE);
  display.drawPixel(*x_pos + 14, 1, SSD1306_WHITE);
  display.drawPixel(*x_pos + 14, 0, SSD1306_WHITE);
  display.drawPixel(*x_pos + 14, 0, SSD1306_WHITE);
  //display.display();
  //delay(1);
  /******************************************/
  display.drawPixel(*x_pos + 15, 0, SSD1306_WHITE);
  display.drawPixel(*x_pos + 15, 1, SSD1306_WHITE);
  display.drawPixel(*x_pos + 15, 2, SSD1306_WHITE);
  display.drawPixel(*x_pos + 15, 3, SSD1306_WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 15, 4, SSD1306_WHITE);
  display.drawPixel(*x_pos + 15, 5, SSD1306_WHITE);
  display.drawPixel(*x_pos + 16, 6, SSD1306_WHITE);
  display.drawPixel(*x_pos + 16, 7, SSD1306_WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 16, 8, SSD1306_WHITE);
  display.drawPixel(*x_pos + 16, 9, SSD1306_WHITE);
  display.drawPixel(*x_pos + 16, 10, SSD1306_WHITE);
  display.drawPixel(*x_pos + 16, 11, SSD1306_WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 17, 12, SSD1306_WHITE);
  display.drawPixel(*x_pos + 17, 13, SSD1306_WHITE);
  display.drawPixel(*x_pos + 17, 14, SSD1306_WHITE);
  display.drawPixel(*x_pos + 17, 15, SSD1306_WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 18, 15, SSD1306_WHITE);
  display.drawPixel(*x_pos + 18, 14, SSD1306_WHITE);
  display.drawPixel(*x_pos + 18, 13, SSD1306_WHITE);
  display.drawPixel(*x_pos + 18, 12, SSD1306_WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 19, 11, SSD1306_WHITE);
  display.drawPixel(*x_pos + 19, 10, SSD1306_WHITE);
  display.drawPixel(*x_pos + 19, 9, SSD1306_WHITE);
  display.drawPixel(*x_pos + 19, 8, SSD1306_WHITE);
  //display.display();
  //delay(1);
  /****************************************************/
  display.drawPixel(*x_pos + 20, 8, SSD1306_WHITE);
  display.drawPixel(*x_pos + 21, 8, SSD1306_WHITE);
  //display.display();
  //delay(1);
  /****************************************************/
  display.drawPixel(*x_pos + 22, 7, SSD1306_WHITE);
  display.drawPixel(*x_pos + 23, 6, SSD1306_WHITE);
  display.drawPixel(*x_pos + 24, 6, SSD1306_WHITE);
  display.drawPixel(*x_pos + 25, 7, SSD1306_WHITE);
  //display.display();
  //delay(1);
  /************************************************/
display.drawPixel(*x_pos + 26, 8, SSD1306_WHITE);
  display.drawPixel(*x_pos + 27, 8, SSD1306_WHITE);
  display.drawPixel(*x_pos + 28, 8, SSD1306_WHITE);
  display.drawPixel(*x_pos + 29, 8, SSD1306_WHITE);
  display.drawPixel(*x_pos + 30, 8, SSD1306_WHITE); // -----
  *x_pos = *x_pos + 30;
  display.display();
 

}  
void displayTemperature() {
  double objectTemp = mlx.readObjectTempC();
  double ambientTemp = mlx.readAmbientTempC();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Object Temp: ");
  display.print(objectTemp);
  display.println(" C");
  display.setCursor(0, 16);
  display.print("Ambient Temp: ");
  display.print(ambientTemp);
  display.println(" C");
  display.display();
}

   
// void printParameters(struct Configuration configuration) {
//   Serial.println("----------------------------------------");

//   Serial.print(F("HEAD : "));  Serial.print(configuration.HEAD, BIN);Serial.print(" ");Serial.print(configuration.HEAD, DEC);Serial.print(" ");Serial.println(configuration.HEAD, HEX);
//   Serial.println(F(" "));
//   Serial.print(F("AddH : "));  Serial.println(configuration.ADDH, DEC);
//   Serial.print(F("AddL : "));  Serial.println(configuration.ADDL, DEC);
//   Serial.print(F("Chan : "));  Serial.print(configuration.CHAN, DEC); Serial.print(" -> "); Serial.println(configuration.getChannelDescription());
//   Serial.println(F(" "));
//   Serial.print(F("SpeedParityBit     : "));  Serial.print(configuration.SPED.uartParity, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTParityDescription());
//   Serial.print(F("SpeedUARTDatte  : "));  Serial.print(configuration.SPED.uartBaudRate, BIN);Serial.print(" -> "); Serial.printl\n(configuration.SPED.getUARTBaudRate());
//   Serial.print(F("SpeedAirDataRate   : "));  Serial.print(configuration.SPED.airDataRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getAirDataRate());

//   Serial.print(F("OptionTrans        : "));  Serial.print(configuration.OPTION.fixedTransmission, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getFixedTransmissionDescription());
//   Serial.print(F("OptionPullup       : "));  Serial.print(configuration.OPTION.ioDriveMode, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getIODroveModeDescription());
//   Serial.print(F("OptionWakeup       : "));  Serial.print(configuration.OPTION.wirelessWakeupTime, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getWirelessWakeUPTimeDescription());
//   Serial.print(F("OptionFEC          : "));  Serial.print(configuration.OPTION.fec, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getFECDescription());
//   Serial.print(F("OptionPower        : "));  Serial.print(configuration.OPTION.transmissionPower, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getTransmissionPowerDescription());

//   Serial.println("----------------------------------------");

// }

// void printModuleInformation(struct ModuleInformation moduleInformation) {
//   Serial.println("----------------------------------------");
//   Serial.print(F("HEAD BIN: "));
// Serial.print(moduleInformation.HEAD, BIN);
//   Serial.print(" ");
//   Serial.print(moduleInformation.HEAD, DEC);
//   Serial.print(" ");
//   Serial.println(moduleInformation.HEAD, HEX);

//   Serial.print(F("Freq.: "));
//   Serial.println(moduleInformation.frequency, HEX);
//   Serial.print(F("Version  : "));
//   Serial.println(moduleInformation.version, HEX);
//   Serial.print(F("Features : "));
//   Serial.println(moduleInformation.features, HEX);
//   Serial.println("----------------------------------------");
// }