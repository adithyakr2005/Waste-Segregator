#include <WiFi.h>
#include <ESPmDNS.h>
#include <TM1637Display.h>

// WiFi Credentials
const char* ssid = "Airtel_akas_4325";
const char* password = "air60336";

// Web Server
WiFiServer server(80);

// 7-segment TM1637 Setup
#define CLK  21   // Clock pin
#define DIO  22   // Data pin
TM1637Display display(CLK, DIO);

// Actuator Pin (LED)
const int actuatorPin = 2;  // GPIO2

bool stopProgram = false;

void setup() {
    Serial.begin(115200);
    pinMode(actuatorPin, OUTPUT);
    digitalWrite(actuatorPin, LOW); // Initially OFF

    display.setBrightness(7); // Max brightness
    display.clear();          // Clear display

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
        digitalWrite(actuatorPin, HIGH); // Turn on LED when connected to WiFi
    } else {
        Serial.println("\nWiFi Connection Failed!");
        digitalWrite(actuatorPin, LOW); // Ensure actuator is OFF if no WiFi
    }

    // Start mDNS responder
    if (MDNS.begin("esp32")) {
        Serial.println("mDNS responder started as esp32.local");
    } else {
        Serial.println("Error setting up mDNS");
    }

    server.begin();
}

void loop() {
    if (stopProgram) return;

    // Check WiFi status and turn off actuator if disconnected
    if (WiFi.status() != WL_CONNECTED) {
        digitalWrite(actuatorPin, LOW);
        Serial.println("WiFi Disconnected! Actuator turned OFF.");
    }

    WiFiClient client = server.available();
    if (!client) return;

    Serial.println("Client Connected");
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
    } 
    else if (request.indexOf("/non-bio") != -1) {
        client.println("Non-Biodegradable Waste Command Received");
        Serial.println("Non-Biodegradable Waste Command");
        displayNonBio();
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

    delay(50);
    client.flush();
    client.stop();
    Serial.println("Client Disconnected");
}

// Function to display BIO ( " 810" )
void displayBio() {
    uint8_t segData[4] = {0x00, 0x7F, 0x06, 0x3F}; // " 810"
    display.setSegments(segData);
}

// Function to display NBIO ( "-810" )
void displayNonBio() {
    uint8_t segData[4] = {0x40, 0x7F, 0x06, 0x3F}; // "-810"
    display.setSegments(segData);
}
