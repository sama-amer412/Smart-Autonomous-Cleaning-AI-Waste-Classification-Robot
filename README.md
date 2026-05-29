# 🧠 Smart Autonomous Cleaning & AI Waste Classification Robot

## 📌 Overview

An IoT-based autonomous cleaning robot that integrates embedded systems, artificial intelligence, and cloud communication to enable intelligent indoor cleaning and real-time waste classification.

#### The system combines:

- Arduino Uno for real-time embedded control and sensor processing  
- ESP8266 module for MQTT-based cloud communication and remote connectivity  
- Deep learning-based AI model (MobileNetV2) for waste classification (plastic, paper, metal, glass, etc.)  

#### 🎯System Goal
- Develop an intelligent low cost autonomous cleaning robot  
- Integrate AI based waste classification with IoT communication
- Improve indoor cleaning efficiency and reduce human effort

---

## ✨ Key Features

- A multi-stage autonomous cleaning cycle that executes sweeping, vacuum cleaning, wetting, and mopping operations  
- Real-time obstacle and edge avoidance using ultrasonic and IR sensors  
- AI-based waste classification using MobileNetV2 deep learning model  
- MQTT-based IoT remote control and communication system  
- Embedded real-time control for navigation and cleaning synchronization  

---

## 🧩 System Architecture Diagram

The diagram below presents the hardware architecture and subsystem interconnections of the smart cleaning robot.


<p align="center">
  <img src="Block_Diagram.jpg" alt="System Firmware Execution Flowchart" width="550">
</p>

---

## ⚙️ System Operation Modes

The robot supports two operational modes controlled remotely through the MQTT cloud dashboard.



### 1️⃣ Manual Mode

In **Manual Mode**, the user can directly control the robot movement and cleaning modules through the web dashboard interface.

#### 🎮 Supported Controls

- Forward / Backward Movement
- Left / Right Turning
- Stop
- Water Pump Control
- Cleaning Module Activation

This mode is mainly used for:

- Direct navigation
- System testing
- Manual cleaning operation



### 2️⃣ Autonomous Mode

In **Autonomous Mode**, the robot performs intelligent navigation and periodic cleaning automatically without user intervention.



#### 🔹 Autonomous Navigation

The robot continuously monitors its surroundings using:

- HC-SR04 Ultrasonic Sensor
- Left & Right IR Obstacle Sensors
- IR Edge Detection Sensor

#### 🧠 Navigation Logic

- Edge detection has the highest priority to prevent falling from edges or stairs.
- Obstacles are avoided using real-time distance evaluation.
- The robot dynamically changes direction based on sensor feedback.



#### 🧼 Multi-Stage Cleaning Cycle

The cleaning system is implemented using a **non-blocking embedded state machine**, allowing cleaning operations and autonomous navigation to run simultaneously.

| Stage | Function |Relay Status   | System Action |
|---|---|---|---|
| Stage 1 | Vacuum Cleaning |Pump Relay: **OFF**<br>Mop Relay: **OFF**<br>Fan: **ON**  | Dust collection and suction fan operation while moving |
| Stage 2 | Water Spraying | Pump Relay: **ON**<br>Mop Relay: **OFF**  | Activates the water pump for wet-cleaning preparation |
| Stage 3 | Mopping |Pump Relay: **OFF**<br>Mop Relay: **ON**  | Mop motor scrubs the surface during controlled movement |

#### ✅ End of Cleaning Cycle

At the end of each cleaning cycle:

- Cleaning modules are disabled
- A buzzer alert is triggered
- The robot returns to autonomous navigation mode



<p align="center">
  <img src="Flowchart.jpg" alt="System Firmware Execution Flowchart" width="500">
</p>

---


## 🌐 Interactive Web Dashboard & Real-Time Cloud Communication

The robot is controlled through a responsive web dashboard designed using **HTML, CSS, and JavaScript**, providing a simple and intuitive user experience. The system enables real-time communication with the robot using MQTT over a cloud-based broker, allowing instant command execution without page refresh.



### ⚙️ Key Features

- Manual Mode and Automatic Mode  
- Automatic Mode includes configurable timer  
- Battery monitoring displaying real-time battery level on the dashboard  
- Password change option for dashboard access security  



### 💡 System Overview

The dashboard acts as the main control interface for the robot, where user commands are sent directly to the cloud and executed instantly by the robot, while system status is returned in real time.



### ☁️ MQTT Cloud Communication Logic

<div style="display: flex; align-items: flex-start; gap: 20px;">

  

  <div style="flex: 2;">

The system uses **MQTT (Message Queuing Telemetry Transport)**, a lightweight publish/subscribe protocol that enables efficient and structured communication between the dashboard and the robot through a cloud broker.

**robot/control (Subscribe)**  
Receives control commands from the dashboard:
- Mode selection (Manual / Automatic)  
- Movement commands (Forward, Backward, Left, Right, Stop)

**robot/telemetry (Publish)**  
Sends robot status updates to the dashboard:
- Battery level , System state  
 

  </div>
  <div style="flex: 2;">
    <img src="communication_flowchart.png" width="150"/>
  </div>

</div>


### 🖼️ Dashboard Preview

<div style="display: flex; align-items: flex-start; gap: 20px;">

  <div style="flex: 2;">
    <img src="site_photo.png" width="370"/>
  </div>

  <div style="flex: 2;">
     <img src="site_photo_2.png" width="370"/>
  </div>
</div>

---
## 🔌 Hardware Configurations & Diagrams

The electrical subsystem interconnections, hardware dependencies, and module abstractions are validated prior to physical prototyping development.

<p align="center">
  <img src="simulation_circuit.jpg" alt="System Circuit Schematic Simulation" width="700">
</p>

---
## 🧠 AI Waste Classification Model (Software Layer)

### **System Overview**
- **Recycle Waste Classifier**: Real-time waste classification system using Deep Learning & Computer Vision
- Classifies waste into: **Plastic, Glass, Metal, Paper, Shoes/Other**
- Built with **TensorFlow** and integrated into **Streamlit** web application

#### **Project Evolution**
- Started with **Custom CNN** model built from scratch
- Achieved only **~66% accuracy**, slow training, and unstable real-time performance
- Switched to **MobileNetV2 Transfer Learning** to overcome limitations
- Result: Significant improvement to **~90%+ accuracy**, faster training, and better stability

---
### **Deep Learning Model**
- Used **MobileNetV2** with Transfer Learning (pre-trained on ImageNet)
- **Why MobileNetV2?**
  - Lightweight & fast for real-time use
  - Efficient on limited hardware
  - Works well with smaller datasets
  - High accuracy

**Model Architecture:**
```python
base_model = MobileNetV2(input_shape=(224, 224, 3), include_top=False, weights='imagenet')
GlobalAveragePooling2D() → Dense(128, relu) → Dropout(0.3) → Dense(num_classes, softmax)
```


#### **Training Results**

- **Train Accuracy**: **95.78%**
- **Validation Accuracy**: **90.06%**

---

### **Streamlit Web Interface**

 Supports: Image upload, Live camera detection, Confidence scores
- Clean and interactive UI
- Displays prediction + probability distribution

**System Examples:**


- **Paper** → Confidence: **0.66**
- **Glass** → Confidence: **0.88**
- **Plastic** → Confidence: **1.00**

---

### **Live Camera Processing**
- Used **`streamlit-webrtc`** for smooth real-time streaming
- Applied **Frame Skipping** (`if frame_count % 10 == 0`) to reduce latency
- Achieved stable live classification

---

### **Cloud Communication (MQTT)**
- Used **MQTT Protocol** for communication with robotic arm
- Lightweight, fast, and ideal for IoT
- Sends prediction results in real-time:
```python
mqtt_client.publish("material", predicted_label)
```

---

### **Model Comparison**

| Feature                  | Custom CNN     | **MobileNetV2** (Final) |
|--------------------------|----------------|--------------------------|
| Accuracy                 | ~66%           | **~90%+**                |
| Training Speed           | Slow           | **Much Faster**          |
| Stability                | Less stable    | **Highly Stable**        |
| Dataset Requirement      | Large          | **Limited data**         |
| Real-Time Performance    | Poor           | **Optimized**            |

---



  






## 🔌 Hardware Components & System Roles

| Hardware Component | Function in System |
|-------------------|------------------|
| **Arduino Uno** | Main microcontroller responsible for real-time control of sensors, motors, and cleaning logic execution |
| **ESP8266 WiFi Module (ESP-01)** | Handles IoT communication using MQTT protocol and connects robot to cloud/dashboard |
| **HC-SR04 Ultrasonic Sensor** | Detects front obstacles by measuring distance for collision avoidance |
| **IR Obstacle Sensors (Left & Right)** | Detect nearby side obstacles for real-time directional correction |
| **IR Edge Sensor** | Detects ground edges or drops to prevent falling/stair accidents |
| **L298N Motor Driver** | Controls DC motor direction and speed for robot movement (forward, backward, turning) |
| **DC Gear Motors (x2)** | Provide locomotion and movement of the robot chassis |
| **Servo Motor** | Performs scanning motion for left/right environmental detection during obstacle avoidance |
| **Relay Module (2-Channel Active LOW)** | Controls high-power cleaning components (pump and mop motor) safely |
| **Water Pump (12V)** | Supplies water for wet cleaning phase during cleaning cycle |
| **Mop Motor / Roller System** | Performs physical mopping action during cleaning stage |
| **Suction Fan (Vacuum System)** | Collects dust and fine particles during sweeping phase |
| **Buzzer** | Provides audio feedback at the end of cleaning cycles or system alerts |
| **Power Supply (12V Battery Pack)** | Supplies power to motors, pump, and cleaning system |
| **Voltage Regulation Circuit (Arduino 5V line)** | Provides stable 5V supply to microcontroller and sensors |
| **MQTT Cloud (HiveMQ)** | Enables real-time communication between robot and web dashboard |
| **Web Dashboard (HTML/CSS/JS)** | User interface for manual control, mode switching, and monitoring |
| **MobileNetV2 AI Model** | Performs real-time waste classification (plastic, glass, metal, paper, etc.) |


---
## 🔌 Hardware Implementation
<div style="display: flex; align-items: flex-start; gap: 20px;">

  <div style="flex: 1;">
    <img src="side_view.jpeg" width="400"/>
  </div>

  <div style="flex: 1;">
     <img src="plan_view.jpeg" width="300"/>
  </div>
</div>

---
## 🚀 How to Run / Deploy the System


### 1. Firmware Upload (Arduino)

- Connect the Arduino Uno to your PC using a USB cable  
- Open the project in Arduino IDE  
- Install required library: `Servo.h`  
- Select board: **Arduino Uno**  
- Choose correct COM port  
- Click **Upload** to flash firmware  

---

### 2. MQTT Cloud Configuration (HiveMQ)

The system uses MQTT for real-time communication.

- Create an account on **HiveMQ Cloud**
- Create a cluster and generate credentials
- Update ESP8266 code:

```cpp
const char* mqtt_server = "your_broker_url";
const int   mqtt_port   = 8883;
const char* mqtt_user   = "robot_admin";
const char* mqtt_pass   = "Robot@2026";
```

- Upload updated code to ESP8266

---

### 3. AI System Setup (Waste Classification)

- Install Python 3.10+
- Install dependencies:

```bash
pip install -r requirements.txt
```

- Place trained model file:
  - `recycle_model_clean.keras`

inside the project directory

---

### 4. Run AI Application

Start Streamlit dashboard:

```bash
streamlit run app.py
```

### Features:
- Live camera waste detection  
- Image upload classification  
- Real-time confidence scores  

---

### 5. Web Control Dashboard (Manual Mode)

- Open web dashboard in browser  
- Login using configured password  
- Connect to MQTT broker  
- Control robot in real time:
  - Forward / Backward / Left / Right / Stop  
  - Switch between Manual and Auto modes  
  - Monitor system status  

---

### 6. System Execution Workflow

Once all components are running:

- Arduino Uno → Handles real-time sensor reading and motor control  
- ESP8266 → Manages MQTT communication  
- AI System → Performs waste classification  
- Web Dashboard → Sends commands via cloud  

The robot operates in two modes:
- **Manual Mode** → Controlled by user via dashboard  
- **Auto Mode** → Fully autonomous cleaning cycles  
---

## 👥 Project Team 
- Sama Ahmed Amer
- Sama Yasin Ali Jad 
- Sarah Ayman Helmy
- Nameer Reda
- Rawda Ibrahim
- Hanaa Sallam


#### **Department & Institution:** Electronics and Communication Engineering Department, Faculty of Engineering, Tanta University

#### **Academic Year**: 2025 / 2026