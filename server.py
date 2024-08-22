from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/submit', methods=['POST'])
def receive_data():
    if request.method == 'POST':
        # Get the form data sent by the ESP32
        data = request.form
        name = data.get('name')
        email = data.get('email')

        # Print received data to the server console
        print(f"Received data: Name={name}, Email={email}")

        # Return a response to the ESP32
        return jsonify({"message": "Data received successfully"}), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
