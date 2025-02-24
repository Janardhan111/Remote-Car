#include <ESP8266WiFi.h>
#include <espnow.h>


// REPLACE WITH THE MAC Address of your receiver 
//Big ESP MAC address - CC:50:E3:5D:22:BB
//Small ESP MAC address - BC:DD:C2:FE:62:34
uint8_t broadcastAddress[] = {0xBC, 0xDD, 0xC2, 0xFE, 0x62, 0x34};

uint8_t forward = D0;
uint8_t backward = D2;
uint8_t left = D6;
uint8_t right = D8;


const long interval = 100; 
unsigned long previousMillis = 0; 


// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    bool forward;
    bool backward;
    bool left;
    bool right;
} struct_message;

// Create a struct_message called DHTReadings to hold sensor readings
struct_message remote_state;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
//void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
//  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
//  Serial.print("Bytes received: ");
//  Serial.println(len);
//  Serial.print("Data Received: ");
//  Serial.print("Forward: ");
//  Serial.print(incomingReadings.forward);
//  Serial.print("Backward: ");
//  Serial.print(incomingReadings.backward);
//  Serial.print("Right: ");
//  Serial.print(incomingReadings.right);
//  Serial.print("Left: ");
//  Serial.println(incomingReadings.left);
////  state = incomingReadings.state;
//  if(incomingReadings.state == true){
//    digitalWrite(LED_Pin,HIGH);
//  }
//  else{
//    digitalWrite(LED_Pin,LOW);
//  }
//}

//void getReadings(){
//  // Read Temperature
//  temperature = dht.readTemperature();
//  // Read temperature as Fahrenheit (isFahrenheit = true)
//  //float t = dht.readTemperature(true);
//  if (isnan(temperature)){
//    Serial.println("Failed to read from DHT");
//    temperature = 0.0;
//  }
//  humidity = dht.readHumidity();
//  if (isnan(humidity)){
//    Serial.println("Failed to read from DHT");
//    humidity = 0.0;
//  }  
//}

void printRemoteState(){
  // Display Readings in Serial Monitor
  Serial.println("Transmitter Button States");
  Serial.print("Forward: ");
  Serial.print(remote_state.forward);
  Serial.print("Backward: ");
  Serial.print(remote_state.backward);
  Serial.print("Right: ");
  Serial.print(remote_state.right);
  Serial.print("Left: ");
  Serial.println(remote_state.left);
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

//  // Init DHT sensor
//  dht.begin();
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  // Register for a callback function that will be called when data is received
//  esp_now_register_recv_cb(OnDataRecv);

  pinMode(forward,INPUT);
  pinMode(backward,INPUT);
  pinMode(left,INPUT);
  pinMode(right,INPUT);
  digitalWrite(forward,LOW);
  digitalWrite(backward,LOW);
  digitalWrite(left,LOW);
  digitalWrite(right,LOW);
  
}
 
void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      remote_state.forward = digitalRead(forward);
      remote_state.backward = digitalRead(backward);
      remote_state.left = digitalRead(left);
      remote_state.right = digitalRead(right);
      // Send message via ESP-NOW
      printRemoteState();
      esp_now_send(broadcastAddress, (uint8_t *) &remote_state, sizeof(remote_state));
  
    }
}
