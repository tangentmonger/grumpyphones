#include <FastLED.h>

#define NUM_LEDS 144
#define LEFT_START 88
#define BOTTOM_START 106
#define RIGHT_START 117
#define TOP_START 135

const int LEFT_N = BOTTOM_START - LEFT_START; // 18
const int BOTTOM_N = RIGHT_START - BOTTOM_START;
const int RIGHT_N = TOP_START - RIGHT_START; // 18
const int TOP_N = NUM_LEDS - TOP_START;

const int SIDE_N = LEFT_N;
const int ROWS_N = SIDE_N + 2;

CRGBArray<NUM_LEDS> leds;

// namespace this to let Arduino see it
namespace phone {
    struct Row {
        int leds;
        int * positions;
    };
}

phone::Row rows[ROWS_N];

const int hue_variance = 40;
const int brightness = 180;

uint8_t * hue[ROWS_N * 2];

void setup() {

    FastLED.addLeds<NEOPIXEL,6>(leds, NUM_LEDS);

    // set up locations of the LEDs in the 'rows' of the display.
    int * top_row = new int[TOP_N];
    for(int i=0; i<TOP_N; i++) {
        top_row[i] = TOP_START + i;
    }
    rows[0] = phone::Row{TOP_N, top_row};

    // intermediate rows, two LEDs each
    for(int i=0; i<SIDE_N; i++) {
        rows[1+i] = phone::Row{2, new int[2]{LEFT_START+i, RIGHT_START+RIGHT_N-i-1}};
    }

    int * bottom_row = new int[BOTTOM_N];
    for(int i=0; i<BOTTOM_N; i++) {
        bottom_row[i] = BOTTOM_START + i;
    }
    rows[SIDE_N+1] = phone::Row{BOTTOM_N, bottom_row};


    
}


// next:
// scroll by less than a screenful (~12, 3 each side?)
// s-curve the accellection, maybe overshoot and snap back
// blend the variation, use a variation delta instead?

// may need to do something differnet to the top/bottom bars, too much colour change at once


void loop(){

    // choose a new base hue
    uint8_t next_hue = random(256 - hue_variance);

    // put that hue (+ variance) into lower half of hues
    for (int i=0; i<ROWS_N; i++) {
        hue[ROWS_N + i] = next_hue + random(hue_variance);
    }

    // scroll: put hues onto rows by moving pointer down and applying hues with short delays
    for (uint8_t top=0; top<=ROWS_N; top++) {
        for(int i=0; i<ROWS_N; i++) {
            for(int led=0; led<rows[i].leds; led++) {
                leds[rows[i].positions[led]] = CHSV(hue[top+i], 255, brightness);
            }

        }

        FastLED.delay(50);

   

    }


    // swap lower half of hues into upper half and reset pointer
    for(int i=0; i<ROWS_N; i++) {
        hue[i] = hue[ROWS_N + i];
    }


    // long delay


     FastLED.delay(1000);


/*

  static uint8_t hue = random(256);

  //fill_solid(&leds[LEFT_START], LEFT_N, CHSV(hue,255,255));
  //fill_solid(&leds[BOTTOM_START], BOTTOM_N, CHSV(hue, 255,255));
  //fill_solid(&leds[RIGHT_START], RIGHT_N, CHSV(hue, 255, 255));
  //fill_solid(&leds[TOP_START], TOP_N, CHSV(hue, 255, 255));

  //for(int i = TOP_START; i < NUM_LEDS; i++) {   

    // let's set an led value
    //leds[i] = CHSV(hue,255,255);

  //}

  uint8_t next_hue = random(256);

  for(int i=0; i<= SIDE_N; i++)
  {
      fill_solid(&leds[LEFT_START], LEFT_N - i, CHSV(hue,255,255));
      fill_solid(&leds[LEFT_START+LEFT_N-i], i, CHSV(next_hue,255,255));
    
      if(i==1){
        fill_solid(&leds[BOTTOM_START], BOTTOM_N, CHSV(next_hue, 255,255));
      }
      fill_solid(&leds[RIGHT_START], i, CHSV(next_hue, 255, 255));
      fill_solid(&leds[RIGHT_START+i], RIGHT_N-i, CHSV(hue, 255, 255));
      
      if(i==SIDE_N){
        fill_solid(&leds[TOP_START], TOP_N, CHSV(next_hue, 255, 255));
        }

      FastLED.delay(30);

  }
    hue = next_hue;
    FastLED.delay(1000);
  */
}

