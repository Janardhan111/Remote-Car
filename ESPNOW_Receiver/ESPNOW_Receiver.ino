#include <ESP8266WiFi.h>
#include <espnow.h>


// REPLACE WITH THE MAC Address of your receiver 
//Big ESP MAC address - CC:50:E3:5D:22:BB
//Small ESP MAC address - BC:DD:C2:FE:62:34
//uint8_t broadcastAddress[] = {0xCC, 0x50, 0xE3, 0x5D, 0x22, 0xBB};

uint8_t forward_motor = D1;
uint8_t backward_motor = D2;
uint8_t left_motor = D4;
uint8_t right_motor = D6;

uint8_t stand_by = D3;

uint8_t m1_speed = D0;
uint8_t m2_speed = D5;

uint8_t battery_left = D7;

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
//struct_message switch_state;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

// Callback when data is sent
//void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
//  Serial.print("Last Packet Send Status: ");
//  if (sendStatus == 0){
//    Serial.println("Delivery success");
//  }
//  else{
//    Serial.println("Delivery fail");
//  }
//}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  printIncomingReadings();
  if(incomingReadings.forward == true){
    digitalWrite(forward_motor,HIGH);
  }
  else{
    digitalWrite(forward_motor,LOW);
  }
  if(incomingReadings.backward == true){
    digitalWrite(backward_motor,HIGH);
  }
  else{
    digitalWrite(backward_motor,LOW);
  }
  if(incomingReadings.left == true){
    digitalWrite(left_motor,HIGH);
  }
  else{
    digitalWrite(left_motor,LOW);
  }
  if(incomingReadings.right == true){
    digitalWrite(right_motor,HIGH);
  }
  else{
    digitalWrite(right_motor,LOW);
  }
}

void printIncomingReadings(){
  // Display Readings in Serial Monitor
  Serial.print("Data Received: ");
  Serial.print("Forward: ");
  Serial.print(incomingReadings.forward);
  Serial.print("Backward: ");
  Serial.print(incomingReadings.backward);
  Serial.print("Right: ");
  Serial.print(incomingReadings.right);
  Serial.print("Left: ");
  Serial.println(incomingReadings.left);
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
//  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
//  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

  pinMode(stand_by, OUTPUT);
  pinMode(m1_speed, OUTPUT);
  pinMode(m2_speed, OUTPUT);
  pinMode(forward_motor, OUTPUT);
  pinMode(backward_motor, OUTPUT);
  pinMode(left_motor, OUTPUT);
  pinMode(right_motor, OUTPUT);
  digitalWrite(forward_motor, LOW);
  digitalWrite(backward_motor, LOW);
  digitalWrite(left_motor, LOW);
  digitalWrite(right_motor, LOW);
  
  analogWrite(stand_by, 1024);
  analogWrite(m1_speed, 140);
  analogWrite(m2_speed, 140);
}
 
void loop() {
//    unsigned long currentMillis = millis();
//    if (currentMillis - previousMillis >= interval) {
//      previousMillis = currentMillis;
//      switch_state.state = digitalRead(Button_Pin);
//      // Send message via ESP-NOW
//      Serial.print("Switch State: ");
//      Serial.println(switch_state.state);
//      esp_now_send(broadcastAddress, (uint8_t *) &switch_state, sizeof(switch_state));
//  
//      // Print incoming readings
//      printIncomingReadings();
//    }
}
