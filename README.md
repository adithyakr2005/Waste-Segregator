# ♻️ AI & IoT-Based Real-Time Waste Segregator

A real-time smart waste segregator that uses a **Convolutional Neural Network (CNN)** model and **IoT** to classify waste as *Biodegradable* or *Non-Biodegradable*. Based on the classification, the system automates sorting using an actuator, LED, and a 4×7 segment display.


## 🧠 CNN Model

- Trained on a custom dataset of bio and non-bio waste images.
- Achieves **96% accuracy** in real-time classification.
- Efficient and lightweight for fast inference.

---

## 📡 IoT Communication (ESP + WebSocket)

- The ESP connects to a Wi-Fi network using credentials provided beforehand.
- On successful connection, an **LED indicator** glows.
- The ESP hosts a **local DNS server** and initiates a WebSocket connection with the device running the AI model.
- Upon receiving a classification message:
  - If **Bio**: Displays `810` on the 4×7 segment display and rotates the actuator to drop the waste into the *Biodegradable* compartment.
  - If **Non-Bio**: Displays `-810` and moves the actuator to drop the waste into the *Non-Biodegradable* compartment.

---

## ⚙️ Components Used

- ESP32 / ESP8266 microcontroller
- TM1637 4×7 segment display
- LED
- Servo motor or actuator
- Webcam for capturing waste images
- Custom waste sorting structure (rotating plane or compartment separator)

---

## 🔄 Workflow

1. The ESP powers on and connects to the Wi-Fi network.
2. LED glows to indicate successful connection.
3. ESP establishes a WebSocket connection with the ML system.
4. When a waste item is introduced:
   - The AI model captures and classifies the image.
   - Sends classification (`BIO` or `NON-BIO`) to the ESP.
5. Based on the result:
   - The display shows `810` for BIO and `-810` for NON-BIO.
   - The actuator sorts the waste into the correct bin.

---

## 📈 Accuracy

> ✅ The CNN model achieves **96% accuracy**, ensuring reliable and real-time waste segregation.

---

## 📷 Example

> 🗑️ *Waste image → Classified by AI → Message sent to ESP → Sorted using actuator*

---
