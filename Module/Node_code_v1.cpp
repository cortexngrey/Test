#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

// Wi-Fi credentials and server URL
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* serverUrl = "http://your_linux_ip:5000/submit";  // Replace with your Flask server's IP

// WebServer on port 80
WebServer server(80);

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

    // Serve the HTML form
    server.on("/", HTTP_GET, []() {
        server.sendHeader("Content-Type", "text/html");
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
        
        // Send data to the remote server (Flask server on Linux)
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(serverUrl);  // Your Flask server URL
            http.addHeader("Content-Type", "application/x-www-form-urlencoded");

            // Prepare the POST data
            String postData = "name=" + name + "&email=" + email;
            
            // Send HTTP POST request
            int httpResponseCode = http.POST(postData);

            if (httpResponseCode > 0) {
                String response = http.getString();
                Serial.println("Server Response: " + response);
            } else {
                Serial.println("Error in sending POST request");
            }

            http.end();  // Close the HTTP connection
        } else {
            Serial.println("WiFi Disconnected. Can't send POST request.");
        }

        // Send response back to the client
        server.send(200, "text/html", "<h1>Thank you for submitting!</h1>");
    });

    // Start the web server
    server.begin();
}

void loop() {
    server.handleClient();  // Handle client requests
}
