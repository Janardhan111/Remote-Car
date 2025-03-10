#include <ESP8266WiFi.h>
#include <espnow.h>


// REPLACE WITH THE MAC Address of your receiver 
//Big ESP MAC address - CC:50:E3:5D:22:BB
//Small ESP MAC address - BC:FF:4D:E8:68:7B
uint8_t broadcastAddress[] = {0xBC, 0xFF, 0x4D, 0xE8, 0x68, 0x7B};

// Joystick pins connection to ESP8266 board

uint8_t LED_pin = D4;

uint8_t left_button = D2;

uint8_t right_button = D5;

uint8_t analogreadpin = A0;

int analog_read_delay = 2;

uint8_t s0 = D8;
uint8_t s1 = D7;
uint8_t s2 = D6;

// Interval to determine the sampling rate
const long interval = 10; 
unsigned long previousMillis = 0; 


//Structure example to send data
//Must match the receiver structure
typedef struct joystick_state {
    int left_x_position;
    int left_y_position;
    bool left_button_state;
    int right_x_position;
    int right_y_position;
    bool right_button_state;
} joystick_state;

struct car_state{
  int battery_percent;
};

car_state current_car_state;
joystick_state current_joystick_state;

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

// Callback when data is received ( battery percent from car ) to be implemented
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&current_car_state, incomingData, sizeof(incomingData));
  Serial.print("Bytes received: ");
  Serial.print(len);
  Serial.print("Battery Percent : ");
  Serial.println(current_car_state.battery_percent);
////  state = incomingReadings.state;
//  if(incomingReadings.state == true){
//    digitalWrite(LED_Pin,HIGH);
//  }
//  else{
//    digitalWrite(LED_Pin,LOW);
//  }
}


void printRemoteState(){
  // Display Readings in Serial Monitor
  Serial.print(" Transmitter Joystick States");
  Serial.print(" X_Position: ");
  Serial.print(current_joystick_state.left_x_position);
  Serial.print(" Y_Position: ");
  Serial.print(current_joystick_state.left_y_position);
  Serial.print(" Button: ");
  Serial.print(current_joystick_state.left_button_state);
  Serial.print(" X_Position: ");
  Serial.print(current_joystick_state.right_x_position);
  Serial.print(" Y_Position: ");
  Serial.print(current_joystick_state.right_y_position);
  Serial.print(" Button: ");
  Serial.println(current_joystick_state.right_button_state);
}

void update_remote_state(){
//  Left x axis : A2 010
//  Left y axis : A1 001
//  right x axis : A3 011
//  right y axis : A0 000

  // read left x value
  digitalWrite(s0,LOW);
  digitalWrite(s1,HIGH);
  delay(analog_read_delay);
  current_joystick_state.left_x_position = analogRead(analogreadpin);

  // read left y value
  digitalWrite(s0,HIGH);
  digitalWrite(s1,LOW);
  delay(analog_read_delay);
  current_joystick_state.left_y_position = analogRead(analogreadpin);

  // read right x value
  digitalWrite(s0,HIGH);
  digitalWrite(s1,HIGH);
  delay(analog_read_delay);
  current_joystick_state.right_x_position = analogRead(analogreadpin);

  // read right y value
  digitalWrite(s0,LOW);
  digitalWrite(s1,LOW);
  delay(analog_read_delay);
  current_joystick_state.right_y_position = analogRead(analogreadpin);

  current_joystick_state.left_button_state = analogRead(left_button);
  current_joystick_state.right_button_state = analogRead(right_button);
}
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
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
  esp_now_register_recv_cb(OnDataRecv);

  pinMode(s0,OUTPUT);
  pinMode(s1,OUTPUT);
  pinMode(s2,OUTPUT);
  
  pinMode(left_button,INPUT);
  pinMode(right_button,INPUT);

  digitalWrite(s2,LOW);

  digitalWrite(LED_pin,LOW);
}
 
void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      update_remote_state();
      printRemoteState();
      // Send message via ESP-NOW
      esp_now_send(broadcastAddress, (uint8_t *) &current_joystick_state, sizeof(current_joystick_state));
    }  
}
