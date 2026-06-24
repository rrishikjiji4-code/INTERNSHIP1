#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "OPPO A54";
const char* password = "oppoa54phone";

// Start web server on standard HTTP port 80
WebServer server(80);

// Use the ESP32's built-in LED (usually GPIO 2)
const int ledPin = 2; 

// Function to generate the mobile-friendly HTML page
String getHtml(bool ledState) {
  String stateText = ledState ? "ON" : "OFF";
  
  String html = "<!DOCTYPE html><html>";
  // Viewport meta tag is crucial for formatting correctly on a phone screen
  html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<style>body{font-family: Arial, sans-serif; text-align: center; margin-top: 50px;}";
  html += ".button{background-color: #4CAF50; border: none; color: white; padding: 15px 32px; text-decoration: none; font-size: 24px; margin: 10px; cursor: pointer; border-radius: 8px;}";
  html += ".button-off{background-color: #f44336;}</style></head>";
  html += "<body><h1>ESP32 LED Control</h1>";
  html += "<p>LED is currently: <strong>" + stateText + "</strong></p>";
  html += "<a href=\"/on\"><button class=\"button\">Turn ON</button></a>";
  html += "<a href=\"/off\"><button class=\"button button-off\">Turn OFF</button></a>";
  html += "</body></html>";
  
  return html;
}

// Route: Homepage
void handleRoot() {
  server.send(200, "text/html", getHtml(digitalRead(ledPin)));
}

// Route: Turn LED ON
void handleLedOn() {
  digitalWrite(ledPin, HIGH);
  server.send(200, "text/html", getHtml(true));
}

// Route: Turn LED OFF
void handleLedOff() {
  digitalWrite(ledPin, LOW);
  server.send(200, "text/html", getHtml(false));
}

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected!");
  Serial.print("ESP32 IP Address: ");
  // This is the IP you will type into your phone's browser
  Serial.println(WiFi.localIP()); 

  // Define server routing
  server.on("/", handleRoot);
  server.on("/on", handleLedOn);
  server.on("/off", handleLedOff);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Listen for incoming client requests from your phone
  server.handleClient();
}