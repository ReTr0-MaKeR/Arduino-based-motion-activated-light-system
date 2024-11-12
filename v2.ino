#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int pirOutsidePin = 10;     // Outside PIR motion sensor pin
const int pirRoomSidePin = 3;   // Room-side PIR motion sensor pin
const int clapMod = 5;
const int buttonPin = 6;

const int led = 2;
const int Lights = 13;

bool outstat = true;
bool instat = true; 
bool clapstat = true;
bool buttonstat = true;
bool flag = false;
bool cflag = true;
bool mflag = false;

int count = 0;                   // Counter for people in the room   

// Initialize the LCD with I2C address (0x27 for 16x2display)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    Serial.begin(9600);
    
    // Set up PIR sensor pins
    pinMode(pirOutsidePin, INPUT);  // Set outside PIR sensor as input
    pinMode(pirRoomSidePin, INPUT); // Set room-side PIR sensor as input
    pinMode(clapMod,INPUT);
    pinMode(buttonPin,INPUT_PULLUP);
    pinMode(led, OUTPUT);
    pinMode(Lights,OUTPUT);
    
    // Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Initializing..");
    delay(2000);
    updateDisplay();
}

void loop() {
  // Detect entry attempt
  outstat = digitalRead(pirOutsidePin); // False on motion
  instat = digitalRead(pirRoomSidePin); // True on motion
  clapstat = digitalRead(clapMod);
  buttonstat = digitalRead(buttonPin);
   
   
  Serial.print("INStat  : ");
  Serial.print(instat);
  Serial.print(" ||   OutStat : ");
  Serial.println(outstat);
  Serial.print("ClapStat : ");
  Serial.print(clapstat);
  Serial.print(" || ButtonStat : ");
  Serial.println(buttonstat);
    
    // Entry detection
    if (!outstat && count < 5) { // Motion detected outside
        unsigned long startTime = millis();
        digitalWrite(led, HIGH); // Indicator LED
        
        // Wait up to 500 ms for inside sensor to detect entry
        while ((millis() - startTime) < 500) {
            instat = digitalRead(pirRoomSidePin);
            Serial.print("Inside Stat:");
            Serial.println(instat);
            digitalWrite(led,LOW);
            if (!instat) { // Entry detected
                count++;
                digitalWrite(led,HIGH);
               // Serial.print("Count: ");
               //Serial.println(count);
                updateDisplay();
                delay(1000);  // Small delay to prevent multiple detections
                digitalWrite(led, LOW);
                break;
            }
        }
    }
    
    
    // Exit detection
    if (!instat && count > 0) 
    { // Motion detected inside
        unsigned long startTime = millis();
        digitalWrite(led, HIGH); // Indicator LED
      
        // Wait up to 500 ms for outside sensor to detect exit
        while ((millis() - startTime) < 500) {
            outstat = digitalRead(pirOutsidePin);
            Serial.print("Outside Stat:");
            Serial.println(outstat);
            digitalWrite(led, LOW); // Indicator LED
            if (!outstat) { // Exit detected
                count--;
                digitalWrite(led, HIGH); // Indicator LED
                //Serial.print("Count: ");
                //Serial.println(count);
                updateDisplay();
                delay(1000);  // Small delay to prevent multiple detections
                digitalWrite(led, LOW); // Indicator LED
                break;
            }
        }
    }
    if(count>0 && mflag == false )
    {
      digitalWrite(Lights, HIGH);
      flag = true;
      cflag = false;
    }
    if(count==0 && cflag == false)
    {
      digitalWrite(Lights,LOW);
    }
    if(clapstat== false || buttonstat == false)  // remove the part (clapstat= false || ) to avoid unwanted sound interuption
    {
        if(flag==false)
        {
            digitalWrite(Lights, HIGH);
            lcd.setCursor(0, 0);
            lcd.print(".Menaually ON..");
            lcd.setCursor(0, 1);
            lcd.print("Person: " );
            lcd.print(count);
            flag = true;   
        }
        
    }
    clapstat = digitalRead(clapMod);
    buttonstat = digitalRead(buttonPin);
    if(clapstat== false || buttonstat == false)  // remove the part (clapstat= false || ) to avoid unwanted sound interuption
    {
        if(flag ==true)
        {
          digitalWrite(Lights,LOW);
          lcd.setCursor(0, 0);
          lcd.print(".Menaually Off.");
          lcd.setCursor(0, 1);
          lcd.print("Person: " );
          lcd.print(count);
          flag = false;
          mflag = true;
        }
    }
}

void updateDisplay() 
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Person Count:");
    lcd.print(count);
}
