#ifndef TOUCH_CONFIG_H
#define TOUCH_CONFIG_H

#include <XPT2046_Touchscreen.h>
#include <SPI.h>


// CONFIGURE TOUCH PINS HERE
#define TOUCH_CS 38   
#define TOUCH_IRQ 3   
#define TOUCH_MOSI 11 
#define TOUCH_MISO 13 
#define TOUCH_CLK 12  

class TouchConfig {
  public:
    XPT2046_Touchscreen* ts;
    SPIClass* touchSPI;

    // CONFIGURE SCREEN DIMENSIONS HERE
    const int screenWidth = 272;
    const int screenHeight = 480;

    // MAP TOUCH COORDINATES TO SCREEN DIMENSIONS
    const int minX = 300;
    const int maxX = 3800;
    const int minY = 300;
    const int maxY = 3800;

    TouchConfig() {
      touchSPI = new SPIClass(HSPI);
      ts = new XPT2046_Touchscreen(TOUCH_CS, TOUCH_IRQ);
    }

    bool begin() {
      touchSPI->begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
        if (!ts->begin(*touchSPI)) {
        return false;
      }
      ts->setRotation(3); // CHOOSE ROTATION HERE
      return true;
    }

    void getScaledPoint(int& x, int& y, int& z) {
      if (ts->touched()) {
        TS_Point p = ts->getPoint();
        // Scale the point to the screen dimensions
        x = constrain(map(p.x, minX, maxX, 0, screenWidth), 0, screenWidth);
        y = constrain(map(p.y, minY, maxY, 0, screenHeight), 0, screenHeight);
        z = p.z;
      }
    }

    bool isTouched() {
      return ts->touched();
    }
};

#endif
