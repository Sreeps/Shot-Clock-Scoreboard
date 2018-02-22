#include <ESP8266WiFi.h>

const int B[] = {3, 13, 12, 14, 2, 0, 4, 5}; // Change for NodeMCU
const String str = "/BUTTON=Start";
const String stp = "/BUTTON=Stop";
const String rst24 = "/BUTTON=Reset24";
const String rst14 = "/BUTTON=Reset14";

// WiFi details
const char* ssid = "nodemcu";
const char* password = "12345678";

// Creating an instance of the server and the client
WiFiServer server(80);
WiFiClient client;
String request;

int prev_state = 24;

int flag = 0; // variable to indicate if it is the first start or not since power up.
int reset24_flag, reset14_flag, start_flag, stop_flag; // These flags are used to start the clock once again during normal running time.

// Function defintions
int int_highlow(char ch){
  if(ch == '1')
    return HIGH;
  else if(ch == '0')
    return LOW;
}

char highlow_int(int ch){
  if(ch == HIGH)
    return '1';
  else if(ch == LOW)
    return '0';
}

void StartClock(int prev_state){    
  
    start_flag = 0;
    
    // Converting prev_state to binary for the uC to understand
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

void StopClock(){  
    
  stop_flag = 0;
  
  // Obtaining the stopped state and converting that to BCD.
  int state_stp = 0;
  int state_stp1 = 0;
  for(int j = 0; j < 4; j++)
    state_stp += digitalRead(B[j]) << j;
  for(int j = 4; j < 8; j++)
    state_stp1 += digitalRead(B[j]) << (j-4); 
  prev_state = state_stp * 10 + state_stp1; 
  
  // Wait for any button (Start or Reset) to be pressed 
  while(1){

    request = client.readStringUntil('\r');
    if(request.indexOf(str) != -1){
        start_flag = 1;
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("");
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.println("<br>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Start\"><button>Start</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Stop\"\"><button>Stop</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Reset14\"\"><button>Reset14</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Reset24\"\"><button>Reset24</button></a><br />");
        client.print("</html>");
        break;
    }
    else if(request.indexOf(rst24) != -1){
      reset24_flag = 1;
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<!DOCTYPE HTML>");
      client.println("<html>");
      client.println("<br>");
      client.println("<a href=\"http://192.168.43.72/BUTTON=Start\"><button>Start</button></a>");
      client.println("<a href=\"http://192.168.43.72/BUTTON=Stop\"\"><button>Stop</button></a>");
      client.println("<a href=\"http://192.168.43.72/BUTTON=Reset14\"\"><button>Reset14</button></a>");
      client.println("<a href=\"http://192.168.43.72/BUTTON=Reset24\"\"><button>Reset24</button></a><br />");
      client.print("</html>");
      break;
    }
    else if(request.indexOf(rst14) != -1){
      reset14_flag = 1;
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<!DOCTYPE HTML>");
      client.println("<html>");
      client.println("<br>");
      client.println("<a href=\"http://192.168.43.72/BUTTON=Start\"><button>Start</button></a>");
      client.println("<a href=\"http://192.168.43.72/BUTTON=Stop\"\"><button>Stop</button></a>");
      client.println("<a href=\"http://192.168.43.72/BUTTON=Reset14\"\"><button>Reset14</button></a>");
      client.println("<a href=\"http://192.168.43.72/BUTTON=Reset24\"\"><button>Reset24</button></a><br />");
      client.print("</html>");
      break;
    }
    client.flush();
  }
  client.flush();
  if(start_flag == 1)
    StartClock(prev_state);
  if(reset24_flag == 1)
    ResetClock_24();
  if(reset14_flag == 1)
    ResetClock_14();
}

void ResetClock_24(){
  
    reset24_flag = 0; 
    
    // Resetting clock to the starting value
    digitalWrite(B[0], LOW);
    digitalWrite(B[1], LOW);
    digitalWrite(B[2], HIGH);
    digitalWrite(B[3], LOW);
    digitalWrite(B[4], LOW);
    digitalWrite(B[5], HIGH);
    digitalWrite(B[6], LOW);
    digitalWrite(B[7], LOW);  
    prev_state = 24;   
}

void ResetClock_14(){
  
    reset14_flag = 0; 
    
    // Resetting clock to 14s
    digitalWrite(B[0], LOW);
    digitalWrite(B[1], LOW);
    digitalWrite(B[2], LOW);
    digitalWrite(B[3], HIGH);
    digitalWrite(B[4], LOW);
    digitalWrite(B[5], HIGH);
    digitalWrite(B[6], LOW);
    digitalWrite(B[7], LOW);  
    prev_state = 14;   
}

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
    
    while(1){
      request = client.readStringUntil('\r'); // Wait until reset is pressed to start the clock again.
      if(request.indexOf(rst24) != -1){
        Serial.println("Reset 24 pressed");
        reset24_flag = 1;
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("");
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.println("<br>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Start\"><button>Start</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Stop\"\"><button>Stop</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Reset14\"\"><button>Reset14</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Reset24\"\"><button>Reset24</button></a><br />");
        client.print("</html>");
        break;   
      }
      client.flush();
    }
    client.flush();
    if(reset24_flag == 1)
      ResetClock_24();
  }
  Serial.print("Previous state: ");
  Serial.println(prev_state);
  for(int i = prev_state; i >= 0; i --){

    Serial.println("Inside Run Clock loop");
    request = client.readStringUntil('\r');
    if(request.indexOf(rst24) != -1){
        Serial.println("Checking if reset is pressed when running");
        reset24_flag = 1;
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("");
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.println("<br>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Start\"><button>Start</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Stop\"\"><button>Stop</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Reset14\"\"><button>Reset14</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Reset24\"\"><button>Reset24</button></a><br />");
        client.print("</html>");
        break;
    }   
    else if(request.indexOf(rst14) != -1){
        Serial.println("Checking if stop is pressed when running");
        reset14_flag = 1;
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("");
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.println("<br>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Start\"><button>Start</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Stop\"\"><button>Stop</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Reset14\"\"><button>Reset14</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Reset24\"\"><button>Reset24</button></a><br />");
        client.print("</html>");
        break;
    }
    else if(request.indexOf(stp) != -1){
        Serial.println("Checking if stop is pressed when running");
        stop_flag = 1;
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("");
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.println("<br>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Start\"><button>Start</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Stop\"\"><button>Stop</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Reset14\"\"><button>Reset14</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Reset24\"\"><button>Reset24</button></a><br />");
        client.print("</html>");
        break;
    }
    client.flush();
    
    int bcd = (i / 10 * 16) + (i % 10);
    String state = String(bcd,BIN);
    int z = 8 - state.length();
    for(int j = 0; j < z ; j ++)
      state = "0" + state;    
    digitalWrite(B[0], int_highlow(state[0]));
    digitalWrite(B[1], int_highlow(state[1]));
    digitalWrite(B[2], int_highlow(state[2]));
    digitalWrite(B[3], int_highlow(state[3]));
    digitalWrite(B[4], int_highlow(state[4]));
    digitalWrite(B[5], int_highlow(state[5]));
    digitalWrite(B[6], int_highlow(state[6]));
    digitalWrite(B[7], int_highlow(state[7]));
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

    while(1){
      request = client.readStringUntil('\r'); // Wait until reset is pressed to start the clock again.
      if(request.indexOf(rst24) != -1){
        Serial.println("Reset 24 pressed");
        reset24_flag = 1;
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("");
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.println("<br>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Start\"><button>Start</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Stop\"\"><button>Stop</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Reset14\"\"><button>Reset14</button></a>");
        client.println("<a href=\"http://192.168.43.72/BUTTON=Reset24\"\"><button>Reset24</button></a><br />");
        client.print("</html>");
        break;   
      }
      client.flush();
    }
    client.flush();
    if(reset24_flag == 1)
      ResetClock_24();
  }
}

void setup() {

    Serial.begin(115200);
    Serial.println("Inside setup");

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
    reset24_flag = 0;
    reset14_flag = 0;
    start_flag = 0;
    stop_flag = 0;    
    
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
    server.begin();
    Serial.print("The server is started. Connect to http://");
    Serial.println(WiFi.localIP()); 
}

void loop() {

   // Checking availability of the client
  client = server.available();
  if (!client) {
    return;
  }

  Serial.println("The server is available");
  
  // Waiting for client to send some data
  while(!client.available()){
    delay(1);
  }

  request = client.readStringUntil('\r');
  if(request.indexOf(str) != -1 & flag == 0){ // Pressing start button for the first time
    flag = 1;
    prev_state = 24;
    RunClock(prev_state);
  }
  else if((request.indexOf(rst24) != -1 & flag != 0) | reset24_flag == 1){ // Requesting clock reset to 24
    ResetClock_24();
    RunClock(prev_state); 
  }
  else if((request.indexOf(rst14) != -1 & flag != 0) | reset14_flag == 1){ // Requesting clock reset to 14
    ResetClock_14();
    RunClock(prev_state); 
  }
  else if((request.indexOf(str) != -1 & flag != 0) | start_flag == 1){ // Requesting clock start
    StartClock(prev_state);
    RunClock(prev_state); 
  }
  else if((request.indexOf(stp) != -1 & flag != 0) | stop_flag == 1){ // Requesting clock stop
    StopClock();
  }
  else if(request.indexOf("/") != -1 & flag == 0){ // Random button press
    ResetClock_24();
  }
  else if(request.indexOf("/") != -1 & flag != 0){ // Random button press
    ResetClock_24();
    RunClock(prev_state);
  }
  else
    ;
  
  client.flush();

  // Printing the HTML
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<br>");
  client.println("<a href=\"http://192.168.43.72/BUTTON=Start\"><button>Start</button></a>");
  client.println("<a href=\"http://192.168.43.72/BUTTON=Stop\"\"><button>Stop</button></a>");
  client.println("<a href=\"http://192.168.43.72/BUTTON=Reset14\"\"><button>Reset14</button></a>");
  client.println("<a href=\"http://192.168.43.72/BUTTON=Reset24\"\"><button>Reset24</button></a><br />");
  client.print("</html>");
}
