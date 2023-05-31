
#define AUDIO_PIN 34      // select the input pin for the Mic
#define PIN_LEDS 13    // select the output pin for the Led
#define LED_RES_X 32 // Number of pixels wide of each INDIVIDUAL panel module. 
#define LED_RES_Y 8 // Number of pixels tall of each INDIVIDUAL panel module.
#define NUM_COLS 2 // Number of INDIVIDUAL PANELS per ROW
#define NUM_ROWS 5 // Number of rows of chained INDIVIDUAL PANELS

const uint8_t MATRIX_HEIGHT = LED_RES_Y * NUM_ROWS; // 40
const uint8_t MATRIX_WIDTH  = LED_RES_X * NUM_COLS; // 64
const uint16_t NUM_LEDS     = MATRIX_WIDTH * MATRIX_HEIGHT;
const uint16_t xybuff       = LED_RES_X * LED_RES_Y;

uint32_t lastTime, lastdoT, lastblack;
#define FFT_N 1024
#define bandCount   16  
float fft_input[FFT_N];
float fft_output[FFT_N];
int levels[bandCount] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
int Exval[bandCount];
int Exdot[bandCount];
int vR, yh, vpx, yx;
uint8_t ac = 0;

float       angle1  = 0.02, angle2  = -0.03, angle3  = 0.07, angle4  = -0.13;
long        hueShift= 0;

static const int8_t PROGMEM sinetab[256] = {
     0,   2,   5,   8,  11,  15,  18,  21,
    24,  27,  30,  33,  36,  39,  42,  45,
    48,  51,  54,  56,  59,  62,  65,  67,
    70,  72,  75,  77,  80,  82,  85,  87,
    89,  91,  93,  96,  98, 100, 101, 103,
   105, 107, 108, 110, 111, 113, 114, 116,
   117, 118, 119, 120, 121, 122, 123, 123,
   124, 125, 125, 126, 126, 126, 126, 126,
   127, 126, 126, 126, 126, 126, 125, 125,
   124, 123, 123, 122, 121, 120, 119, 118,
   117, 116, 114, 113, 111, 110, 108, 107,
   105, 103, 101, 100,  98,  96,  93,  91,
    89,  87,  85,  82,  80,  77,  75,  72,
    70,  67,  65,  62,  59,  56,  54,  51,
    48,  45,  42,  39,  36,  33,  30,  27,
    24,  21,  18,  15,  11,   8,   5,   2,
     0,  -3,  -6,  -9, -12, -16, -19, -22,
   -25, -28, -31, -34, -37, -40, -43, -46,
   -49, -52, -55, -57, -60, -63, -66, -68,
   -71, -73, -76, -78, -81, -83, -86, -88,
   -90, -92, -94, -97, -99,-101,-102,-104,
  -106,-108,-109,-111,-112,-114,-115,-117,
  -118,-119,-120,-121,-122,-123,-124,-124,
  -125,-126,-126,-127,-127,-127,-127,-127,
  -128,-127,-127,-127,-127,-127,-126,-126,
  -125,-124,-124,-123,-122,-121,-120,-119,
  -118,-117,-115,-114,-112,-111,-109,-108,
  -106,-104,-102,-101, -99, -97, -94, -92,
   -90, -88, -86, -83, -81, -78, -76, -73,
   -71, -68, -66, -63, -60, -57, -55, -52,
   -49, -46, -43, -40, -37, -34, -31, -28,
   -25, -22, -19, -16, -12,  -9,  -6,  -3
};
