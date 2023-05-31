
/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Portions of this code are adapted from "Funky Clouds" by Stefan Petrick: https://gist.github.com/anonymous/876f908333cd95315c35
 * Portions of this code are adapted from "NoiseSmearing" by Stefan Petrick: https://gist.github.com/StefanPetrick/9ee2f677dbff64e3ba7a
 * Copyright (c) 2014 Stefan Petrick
 * http://www.stefan-petrick.de/wordpress_beta
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef Effects_H
#define Effects_H

/* ---------------------------- GLOBAL CONSTANTS ----------------------------- */

const int  MATRIX_CENTER_X = MATRIX_WIDTH / 2;
const int  MATRIX_CENTER_Y = MATRIX_HEIGHT / 2;
uint8_t MATRIX_CENTRE_X = MATRIX_CENTER_X;
uint8_t MATRIX_CENTRE_Y = MATRIX_CENTER_Y;
uint8_t rotation = 0;

  uint16_t XY( uint8_t x, uint8_t y) {
    if( x >= MATRIX_WIDTH || x < 0 || y >= MATRIX_HEIGHT || y < 0) return -1;
    uint16_t i;
    uint8_t yi = y / LED_RES_Y;
    y %= LED_RES_Y;   
    if( yi & 0x01) {
      if( x & 0x01) {
        uint8_t reverseY = (LED_RES_Y - 1) - y;
        i = (((LED_RES_X * NUM_COLS) - x - 1) * LED_RES_Y) + reverseY;
      } else {
        i = (((LED_RES_X * NUM_COLS) - x - 1) * LED_RES_Y) + y;
      }
    } else {
      if( x & 0x01) {
        uint8_t reverseY = (LED_RES_Y - 1) - y ;
        i = (x * LED_RES_Y) + reverseY;
      } else {
        i = (x * LED_RES_Y) + y;
      }
    }
    i += xybuff * yi * NUM_COLS;
    return i;
}
    

uint8_t beatcos8(accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timebase = 0, uint8_t phase_offset = 0)
{
  uint8_t beat = beat8(beats_per_minute, timebase);
  uint8_t beatcos = cos8(beat + phase_offset);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatcos, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

uint8_t mapsin8(uint8_t theta, uint8_t lowest = 0, uint8_t highest = 255) {
  uint8_t beatsin = sin8(theta);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatsin, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

uint8_t mapcos8(uint8_t theta, uint8_t lowest = 0, uint8_t highest = 255) {
  uint8_t beatcos = cos8(theta);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatcos, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}


class Effects {
  
private:
uint8_t WIDTH;
uint8_t HEIGHT;
uint8_t CENTRE_X;
uint8_t CENTRE_Y;
public:

uint8_t plasma[MATRIX_WIDTH][MATRIX_HEIGHT];

void setRotation(uint8_t x) {
  rotation = (x & 3);
  switch (rotation) {
  case 0:
    WIDTH  = MATRIX_WIDTH;
    HEIGHT = MATRIX_HEIGHT;
    break;
  case 2:
    WIDTH  = MATRIX_WIDTH;
    HEIGHT = MATRIX_HEIGHT;
    break;
  case 1:
    WIDTH  = MATRIX_HEIGHT;
    HEIGHT = MATRIX_WIDTH;
    CENTRE_X = MATRIX_CENTRE_Y;
    CENTRE_Y = MATRIX_CENTRE_X;
  case 3:
    WIDTH  = MATRIX_HEIGHT;
    HEIGHT = MATRIX_WIDTH;
    break;
  }
}
  CRGB HsvToRgb(uint8_t h, uint8_t s, uint8_t v) {
    CHSV hsv = CHSV(h, s, v);
    CRGB rgb;
    hsv2rgb_spectrum(hsv, rgb);
    return rgb;
  }
  void SetupPalette() {
    for (int k = 0; k < 256; k++)
      usPalette[k] = HsvToRgb(k, 255, 255);
  }  
  void drawPixel(int16_t x, int16_t y) {
    uint8_t k = plasma[x][y];
    leds[XY(x, y)] = usPalette[k];
  }  
  void drawPixel(int16_t x, int16_t y, CRGB color)
  {
    leds[XY(x, y)] = color;
  }
  void drawPixel(int x, int y, uint8_t colorIndex) {
    leds[XY(x, y)] = usPalette[colorIndex];
  }
  
 void PrepareFrame() {
   // leds = (CRGB*) backgroundLayer.backBuffer();
  }

  void ShowFrame() {
    FastLED.show();
  }

  // scale the brightness of the screenbuffer down
  void DimAll(byte value) {
    for (int i = 0; i < NUM_LEDS; i++)
      leds[i].nscale8(value);
  } 

  void ClearFrame() {
    memset(leds, 0,  sizeof(leds)); // flush 
  }
  
   
  

 
  // All the caleidoscope functions work directly within the screenbuffer (leds array).
  // Draw whatever you like in the area x(0-15) and y (0-15) and then copy it arround.

  // rotates the first 16x16 quadrant 3 times onto a 32x32 (+90 degrees rotation for each one)
  void Caleidoscope1() {
    for (int x = 0; x < MATRIX_CENTER_X; x++) {
      for (int y = 0; y < MATRIX_CENTER_Y; y++) {
        leds[XY(MATRIX_WIDTH - 1 - x, y)] = leds[XY(x, y)];
        leds[XY(MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y)] = leds[XY(x, y)];
        leds[XY(x, MATRIX_HEIGHT - 1 - y)] = leds[XY(x, y)];
      }
    }
  }


  // mirror the first 16x16 quadrant 3 times onto a 32x32
  void Caleidoscope2() {
    for (int x = 0; x < MATRIX_CENTER_X; x++) {
      for (int y = 0; y < MATRIX_CENTER_Y; y++) {
        leds[XY(MATRIX_WIDTH - 1 - x, y)] = leds[XY(y, x)];
        leds[XY(x, MATRIX_HEIGHT - 1 - y)] = leds[XY(y, x)];
        leds[XY(MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y)] = leds[XY(x, y)];
      }
    }
  }

  // copy one diagonal triangle into the other one within a 16x16
  void Caleidoscope3() {
    for (int x = 0; x <= MATRIX_CENTRE_X; x++) {
      for (int y = 0; y <= x; y++) {
        leds[XY(x, y)] = leds[XY(y, x)];
      }
    }
  }

  // copy one diagonal triangle into the other one within a 16x16 (90 degrees rotated compared to Caleidoscope3)
  void Caleidoscope4() {
    for (int x = 0; x <= MATRIX_CENTRE_X; x++) {
      for (int y = 0; y <= MATRIX_CENTRE_Y - x; y++) {
        leds[XY(MATRIX_CENTRE_Y - y, MATRIX_CENTRE_X - x)] = leds[XY(x, y)];
      }
    }
  }

  // copy one diagonal triangle into the other one within a 8x8
  void Caleidoscope5() {
    for (int x = 0; x < MATRIX_WIDTH / 4; x++) {
      for (int y = 0; y <= x; y++) {
        leds[XY(x, y)] = leds[XY(y, x)];
      }
    }

    for (int x = MATRIX_WIDTH / 4; x < MATRIX_WIDTH / 2; x++) {
      for (int y = MATRIX_HEIGHT / 4; y >= 0; y--) {
        leds[XY(x, y)] = leds[XY(y, x)];
      }
    }
  }

  void Caleidoscope6() {
    for (int x = 1; x < MATRIX_CENTER_X; x++) {
      leds[XY(7 - x, 7)] = leds[XY(x, 0)];
    } //a
    for (int x = 2; x < MATRIX_CENTER_X; x++) {
      leds[XY(7 - x, 6)] = leds[XY(x, 1)];
    } //b
    for (int x = 3; x < MATRIX_CENTER_X; x++) {
      leds[XY(7 - x, 5)] = leds[XY(x, 2)];
    } //c
    for (int x = 4; x < MATRIX_CENTER_X; x++) {
      leds[XY(7 - x, 4)] = leds[XY(x, 3)];
    } //d
    for (int x = 5; x < MATRIX_CENTER_X; x++) {
      leds[XY(7 - x, 3)] = leds[XY(x, 4)];
    } //e
    for (int x = 6; x < MATRIX_CENTER_X; x++) {
      leds[XY(7 - x, 2)] = leds[XY(x, 5)];
    } //f
    for (int x = 7; x < MATRIX_CENTER_X; x++) {
      leds[XY(7 - x, 1)] = leds[XY(x, 6)];
    } //g
  }

  // create a square twister to the left or counter-clockwise
  // x and y for center, r for radius
  void SpiralStream(int x, int y, int r, byte dimm) {
    for (int d = r; d >= 0; d--) { // from the outside to the inside
      for (int i = x - d; i <= x + d; i++) {
        leds[XY(i, y - d)] += leds[XY(i + 1, y - d)]; // lowest row to the right
        leds[XY(i, y - d)].nscale8(dimm);
      }
      for (int i = y - d; i <= y + d; i++) {
        leds[XY(x + d, i)] += leds[XY(x + d, i + 1)]; // right colum up
        leds[XY(x + d, i)].nscale8(dimm);
      }
      for (int i = x + d; i >= x - d; i--) {
        leds[XY(i, y + d)] += leds[XY(i - 1, y + d)]; // upper row to the left
        leds[XY(i, y + d)].nscale8(dimm);
      }
      for (int i = y + d; i >= y - d; i--) {
        leds[XY(x - d, i)] += leds[XY(x - d, i - 1)]; // left colum down
        leds[XY(x - d, i)].nscale8(dimm);
      }
    }
  }

  // expand everything within a circle
  void Expand(int centerX, int centerY, int radius, byte dimm) {
    if (radius == 0)
      return;

    int currentRadius = radius;

    while (currentRadius > 0) {
      int a = radius, b = 0;
      int radiusError = 1 - a;

      int nextRadius = currentRadius - 1;
      int nextA = nextRadius - 1, nextB = 0;
      int nextRadiusError = 1 - nextA;

      while (a >= b)
      {
        // move them out one pixel on the radius
        leds[XY(a + centerX, b + centerY)] = leds[XY(nextA + centerX, nextB + centerY)];
        leds[XY(b + centerX, a + centerY)] = leds[XY(nextB + centerX, nextA + centerY)];
        leds[XY(-a + centerX, b + centerY)] = leds[XY(-nextA + centerX, nextB + centerY)];
        leds[XY(-b + centerX, a + centerY)] = leds[XY(-nextB + centerX, nextA + centerY)];
        leds[XY(-a + centerX, -b + centerY)] = leds[XY(-nextA + centerX, -nextB + centerY)];
        leds[XY(-b + centerX, -a + centerY)] = leds[XY(-nextB + centerX, -nextA + centerY)];
        leds[XY(a + centerX, -b + centerY)] = leds[XY(nextA + centerX, -nextB + centerY)];
        leds[XY(b + centerX, -a + centerY)] = leds[XY(nextB + centerX, -nextA + centerY)];

        // dim them
        leds[XY(a + centerX, b + centerY)].nscale8(dimm);
        leds[XY(b + centerX, a + centerY)].nscale8(dimm);
        leds[XY(-a + centerX, b + centerY)].nscale8(dimm);
        leds[XY(-b + centerX, a + centerY)].nscale8(dimm);
        leds[XY(-a + centerX, -b + centerY)].nscale8(dimm);
        leds[XY(-b + centerX, -a + centerY)].nscale8(dimm);
        leds[XY(a + centerX, -b + centerY)].nscale8(dimm);
        leds[XY(b + centerX, -a + centerY)].nscale8(dimm);

        b++;
        if (radiusError < 0)
          radiusError += 2 * b + 1;
        else
        {
          a--;
          radiusError += 2 * (b - a + 1);
        }

        nextB++;
        if (nextRadiusError < 0)
          nextRadiusError += 2 * nextB + 1;
        else
        {
          nextA--;
          nextRadiusError += 2 * (nextB - nextA + 1);
        }
      }

      currentRadius--;
    }
  }

  // give it a linear tail to the right
  void StreamRight(byte scale, int fromX = 0, int toX = MATRIX_WIDTH, int fromY = 0, int toY = MATRIX_HEIGHT)
  {
    for (int x = fromX + 1; x < toX; x++) {
      for (int y = fromY; y < toY; y++) {
        leds[XY(x, y)] += leds[XY(x - 1, y)];
        leds[XY(x, y)].nscale8(scale);
      }
    }
    for (int y = fromY; y < toY; y++)
      leds[XY(0, y)].nscale8(scale);
  }

  // give it a linear tail to the left
  void StreamLeft(byte scale, int fromX = MATRIX_WIDTH, int toX = 0, int fromY = 0, int toY = MATRIX_HEIGHT)
  {
    for (int x = toX; x < fromX; x++) {
      for (int y = fromY; y < toY; y++) {
        leds[XY(x, y)] += leds[XY(x + 1, y)];
        leds[XY(x, y)].nscale8(scale);
      }
    }
    for (int y = fromY; y < toY; y++)
      leds[XY(0, y)].nscale8(scale);
  }

  // give it a linear tail downwards
  void StreamDown(byte scale)
  {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      for (int y = 1; y < MATRIX_HEIGHT; y++) {
        leds[XY(x, y)] += leds[XY(x, y - 1)];
        leds[XY(x, y)].nscale8(scale);
      }
    }
    for (int x = 0; x < MATRIX_WIDTH; x++)
      leds[XY(x, 0)].nscale8(scale);
  }

  // give it a linear tail upwards
  void StreamUp(byte scale)
  {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      for (int y = MATRIX_HEIGHT - 2; y >= 0; y--) {
        leds[XY(x, y)] += leds[XY(x, y + 1)];
        leds[XY(x, y)].nscale8(scale);
      }
    }
    for (int x = 0; x < MATRIX_WIDTH; x++)
      leds[XY(x, MATRIX_HEIGHT - 1)].nscale8(scale);
  }

  // give it a linear tail up and to the left
  void StreamUpAndLeft(byte scale)
  {
    for (int x = 0; x < MATRIX_WIDTH - 1; x++) {
      for (int y = MATRIX_HEIGHT - 2; y >= 0; y--) {
        leds[XY(x, y)] += leds[XY(x + 1, y + 1)];
        leds[XY(x, y)].nscale8(scale);
      }
    }
    for (int x = 0; x < MATRIX_WIDTH; x++)
      leds[XY(x, MATRIX_HEIGHT - 1)].nscale8(scale);
    for (int y = 0; y < MATRIX_HEIGHT; y++)
      leds[XY(MATRIX_WIDTH - 1, y)].nscale8(scale);
  }

  // give it a linear tail up and to the right
  void StreamUpAndRight(byte scale)
  {
    for (int x = 0; x < MATRIX_WIDTH - 1; x++) {
      for (int y = MATRIX_HEIGHT - 2; y >= 0; y--) {
        leds[XY(x + 1, y)] += leds[XY(x, y + 1)];
        leds[XY(x, y)].nscale8(scale);
      }
    }
    // fade the bottom row
    for (int x = 0; x < MATRIX_WIDTH; x++)
      leds[XY(x, MATRIX_HEIGHT - 1)].nscale8(scale);

    // fade the right column
    for (int y = 0; y < MATRIX_HEIGHT; y++)
      leds[XY(MATRIX_WIDTH - 1, y)].nscale8(scale);
  }

  // just move everything one line down
  void MoveDown() {
    for (int y = MATRIX_HEIGHT - 1; y > 0; y--) {
      for (int x = 0; x < MATRIX_WIDTH; x++) {
        leds[XY(x, y)] = leds[XY(x, y - 1)];
      }
    }
  }

  // just move everything one line down
  void VerticalMoveFrom(int start, int end) {
    for (int y = end; y > start; y--) {
      for (int x = 0; x < MATRIX_WIDTH; x++) {
        leds[XY(x, y)] = leds[XY(x, y - 1)];
      }
    }
  }

  // copy the rectangle defined with 2 points x0, y0, x1, y1
  // to the rectangle beginning at x2, x3
  void Copy(byte x0, byte y0, byte x1, byte y1, byte x2, byte y2) {
    for (int y = y0; y < y1 + 1; y++) {
      for (int x = x0; x < x1 + 1; x++) {
        leds[XY(x + x2 - x0, y + y2 - y0)] = leds[XY(x, y)];
      }
    }
  }

  // rotate + copy triangle (MATRIX_CENTER_X*MATRIX_CENTER_X)
  void RotateTriangle() {
    for (int x = 1; x < MATRIX_CENTER_X; x++) {
      for (int y = 0; y < x; y++) {
        leds[XY(x, 7 - y)] = leds[XY(7 - x, y)];
      }
    }
  }

  // mirror + copy triangle (MATRIX_CENTER_X*MATRIX_CENTER_X)
  void MirrorTriangle() {
    for (int x = 1; x < MATRIX_CENTER_X; x++) {
      for (int y = 0; y < x; y++) {
        leds[XY(7 - y, x)] = leds[XY(7 - x, y)];
      }
    }
  }

  // draw static rainbow triangle pattern (MATRIX_CENTER_XxWIDTH / 2)
  // (just for debugging)
  void RainbowTriangle() {
    for (int i = 0; i < MATRIX_CENTER_X; i++) {
      for (int j = 0; j <= i; j++) {
        drawPixel(7 - i, j, i * j * 4);
      }
    }
  }

  void drawLine(int x0, int y0, int x1, int y1, byte colorIndex) {
    uint8_t k = plasma[x0][y0];
    drawLine(x0, y0, x1, y1, usPalette[k]);
  }

  void drawLine(int x0, int y0, int x1, int y1, CRGB color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    for (;;) {
      leds[XY(x0, y0)] += color;
      if (x0 == x1 && y0 == y1) break;
      e2 = 2 * err;
      if (e2 > dy) {
        err += dy;
        x0 += sx;
      }
      if (e2 < dx) {
        err += dx;
        y0 += sy;
      }
    }
  }

  void FillPlasma() {
  int           x1, x2, x3, x4, y1, y2, y3, y4, sx1, sx2, sx3, sx4;
  unsigned char i, j;
  uint8_t          value;
  sx1 = (int)(cos(angle1) * levels[0] + levels[4]);
  sx2 = (int)(cos(angle2) * levels[1] + levels[5]);
  sx3 = (int)(cos(angle3) * levels[2] + levels[6]);
  sx4 = (int)(cos(angle4) * levels[3] + levels[7]);
  y1  = (int)(sin(angle1) * levels[8] + levels[12]);
  y2  = (int)(sin(angle2) * levels[9] + levels[13]);
  y3  = (int)(sin(angle3) * levels[10] + levels[14]);
  y4  = (int)(sin(angle4) * levels[11] + levels[15]);

  for(j=0; j<MATRIX_HEIGHT; j++) {
    x1 = sx1; x2 = sx2; x3 = sx3; x4 = sx4;
    for(i=0; i<MATRIX_CENTRE_X; i++) {
      value = hueShift
        + (int8_t)pgm_read_byte(sinetab + (uint8_t)((x1 * x1 + y1 * y1) >> 5))
        + (int8_t)pgm_read_byte(sinetab + (uint8_t)((x2 * x2 + y2 * y2) >> 5))
        + (int8_t)pgm_read_byte(sinetab + (uint8_t)((x3 * x3 + y3 * y3) >> 6))
        + (int8_t)pgm_read_byte(sinetab + (uint8_t)((x4 * x4 + y4 * y4) >> 6));
      plasma[i][j] = plasma[63-i][j] = value;
      x1--; x2--; x3--; x4--;
    }
    y1--; y2--; y3--; y4--;
  }
  hueShift += 2;
}

  // non leds2 memory version.
  void MoveX(byte delta) 
  {

    CRGB tmp = 0;

    for (int y = 0; y < MATRIX_HEIGHT; y++) 
    {
 
      // Shift Left: https://codedost.com/c/arraypointers-in-c/c-program-shift-elements-array-left-direction/
      // Computationally heavier but doesn't need an entire leds2 array

      tmp = leds[XY(0, y)];      
      for (int m = 0; m < delta; m++)
      {
        // Do this delta time for each row... computationally expensive potentially.
        for(int x = 0; x < MATRIX_WIDTH; x++)
        {
            leds[XY(x, y)] = leds [XY(x+1, y)];
        }

        leds[XY(MATRIX_WIDTH-1, y)] = tmp;
      }
   

      /*
      // Shift
      for (int x = 0; x < MATRIX_WIDTH - delta; x++) {
        leds2[XY(x, y)] = leds[XY(x + delta, y)];
      }

      // Wrap around
      for (int x = MATRIX_WIDTH - delta; x < MATRIX_WIDTH; x++) {
        leds2[XY(x, y)] = leds[XY(x + delta - MATRIX_WIDTH, y)];
      }
      */
    } // end row loop

    /*
    // write back to leds
    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
      for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
        leds[XY(x, y)] = leds2[XY(x, y)];
      }
    }
    */
  }

  void MoveY(byte delta)
  {
    
    CRGB tmp = 0; 
    for (int x = 0; x < MATRIX_WIDTH; x++) 
    {
      tmp = leds[XY(x, 0)];      
      for (int m = 0; m < delta; m++) // moves
      {
        // Do this delta time for each row... computationally expensive potentially.
        for(int y = 0; y < MATRIX_HEIGHT; y++)
        {
            leds[XY(x, y)] = leds [XY(x, y+1)];
        }

        leds[XY(x, MATRIX_HEIGHT-1)] = tmp;
      }
    } // end column loop
  } /// MoveY
  

void drawCircle(int16_t x0, int16_t y0, int16_t r, CRGB color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  leds[XY(x0, y0 + r)] += color;
  leds[XY(x0, y0 - r)] += color;
  leds[XY(x0 + r, y0)] += color;
  leds[XY(x0 - r, y0)] += color;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    leds[XY(x0 + x, y0 + y)] += color;
    leds[XY(x0 - x, y0 + y)] += color;
    leds[XY(x0 + x, y0 - y)] += color;
    leds[XY(x0 - x, y0 - y)] += color;
    leds[XY(x0 + y, y0 + x)] += color;
    leds[XY(x0 - y, y0 + x)] += color;
    leds[XY(x0 + y, y0 - x)] += color;
    leds[XY(x0 - y, y0 - x)] += color;
  }
}

void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, CRGB color) {
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}
void xLine(int x0, int x1, int y, CRGB color)
  {
    if (y < 0 || y >= MATRIX_HEIGHT)
      return;
    if (x0 > x1)
    {
      int xb = x0;
      x0 = x1;
      x1 = xb;
    }
    if (x0 < 0)
      x0 = 0;
    if (x1 > MATRIX_WIDTH)
      x1 = MATRIX_WIDTH;
    for (int x = x0; x < x1; x++)
    leds[XY(x, y)] += color;
  }
  
void ellipse(int x, int y, int rx, int ry, CRGB color)
  {
    if(ry == 0)
      return;
    int oxr = rx;
    float f = float(rx) / ry;
    f *= f;
    for(int i = 0; i < ry + 1; i++)
    {
      float s = rx * rx - i * i * f;
      int xr = (int)sqrt(s <= 0 ? 0 : s);
      xLine(x - oxr, x - xr + 1, y + i, color);
      xLine(x + xr, x + oxr + 1, y + i, color);
      if(i) 
      {
        xLine(x - oxr, x - xr + 1, y - i, color);
        xLine(x + xr, x + oxr + 1, y - i, color);
      }
      oxr = xr;
    }
  }

  void fillRect(int x, int y, int w, int h, CRGB color)
  {
    if (x < 0)
    {
      w += x;
      x = 0;
    }
    if (y < 0)
    {
      h += y;
      y = 0;
    }
    if (x + w > MATRIX_WIDTH)
      w = MATRIX_WIDTH - x;
    if (y + h > MATRIX_HEIGHT)
      h = MATRIX_HEIGHT - y;
    for (int j = y; j < y + h; j++)
      for (int i = x; i < x + w; i++)
        drawPixel(i, j, color);
  }

  void fillRect(int x, int y, int w, int h)
  {
    if (x < 0)
    {
      w += x;
      x = 0;
    }
    if (y < 0)
    {
      h += y;
      y = 0;
    }
    if (x + w > MATRIX_WIDTH)
      w = MATRIX_WIDTH - x;
    if (y + h > MATRIX_HEIGHT)
      h = MATRIX_HEIGHT - y;
    for (int j = y; j < y + h; j++)
      for (int i = x; i < x + w; i++)
        drawPixel(i, j);
  }
};

#endif
