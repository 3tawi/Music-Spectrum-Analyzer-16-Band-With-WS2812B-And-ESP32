/*
 Lien vidéo: https://youtu.be/SpQb614-Vug
 REQUIRES the following Arduino libraries:
 - FastLED Library: https://github.com/pixelmatix/FastLED
 - FFT Library: http://www.robinscheibler.org/2017/12/12/esp32-fft.html
 - Getting Started ESP32 : https://www.youtube.com/watch?v=9b0Txt-yF7E
 - Find All "Great Projects" Videos : https://www.youtube.com/c/GreatProjects
*/

#include <WiFi.h>
#include <FastLED.h>
#include "FFT.h"
#include "Plasma.h"

CRGB leds[NUM_LEDS];
CRGB usPalette[256];
CRGB black = CRGB::Black;
CRGB white = CRGB::White;
fft_config_t *real_fft_plan = fft_init(FFT_N, FFT_REAL, FFT_FORWARD, fft_input, fft_output);
    
#include "Effects.h"
Effects effects;


void getdata(){
  for (int k = 0 ; k < FFT_N ; k++)
    real_fft_plan->input[k] = analogRead(AUDIO_PIN);
  fft_execute(real_fft_plan);
  
  hueShift = 0;
  for (int i = 2 ; i < real_fft_plan->size / 2 ; i++)
  {
    int mag = sqrt(pow(real_fft_plan->output[i],2))/1000;
    mag = map(mag, 0, 72, 39, 0);
    if(mag<0) mag = 0;
    //16 bands, 12kHz top band
      if (i<=3 )                SetBand(0, mag);
      else if (i>3   && i<=5  ) SetBand(1, mag);
      else if (i>5   && i<=7  ) SetBand(2, mag);
      else if (i>7   && i<=9  ) SetBand(3, mag);
      else if (i>9   && i<=13 ) SetBand(4, mag);
      else if (i>13  && i<=18 ) SetBand(5, mag);
      else if (i>18  && i<=25 ) SetBand(6, mag);
      else if (i>25  && i<=34 ) SetBand(7, mag);
      else if (i>34  && i<=45 ) SetBand(8, mag);
      else if (i>45  && i<=58 ) SetBand(9, mag);
      else if (i>58  && i<=73 ) SetBand(10, mag);
      else if (i>73  && i<=97 ) SetBand(11, mag);
      else if (i>97  && i<=127) SetBand(12, mag);
      else if (i>127 && i<=169) SetBand(13, mag);
      else if (i>169 && i<=226) SetBand(14, mag);
      else if (i>226          ) SetBand(15, mag);
  }
  displayBand();
}
void SetBand(int ban, int vmic){
  Exval[ban] = (Exval[ban] > vmic) ? vmic : Exval[ban];
  Exdot[ban] = (Exdot[ban] > Exval[ban]) ? Exval[ban] : Exdot[ban];
  levels[ban] = map(Exval[ban], 0, 39, 40, 10);
  hueShift += Exval[ban];
}
void displayBand(){
  effects.FillPlasma();
  for(int x=0; x<bandCount; x++) {
    int xl = 2*x, xr = 62 - xl;
    effects.fillRect(xl, Exval[x], 2, 39-Exval[x]);
    effects.fillRect(xr, Exval[x], 2, 39-Exval[x]);
  }
}
void getdoT() {
  for(int x=0; x<bandCount; x++) {
      int xl = 2*x, xr = 62 - xl;
      yh = Exdot[x]; 
      if (yh < 40) { 
      effects.fillRect(xl, yh-1, 2, 2, black);
      effects.fillRect(xr, yh-1, 2, 2, black);
      effects.fillRect(xl, yh, 2, 1, (usPalette[ac++]));
      effects.fillRect(xr, yh, 2, 1, (usPalette[ac++]));
      Exdot[x]++; 
    }
  } 
}
void getblack() {
  for(int x=0; x<bandCount; x++) {
    int xl = 2*x, xr = 62 - xl;
    yh = Exval[x]; 
    if (yh < 39) { 
      effects.fillRect(xl, yh, 2, 2, black);
      effects.fillRect(xr, yh, 2, 2, black);
      Exval[x]+=2; 
    }
  }    
}


void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812B,PIN_LEDS,GRB>(leds,NUM_LEDS);
  FastLED.setBrightness(40);
  effects.SetupPalette();
}

void loop() {
  if(millis() - lastTime >= 7) {
    getdata();
    lastTime = millis();
  }
  if(millis() - lastblack >= 2) {
    getblack();
    lastblack = millis();
  }
  if(millis() - lastdoT >= 100) {
    getdoT();
    lastdoT = millis();
  }
  FastLED.show();
}
