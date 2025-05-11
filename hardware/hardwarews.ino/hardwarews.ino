#include <WiFi.h>
#include <ESPmDNS.h>
#include <TM1637Display.h>
#include <WebSocketsServer.h>

// WiFi Credentials
const char* ssid = "Airtel_akas_4325";
const char* password = "air60336";

// 7-segment TM1637 Setup
#define CLK  21   // Clock pin
#define DIO  22   // Data pin
TM1637Display display(CLK, DIO);

// Actuator Pin (LED)
const int actuatorPin = 2;  // GPIO2

// WebSocket Server
WebSocketsServer ws = WebSocketsServer(81);

bool stopProgram = false;

void displayBio() {
    uint8_t segData[4] = {0x00, 0x7F, 0x06, 0x3F}; // " 810"
    display.setSegments(segData);
}

void displayNonBio() {
    uint8_t segData[4] = {0x40, 0x7F, 0x06, 0x3F}; // "-810"
    display.setSegments(segData);
}

void displayReady() {
    uint8_t segData[4] = {0x5E, 0x54, 0x5E, 0x00};  // "RDY "
    display.setSegments(segData);
}

void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_TEXT) {
        String msg = String((char*) payload);
        Serial.printf("[WS] Received message: %s\n", msg.c_str());

        if (stopProgram && msg != "start") {
            ws.sendTXT(client_num, "Program is currently stopped. Send 'start' to resume.");
            return;
        }

        if (msg == "bio") {
            ws.textAll("Biodegradable Waste Command Received");
            Serial.println("Biodegradable Waste Command");
            displayBio();
        } 
        else if (msg == "non-bio") {
            ws.textAll("Non-Biodegradable Waste Command Received");
            Serial.println("Non-Biodegradable Waste Command");
            displayNonBio();
        }
        else if (msg == "q") {
            ws.textAll("Quit command received");
            Serial.println("Quit command received");
            display.clear();
            stopProgram = true;
            digitalWrite(actuatorPin, LOW);
        }
        else if (msg == "start") {
            ws.textAll("Resuming Program...");
            Serial.println("Start command received!");
            stopProgram = false;
            digitalWrite(actuatorPin, HIGH);
            displayReady();
        }
        else {
            ws.sendTXT(client_num, "Unknown Command");
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(actuatorPin, OUTPUT);
    digitalWrite(actuatorPin, LOW); // Initially OFF

    display.setBrightness(7); // Max brightness
    display.clear();

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected!");
        Serial.print("ESP IP Address: ");
        Serial.println(WiFi.localIP());
        digitalWrite(actuatorPin, HIGH); // Turn on LED when connected
    } else {
        Serial.println("\nWiFi Connection Failed!");
        digitalWrite(actuatorPin, LOW);
    }

    if (MDNS.begin("esp32")) {
        Serial.println("mDNS responder started as esp32.local");
    } else {
        Serial.println("Error setting up mDNS");
    }

    ws.begin();
    ws.onEvent(onWebSocketEvent);
    Serial.println("WebSocket server started on port 81");
}

void loop() {
    ws.loop();
}