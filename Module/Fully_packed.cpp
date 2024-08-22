#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <WebSocketsClient.h>

// WiFi and Server Configuration
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* remote_server = "http://your_remote_server.com";  // Replace with your server URL
const char* mqtt_server = "broker.hivemq.com";  // Example MQTT broker

WiFiClient espClient;
PubSubClient mqttClient(espClient);
WebSocketsClient webSocket;

// Create a web server on port 80
WebServer server(80);

// Setup function
void setup() {
    Serial.begin(115200);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // MQTT Setup
    mqttClient.setServer(mqtt_server, 1883);

    // WebSocket Setup
    webSocket.begin("your_websocket_server.com", 80, "/");  // Replace with WebSocket server

    // Serve the HTML form
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", R"(
            <!DOCTYPE html>
            <html>
            <head>
                <title>Basic Info Form</title>
            </head>
            <body>
                <h1>Capture Basic Information</h1>
                <form action="/submit" method="POST">
                    <label for="name">Name:</label>
                    <input type="text" id="name" name="name" required><br><br>
                    
                    <label for="email">Email:</label>
                    <input type="email" id="email" name="email" required><br><br>
                    
                    <input type="submit" value="Submit">
                </form>
            </body>
            </html>
        )");
    });

    // Handle form submission
    server.on("/submit", HTTP_POST, []() {
        String name = server.arg("name");
        String email = server.arg("email");

        // Print the form data to the serial monitor
        Serial.println("Form Submitted:");
        Serial.println("Name: " + name);
        Serial.println("Email: " + email);

        // Send the data to remote server using various methods
        sendDataHTTP_GET(name, email);
        sendDataHTTP_POST(name, email);
        sendDataMQTT(name, email);
        sendDataWebSocket(name, email);

        server.send(200, "text/html", "<h1>Thank you for submitting!</h1>");
    });

    // Start the web server
    server.begin();
}

// Loop function
void loop() {
    server.handleClient(); // Handle client requests
    mqttClient.loop();     // Keep MQTT connection alive
    webSocket.loop();      // Keep WebSocket connection alive
}

// Function to send data using HTTP GET request
void sendDataHTTP_GET(String name, String email) {
    HTTPClient http;
    String url = remote_server + "/submit?name=" + name + "&email=" + email;

    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
        String response = http.getString();
        Serial.println("HTTP GET Response: " + response);
    } else {
        Serial.println("HTTP GET failed");
    }
    http.end();
}

// Function to send data using HTTP POST request
void sendDataHTTP_POST(String name, String email) {
    HTTPClient http;
    http.begin(remote_server + "/submit");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String payload = "name=" + name + "&email=" + email;
    int httpCode = http.POST(payload);

    if (httpCode > 0) {
        String response = http.getString();
        Serial.println("HTTP POST Response: " + response);
    } else {
        Serial.println("HTTP POST failed");
    }
    http.end();
}

// Function to send data using MQTT
void sendDataMQTT(String name, String email) {
    if (!mqttClient.connected()) {
        reconnectMQTT();
    }

    String topic = "esp32/data";
    String payload = "Name: " + name + ", Email: " + email;
    mqttClient.publish(topic.c_str(), payload.c_str());
    Serial.println("MQTT Message Sent");
}

// Function to reconnect MQTT client
void reconnectMQTT() {
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (mqttClient.connect("ESP32Client")) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            delay(5000);
        }
    }
}

// Function to send data using WebSocket
void sendDataWebSocket(String name, String email) {
    if (webSocket.isConnected()) {
        String message = "Name: " + name + ", Email: " + email;
        webSocket.sendTXT(message);
        Serial.println("WebSocket Message Sent");
    } else {
        Serial.println("WebSocket not connected");
    }
}
