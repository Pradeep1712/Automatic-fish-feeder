#include <ESP8266WiFi.h>
#include <Servo.h>

// Replace with your actual WiFi details
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Initialize Servo and Web Server on port 80
Servo feederServo;
WiFiServer server(80);

// Define the Servo Pin (D4 on NodeMCU is GPIO 2)
const int servoPin = 2; 

void setup() {
  Serial.begin(115200);
  
  // Attach servo and set to default closed position (0 degrees)
  feederServo.attach(servoPin); 
  feederServo.write(0);  

  // Connect to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected.");
  Serial.print("Access Control Panel at IP: ");
  Serial.println(WiFi.localIP());
  
  // Start the server
  server.begin();
}

void loop() {
  // Check if a client (browser) has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  client.flush();

  // Check if the user clicked the "FEED" button
  if (request.indexOf("/FEED") != -1) {
    Serial.println("Dispensing Food...");
    
    // Rotate servo to open position (90 degrees), wait 1 second, then close
    feederServo.write(90);
    delay(1000); 
    feederServo.write(0);
    
    Serial.println("Feeding complete.");
  }

  // Send the HTML response back to the browser
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<style>body { font-family: Arial; text-align: center; margin-top: 50px; background-color: #2c3e50; color: white; }");
  client.println("button { background-color: #e74c3c; color: white; padding: 20px 40px; font-size: 24px; border: none; border-radius: 8px; cursor: pointer; }</style></head>");
  client.println("<body>");
  client.println("<h2>IoT Automated Fish Feeder</h2>");
  client.println("<p>Status: Online</p>");
  client.println("<a href=\"/FEED\"><button>DISPENSE FOOD</button></a>");
  client.println("</body></html>");
}