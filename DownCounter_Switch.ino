const int B[] = {10, 11, 12, 13, 9, 8, 7, 6};
const int str = 5;
const int stp = 4;
const int rst = 3;

int str_state = LOW;
int stp_state = LOW;
int rst_state = LOW;
int prev_state = 24;
// int dec_prev_state = 24;
int flag = 0; // variable to indicate if it is the first start or not since power up.
int reset_flag, start_flag, stop_flag; // These flags are used to start the clock once again during normal running time.
int buttons[] = {HIGH, HIGH, HIGH}; // Order is Start Stop Reset


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

int digitalRead_v2(const int pin, int buttons[],int index){
  buttons[index] = digitalRead(pin);
  return buttons[index]; 
}

void StartClock(int prev_state){    
    // Setting the clock to the previous state
    // Check prompt
    Serial.println("Inside Start Clock");

    // Converting prev_state to binary for the uC to understand
    start_flag = 0;
    Serial.println(prev_state);
    int bcd = (prev_state / 10 * 16) + (prev_state % 10);
    String state_str = String(bcd, BIN);
    Serial.println(state_str);
    int z = 8-state_str.length();
    for(int j = 0 ; j < z ; j++)
       state_str = "0" + state_str;
    Serial.println(state_str);
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
  // Check prompt
  Serial.println("Inside Stop Clock");
  
  // Obtaining the stopped state and converting that to BCD.
  stop_flag = 0;
  int state_stp = 0;
  int state_stp1 = 0;
  for(int j = 0; j < 4; j++)
    //state_stp[j] = char(highlow_int(digitalRead(B[j])));
    state_stp += digitalRead(B[j]) << j;
  for(int j = 4; j < 8; j++)
    state_stp1 += digitalRead(B[j]) << (j-4); 
  prev_state = state_stp * 10 + state_stp1; 
  // Wait for any button (Start or Reset) to be pressed
  while(digitalRead_v2(str, buttons, 0) == HIGH | digitalRead_v2(rst, buttons, 2) == HIGH){
    
    // Do nothing. But what if the start or reset is pressed?
    if(buttons[0] == LOW){
        Serial.println(buttons[0]);
        Serial.println(buttons[1]);
        Serial.println(buttons[2]);
        Serial.println(prev_state);
        StartClock(prev_state);
        buttons[2] = buttons[1] = buttons[0] = HIGH;
        return;
    }
    if(buttons[2] == LOW){
      Serial.println("Second condition");
      buttons[2] = HIGH;
      Serial.println(buttons[0]);
      Serial.println(buttons[1]);
      Serial.println(buttons[2]);
      ResetClock();
      buttons[2] = buttons[1] = buttons[0] = HIGH;
      return;
    }
      ;// Now you do nothing!
  }
}

void ResetClock(){
    // Resetting clock to the starting value
    Serial.println("Inside Reset Clock");
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

// RunClock function needs work. Basically it should stop counting whenever I press a button. Make sure to restart the function from scratch when using th functions StartClock, StopClock and ResetClock 
void RunClock(int prev_state){
  
  Serial.println("Inside Run Clock");
  // Coverting previous state to required decimal value
  // dec_prev_state = (prev_state / 16 * 10) + (prev_state % 16);

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

    while(digitalRead(rst) == HIGH){
      ; // Wait until reset is pressed to start the clock again.
    }
    Serial.println("reset pressed");
    reset_flag = 1;
    return;
  }
  Serial.print("Previous state: ");
  Serial.println(prev_state);
  for(int i = prev_state; i >= 0; i --){

    Serial.println("Inside Run Clock loop");
    if(digitalRead_v2(rst, buttons, 2) == LOW){
      Serial.println("Checking if reset is pressed when running");
      reset_flag = 1;
      buttons[0] = buttons[1] = buttons[2] = HIGH;
      return;
    }   
    else if(digitalRead_v2(stp, buttons, 1) == LOW){
        Serial.println("Checking if stop is pressed when running");
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

    while(digitalRead(rst) == HIGH){
      Serial.println("Waiting for reset to pressed"); // Wait until reset is pressed to start the clock again.
    }
    reset_flag = 1;
    return;
  }
}

void setup() {
    Serial.begin(115200);
  
    Serial.println("Inside setup");
    // Setting the digit pins to OUTPUT
    for(int i = 0; i <= 7; i ++)
      pinMode(B[i], OUTPUT);

    // Setting button pins to input
    pinMode(str, INPUT);
    pinMode(stp, INPUT);
    pinMode(rst, INPUT);
    
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

    // Wait for a button to be pressed to start the next part
    while((digitalRead_v2(str, buttons, 0) == HIGH and digitalRead_v2(rst, buttons, 2) == HIGH and digitalRead_v2(stp, buttons, 1) == HIGH) and (reset_flag == 0 and start_flag == 0 and stop_flag == 0) or flag != 0){
      // Do nothing until something is pressed
    }
    Serial.println(prev_state);
    Serial.println(buttons[0]);
    Serial.println(buttons[1]);
    Serial.println(buttons[2]);
    Serial.println("Going to loop");
}

void loop(){  
  // If not button is pressed initially then do nothing. Start the clock when the start button is pressed first.
  // To detect the press of start, stop or reset
  Serial.println("Inside loop");
  if(buttons[0] == LOW and flag == 0){
    // Make the flag variable one to indicate the initial startup is complete.
    Serial.println("First condition inside loop");
    flag = 1;
    prev_state = 24;
    buttons[0] = buttons[1] = buttons[2] = HIGH;
    RunClock(prev_state);
  }
  else if(((buttons[2] == LOW and buttons[0] == LOW) or (buttons[2] == LOW and buttons[1] == LOW) or (buttons[1] == LOW and buttons[0] == LOW)) and flag == 0){
    Serial.println("Second condition inside loop");
    delay(1000);
    ResetClock();
  }
  else if(((buttons[2] == LOW and buttons[0] == LOW) or (buttons[2] == LOW and buttons[1] == LOW) or (buttons[1] == LOW and buttons[0] == LOW)) and flag != 0){
    Serial.println("Third condition inside loop");
    delay(1000);
    ResetClock();
    RunClock(prev_state);
  }
  else if((buttons[2] == LOW and flag != 0) or reset_flag == 1){
    Serial.println("Fourth condition inside loop");
    delay(1000);
    ResetClock();
    RunClock(prev_state);
  }
  else if((buttons[0] == LOW and flag != 0) or start_flag == 1){
    Serial.println("Fifth condition loop");
    delay(1000);
    StartClock(prev_state);
    RunClock(prev_state);
  }
  else if((buttons[1] == LOW and flag != 0) or stop_flag == 1){
    Serial.println("Sixth condition inside loop");
    buttons[0] = buttons[1] = buttons[2] = HIGH;
    StopClock();
  }
  else{
    Serial.println("Final condition inside loop");
    // Wait for start to be pressed.
    while(digitalRead_v2(str, buttons, 0) == HIGH){
      Serial.print(buttons[0]);
      Serial.print("\t");
      Serial.print(buttons[1]);
      Serial.print("\t");
      Serial.println(buttons[2]);// Do Nothing (for now)
    }
    buttons[0] = buttons[1] = buttons[2] = HIGH;   
    StartClock(prev_state);
    RunClock(prev_state); 
  }
}

 
