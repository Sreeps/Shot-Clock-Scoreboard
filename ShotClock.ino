#include <ESP8266WiFi.h>

//WIfi AP name and password
const char* ssid = "nodemcu";
const char* password = "12345678";

// Pins for the LEDs
const int B[] = {0, 4, 5, 16, 15, 13, 12, 14};
const char* str = "/LED_1/ON/";
const char* stp = "/LED_1/Stop/";
const char* rst = "/LED_1/Reset/";

// Initial states
int str_state = LOW;
int stp_state = LOW;
int rst_state = LOW;
int prev_state = 24;

// Flag values
int flag = 0; // variable to indicate if it is the first start or not since power up.
int reset_flag, start_flag, stop_flag; // These flags are used to start the clock once again during normal running time.
int buttons[] = {HIGH, HIGH, HIGH}; // Order is Start Stop Reset

// Create an instance of the server and specify the port to listen on as an argument
WiFiServer server(80);

// Creating a client
WiFiClient client;
  
// Function definitions
// Converts from integer to HIGH/LOW
int int_highlow(char ch){
  if(ch == '1')
    return HIGH;
  else if(ch == '0')
    return LOW;
}

// Converts from HIGH/LOW to integer
char highlow_int(int ch){
  if(ch == HIGH)
    return '1';
  else if(ch == LOW)
    return '0';
}

// Reads the requests from the client
int digitalRead_v2(const char* pin, int buttons[],int index){
  
  String req = client.readStringUntil('\r');
  client.flush();
  if(req.indexOf(pin) != -1)
    buttons[index] = LOW;
  else if(req.indexOf("/") != -1)
    ; // Do nothing
  else
    client.stop();
    return 0;
  client.flush();
  return buttons[index]; 
}

// Starts/ Restarts the clock.
void StartClock(int prev_state){    

    // Converting prev_state to binary for the uC to understand
    start_flag = 0;
    int bcd = (prev_state / 10 * 16) + (prev_state % 10);
    String state_str = String(bcd, BIN);
    int z = 8-state_str.length();
    for(int j = 0 ; j < z ; j++)
       state_str = "0" + state_str;
    digitalWrite(B[0], int_highlow(state_str[3]));
    digitalWrite(B[1], int_highlow(state_str[2]));
    digitalWrite(B[2], int_highlow(state_str[1]));
    digitalWrite(B[3], int_highlow(state_str[0]));
    digitalWrite(B[4], int_highlow(state_str[7]));
    digitalWrite(B[5], int_highlow(state_str[6]));
    digitalWrite(B[6], int_highlow(state_str[5]));
    digitalWrite(B[7], int_highlow(state_str[4]));
    RunClock(prev_state);

}

// Pauses the clock.
void StopClock(){
    
  // Obtaining the stopped state and converting that to BCD.
  stop_flag = 0;
  int state_stp = 0;
  int state_stp1 = 0;
  for(int j = 0; j < 4; j++)
    state_stp += digitalRead(B[j]) << j;
  for(int j = 4; j < 8; j++)
    state_stp1 += digitalRead(B[j]) << (j-4); 
  prev_state = state_stp * 10 + state_stp1; 
  // Wait for any button (Start or Reset) to be pressed
  while(digitalRead_v2(str, buttons, 0) == HIGH | digitalRead_v2(rst, buttons, 2) == HIGH){
    
    // Do nothing. But what if the start or reset is pressed?
    if(buttons[0] == LOW){
        StartClock(prev_state);
        buttons[2] = buttons[1] = buttons[0] = HIGH;
        return;
    }
    if(buttons[2] == LOW){
      buttons[2] = HIGH;
      ResetClock();
      buttons[2] = buttons[1] = buttons[0] = HIGH;
      return;
    }
      ;// Now you do nothing!
  }
}

// Resets the clock to 24.
void ResetClock(){
    // Resetting clock to the starting value
    digitalWrite(B[0], LOW);
    digitalWrite(B[1], HIGH);
    digitalWrite(B[2], LOW);
    digitalWrite(B[3], LOW);
    digitalWrite(B[4], LOW);
    digitalWrite(B[5], HIGH);
    digitalWrite(B[6], LOW);
    digitalWrite(B[7], LOW);  
    prev_state = 24; 
    reset_flag = 0;   
}

// Runs the clock.  
void RunClock(int prev_state){ 

  // Resetting the clock to the 24 second mark once countdown is over. All Leds are lit to show time is up.
  if(digitalRead(B[0]) == LOW and digitalRead(B[1]) == LOW and digitalRead(B[2]) == LOW and digitalRead(B[3]) == LOW and digitalRead(B[4]) == LOW and digitalRead(B[5]) == LOW and digitalRead(B[6]) == LOW and digitalRead(B[7]) == LOW ){
    
    Serial.println("End of countdown. Time is up!!");
    digitalWrite(B[0], HIGH);
    digitalWrite(B[1], HIGH);
    digitalWrite(B[2], HIGH);
    digitalWrite(B[3], HIGH);
    digitalWrite(B[4], HIGH);
    digitalWrite(B[5], HIGH);
    digitalWrite(B[6], HIGH);
    digitalWrite(B[7], HIGH);

    while((client.readStringUntil('\r')).indexOf(rst) == -1){
      client.flush(); // Wait until reset is pressed to start the clock again.
    }
    reset_flag = 1;
    return;
  }

  for(int i = prev_state; i >= 0; i --){

    if(digitalRead_v2(rst, buttons, 2) == LOW){
      reset_flag = 1;
      buttons[0] = buttons[1] = buttons[2] = HIGH;
      return;
    }   
    else if(digitalRead_v2(stp, buttons, 1) == LOW){
        stop_flag = 1;
        buttons[0] = buttons[1] = buttons[2] = HIGH;
        return;
    }
    int bcd = (i / 10 * 16) + (i % 10);
    String state = String(bcd,BIN);
    int z = 8 - state.length();
    for(int j = 0; j < z ; j ++)
      state = "0" + state;    
    digitalWrite(B[0], int_highlow(state[3]));
    digitalWrite(B[1], int_highlow(state[2]));
    digitalWrite(B[2], int_highlow(state[1]));
    digitalWrite(B[3], int_highlow(state[0]));
    digitalWrite(B[4], int_highlow(state[7]));
    digitalWrite(B[5], int_highlow(state[6]));
    digitalWrite(B[6], int_highlow(state[5]));
    digitalWrite(B[7], int_highlow(state[4]));
    delay(1000);
  }
  if(digitalRead(B[0]) == LOW and digitalRead(B[1]) == LOW and digitalRead(B[2]) == LOW and digitalRead(B[3]) == LOW and digitalRead(B[4]) == LOW and digitalRead(B[5]) == LOW and digitalRead(B[6]) == LOW and digitalRead(B[7]) == LOW ){
    
    Serial.println("End of countdown. Time is up!!");
    digitalWrite(B[0], HIGH);
    digitalWrite(B[1], HIGH);
    digitalWrite(B[2], HIGH);
    digitalWrite(B[3], HIGH);
    digitalWrite(B[4], HIGH);
    digitalWrite(B[5], HIGH);
    digitalWrite(B[6], HIGH);
    digitalWrite(B[7], HIGH);

    while((client.readStringUntil('\r')).indexOf(rst) == -1){
      client.flush(); // Wait until reset is pressed to start the clock again.
    }
    reset_flag = 1;
    return;
  }
}

void setup() {

   // Starting the WiFi Server
   WiFi.softAP(ssid, password); // remove the password parameter if you want the AP to be open.
   server.begin();
    delay(2000);
    // Setting the digit pins to OUTPUT
    for(int i = 0; i <= 7; i ++)
      pinMode(B[i], OUTPUT);
    
    // Initializing the output
    digitalWrite(B[0], LOW);
    digitalWrite(B[1], HIGH);
    digitalWrite(B[2], LOW);
    digitalWrite(B[3], LOW);
    digitalWrite(B[4], LOW);
    digitalWrite(B[5], HIGH);
    digitalWrite(B[6], LOW);
    digitalWrite(B[7], LOW);  
    prev_state = 24;     

    // Initializing flags
    reset_flag = 0;
    start_flag = 0;
    stop_flag = 0;

  // Checking availability of the client
  client = server.available();
  if (!client) {
    return;
  }

  // Waiting for client to send some data
  while(!client.available()){
    delay(1);
  }
  String HTML = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html> <h1>DIY HowToDoIt</h1> <h2>ESP8266 Web Server</h2>";
  HTML += "<h3>Blue LED is now ";
  HTML += (digitalRead(BUILTIN_LED))?"OFF":"ON</h3>"; //Low = LED ON and High = LED OFF
  HTML += "<h3>Blue LED <button onclick=\"window.location.href='/LED_1/on'\">LED ON</button>&nbsp;<button onclick=\"window.location.href='/LED_1/off'\">LED OFF</button></h3></html>";
 

  client.print(HTML);
  delay(10);
    // Wait for a button to be pressed to start the next part
    while((digitalRead_v2(str, buttons, 0) == HIGH and digitalRead_v2(rst, buttons, 2) == HIGH and digitalRead_v2(stp, buttons, 1) == HIGH) and (reset_flag == 0 and start_flag == 0 and stop_flag == 0) or flag != 0){
      // Do nothing until something is pressed
    }
}

void loop(){  

  // If not button is pressed initially then do nothing. Start the clock when the start button is pressed first.
  // To detect the press of start, stop or reset
  if(buttons[0] == LOW and flag == 0){
    // Make the flag variable one to indicate the initial startup is complete.
    flag = 1;
    prev_state = 24;
    buttons[0] = buttons[1] = buttons[2] = HIGH;
    RunClock(prev_state);
  }
  else if(((buttons[2] == LOW and buttons[0] == LOW) or (buttons[2] == LOW and buttons[1] == LOW) or (buttons[1] == LOW and buttons[0] == LOW)) and flag == 0){
    ResetClock();
  }
  else if(((buttons[2] == LOW and buttons[0] == LOW) or (buttons[2] == LOW and buttons[1] == LOW) or (buttons[1] == LOW and buttons[0] == LOW)) and flag != 0){
    ResetClock();
    RunClock(prev_state);
  }
  else if((buttons[2] == LOW and flag != 0) or reset_flag == 1){
    ResetClock();
    RunClock(prev_state);
  }
  else if((buttons[0] == LOW and flag != 0) or start_flag == 1){
    StartClock(prev_state);
    RunClock(prev_state);
  }
  else if((buttons[1] == LOW and flag != 0) or stop_flag == 1){
    buttons[0] = buttons[1] = buttons[2] = HIGH;
    StopClock();
  }
  else{
    // Wait for start to be pressed.
    while(digitalRead_v2(str, buttons, 0) == HIGH){
      ;// Do Nothing (for now)
    }
    buttons[0] = buttons[1] = buttons[2] = HIGH;   
    StartClock(prev_state);
    RunClock(prev_state); 
  }

   String HTML = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html> <h1>DIY HowToDoIt</h1> <h2>ESP8266 Web Server</h2>";
  HTML += "<h3>Blue LED is now ";
  HTML += (digitalRead(BUILTIN_LED))?"OFF":"ON</h3>"; //Low = LED ON and High = LED OFF
  HTML += "<h3>Blue LED <button onclick=\"window.location.href='/LED_1/on'\">LED ON</button>&nbsp;<button onclick=\"window.location.href='/LED_1/off'\">LED OFF</button></h3></html>";
 

  client.print(HTML);
  delay(10);
  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}
