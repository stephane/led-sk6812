# Setup

- Strip SK6812 60 leds/m, consumption 18 W/m.
- main power 100W (DC5V and 20A).
- Arduino UNO.
- the GND of the Arduino should be connected to ground of the alimentation.

# Arduino Library

NeoPixelBus by Makuna (IDE -> Tools -> Library Manager)

# Animation

The 3 animations run in parallel:
1. light pixel from right to left and reverse
2. darken until off all the leds of the strip
3. linear blend of the color used to light the pixels
