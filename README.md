# SDL_BGI Clock that shows Time Dilation.  

## Demonstrate Time dilation using a quartz action clock.

This is a graphic representation of "Time Dilation" imposed upon
an analog clock with quartz like motion. The clock shows the second hand as viewed from a stationary observer in Blue. aka the stationary observer looking at the clock.  
Another second hand (Yellow/Green) shows the accumulated time difference due to time dilation as observed by the second hand or an observer that
is moving with the second hand.  

The clock has a mathematical radius of:2862807095.5421653553357478091848m  
and a circumference of:17987547480m  
resulting in 1 second interval at the outside of the second hand being equal to 299792458m/s (Speed of light 'c').  

The tip of the second hand is moving at the speed of light and the center of the clocks second hand is stationary. This gives a somewhat interesting view of time dilation as with the second hand showing from 0m/s to 299792458m/s at the outside radius, accumulating over the 60 second period of a normal analog clock, although scaled to fit a 500px radius.  
The intervals of the radius (500px) are 5725614.1910843307106714956183696m each.  

This is puerly a mathematical graph of time dilation of linear velocity up to the speed of light, but displayed upon a circular clock face. Mass, circular forces such as acceleration and centripital/centrifugal etc are not accounted for. I am working on some similar representations for the clock to display newtons and Eulers laws, general and special relativity as well as gravitational time dilation and the Schwarzschild radius plus many other, but the complexity is high and my available time is low so I may not make progress on that in the neer future.  

We can draw many other interesting interpretations from this as well but will leave that up to your imagination :)  

---
Please note that the current source has been staticaly set to a 1410 x 1010 px window meaning you will need a display with a screen reolution greater than the clock window to run the application.  

The numerals of the clock face can be turned ON or OFF using the line:  
``#define CLOCK_NUMERALS 0 // 1 == ON | 0 == OFF``

For setting up the C compiler and IDE, SDL-Bgi library <graphics.h> as well as SDL2 please see the sections under:
[2_Development_Environment_Overview](https://github.com/Axle-Ozz-i-sofT/A-BEGINNERS-GUIDE-TO-PROGRAMMING/tree/main/2_Development_Environment_Overview)  
[A Beginners_Guide_To_SDL_bgi](https://github.com/Axle-Ozz-i-sofT/A-BEGINNERS-GUIDE-TO-PROGRAMMING/tree/main/Supplimental/A%20Beginners_Guide_To_SDL_bgi)  

LICENCE MIT
2023
Axle
