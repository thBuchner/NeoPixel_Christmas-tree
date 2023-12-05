#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN        6
#define NUMPIXELS 17

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 200

void setup() {
  pinMode(13, OUTPUT);

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();
}

const uint8_t addressFromIdx[] {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 16, 17};

void loop() {
  pixels.clear();
  
  for(int i=0; i<4; i++) {
    delay(DELAYVAL);
    if(i%2==0){
        pixels.setPixelColor(i, pixels.Color(150, 0, 0));}
    else{
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    }
    pixels.show();
    delay(DELAYVAL);
    digitalWrite(13, HIGH);   
    delay(DELAYVAL);              
    digitalWrite(13, LOW);   
    delay(DELAYVAL);
  }

  for(int i=4; i<8; i++) {
    delay(DELAYVAL);
    if(i%2==0){
        pixels.setPixelColor(i, pixels.Color(0, 150, 0));}
    else{
    pixels.setPixelColor(i, pixels.Color(150, 0, 0));
    }
    pixels.show();
    delay(DELAYVAL);
    digitalWrite(13, HIGH);   
    delay(DELAYVAL);              
    digitalWrite(13, LOW);   
    delay(DELAYVAL);
  }

  for(int i=8; i<12; i++) {
    delay(DELAYVAL);
    if(i%2==0){
        pixels.setPixelColor(i, pixels.Color(150, 0, 0));}
    else{
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    }
    pixels.show();
    delay(DELAYVAL);
    digitalWrite(13, HIGH);   
    delay(DELAYVAL);              
    digitalWrite(13, LOW);   
    delay(DELAYVAL);
  }

  for(int i=13; i<NUMPIXELS; i++) {
    delay(DELAYVAL);
    if(i%2==0){
        pixels.setPixelColor(i, pixels.Color(150, 0, 0));}
    else{
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    }
    pixels.show();
    delay(DELAYVAL);
    digitalWrite(13, HIGH);   
    delay(DELAYVAL);              
    digitalWrite(13, LOW);   
    delay(DELAYVAL);  
  }       
}