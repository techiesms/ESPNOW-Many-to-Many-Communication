/*
 *  This is the code for Many To Many Communication using ESPNOW for ESP32
 * 
 * 
 * The Full tutorial video of this project is uploaded on our youtube channel 
 * 
 * https://www.youtube.com/techiesms
 * 
 */

#include <esp_now.h>
#include <WiFi.h>

#define but_on 13
#define but_off 12
#define LED 14


bool new_data = 0; // Flag to send data only once


// Universal MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

bool incomingLED_status;

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
  int but_status;
} struct_message;

// Create a struct_message called myData
struct_message myData;

struct_message incomingReadings;
// Create peer interface
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingLED_status = incomingReadings.but_status;
  Serial.println(incomingReadings.but_status);
  digitalWrite(LED, incomingLED_status);

}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  pinMode(but_on, INPUT_PULLUP);
  pinMode(but_off, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Send Callback Function
  esp_now_register_send_cb(OnDataSent);

  // Receive Callback Function
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop()
{
  // Set values to send
  if (digitalRead(but_on) == LOW && new_data == 1)
  {
    myData.but_status = 1;
    new_data = 0;
  }
  if (digitalRead(but_off) == LOW && new_data == 1)
  {
    myData.but_status = 0;
    new_data = 0;
  }

  esp_err_t result; // declaration

  // Send message via ESP-NOW
  if (new_data == 0)
  {

    result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    new_data = 1;
  }

  if (result == ESP_OK) 
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }

  // delay(10000);
}
