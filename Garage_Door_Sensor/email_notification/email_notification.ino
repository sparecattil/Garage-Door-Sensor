#define WIFI_SSID "UI-DeviceNet"
#define WIFI_PASSWORD "UI-DeviceNet"
#define REFERENCE_URL "https://garage-door-sensor-default-rtdb.firebaseio.com/"
#include <Firebase.h>

Firebase fb(REFERENCE_URL);

void setup() {
  Serial.begin(115200);

  WiFi.disconnect();
  delay(1000);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println();
  Serial.println("WiFi Connected");

  fb.setBool("Send_Email", true);

}

void loop() {
  Serial.println("Reached Loop");
  delay(5000);
}
