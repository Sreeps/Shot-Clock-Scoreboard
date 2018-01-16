// Pin mappings
const int B[] = {16,5,4,0 , 14,12,13,15};



void dispBin(int num){
  
  for(int i = 0; i < 4; i++){

    // bitRead reads the binary bit of the number at the position indicated by the second argument
    if(bitRead(num, i) == 1)
      digitalWrite(B[i], HIGH);
    else
      digitalWrite(B[i], LOW);
  }
  Serial.println("Inside DispBin");
  Serial.println(num);

// Defining the required function
  Serial.print(digitalRead(B[3]));
  Serial.print("\t");
  Serial.print(digitalRead(B[2]));
  Serial.print("\t");
  Serial.print(digitalRead(B[1]));
  Serial.print("\t");
  Serial.print(digitalRead(B[0]));
  Serial.println("");
}
int highlow(char ch)
{
  if(ch=='1')
  {
    return HIGH;
  }
  else if(ch=='0')
  return LOW;

}
void setup() {
    Serial.begin(115200);
  
    //Setting the first digit pins to OUTPUT
    for(int i = 0; i <= 7; i ++)
      pinMode(B[i], OUTPUT);

    //Initializing the output
      digitalWrite(B[0], LOW);
      digitalWrite(B[1], LOW);
      digitalWrite(B[2], HIGH);
      digitalWrite(B[3], HIGH);

      digitalWrite(B[4], LOW);
      digitalWrite(B[5], HIGH);
      digitalWrite(B[6], LOW);
      digitalWrite(B[7], LOW);  

    for(int i = 0; i <= 3; i ++)
      Serial.print(B[i]);

}

void loop() {
  
  if(digitalRead(B[0]) == LOW && digitalRead(B[1]) == LOW && digitalRead(B[2]) == LOW && digitalRead(B[3]) == LOW && digitalRead(B[4]) == LOW && digitalRead(B[5]) == LOW && digitalRead(B[6]) == LOW && digitalRead(B[7]) == LOW ){
      digitalWrite(B[0], LOW);
      digitalWrite(B[1], LOW);
      digitalWrite(B[2], HIGH);
      digitalWrite(B[3], HIGH);

      digitalWrite(B[4], LOW);
      digitalWrite(B[5], HIGH);
      digitalWrite(B[6], LOW);
      digitalWrite(B[7], LOW);
    }
    for(int i = 24; i >= 0; i --){
      int bcd=(i/10*16)+(i%10);
      //dispBin(i);
      String a = String(bcd,BIN);
      int z=8-a.length();
      for(int j=0;j<z;j++)
      a="0"+a;
      Serial.println(a);
      digitalWrite(B[0],highlow(a[3]));
      Serial.println(highlow(a[3]));
      digitalWrite(B[1],highlow(a[2]));
      Serial.println(highlow(a[2]));
      digitalWrite(B[2],highlow(a[1]));
      Serial.println(highlow(a[1]));
      digitalWrite(B[3],highlow(a[0]));
      Serial.println(highlow(a[0]));
      Serial.println("");
      Serial.println("");

      digitalWrite(B[4],highlow(a[7]));
      Serial.println(highlow(a[7]));
      digitalWrite(B[5],highlow(a[6]));
      Serial.println(highlow(a[6]));
      digitalWrite(B[6],highlow(a[5]));
      Serial.println(highlow(a[5]));
      digitalWrite(B[7],highlow(a[4]));
      Serial.println(highlow(a[4]));
      Serial.println("");

      delay(1000);
    }  

}


