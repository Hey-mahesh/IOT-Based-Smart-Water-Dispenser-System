📌 IoT-Based Smart Water Dispenser System
📖 Overview

This project presents an IoT-based smart water dispenser system that automatically dispenses a precise amount of water using weight measurement. It combines embedded systems and cloud connectivity to enable accurate dispensing, real-time monitoring, and remote control.

🚀 Features
Automatic water dispensing based on weight
High accuracy using Load Cell + HX711
Real-time monitoring via cloud
Remote control using WiFi (NodeMCU)
Reliable communication using RS485
Low-cost and scalable system

🛠️ Components Used
Arduino Nano
NodeMCU (ESP8266)
Load Cell
HX711 Amplifier Module
RS485 Module
Water Pump / Motor
Power Supply

⚙️ Working Principle
The load cell measures the weight of the container.
HX711 converts the analog signal into digital form.
Arduino Nano processes the data and controls the system.
The motor/pump dispenses water until the desired weight is reached.
NodeMCU sends data to the cloud for monitoring and control.

🏗️ System Architecture

Sensing Layer: Load Cell + HX711
Processing Layer: Arduino Nano
Communication Layer: RS485
IoT Layer: NodeMCU
Actuation Layer: Motor/Pump
📂 Project Structure
IoT-Water-Dispenser-System/
│
├── code/
│   ├── arduino_nano/
│   ├── nodemcu/
│
├── connections/
│   ├── block_diagram.png
│   ├── circuit_diagram.png
│
├── docs/
│   ├── report/
│
└── README.md

📊 Applications
Water vending machines
Industrial liquid dispensing
Smart home automation
Automated filling systems

📈 Future Scope
Mobile app integration
Payment-based water dispensing
AI-based usage monitoring
Solar-powered system

🧠 Technologies Used
Embedded Systems
IoT (Internet of Things)
RS485 Communication
Cloud Integration

👨‍💻 Author
Mahesh Phand
