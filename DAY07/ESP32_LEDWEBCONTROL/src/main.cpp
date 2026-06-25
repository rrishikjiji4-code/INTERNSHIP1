#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "rrishik";
const char* password = "rrishik007";

// Start web server on standard HTTP port 80
WebServer server(80);

// Use the ESP32's built-in LED (usually GPIO 2)
const int ledPin = 2; 

// PWM Configuration for ESP32
const int pwmFreq = 5000;
const int pwmChannel = 0;
const int pwmResolution = 8; // 8-bit resolution means values from 0 to 255

// Function to generate the HTML page with a slider
String getHtml() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<style>body{font-family: Arial, sans-serif; text-align: center; margin-top: 50px;}";
  html += "input[type=range] { width: 80%; margin: 30px 0; }</style></head>";
  html += "<body><h1>ESP32 Brightness Control</h1>";
  html += "<p>Brightness Level: <strong id=\"bval\">0</strong></p>";
  
  // The Slider (Range 0 to 255)
  html += "<input type=\"range\" min=\"0\" max=\"255\" value=\"0\" id=\"pwmSlider\" oninput=\"updateSlider(this.value)\">";
  
  // JavaScript to send the slider value to the ESP32 without reloading the page
  html += "<script>";
  html += "function updateSlider(val) {";
  html += "  document.getElementById('bval').innerText = val;";
  html += "  fetch('/set?val=' + val);"; // Sends a hidden HTTP request to the ESP32
  html += "}";
  html += "</script></body></html>";
  
  return html;
}

// Route: Homepage
void handleRoot() {
  server.send(200, "text/html", getHtml());
}

// Route: Handle Slider Updates
void handleSetBrightness() {
  if (server.hasArg("val")) {
    // Read the value sent by the JavaScript fetch request
    int brightness = server.arg("val").toInt();
    
    // Safety check to ensure the value stays within 0-255
    brightness = constrain(brightness, 0, 255);
    
    // Set the new brightness
    ledcWrite(pwmChannel, brightness);
    
    // Acknowledge the request
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing value");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Configure ESP32 PWM
  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(ledPin, pwmChannel);
  ledcWrite(pwmChannel, 0); // Start with LED off

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP()); 

  // Define server routing
  server.on("/", handleRoot);
  server.on("/set", handleSetBrightness); // New route for the slider
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Listen for incoming client requests
  server.handleClient();
}