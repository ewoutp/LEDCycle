# LEDCycle 

Firmware for 360 degree LED Indicator (see https://www.thingiverse.com/thing:2363013)

## Dependencies

- Homie (https://github.com/marvinroger/homie-esp8266)
- Adafruit_NeoPixel

## Usage:

Send MQTT message to `<prefix>/<device-name>/led/color/set` with a value of:

- `off` To turn all LEDs off
- `red` To set all LEDs to red
- `green` To set all LEDs to green
- `blue` to set all LEDs to blue 
- `rainbow` to activate a rainbow effect
