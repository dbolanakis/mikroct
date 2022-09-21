# Rebuild An Rc Car With Esp32 & Arduino Code
The present document provides all the information needed to rebuild an RC CAR from scratch using ESP32-based microcontroller units (MCUs), programmed in Arduino environment. The current project applies to the 1:16 Xinlehong (XLH)9136 model, but can be easily adapted to any RC Car model that incorporates:

a) DC motor for the forward/backward (FW/BW) movement of the car,

b) 3-wireServo Motor (operating at 5V) for the control of the steering wheel,

c) 2S LiPo battery (of 7.4 nominal voltage or perhaps of higher voltage if taking into consideration the specifications and limitations of the boards held by the proposed system).


To complile this project with Arduino IDE you need to install:
1) ESP32 package (run Boards Manager in Arduino IDE)
2) Servo ESP32 library, available at: https://github.com/RoboticsBrno/ServoESP32
3) Adafruit_INA219, available at: https://github.com/adafruit/Adafruit_INA219
4) ThingPulse OLED SSD1306, available at: https://github.com/ThingPulse/esp8266-oled-ssd1306


Follow a step-by-step guide through a playlist of 5 short videos found below:

https://www.youtube.com/channel/UC-wGLN7KYQ-hoKo3BcTTFVg

More code examples for Arduino can be found below:
https://mikroct.com/
