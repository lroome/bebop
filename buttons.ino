#define ENABLE_DEBUG

// constants won't change. They're used here to set pin numbers:

#define pinCount 5
const int buttonPins[] = { 39, 35, 32, 33, 34 };
const int ledPin = 13;  // the number of the LED pin

// Variables will change:
int ledState = HIGH;        // the current state of the output pin
int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


BfButton btn1(BfButton::STANDALONE_DIGITAL, buttonPins[0], false, LOW);
BfButton btn2(BfButton::STANDALONE_DIGITAL, buttonPins[1], false, LOW);
BfButton btn3(BfButton::STANDALONE_DIGITAL, buttonPins[2], false, LOW);
BfButton btn4(BfButton::STANDALONE_DIGITAL, buttonPins[3], false, LOW);
BfButton btn5(BfButton::STANDALONE_DIGITAL, buttonPins[4], false, LOW);


void buttonSetup() {

  for (int i = 0; i < pinCount; i++) {
    pinMode(buttonPins[i], INPUT);
  }

  btn1.onPress(button1Handler)
    .onDoublePress(button1Handler)      // default timeout
    .onPressFor(button1Handler, 1000);  // custom timeout for 1 second

  btn2.onPress(button2Handler)
    .onDoublePress(button2Handler)      // default timeout
    .onPressFor(button2Handler, 1000);  // custom timeout for 1 second

  btn3.onPress(button3Handler)
    .onDoublePress(button3Handler)      // default timeout
    .onPressFor(button3Handler, 1000);  // custom timeout for 1 second

  btn4.onPress(button4Handler)
    .onDoublePress(button4Handler)      // default timeout
    .onPressFor(button4Handler, 1000);  // custom timeout for 1 second

  btn5.onPress(button5Handler)
    .onDoublePress(button5Handler)      // default timeout
    .onPressFor(button5Handler, 1000);  // custom timeout for 1 second

  DEBUG_I("buttons configured");
}

void button1Handler(BfButton *btn, BfButton::press_pattern_t pattern) {
  
  Serial.println("Button hander 1");

  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      Serial.println(" pressed.");
      bitWrite(futureSong, 0, !bitRead(futureSong, 0)); 
      break;
    case BfButton::DOUBLE_PRESS:
      Serial.println(" double pressed.");
      nextSong();
      break;
    case BfButton::LONG_PRESS:
      Serial.println(" long pressed.");
      audioUp();
      btn->updateState(BfButton::IDLE);
      break;
  }
}

void button2Handler(BfButton *btn, BfButton::press_pattern_t pattern) {
 
 Serial.println("Button hander 2");

  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      Serial.println(" pressed.");
      bitWrite(futureSong, 1, !bitRead(futureSong, 1));
      break;
    case BfButton::DOUBLE_PRESS:
      Serial.println(" double pressed.");
      nextSong();
      break;
    case BfButton::LONG_PRESS:
      Serial.println(" long pressed.");
      break;
  }
}

void button3Handler(BfButton *btn, BfButton::press_pattern_t pattern) {
 Serial.println("Button hander 3");

  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      Serial.println(" pressed.");
      bitWrite(futureSong, 2, !bitRead(futureSong, 2));
      break;
    case BfButton::DOUBLE_PRESS:
      Serial.println(" double pressed.");
      nextSong();
      break;
    case BfButton::LONG_PRESS:
      Serial.println(" long pressed.");
      break;
  }
}

void button4Handler(BfButton *btn, BfButton::press_pattern_t pattern) {
   Serial.println("Button hander 4");

  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      Serial.println(" pressed.");
       bitWrite(futureSong, 3, !bitRead(futureSong, 3));
      break;
    case BfButton::DOUBLE_PRESS:
      nextSong();
      Serial.println(" double pressed.");
      break;
    case BfButton::LONG_PRESS:
      Serial.println(" long pressed.");
      break;
  }
}

void button5Handler(BfButton *btn, BfButton::press_pattern_t pattern) {
 Serial.println("Button hander 5");

  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      Serial.println(" pressed.");
      soundEffect();
      break;
    case BfButton::DOUBLE_PRESS:
      Serial.println(" double pressed.");
      nextSong();
      break;
    case BfButton::LONG_PRESS:
      Serial.println(" long pressed.");
      audioDown();
      btn->updateState(BfButton::IDLE);
      break;
  }
}


void buttonLoop(void *pvParameters) {
  // read the state of the switch into a local variable:
  for (;;) {
    vTaskDelay(10);
    
    btn1.read();
    btn2.read();
    btn3.read();
    btn4.read();
    btn5.read();
  }
}