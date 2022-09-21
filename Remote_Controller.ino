#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include "SSD1306Wire.h"
SSD1306Wire display(0x3d, SDA, SCL);   // I2C Addr = 0x3D
typedef void (*Demo)(void);
int demoMode = 0;

uint8_t broadcastAddress[] = {0x78, 0xE3, 0x6D, 0x1B, 0x1B, 0xC8}; //D1R32 MAC Address on RC Car

float Battery;
int Battery_Percentage;
unsigned int I_mA;
int Wheels=A2, Steering=A1, ServoAdj=A0;  // Define Analog Channels
int ADC_Resolution = 4095;                //ADC InputRange: 0-3V3; thus lsb=InputRange/2^12=0.8mV                
int ADC_Wheels, ADC_Steering, ADC_ServoAdj, Wheels_Mapped, Steering_Mapped, ServoAdj_Mapped; 
int FW_BW, L_R; 
const int SteeringQ=35, WheelsQ=27; // quiescent point steering & wheels
const int Hysteresis = 3;           // add hysteresis from quiescent point


// Structure holding values to be sent by the REMOTE CONTROL
typedef struct RemoteControl_Outgoing {
    signed char ForwardBackward;
    signed char LeftRight;
    signed char LeftRight_Adj;
} RemoteControl_Outgoing;
RemoteControl_Outgoing CAR;

// Structure holding values to be sent by the RC CAR
typedef struct CAR_Outgoing {
    unsigned char BatVoltage;
    unsigned int  Current;
} CAR_Outgoing;
CAR_Outgoing REMOTE_CONTROL;

// Create a struct_message to hold incoming readings sent by the RC CAR
CAR_Outgoing incomingReadings;


esp_now_peer_info_t peerInfo;


// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Battery = (float) incomingReadings.BatVoltage;
  Battery = (Battery * 3.3/63.0) * 4.0; // ADC for battery is mapped in the RC Car system from 1-4095 to 1-31(0.1V resolution)
  Battery_Percentage = (int)(Battery*100.0);
  Battery_Percentage = map(Battery_Percentage,640,840,0,100);
  //Serial.println(incomingReadings.Current);
  //!!//Serial.println((String) "Battery=" + Battery + '\t' + "Battery_Percentage=" + Battery_Percentage + '%' + '\t' + "Current=" + incomingReadings.Current); 
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}



void setup() { 
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10); 
  //!!//Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_now_init();
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  esp_now_register_send_cb(OnDataSent);              // Get the status of Trasnmitted packet
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);   // Register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);                      // Add peer
  esp_now_register_recv_cb(OnDataRecv);    // Register for a callback function that will be called when data is received
}

void drawProgressBarDemo() { 
  // draw the progress bar
  display.drawProgressBar(0, 0/*32*/, 120, 10, Battery_Percentage);
  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, "BATTERY= " + String(Battery_Percentage) + "%");
}
Demo demos[] = {drawProgressBarDemo}; ///Demo demos[] = {drawFontFaceDemo, drawTextFlowDemo, drawTextAlignmentDemo, drawRectDemo, drawCircleDemo, drawProgressBarDemo, drawImageDemo};
int demoLength = (sizeof(demos) / sizeof(Demo));


void loop() {
  if (incomingReadings.Current == 0) Battery=0;
  
  // based on a revision of ..Arduino\libraries\esp8266-oled-ssd1306-master\examples\SSD1306SimpleDemo
  display.clear();
  // draw the current demo method
  demos[demoMode]();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 48, String(Battery) + "V"); // Battery Voltage
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(1, 48, String(incomingReadings.Current) + "mA" );///display.drawString(128, 54, String(millis()));
  // write the buffer to the display
  display.display();
    
  //Steering Wheel 
  ADC_Steering = analogRead(Steering);
  Steering_Mapped = map(ADC_Steering,160,4095,1,63); //Decrease Resolution of Steering Wheels
  //Serial.println((String) "Steering_Mapped=" + Steering_Mapped); 
  if (Steering_Mapped > SteeringQ+Hysteresis) L_R = Steering_Mapped - (SteeringQ+Hysteresis);     // LEFT (positive value)
  else if (Steering_Mapped < SteeringQ-Hysteresis) L_R = (Steering_Mapped-SteeringQ+Hysteresis);  //RIGHT (negative value)
  else L_R=0;

  //4WD Movement
  ADC_Wheels = analogRead(Wheels);  
  Wheels_Mapped = map(ADC_Wheels,1270,4095,1,63); //Decrease Resolution of Wheels
  if (Wheels_Mapped > WheelsQ+Hysteresis) FW_BW = Wheels_Mapped - (WheelsQ+Hysteresis);    //FORWARD  (positive value)
  else if (Wheels_Mapped < WheelsQ-Hysteresis) FW_BW = (Wheels_Mapped-WheelsQ+Hysteresis); //BACKWARD (negative value)
  else FW_BW=0;

  //Servo adjustment (Steering Wheel alignment) 
  ADC_ServoAdj = analogRead(ServoAdj); 
  ServoAdj_Mapped = map(ADC_ServoAdj,0,4095,0,32); //Decrease Resolution of Servo trimming value
  ServoAdj_Mapped -= 16;


  //Serial.println((String) "Forward/Backward=" + FW_BW + '\t' + "Left/Right=" + L_R + '\t' + "Steering Alignment=" + ServoAdj_Mapped); 

  // Prepare & Transmit data to the RC Car
  CAR.ForwardBackward = (signed char)FW_BW;
  CAR.LeftRight = (signed char)L_R;
  CAR.LeftRight_Adj =  (signed char)(ServoAdj_Mapped);
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &CAR, sizeof(CAR));
}
