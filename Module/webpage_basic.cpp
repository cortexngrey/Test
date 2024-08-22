#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

WebServer server(80); // Create a web server on port 80

void setup() {
    Serial.begin(115200);
    
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
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
        
        server.send(200, "text/html", "<h1>Thank you for submitting!</h1>");
    });

    // Start the server
    server.begin();
}

void loop() {
    server.handleClient(); // Handle client requests
}
