#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Wire.h>

const uint8_t pinRS = 2;
const uint8_t pinEN = 3;
const uint8_t pinD4 = 4;
const uint8_t pinD5 = 5;
const uint8_t pinD6 = 6;
const uint8_t pinD7 = 7;

const uint8_t pinButtonPlus = 8;
const uint8_t pinButtonMinus = 9;
const uint8_t pinButtonSelect = 10;
const uint8_t pinButtonStart = 11;

const uint8_t pinLed = 12;

uint8_t minutesHigh = 0;
uint8_t minutesLow = 0;
uint8_t secondsHigh = 0;
uint8_t secondsLow = 0;

bool alarm = false;
bool isRunning = false;
uint8_t cursorPosition = 0;

unsigned long debounceDelay = 50UL;

struct Button{
  const uint8_t pin;
  uint8_t state;
  uint8_t lastState;
  unsigned long lastDebounceTime;
};

struct Button buttons[] = {
  {pinButtonPlus, LOW, LOW, 0},
  {pinButtonMinus, LOW, LOW, 0},
  {pinButtonSelect, LOW, LOW, 0},
  {pinButtonStart, LOW, LOW, 0},
};

boolean Debounce(struct Button* b);
void show();

LiquidCrystal lcd(pinRS, pinEN, pinD4, pinD5, pinD6, pinD7);

void setup()
{
  cli();
  TCCR1A = 0,
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12);               // CTC mode on
  TCNT1 = 0;                            //initial value
  OCR1A = 15624;                        //top value [16MHz/(prescaler*ISRfrequency)]-1
  TIMSK1 |= (1 << OCIE1A);              //compare match interrupt enabled
  sei();

  lcd.begin(16, 2);
  pinMode(pinButtonPlus, INPUT);
  pinMode(pinButtonMinus, INPUT);
  pinMode(pinButtonSelect, INPUT);
  pinMode(pinButtonStart, INPUT);
  pinMode(pinLed, OUTPUT);

  show();
}

ISR(TIMER1_COMPA_vect){
  if (minutesHigh == 0 && minutesLow == 0 && secondsHigh == 0 && secondsLow == 0){
    TCCR1B &= ~(1 << CS12) & ~(1 << CS10);
    isRunning = false;
    return;
  }
  else{
    secondsLow--;
    if (secondsLow == 255){
      secondsLow = 9;
      secondsHigh--;
      if (secondsHigh == 255){
        secondsHigh = 5;
        minutesLow--;
        if (minutesLow == 255){
          minutesLow = 9;
          minutesHigh--;
        }
      }
    }
  }
  if (minutesHigh == 0 && minutesLow == 0 && secondsHigh == 0 && secondsLow == 0){
    TCCR1B &= ~(1 << CS12) & ~(1 << CS10);
    isRunning = false;
    alarm = true;
    digitalWrite(pinLed, HIGH);
  }
  show();
}

void loop()
{
  if ((Debounce(&buttons[0]) && !isRunning) == true){
    switch (cursorPosition){
    case 0:
      break;
    case 7:
      minutesLow++;
      if (minutesLow > 9){
        minutesLow = 0;
        minutesHigh++;
        if (minutesHigh > 5) minutesHigh = 0;
      }
      show();
      break;
    case 10:
      secondsLow++;
      if (secondsLow > 9){
        secondsLow = 0;
        secondsHigh++;
        if (secondsHigh > 5) secondsHigh = 0;
      }
      show();
      break;
    }
  }
    if ((Debounce(&buttons[1]) && !isRunning) == true){
    switch (cursorPosition){
    case 0:
      break;
    case 7:
      minutesLow--;
      if (minutesLow == 255){
        minutesLow = 9;
        minutesHigh--;
        if (minutesHigh == 255) minutesHigh = 5;
      }
      show();
      break;
    case 10:
      secondsLow--;
      if (secondsLow == 255){
        secondsLow = 9;
        secondsHigh--;
        if (secondsHigh == 255) secondsHigh = 5;
      }
      show();
      break;
    }
  }
  if ((Debounce(&buttons[2]) && !isRunning) == true){
    if (cursorPosition == 0){
      cursorPosition = 7;
      lcd.cursor();
      lcd.blink();
    }
    else if (cursorPosition == 7){
      cursorPosition = 10;
    }
    else if (cursorPosition == 10){
      cursorPosition = 0;
      lcd.noBlink();
      lcd.noCursor();
    }
    show();
  }
  if (Debounce(&buttons[3]) == true){
    if (alarm == true){
      digitalWrite(pinLed, LOW);
      alarm = false;
    }else{
      TCCR1B |= (1 << CS12) | (1 << CS10);  //prescaler 1024 and turning on timer
      isRunning = true;
      cursorPosition = 0;
      lcd.noBlink();
      lcd.noCursor();
    }
  }
}

boolean Debounce(struct Button* b)
{
  uint8_t reading = digitalRead(b->pin);

  if (reading != b->lastState) b->lastDebounceTime = millis();
  if ((millis() - b->lastDebounceTime) > debounceDelay){
    if (reading != b->state){
      b->state = reading;
      if (b->state == HIGH){
        b->lastState = reading;
        return true;
      }
    }
  }
  b->lastState = reading;
  return false;
}

void show(){
  lcd.setCursor(6, 0);
  lcd.print(minutesHigh);
  lcd.print(minutesLow);
  lcd.write(':');
  lcd.print(secondsHigh);
  lcd.print(secondsLow);
  lcd.setCursor(cursorPosition, 0);
}