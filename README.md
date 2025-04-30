# Colour-Yeeter

An automated colour sorting conveyor belt with a motorized push-arm mechanism to sort objects based on their colour. Designed for real-time operation, the system utilizes an OV7670 camera for colour detection and motors to control object movement connected to an STM32 micrcontroller. The camera captures the colour of each object before it reaches the sorting mechanism, which then activates the corresponding motorized push arm to direct the object into the correct bin.



## 🚀 Demo
https://github.com/user-attachments/assets/36c271b4-2a6e-4fd6-b317-2e4f87ac8145

## Features
- Conveyor belt system: Moves objects forward using PWM-controlled motors
- Color detection: Captures center pixels from a camera and classifies objects as Red, Green, or Blue
- Motorized sorting arm: Pushes objects into the appropriate bin based on detected color
- LED indicators: Lights up corresponding onboard LED (Red, Green, Blue) for visual feedback
- Efficient performance: Uses DMA for fast camera data capture and processing
- Modular C code: Cleanly organized for the STM32 microcontroller, easy to expand or modify

## 💬 About
This project was created as part of an embedded systems course to explore embedded control using different peripherals. It's an ideal demonstration of using sensors, motors, and real-time processing to build a responsive physical system.


## Team
| Zuha Mujib | Paige McDougal |
| ------------- | ------------- |
| <img src="https://github.com/user-attachments/assets/bdf46a6e-7cd0-4d35-abd7-a113a2601502" width="120"/> | <img src="https://github.com/user-attachments/assets/c69d5e58-8972-4d99-8d4c-5886ccd0dbae" width="120"/> |
| [@zuhamujib](https://github.com/zuhamujib) | [@paigemcdo](https://github.com/paigemcdo) |
