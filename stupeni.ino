#include <Wire.h>
 
const byte  mcp_address=0x20; // I2C Address of MCP23017 Chip
const byte  GPIOA=0x13; // Register Address of Port A
const byte  GPIOB=0x12; // Register Address of Port B


const int gpio_led_arr[8] = {4, 5,6,7,8,9,10,11};
 
const byte sequence[9] = {0b00000000, 0b00000001, 0b00000011, 0b00000111, 0b00001111, 0b00011111, 0b00111111, 0b01111111, 0b11111111};
const byte only_one[8] = {0b01111111, 0b10111111, 0b11011111, 0b11101111, 0b11110111, 0b11111011, 0b11111101, 0b11111110};
const byte only_one_light[10] = {0b11111111,0b01111111, 0b10111111, 0b11011111, 0b11101111, 0b11110111, 0b11111011, 0b11111101, 0b11111110,0b11111111};
const byte only_tow[11] = {0b11111111,0b01111111, 0b00111111, 0b10011111, 0b11001111, 0b11100111, 0b11110011, 0b11111001, 0b11111100,0b11111110,0b11111111};
const byte r_two_random[8] = {0b01011111, 0b10101111, 0b11010111, 0b11101011, 0b11110101, 0b11111010, 0b01111101, 0b10111110};

int typeAlgoShow=1;
byte GPIO_w=0x12;

const int buttonPin = 2;
int stop_type = 0;
const int ledPin =  13;
int buttonState = 0;


void buttonPush(){
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    digitalWrite(ledPin, HIGH);
    stop_type=1;
    typeAlgoShow++;
    Serial.println("algo num:"+(String)typeAlgoShow);
    if ( typeAlgoShow >5 ) { 
      typeAlgoShow=1;
    }
  } else {
    stop_type=0;
    digitalWrite(ledPin, LOW);
    if ( typeAlgoShow >5 ) { 
      typeAlgoShow=1;
    }
  }
}
void setup()
{
  Serial.begin(9600);
    Wire.begin();
    Wire.beginTransmission(mcp_address);
    Wire.write((byte)0x00);
    Wire.write((byte)0x00); // set all of bank A to outputs
    Wire.write((byte)0x00); // set all of bank B to outputs
    Wire.endTransmission();
     pinMode(buttonPin, INPUT_PULLUP);
     pinMode(ledPin, OUTPUT);

     int t=sizeof(gpio_led_arr)/2;
     for (int i=0;i<t;i++){
        pinMode(gpio_led_arr[i], OUTPUT); 
        digitalWrite(gpio_led_arr[i], HIGH);
        Serial.print( " -"+(String)gpio_led_arr[i]+"- ");
     }
     //digitalWrite(gpio_led_arr[1], LOW);
     Serial.println(" setup end len:"+(String)t);
}
void two_random(){
  int pos = 0;
  int pos1 = 0;
    for (pos=0; pos < 16; pos++)
    {
      int random1 =random(0, 8);
      buttonPush();
      Serial.println ( "1random:"+(String)random1 + " pos:" +(String)pos);
      Wire.beginTransmission(mcp_address);
      Wire.write(GPIOB);
      Wire.write((byte)r_two_random[random1]);  // value to send - all HIGH
      Wire.endTransmission();
      delay(200);
    }
    for (pos=0; pos < 16; pos++)
    {
      int random1 =random(0, 8);
      buttonPush();
      Serial.println ( "random:"+(String)random1 + " pos:" +(String)pos);
      Wire.beginTransmission(mcp_address);
      Wire.write(GPIOA);
      Wire.write((byte)r_two_random[random1]);  // value to send - all HIGH
      Wire.endTransmission();
      delay(200);
    }
 }

void off_light_All(){
  //off all light BPIOB
      Wire.beginTransmission(mcp_address);
      Wire.write(GPIOA);
      Wire.write((byte)sequence[8]);  // value to send - all HIGH
      Wire.endTransmission();
      Wire.beginTransmission(mcp_address);
      Wire.write(GPIOB);
      Wire.write((byte)sequence[8]);  // value to send - all HIGH
      Wire.endTransmission();
}
void random_light(){
  
  int pos = 0;
  int pos1 = 0;
    for (pos=0; pos < 16; pos++)
    {
      int random1 =random(0, 16);
      buttonPush();
      off_light_All();
      
      if ( ( random1 > 0 ) && ( random1 <= 8 ) ) {
        GPIO_w = GPIOB;
      }
      else if ( ( random1>8 ) && ( random1 <= 16 )) {
        GPIO_w = GPIOA;
      } else { }
      if ( random1 > 8) 
        random1 =random1-8;
        
      Serial.println ( "random:"+(String)random1 + " pos:" +(String)pos);
      Wire.beginTransmission(mcp_address);
      Wire.write(GPIO_w);
      Wire.write((byte)only_one_light[random1]);  // value to send - all HIGH
      Wire.endTransmission();
      delay(500);
    }
 }
 
 void step_by_step(){
  int pos = 0;
  int pos1 = 0;
  int way=0;
  int addr_light=0;
  Wire.beginTransmission(mcp_address);
  Wire.write(GPIOB);
  Wire.write((byte)sequence[0]);  // value to send - all HIGH
  Wire.endTransmission();
  Wire.beginTransmission(mcp_address);
  Wire.write(GPIOA);
  Wire.write((byte)sequence[0]);  // value to send - all HIGH
  Wire.endTransmission();
    for (pos=0; pos < 48; pos++)
    {
      if (stop_type != 1){
        buttonPush();
        Serial.println ( "pos:"+(String)pos + " pos1:" +(String)pos1 + "way:" + (String)way );
        if ( pos1<=16 ){
          Wire.beginTransmission(mcp_address);
          addr_light = pos1;
        
          if ( ( pos1 > 0 ) && ( pos1 <= 8 ) ) {
            GPIO_w = GPIOB;
          }
          else if ( ( pos1>8 ) && ( pos1 <= 16 )) {
            GPIO_w = GPIOA;
          } else {}
         
          if ((pos1>8) && (pos1<17) && ( way==0) ){
            addr_light = addr_light-8;
          } else if (( way==1) && (pos1>=8) ){
            addr_light = addr_light-8;
            if ( ( pos1 == 8) && (way ==1) && ( addr_light==0 ) )  {
                Serial.println("back 0 then ");
                GPIO_w = GPIOA; 
            }
          }
          Wire.write(GPIO_w);
          Wire.write((byte)sequence[addr_light]);
          Wire.endTransmission();
        }
        
        if ((pos1>16) && (pos1<=24) && ( way==0) ){
            digitalWrite(gpio_led_arr[(pos-17)], LOW);
            Serial.print( " -"+(String)gpio_led_arr[(pos-17)]+"- ");
        }
        if ( (pos1>=16) && (pos1<=24) && ( way==1) ){
            int t=(sizeof(gpio_led_arr)/2)-1;
            digitalWrite(gpio_led_arr[(-pos+25+t)], HIGH);
            Serial.println( " -|"+(String)(25)+ "-"+(String)(pos)+"+"+(String)(t)+"="+(String)(((-pos+25)+t))+ "|"+(String)gpio_led_arr[((-pos+25)+t)]+"- ");
        }
        delay(500);
        if (pos>=24){
          way=1;
        }
        if (way==1){
          pos1--;
        }else {
          pos1++;
        }
      } else {
        return 1;
      }
      
    }
 }


void step_by_step_one(){
  int pos = 0;
  int pos1 = 0;
  int way=0;
        
  Serial.println("start");
    for (pos=0; pos < 18; pos++)
    {
      Wire.beginTransmission(mcp_address);
      Wire.write(GPIOB);
      Wire.write((byte)only_one_light[pos1]);  // value to send - all HIGH
      Wire.endTransmission();
      delay(1500);
      if (pos>=9){
        way=1;
      }
      if (way==1){
        pos1--;
      }else { 
        pos1++;
      }
      buttonPush();
      Serial.println ( "pos:"+(String)pos + " pos1:" +(String)pos1 + "way:" + (String)way );
    }
 }
 
 void step_by_step_two(){
  int pos = 0;
  int pos1 = 0;
  int way=0;
        
  Serial.println("start");
    for (pos=0; pos < 20; pos++)
    {
      if (stop_type != 1){
        Wire.beginTransmission(mcp_address);
        Wire.write(GPIOB);
        Wire.write((byte)only_tow[pos1]);  // value to send - all HIGH
        Wire.endTransmission();
        delay(1500);
        if (pos>=10){
          way=1;
        }
        if (way==1){
          pos1--;
        }else { 
          pos1++;
        }
        buttonPush();
        Serial.println ( "pos:"+(String)pos + " pos1:" +(String)pos1 + "way:" + (String)way);
      }else { 
        return 0;
        }
    }
 }


void loop()
{
  if ( typeAlgoShow == 1 ) {  //16 ok
    Serial.println("1");
    step_by_step();
  };
  if ( typeAlgoShow == 2 ) {
    Serial.println("2");
    step_by_step_one();
  };
  if ( typeAlgoShow == 3 ) {
    Serial.println("3");
    step_by_step_two();
  };
  if  ( typeAlgoShow == 4) {   // 16 ok
    random_light();
    Serial.println("4");
  }
  if ( typeAlgoShow == 5) {
    Serial.println("5");
    two_random();
  }
  
  
}
