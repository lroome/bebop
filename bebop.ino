#include <BfButton.h>

#include <Arduino.h>
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceSD.h"
#include "driver/i2s.h"
#include <SD.h>
#include "FastLED.h"
#include "soc/rtc_wdt.h"

#include <MacroDebugger.h>

#define DEMO

// I2s
//#define I2S_DOUT 25
//#define I2S_LRC 26
//#define I2S_BCLK 27

#define I2S_DOUT 27
#define I2S_LRC 25
#define I2S_BCLK 26

#define SS 5
#define MOSI 23
#define MISO 19
#define SCK 18

#define INITIAL_GAIN 1.0
#define MAX_GAIN 4.0
#define MIN_GAIN 0.0
#define GAIN_STEP 0.2
volatile float gain = INITIAL_GAIN;

#define FIRST_TRACK 1
#define LAST_TRACK 3
volatile int song = 1;
volatile int track = FIRST_TRACK;

#define FIRST_PATTERN 1
#define LAST_PATTERN 1
volatile int pattern = FIRST_PATTERN;



//Initialize ESP8266 Audio Library classes
AudioGeneratorMP3 *mp3;
AudioFileSourceSD *file;
AudioOutputI2S *out;

volatile bool playing = 0;
volatile uint32_t lighting = 0;
volatile byte loadTrack = 0;
volatile byte requestStop = 0;
volatile uint32_t pauseLocation = 0;
volatile int beats = 0;
volatile int lastBeat = -1;
volatile int color_loops = 0;
volatile int row_loops = 0;
volatile int col_loops = 0;
volatile int curr_dot = 0;
volatile int audio_mode = 0;

volatile uint8_t currentSong = 0;
volatile uint8_t futureSong = 0;

hw_timer_t *My_timer = NULL;

void IRAM_ATTR onTimer() {
  beats += 1;
}

// tasks
TaskHandle_t ButtonLoopHandle;
TaskHandle_t LEDControllerHandle;

void resetBeats() {
  beats = 0;
  lastBeat = -1;
  pauseLocation = 0;
  color_loops = 0;
  row_loops = 0;
}

void buttonTask() {
  xTaskCreatePinnedToCore(
    buttonLoop,        /* Task function. */
    "ButtonLoop",      /* name of task. */
    10000,             /* Stack size of task */
    NULL,              /* parameter of the task */
    1,                 /* priority of the task */
    &ButtonLoopHandle, /* Task handle to keep track of created task */
    0);                /* pin task to core 1 */
}

void ledTask() {
  xTaskCreatePinnedToCore(
    LEDController,        /* Task function. */
    "LEDController",      /* name of task. */
    10000,                /* Stack size of task */
    NULL,                 /* parameter of the task */
    1,                    /* priority of the task */
    &LEDControllerHandle, /* Task handle to keep track of created task */
    0);                   /* pin task to core 1 */
}


void initializeTasks() {
  // tasks
  buttonTask();
  ledTask();

  My_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 250000, true);
  timerSetAutoReload(My_timer, true);
  timerAlarmEnable(My_timer); //start timer

}

void sdSetup() {
  Serial.print("Initializing SD card...");
  //SPI.begin(SCK, MISO, MOSI);
  if (!SD.begin(SS)) {
    Serial.println("initialization failed!");
  }
  Serial.println("initialization done.");
}

void audioSetup() {
  out = new AudioOutputI2S();
  mp3 = new AudioGeneratorMP3();

  out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  out->SetGain(INITIAL_GAIN);
  //out->SetBitsPerSample(128);
  //out->SetRate(10);
  file = new AudioFileSourceSD("/group0/1.mp3");
  mp3->begin(file, out);  //Start playing the track loaded
  mp3->stop();
}

void audioUp() {
  Serial.println("Increasing volume");
  gain += GAIN_STEP;
  if (gain > MAX_GAIN) gain = MAX_GAIN;
  out->SetGain(gain);
}

void audioDown() {
  Serial.println("Decreasing volume");
  gain -= GAIN_STEP;
  if (gain < MIN_GAIN) gain = MIN_GAIN;
  out->SetGain(gain);
}

void nextSong() {
  Serial.println("nextSong<<");
  Serial.println(track);
  track++;
  if (track > LAST_TRACK) track = FIRST_TRACK;
  if (mp3->isRunning()) pauseLocation = file->getPos();
  loadTrack = track;
  lighting = track;
  playing = 1;
  Serial.println(track);
  Serial.println("nextSong>>");
}

void previousSong() {
  track--;
  if (track < FIRST_TRACK) track = LAST_TRACK;
  if (mp3->isRunning()) pauseLocation = file->getPos();
  loadTrack = track;
  lighting = track;
  playing = 1;
}

void nextLEDPattern() {
  pattern = FIRST_PATTERN;
}

void previousLEDPattern() {
  pattern = FIRST_PATTERN;
}

void actionsOn() {
  playing = 1;
  lighting = track;
  timerAlarmEnable(My_timer);
}

void actionsOff() {
  playing = 0;
  lighting = 0;
  curr_dot = 0;
  col_loops = 0;
  row_loops = 0;

  timerAlarmDisable(My_timer);
}


// stop playing the track
void stop() {
  actionsOff();
  requestStop = 0;
  resetBeats();
  mp3->stop();
  futureSong = 0;
  currentSong = 0;
  Serial.println("Stopped");
  Serial.println(beats);
}

// play or pause the track
void playPause() {
  if (playing) {
    Serial.println("Pause");
    pauseLocation = file->getPos();
    Serial.println(pauseLocation);
    out->stop();
    actionsOff();

  } else if (pauseLocation > 0) {
    Serial.println("Resume");
    out->begin();
    actionsOn();

    pauseLocation = 0;
  } else {
    Serial.println("Play");
    loadTrack = track;
    resetBeats();
    actionsOn();
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  rtc_wdt_protect_off();
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  buttonSetup();
  lightsSetup();
  sdSetup();
  audioSetup();

  // this has to be the last call
  initializeTasks();

  // remove these lines before shipping.
  #ifdef DEMO
  track = 1;
  setTrack(0);
  #endif 
}


void setTrack(int pos) {
  

  char buffer[20];
  sprintf(buffer, "/group%d/%d.mp3", audio_mode, track);
  Serial.println(buffer);

  file = new AudioFileSourceSD(buffer);
  if (pos > 0) {
    Serial.println(file->seek(pos, SEEK_SET));
  }
  
  //Stop the current track if playing
  if (playing && mp3->isRunning()) mp3->stop();
  mp3->begin(file, out);  //Start playing the track loaded
  
  lighting = track;
  loadTrack = 0;
  if (track != 0) {
    playing = 1;
  } else {
    playing = 0;    
  }
}


void changeSongAndLights() {
  if (futureSong != currentSong) {
    track = futureSong;
    lighting = futureSong;
    currentSong = futureSong;
    pauseLocation = file->getPos();
    loadTrack = 1;   
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //
  if (lastBeat != beats) {
   // Serial.print("beat ");
   // Serial.println(beats);
    lastBeat = beats;
  }
  changeSongAndLights();

  if (loadTrack) setTrack(pauseLocation);  //Load the track we want to play

  if (playing && mp3->isRunning()) {
    if (!mp3->loop() || requestStop) {
      stop();
    }
  }
}