#include <Homie.h>
#include <Adafruit_NeoPixel.h>

#define FW_NAME "led-cycle"
#define FW_VERSION "1.0.0"

#define PIN            D1
#define LED_COUNT      13

#define ACTION_OFF 0
#define ACTION_SOLID 1
#define ACTION_RAINBOW 2
#define ACTION_RAINBOW_LOOP 3

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);
//int delayval = 500; 
int showType = 0;
int SoffitR;
int SoffitG;
int SoffitB;
int numPixels = LED_COUNT;

HomieNode ledNode("led", "led");

int action = ACTION_OFF;

struct {
    int j;
    int q;
  } theaterChaseRainbow = {0,0};

bool lightOnHandler(HomieRange range, String value) {
  Serial.print("Light On Handler");
  if (value == "off") {
    action = ACTION_OFF;
    SoffitR = 0;
    SoffitG = 0;
    SoffitB = 0;
    ledNode.setProperty("color").send("0,0,0,0");
  } else if (value == "red") {
    action = ACTION_SOLID;
    SoffitR = 255;
    SoffitG = 0;
    SoffitB = 0;
    ledNode.setProperty("color").send("0,255,0,0");
  } else if (value == "green") {
    action = ACTION_SOLID;
    SoffitR = 0;
    SoffitG = 255;
    SoffitB = 0;
    ledNode.setProperty("color").send("0,0,255,0");
  } else if (value == "blue") {
    action = ACTION_SOLID;
    SoffitR = 0;
    SoffitG = 0;
    SoffitB = 255;
    ledNode.setProperty("color").send("0,0,255,0");
  } else if (value == "rainbow") {
    action = ACTION_RAINBOW;
    theaterChaseRainbow.j = 0;
    theaterChaseRainbow.q = 0;
    ledNode.setProperty("color").send("0,0,0,1");
  } else if (value == "rainbow-loop") {
    action = ACTION_RAINBOW_LOOP;
    theaterChaseRainbow.j = 0;
    theaterChaseRainbow.q = 0;
    ledNode.setProperty("color").send("0,0,0,1");
  } else {
    action = ACTION_SOLID;
    // split string at every "," and store in proper variable
    // convert final result to integer
    SoffitR = value.substring(0,value.indexOf(',')).toInt();
    SoffitG = value.substring(value.indexOf(',')+1,value.lastIndexOf(',')).toInt();
    SoffitB = value.substring(value.lastIndexOf(',')+1).toInt();
    ledNode.setProperty("color").send(value);
  }
  return true;
}


void loopHandler() {
  if (action == ACTION_OFF) {
    colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
    delay(50);
  } else if (action == ACTION_SOLID) {
    colorWipe(strip.Color(SoffitR, SoffitG, SoffitB), 50);
    delay(50);
  } else if ((action == ACTION_RAINBOW) || (action == ACTION_RAINBOW_LOOP)) {
    theaterChaseRainbow.q++;
    if (theaterChaseRainbow.q == 3) {
      theaterChaseRainbow.q = 0;
      theaterChaseRainbow.j++;
    }
    if (theaterChaseRainbow.j == 256) {
      if (action == ACTION_RAINBOW_LOOP) {
        theaterChaseRainbow.j = 0;
      } else {
        action = ACTION_OFF;
        return;
      }
    }
    for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
      strip.setPixelColor(i+theaterChaseRainbow.q, Wheel( (i+theaterChaseRainbow.j) % 255));    //turn every third pixel on
    }
    strip.show();

    delay(50);

    for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
      strip.setPixelColor(i+theaterChaseRainbow.q, 0);        //turn every third pixel off
    }
  } else {
    delay(50);
  }
}

void colorWipe(uint32_t c, uint8_t wait) {
  static int i;
  for(uint16_t i=0; i<strip.numPixels(); i++) { //<strip.numPixels
    strip.setPixelColor(i, c);
    strip.show();
    //delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void setup() {
  Serial.begin(115200);
  Serial.println(FW_NAME FW_VERSION);
  
  strip.begin();

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setLoopFunction(loopHandler);
  ledNode.advertise("color").settable(lightOnHandler);

  Homie.setup();

}

void loop() {
  Homie.loop();
}

