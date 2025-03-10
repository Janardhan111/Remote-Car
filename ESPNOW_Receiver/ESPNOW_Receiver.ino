 #include <ESP8266WiFi.h>
#include <espnow.h>


// REPLACE WITH THE MAC Address of your receiver 
//Big ESP MAC address - CC:50:E3:5D:22:BB
//Small ESP MAC address -  BC:FF:4D:E8:68:7B
uint8_t broadcastAddress[] = {0xCC, 0x50, 0xE3, 0x5D, 0x22, 0xBB};

uint8_t forward_motor = D1;
uint8_t backward_motor = D2;
uint8_t left_motor = D4;
uint8_t right_motor = D6;

uint8_t stand_by = D3;

uint8_t m1_speed = D7;
uint8_t m2_speed = D5;

uint8_t battery_pin = A0;

const long interval = 1000; 
unsigned long previousMillis = 0; 

long left_x_middle_ref = 532;
long left_y_middle_ref = 517;
long right_x_middle_ref = 521;
long right_y_middle_ref = 523;

uint8_t counter_for_ref = 100;
bool emergency_stop_flag = 0;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
struct remote_sate {
    bool forward;
    bool backward;
    bool left;
    bool right;
};

struct joystick_state {
    int left_x_position;
    int left_y_position;
    bool left_button_state;
    int right_x_position;
    int right_y_position;
    bool right_button_state;
};

struct out_going_readings{
  int battery_percent;
};


// Create a struct called battery_readings to hold battery readings
out_going_readings car_state;


joystick_state current_joystick_state,joystick_rest_state;


//joystick_rest_state.left_x_position = 532;
//joystick_rest_state.left_y_position = 517;
//joystick_rest_state.right_x_position = 521;
//joystick_rest_state.right_y_position = 523;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
//  Serial.print("Last Packet Send Status: ");
//  if (sendStatus == 0){
//    Serial.println("Delivery success");
//  }
//  else{
//    Serial.println("Delivery fail");
//  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&current_joystick_state, incomingData, sizeof(current_joystick_state));
  update_motors();
}

void update_motors(){
  printRemoteState();
  Serial.print(" L_X_ref : ");
  Serial.print(left_x_middle_ref);
  Serial.print(" L_y_ref : ");
  Serial.print(left_y_middle_ref);
  Serial.print(" r_X_ref : ");
  Serial.print(right_x_middle_ref);
  Serial.print(" r_y_ref : ");
  Serial.print(right_y_middle_ref);
  if ((current_joystick_state.left_x_position == 0 && current_joystick_state.left_y_position == 0) || 
  (current_joystick_state.right_x_position == 0 && current_joystick_state.right_y_position == 0)){
    emergency_stop_flag = 1;
  }
  if (!emergency_stop_flag){
    if(current_joystick_state.left_x_position < left_x_middle_ref-100){
      digitalWrite(forward_motor,HIGH);
      Serial.print(" F: ");
      Serial.print("1");
    }
    else{
      digitalWrite(forward_motor,LOW);
      Serial.print(" F: ");
      Serial.print("0");
    }
    if(current_joystick_state.left_x_position > left_x_middle_ref+100){
      digitalWrite(backward_motor,HIGH);
      Serial.print(" B: ");
      Serial.print("1");
    }
    else{
      digitalWrite(backward_motor,LOW);
      Serial.print(" F: ");
      Serial.print("0");
    }
    if(current_joystick_state.right_y_position < right_y_middle_ref-100){
      digitalWrite(left_motor,HIGH);
      Serial.print(" L: ");
      Serial.print("1");
    }
    else{
      digitalWrite(left_motor,LOW);
      Serial.print(" L: ");
      Serial.print("0");
    }
    if(current_joystick_state.right_y_position > right_y_middle_ref+100){
      digitalWrite(right_motor,HIGH);
      Serial.print(" R: ");
      Serial.print("1");
    }
    else{
      digitalWrite(right_motor,LOW);
      Serial.print(" R: ");
      Serial.print("0");
    }
  }
}

void printRemoteState(){
  // Display Readings in Serial Monitor
  Serial.print(" | L_X: ");
  Serial.print(current_joystick_state.left_x_position);
  Serial.print(" L_Y: ");
  Serial.print(current_joystick_state.left_y_position);
  Serial.print(" L_B: ");
  Serial.print(current_joystick_state.left_button_state);
  Serial.print(" R_X: ");
  Serial.print(current_joystick_state.right_x_position);
  Serial.print(" R_Y: ");
  Serial.print(current_joystick_state.right_y_position);
  Serial.print(" R_B: ");
  Serial.println(current_joystick_state.right_button_state);
}

//void update_middle_ref(){
//    left_x_middle_ref = current_joystick_state.left_x_position;
//    left_y_middle_ref = current_joystick_state.left_y_position;
//    right_x_middle_ref = current_joystick_state.right_x_position;
//    right_y_middle_ref = current_joystick_state.right_y_position;
//}
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
  esp_now_register_recv_cb(OnDataRecv);

  pinMode(stand_by, OUTPUT);
  pinMode(m1_speed, OUTPUT);
  pinMode(m2_speed, OUTPUT);
  pinMode(forward_motor, OUTPUT);
  pinMode(backward_motor, OUTPUT);
  pinMode(left_motor, OUTPUT);
  pinMode(right_motor, OUTPUT);
  pinMode(battery_pin, INPUT);
  
  digitalWrite(forward_motor, LOW);
  digitalWrite(backward_motor, LOW);
  digitalWrite(left_motor, LOW);
  digitalWrite(right_motor, LOW);
  
  analogWrite(stand_by, 1024);
  analogWrite(m1_speed, 140);
  analogWrite(m2_speed, 140);

}
 
void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      printRemoteState();
      previousMillis = currentMillis;
      car_state.battery_percent = analogRead(battery_pin);
      // Send message via ESP-NOW
//      Serial.print("battery percentage : ");
//      Serial.println(car_state.battery_percent);
      esp_now_send(broadcastAddress, (uint8_t *) &car_state, sizeof(car_state));
    }
}
