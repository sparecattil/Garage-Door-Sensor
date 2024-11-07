///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////<Garage Door Sensor>/////////////////////////////////
// Group Members: Sebastian Parecatti, Shiv Patel, John Finch, Logan Schimanski
// Description: 
// Contains software implementations for the email notification system via the ESP32
// microcontroller, the ESP 32 will continously read the On/Off status of the receiver's
// LED and send an email notification when the LED is turn on, The email is sent by
// connecting to Firebase and sending a boolean status which will be read by a Node.js 
// file, "notification.js"
//////////////////////////////////<Garage Door Sensor>/////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h> 
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "UI-DeviceNet"
#define WIFI_PASSWORD "UI-DeviceNet"
#define API_KEY "AIzaSyBwOJ2HDW0Au-BpXfcX7Ii6EXwGKJ5GFhg"
#define DATABASE_URL "https://garage-door-sensor-default-rtdb.firebaseio.com/"

FirebaseData fbdo; // Firebase data object for sending and receiving data from Firebase
FirebaseAuth auth; // Used for authentication to Firebase
FirebaseConfig config; // Used to set the configure properties of the connection to Firebase

unsigned long sendDataPrevMillis = 0; // Used to find the time elapsed since the data was last sent to Firebase
bool signupOK = false; // Signed into firebase check

int pinNumber = 5; // Pin to read from LED
int pinStatus; // Current status of the LED pin

bool emailLock = false; // Locks email functionality to prevent multiple successive email messages on a single pin status change

void setup() {
  Serial.begin(115200);

  pinMode(pinNumber, INPUT); // Set the pin as an input

  ////--------WIFI Setup--------////
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Begin connection to WIFI
  Serial.print("Connecting to Wi-FI");
  // Loop until connect to the WIFI
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print("."); 
    delay(1000);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  ////--------Firebase Setup--------////
  config.api_key = API_KEY; // Configuring the api key to the access Firebase
  config.database_url = DATABASE_URL; // Configuring the URL of the database
  // Signing up to Firebase
  if(Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("signUp OK");
    signupOK = true; // Successful sign up to Firebase
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback; // Callback function to monitor the status of the Firebase authentication token
  Firebase.begin(&config, &auth); // Initializes Firebase with the configuration and authentication details
  
  Firebase.reconnectWiFi(true); // Configures Firebase to reconnect to WIFI when connection is lost

  // Checking if Firebase is ready
  if (Firebase.ready() && signupOK) {
    sendToFirebase("Send_Email", false); // Initializing boolean status variable on Firebase
  }

}

void loop() {
  pinStatus = digitalRead(pinNumber); // Read the status of the pin (HIGH or LOW)

  ////--------Sending to Firebase--------////
  // Checking if Firebase is ready
  if (Firebase.ready() && signupOK) {
    // Checking if LED is on and that the status of the LED has changed
    if (pinStatus == LOW && !emailLock) {
      Serial.println();
      Serial.println("Pin is LOW");
      Serial.println(pinStatus);
      sendToFirebase("Send_Email", true); // Sending boolean status for email notification to Firebase
      emailLock = true; // Setting the email lock
    }
    // Checking if the LED is off
    else if (pinStatus == HIGH) {
      Serial.println();
      Serial.println("Pin is HIGH");
      emailLock = false; // Disabling email lock since the status of the LED has changed
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////
// Method : sendToFirebase(String path, float value)
// Inputs : String path - Location path in Firebase which is written to
//          bool value - Status to send an email
//                       true - send an email
//                       false - do not send email
// Outputs: None
///////////////////////////////////////////////////////////////////////////////////////
void sendToFirebase(String path, bool value) {
  if (Firebase.RTDB.setBool(&fbdo, path, value)) {
      Serial.println(); 
      Serial.print(value);
      Serial.print("- successfully saved to: " + fbdo.dataPath());
  }
  else {
    Serial.println("FAILED: " + fbdo.errorReason());
  }
}
