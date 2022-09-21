#include <Wire.h>
#include <Adafruit_INA219.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Servo.h>
Servo myservo;
Adafruit_INA219 ina219;

uint8_t broadcastAddress[] = {0x60, 0x55, 0xF9, 0x21, 0xCC, 0xD4}; //C3 MINI MAC Address on Remote Control

int LiPo=A7, Voltage, Voltage_Mapped; // Read the (divided by 2) Battery voltage from Analog Pin 7
unsigned char Battery;
float current_mA;
unsigned int I_mA;
int Receive_Timeout;
// Steering Wheel initally alignment 
const int SERVO_QUIESCENT = 90; 
// 4WD Movement
const int pwmMOTOR = 13;  // Forward/Backward PWM pin
const int dirMOTOR = 12;  // Forward/Backward DIR pin
const int pwmChannel_MOTOR = 2; //!!//SERVO is attached to pwm channel 2
// PWM settings  
const int pwmFreq = 50;  
const int pwmResolution = 8; // 8bits (0-255)
// Servo is initially set at 90 degrees
unsigned char servoValue=SERVO_QUIESCENT;

// Variables to store incoming readings
int FW_BW, L_R, L_R_Adj;

// Variables to drive the RC Car
int CAR_FW, CAR_BW, CAR_LEFT, CAR_RIGHT;



// Structure holding values to be sent by the REMOTE CONTROL
typedef struct RemoteControl_Outgoing {
    signed char ForwardBackward;
    signed char LeftRight;
    signed char LeftRight_Adj;
} RemoteControl_Outgoing;
RemoteControl_Outgoing CAR;

// Create a struct_message to hold incoming readings sent by the REMOTE CONTROL
RemoteControl_Outgoing incomingReadings;

// Structure holding values to be sent by the RC CAR
typedef struct CAR_Outgoing {
    unsigned char BatVoltage;
    unsigned int  Current;
} CAR_Outgoing;
CAR_Outgoing REMOTE_CONTROL;


esp_now_peer_info_t peerInfo;


// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  Receive_Timeout=0;
  
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  FW_BW = incomingReadings.ForwardBackward;
  L_R = incomingReadings.LeftRight;
  L_R_Adj = incomingReadings.LeftRight_Adj;
  //!!//Serial.println ( (String) "FW_BW=" + FW_BW + '\t' + '\t' + "L_R=" + L_R + '\t' + '\t' + "L_R_Adj=" + L_R_Adj ); 
  
 // Prepare PWM configuration for the RC Car (Forward: 1 to 33 , Backward: -1 to -23  , Left: 1 to 25 , Right: -1 to -31)
  if (FW_BW>0)    CAR_FW = map(FW_BW,1,33,1,255); 
  if (FW_BW<0)    CAR_BW = map(FW_BW,-1,-23,1,255);
  if (FW_BW==0)   CAR_BW = CAR_FW = 0;
  if (L_R>0)      CAR_LEFT = map(L_R,1,25,1,35);    //1,255); // Don't let servo to turn from 0-180 (instead from 1-255 map to 1-35);
  if (L_R<0)      CAR_RIGHT = map(L_R,-1,-31,1,35); //1,255); // thus the SERVO will turn from 90-35 to 90+35
  if (L_R==0)     CAR_RIGHT = CAR_LEFT = 0;
  Serial.println ( (String) '\t' + "CAR_FW=" + CAR_FW + '\t' + "CAR_BW=" + CAR_BW + '\t' + "CAR_LEFT=" + CAR_LEFT + '\t' + "CAR_RIGHT=" + CAR_RIGHT );
 // 4WD Movement: generate PWM signal
  if (CAR_FW>0 && CAR_BW==0) {
    digitalWrite(dirMOTOR,LOW);         //Forward
    ledcWrite(pwmChannel_MOTOR, (CAR_FW+CAR_BW) );
  }
  else if (CAR_BW>0 && CAR_FW==0) {
    digitalWrite(dirMOTOR,HIGH);        //Backward
    ledcWrite(pwmChannel_MOTOR, (CAR_FW+CAR_BW) );
  }
  else ledcWrite(pwmChannel_MOTOR, 0 );
 // Steering Wheel: Prepare & Write Servo value
  if (CAR_LEFT>0 && CAR_RIGHT==0) {
    servoValue = (unsigned char) (SERVO_QUIESCENT-CAR_LEFT+L_R_Adj) ;
  }
  else if (CAR_RIGHT>0 && CAR_LEFT==0) {
    servoValue = (unsigned char) (SERVO_QUIESCENT+CAR_RIGHT+L_R_Adj);
  }
  else servoValue = (unsigned char)(SERVO_QUIESCENT+L_R_Adj);
  myservo.write(servoValue); // Write Servo value
  //!!//Serial.println (servoValue);
  
  // Prepare Voltage and Current Measurements
  Voltage = analogRead(LiPo);
  Voltage_Mapped = map(Voltage,1,4095,1,63);
  Battery = (unsigned char)Voltage_Mapped; 
  //read Current
  float current_mA = ina219.getCurrent_mA();
  I_mA = (unsigned int)current_mA; 
  // Transmit Voltage and Current data to the Remote Control device
  REMOTE_CONTROL.BatVoltage = Battery;
  REMOTE_CONTROL.Current = I_mA;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &REMOTE_CONTROL, sizeof(REMOTE_CONTROL));
  
}

  // Callback when data is sent
  void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  }

  

 
void setup() {
  
  ina219.begin();
  
  // 4WD Movement
  pinMode(dirMOTOR, OUTPUT);                             // Forward/Backward DIR pin
  ledcSetup(pwmChannel_MOTOR, pwmFreq, pwmResolution);   // configure PWM 
  ledcAttachPin(pwmMOTOR, pwmChannel_MOTOR);             // attach PWM channel to the GPIO pin to be controlled
  // Steering Wheel
  myservo.attach(27);     // attach Servo pin
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_now_init();
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);  // Register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);                     // Add peer        
      
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

}
 
void loop() {
  Receive_Timeout++;
  if (Receive_Timeout > 200000) {
    ledcWrite(pwmChannel_MOTOR, 0 );
    Receive_Timeout=200000; // stall counter (@ value higher than 200000) until RF communication is restored (about 2sec delay)
  }
}
  

  
