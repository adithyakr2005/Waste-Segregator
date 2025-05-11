#include <WiFi.h>
#include <ESPmDNS.h>
#include <TM1637Display.h>
#include <WebSocketsServer.h>
#include <ESP32Servo.h>  // ✅ Servo Support

// WiFi Credentials
const char* ssid = "Null";
const char* password = "90909090";

// 7-segment TM1637 Setup
#define CLK  23
#define DIO  22
TM1637Display display(CLK, DIO);

// Actuator Pin (LED)
const int actuatorPin = 2;

//Button setup
//const int buttonPin = 5;  // GPIO pin where the button is connected
//int buttonState = 0;       // Variable to store the button state

// Servo setup
Servo servoLeft;
Servo servoRight;
const int leftServoPin = 16;
const int rightServoPin = 17;

// Web Server & WebSocket
WiFiServer server(80);
WebSocketsServer ws = WebSocketsServer(81);

bool stopProgram = false;

void setup() {
    Serial.begin(115200);

    pinMode(actuatorPin, OUTPUT);
    digitalWrite(actuatorPin, LOW);

    display.setBrightness(7);
    display.clear();

    servoLeft.attach(leftServoPin);
    servoRight.attach(rightServoPin);
    servoLeft.write(100);
    servoRight.write(80);

    //pinMode(buttonPin, INPUT_PULLUP);

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\n✅ Connected!");
    Serial.print("📡 IP Address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(actuatorPin, HIGH);

    if (MDNS.begin("esp32")) {
        Serial.println("🌐 mDNS started: http://esp32.local");
    } else {
        Serial.println("❌ mDNS failed");
    }

    server.begin();
    ws.begin();
    ws.onEvent(onWebSocketEvent);
    Serial.println("🚀 WebSocket server started");
}

void loop() {
    if (stopProgram) return;

    ws.loop();

    WiFiClient client = server.available();
    if (!client) return;

    String request = "";
    unsigned long timeout = millis();
    while (client.connected() && millis() - timeout < 1000) {
        while (client.available()) {
            char c = client.read();
            request += c;
            timeout = millis();
        }
    }

    Serial.println(request);
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();

    if (request.indexOf("/bio") != -1) {
        client.println("Biodegradable Waste Command Received");
        Serial.println("Biodegradable Waste Command");
        displayBio();
        rotateRight();
    } 
    else if (request.indexOf("/non-bio") != -1) {
        client.println("Non-Biodegradable Waste Command Received");
        Serial.println("Non-Biodegradable Waste Command");
        displayNonBio();
        rotateLeft();
    } 
    else if (request.indexOf("/q") != -1) {
        client.println("Quitting Program...");
        Serial.println("Quit command received!");
        display.clear();
        stopProgram = true;
        digitalWrite(actuatorPin, LOW);
    } 
    else {
        client.println("Unknown Command");
    }

    // Read the button state
    //buttonState = digitalRead(buttonPin);

    // If the button is pressed (LOW because we're using INPUT_PULLUP)
    //if (buttonState == LOW) {
        //Serial.println("Button pressed!");
        // Add the code to send a WebSocket message here, or call the function for photo capture
        //ws.broadcastTXT("capture");  // Call your function to capture the photo
        //delay(200);  // Debounce delay
    //}

    delay(50);
    client.flush();
    client.stop();
}

void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_TEXT) {
        String msg = String((char*) payload);
        Serial.printf("[WS] Received: %s\n", msg.c_str());

        if (stopProgram && msg != "start") {
            ws.sendTXT(client_num, "❌ Program is stopped. Send 'start' to resume.");
            return;
        }

        if (msg == "bio") {
            ws.broadcastTXT("Biodegradable Waste");
            displayBio();
            rotateRight();
        } 
        else if (msg == "non-bio") {
            ws.broadcastTXT("Non-Biodegradable Waste");
            displayNonBio();
            rotateLeft();
        } 
        else if (msg == "q") {
            ws.broadcastTXT("❌ Quit command received");
            display.clear();
            stopProgram = true;
            digitalWrite(actuatorPin, LOW);
        } 
        else if (msg == "start") {
            ws.broadcastTXT("✅ Program Resumed");
            stopProgram = false;
            digitalWrite(actuatorPin, HIGH);
            displayReady();
        } 
        else {
            ws.sendTXT(client_num, "⚠️ Unknown command");
        }
    }
}

// 📟 Display Functions
void displayBio() {
    uint8_t segData[4] = {0x00, 0x7F, 0x06, 0x3F};  // " 810"
    display.setSegments(segData);
}

void displayNonBio() {
    uint8_t segData[4] = {0x40, 0x7F, 0x06, 0x3F};  // "-810"
    display.setSegments(segData);
}

void displayReady() {
    uint8_t segData[4] = {0x5E, 0x54, 0x5E, 0x00};  // "RDY "
    display.setSegments(segData);
}

// 🔁 Servo Functions
void rotateRight() {
    Serial.println("Turning Right (BIO)");
    servoLeft.write(170);
    servoRight.write(10);
    delay(3000);
    servoLeft.write(100);
    servoRight.write(80);
}

void rotateLeft() {
    Serial.println("Turning Left (NON-BIO)");
    servoLeft.write(10);
    servoRight.write(170);
    delay(3000);
    servoLeft.write(100);
    servoRight.write(80);
}
