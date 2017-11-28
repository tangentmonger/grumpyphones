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

const int TOP_PAGE_SHOW = 3;
const int PAGE_N = 12;

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
const int required_hue_difference = 50;

uint8_t previous_hue = 0;

uint8_t * hue[TOP_PAGE_SHOW + PAGE_N * 3]; // enough space to build up a full screenful of pages before it's needed
uint8_t * sat[TOP_PAGE_SHOW + PAGE_N * 3]; // enough space to build up a full screenful of pages before it's needed

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

// scroll speed linger time: linger on 'posts' they like (similar to heart colour)

void loop(){

    // choose a new base hue
    uint8_t next_hue;
    bool ok = false;
    while(!ok) {
        next_hue = random(256 - hue_variance);
        if(next_hue > previous_hue && next_hue - previous_hue > required_hue_difference) ok = true;
        if(next_hue < previous_hue && previous_hue - next_hue > required_hue_difference) ok = true;
    }


    // put that hue (+ variance) into third full page
    for (int i=0; i<PAGE_N; i++) {
        hue[PAGE_N + PAGE_N + TOP_PAGE_SHOW + i] = next_hue + random(hue_variance);
    }

    // add flapbook borders by setting hue and saturation
    sat[PAGE_N+PAGE_N+TOP_PAGE_SHOW] = 0;
    //hue[PAGE_N+PAGE_N+TOP_PAGE_SHOW] = 160; // blue
    sat[PAGE_N+PAGE_N+TOP_PAGE_SHOW + 1] = 0;
    sat[PAGE_N+PAGE_N+TOP_PAGE_SHOW + 2] = 0;
    //hue[PAGE_N+PAGE_N+TOP_PAGE_SHOW + 2] = 160; //blue
    //sat[PAGE_N+PAGE_N+TOP_PAGE_SHOW + 3] = 200;
    //hue[PAGE_N+PAGE_N+TOP_PAGE_SHOW + 3] = 160; //blue
    

    //sat[PAGE_N+PAGE_N+TOP_PAGE_SHOW + 4] = 0; //white
    //sat[PAGE_N+PAGE_N+TOP_PAGE_SHOW + 5] = 0; //white
    for (int i=3; i< PAGE_N; i++) {
        sat[PAGE_N + PAGE_N + TOP_PAGE_SHOW + i] = 255;
    }

    // scroll: put pageful of hues onto rows by moving pointer down and applying hues with short delays
    for (uint8_t top=0; top<=PAGE_N; top++) { // for each row of the page...
        for(uint8_t i=0; i<ROWS_N; i++) { // for each row of the display ...
            for(int led=0; led<rows[i].leds; led++) { // for each LED in the row ...
                if(i<3 || i>17) {
                    leds[rows[i].positions[led]] = CHSV(160, 255, brightness / 4 * 3); // set the LED's colour (flapbook border)
                    // leds[rows[i].positions[led]] = CHSV(145, 255, brightness); // set the LED's colour (twitflange border)
                } else {
                    leds[rows[i].positions[led]] = CHSV(hue[top+i], sat[top+i], brightness); // set the LED's colour
                }
            }

        }

        FastLED.delay(50);

   

    }


    // shuffle second page of hues and saturations into first page
    for(int i=0; i<PAGE_N + PAGE_N + TOP_PAGE_SHOW; i++) {
        hue[i] = hue[PAGE_N + i];
        sat[i] = sat[PAGE_N + i];
    }


    // long delay

    previous_hue = next_hue;
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

