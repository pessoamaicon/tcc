#include <Keypad.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h> 
#include <DS1307.h> 
#include <SimpleModbusSlave.h> 

#define DS1307_ADDRESS 0x68 

#define s0 A0 
#define bl A1 
#define s1 A2 
#define s2 A3
#define s3 A6 
#define s4 A7 
#define s5 12 
#define m1 8 
#define m2 9 
#define m3 10
#define m4 11 

void tempo();                                   
void temperatura();                                     
void datatime();                                      
void changeMenu();                                    
void dispMenu();                                      
void menu1();                                           
void menu2();                                           
void menu3();                                          
void menu4();                                          

void dispSubMenu4();                                   
void readSelect(char option);                       
void subMenu4_1();                                    
void subMenu4_2();                                  
void subMenu4_3();                                     

int seg =0, seg2 =0, min= 0; 
int count = 0; 
int velo, velo1, velo2; 
double temp=0; 
bool s_s1 = 0, s_s2 = 0, s_s3 = 0, s_s4 = 0, s_s5 = 0;
enum { MOTOR1, 
       MOTOR2,
       MOTOR3, 
       MOTOR4,
       SENSOR0, 
       SENSOR1, 
       SENSOR2,
       SENSOR3, 
       SENSOR4, 
       SENSOR5, 
       BL,
       QTD, 
       HOLDING_REGS_SIZE }; 
unsigned int holdingRegs[HOLDING_REGS_SIZE];
bool s_bl; 
char menu = 0x01;                                       
char set1 = 0x00, set2 = 0x00, set3 = 0x00, set4 = 0x00;

const byte ROWS = 4; 
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {1, 0, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7}; 
byte zero = 0x00;

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x3f,20,4);
DS1307 rtc(A4, A5);

void setup(){
    modbus_configure(&Serial, 9600, SERIAL_8N1, 1, 2, HOLDING_REGS_SIZE,                                  holdingRegs);
    modbus_update_comms(9600, SERIAL_8N1, 1);

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
    //rtc.begin();
    rtc.halt(false);
    rtc.setDOW(SUNDAY);
    rtc.setTime(12, 0, 0);
    rtc.setDate(3, 10, 2018);
    rtc.setSQWRate(SQW_RATE_1);
    rtc.enableSQW(true);
    lcd.init();
    lcd.backlight();
    Wire.begin();                                   
    Serial.begin(9600);
    pinMode(bl, INPUT);
    pinMode(s0, INPUT);
    pinMode(s1, INPUT);
    pinMode(s2, INPUT);
    pinMode(s3, INPUT);
    pinMode(s4, INPUT);
    pinMode(m1, OUTPUT);
    pinMode(m2, OUTPUT);
    pinMode(m3, OUTPUT);
    pinMode(m4, OUTPUT);
    //pinMode(m5, OUTPUT);

    keypad.addEventListener(keypadEvent);
}

void loop(){
  modbus_update();
  analogWrite(m1, holdingRegs[MOTOR1]);
  analogWrite(m2, holdingRegs[MOTOR2]);
  analogWrite(m3, holdingRegs[MOTOR3]);
  analogWrite(m4, holdingRegs[MOTOR4]);
  digitalWrite(A1, holdingRegs[BL]);
  holdingRegs[SENSOR0] = analogRead(A0); 
  holdingRegs[SENSOR1] = s_s1;
  holdingRegs[SENSOR2] = s_s2;
  holdingRegs[SENSOR3] = s_s3;
  holdingRegs[SENSOR4] = s_s4;
  holdingRegs[SENSOR5] = s_s5;
  holdingRegs[BL] = s_bl;
  holdingRegs[QTD] = s_s5;
   
  datatime();
  tempo();
  temperatura();

  if (s_s5 == 1){
    count++;
        }

  Serial.println(count);

  s_bl = digitalRead(bl);
  if (s_bl == HIGH){
      velo1=velo;
      velo2=255;
      analogWrite(m1, velo1);
      analogWrite(m2, velo1);
      analogWrite(m3, velo1);
      analogWrite(m4, velo1);
     // analogWrite(m5, velo2);
                    } 
    else {
      velo1=velo;
      velo2=0;
      analogWrite(m1, 0);
      analogWrite(m2, 0);
      analogWrite(m3, 0);
      analogWrite(m4, 0);
      //analogWrite(m5, 0);
        } 
  
  s_s1 = digitalRead(s1);
  Serial.println(s_s1);
  s_s2 = digitalRead(s2);
  Serial.println(s_s2);
  s_s3 = digitalRead(s3);
  Serial.println(s_s3);
  s_s4 = digitalRead(s4);
  Serial.println(s_s4);
  s_s5 = digitalRead(s5);
  Serial.println(s_s5);
          
  dispMenu();
   
    char key = keypad.getKey();
}

void keypadEvent(KeypadEvent key){
    switch (keypad.getState()){
    case PRESSED:
     if (key == 'B') {
        lcd.clear();                                     
        menu++;                                         
          if(menu > 0x04) menu = 0x01;                   
          break;
                      } 
       
      if (key == 'D') {
        lcd.clear();                                     
        menu--;                                          
          if(menu < 0x01) menu = 0x04;                  
          break;
                      } 
       } 
} 
void dispMenu() {                                       

    switch(menu) {                                     
    
       case 0x01:                                      
             menu1();                                  
             break; 
                                  
       case 0x02:                                       
             menu2();                                  
             break;
                                                  
       case 0x03:                                    
             menu3();                                 
             break;
                                                  
       case 0x04:                                    
             menu4();                                  
             break;                                     
    } 
} 

void datatime() {
  Serial.print("Hora : ");
  Serial.print(rtc.getTimeStr());
  Serial.print(" ");
  Serial.print("Data : ");
  Serial.print(rtc.getDateStr(FORMAT_SHORT));
  Serial.print(" ");
  //Serial.println(rtc.getDOWStr(FORMAT_SHORT));
} 

void temperatura() {
  for(int i=0;i<1000;i++) {
    temp = temp + analogRead(s0);
                        } 
  temp = temp/1000; 
  temp = (temp*(1.1/1024)); 
  temp = temp*1000; 
  Serial.println(temp);
} 

void tempo() {
static unsigned long ult_tempo = 0;
int tempo = millis();
if((tempo - ult_tempo) >= 1000) {
  ult_tempo = tempo;
  seg++; seg2++;
                                }  
if(seg >= 60){
  seg =0;
  seg2 =0;
  min++;
              } 
Serial.print("Tempo: ");
Serial.print(min);
Serial.print(" : ");
Serial.print(seg2);
Serial.print(" : ");
Serial.println(millis());
}  

void menu1() {
   lcd.setCursor(0,0);                                 
   lcd.print(">Pizza Machine vs2.0");                          
    if (s_bl == LOW){
      lcd.setCursor(0,2);
      lcd.print("MAQUINA DESLIGADA");
      lcd.setCursor(0,3);
      lcd.print("PRESS. BOTAO LIGA");
                    } 
    else {
      lcd.setCursor(0,2);
      lcd.print("MAQUINA LIGADA      ");
      lcd.setCursor(0,3);
      lcd.print("N. DE PIZZAS:");
      lcd.print(count);
      lcd.print("       ");
         } 
   lcd.setCursor(1,1); 
   lcd.print(rtc.getDateStr(FORMAT_SHORT));
   lcd.setCursor(11,1); 
   lcd.print(rtc.getTimeStr());
} 

void menu2() {
   lcd.setCursor(0,0);                                
   lcd.print(">>>Time OP:");
   lcd.print(min);
   lcd.print(":");
   lcd.print(seg);
   lcd.setCursor(0,1);
   lcd.print("Temperatura:");
   lcd.print(temp);
   lcd.print("C");
   lcd.setCursor(0,2);
   lcd.print("S1:");
   lcd.print(s_s1);
   lcd.print("S2:");
   lcd.print(s_s2);
   lcd.print("S3:");
   lcd.print(s_s3);
   lcd.print("S4:");
   lcd.print(s_s4);
   lcd.print("S5:");
   lcd.print(s_s5);
   if (s_bl == LOW){
   lcd.setCursor(0,3);
   lcd.print("PWM M1-M4:");
   lcd.print(velo);
   lcd.setCursor(14,3);
   lcd.print("M5:");
   lcd.print(velo2);
   lcd.print("   ");
                    } 
   else {
   lcd.setCursor(0,3);
   lcd.print("PWM M1-M4:");
   lcd.print(velo);
   lcd.setCursor(14,3);
   lcd.print("M5:");
   lcd.print(velo2);
              } 
} 
 
void menu3() {
    
   lcd.setCursor(0,0);                                 
   lcd.print(">>PWM DOS MOTORES<<");
   lcd.setCursor(0,1);                                 
   lcd.print("SELC. VELO. MOTORES");                      
    char key = keypad.getKey();
    switch (keypad.getState()){
   case PRESSED:
     if (key == '5') {
        set1++;
         if(set1 > 2) set1 = 0x01;       
      
          switch(set1) {
            case 0x01:                  
                lcd.setCursor(0,2);     
                lcd.print("25%  on");    
                velo = 64;              
                break;                  
            case 0x02:                  
                lcd.setCursor(0,2);     
                lcd.print("25% off");   
                velo = 0;
                break;                   
                        } 
     } 
     
     if (key == '4') {
        set2++;
          if(set2 > 2) set2 = 0x01;       
      
          switch(set2) {
            case 0x01:                  
                lcd.setCursor(8,2);     
                lcd.print("50%  on");    
                velo = 128;              
                break;                  
            case 0x02:                  
                lcd.setCursor(8,2);     
                lcd.print("50% off");   
                velo = 0;
                break;                   
                        } 
                       } 
      
      if (key == '7') {
        set3++;
          if(set3 > 2) set3 = 0x01;       
      
          switch(set3) {
            case 0x01:                  
                lcd.setCursor(0,3);     
                lcd.print("75%  on");    
                velo = 192;              
                break;                  
            case 0x02:                  
                lcd.setCursor(0,3);     
                lcd.print("75% off");   
                velo = 0;
                break;                   
                        }
                       } 
    
    if (key == '*') {
        set4++;
          if(set4 > 2) set4 = 0x01;       
      
          switch(set4) {
            case 0x01:                  
                lcd.setCursor(8,3);     
                lcd.print("100%  on");    
                velo = 255;              
                break;                  
            case 0x02:                  
                lcd.setCursor(8,3);     
                lcd.print("100% off");   
                velo = 0;
                break;                   
                        } 
                       } 
  }
}

void menu4() {
   lcd.setCursor(0,0);
   lcd.print(">>DADOS DA MAQUINA<<");
   lcd.setCursor(0,1);
   lcd.print(">>IP:192.168.1.1");
   lcd.setCursor(0,2);
   lcd.print(">>                <<"); 
   lcd.setCursor(0,3);     
   lcd.print(">>Time OP:");
   lcd.print(min);
   lcd.print(":");
   lcd.print(seg);
} 


\end{lstlisting}
\end{apendicesenv}