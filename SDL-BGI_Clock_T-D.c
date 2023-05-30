//------------------------------------------------------------------------------
// Name:        SDL-BGI_Clock_T-D.c
// Purpose:     Demonstrate Time dilation using a quartz action clock.
// Title:       "Time Dilation Clock"
//
// Platform:    Win64, Ubuntu64
//
// Compiler:    GCC V9.x.x, MinGw-64, libc (ISO C99)
// Depends:     SDL2-devel, SDL_bgi-3.0.0,
// Requires:    Minimum screen resolution of 1410x1010 for the clock window.
//              ie. 1920 x 1080 (FHD)
//
// Author:      Axle
// Created:     12/03/2023
// Updated:     30/05/2023
// Version:     0.0.2.1
// Copyright:   (c) Axle 2022
// Licence:     MIT
//------------------------------------------------------------------------------
// NOTES:
// This is a representation of time dilation using a quartz like action clock.
// The radius is 2862807095.5421653553357478091848m or
// (~2862807km |~4.1 times the radius of the Sun, or half the orbital radius
// of Mercury). The circumference of the clock is
// 17987547480m so when divided by the number of seconds (60) for each rotation
// the outer tip of the second hand is travelling at 299792458m/s or the speed
// of light. The radius is divided evenly into 500 points of radius each with
// it's own circumference. Time dilation is calculated against the velocity
// of the second hand at each of the 500 radii and then plotted to another second
// hand depiction the time dilation.
// The mathematical calculations are against the full size clock and are then
// scaled to fit the 500px radius and 3600 points of circumference. The values
// are rounded to the nearest pixel (x,y) position.
// Due to the size of the numbers involved there is some rounding of the floating
// point values from the 1:1 scale but is not detectable in this representation.
// The final display offers a unique perspective of the maped time dilation over
// and extended period as this would be more difficult to exemplify in a linear
// x.y graph. In some sense it offers a 2D perspective plus time.
// This is a mathematical representation only and effects from mass are
// disregarded.
//
// The clock now follows the computer clock time as well as keeping both
// second hands in sync. The calculations for the time dilation period is
// calculated at each update from the computer clock.
//
// The x.y plot positions are calculated from a 500 * 3600 pre-created look up
// table. Each period of dilation is calculated for the current clock time for
// the 500 positions of radius against the time when the clock app started.
//
// Although some amount of accumulated error occurs over time due to the size
// limitations of the floating point precision I don't think this would be
// recognisable at the scale of the clock with a 1000 pixel diameter.
//
// The clock is calculated to a precision of 60 frames per second. In practice
// the clock may update at a faster or slower FPS, but overall accuracy remains.
// 3600 points are calculated for the circumference of the clock or 60 seconds
// multiplied by 60 updates per second.
//
// Some of the calculations can be improved to provide greater accuracy as well
// as be more efficient in CPU cycles, so much can be improved.
//
// In time I may include additional physics calculations such as centripetal
// force, acceleration and gravity. This is purely a mathematical representation
// and the mass of the second hand is not currently taken into account. In a
// real world (physical structure) the forces applied to the second hand would
// tear it away from its central axis long before reaching the speed of light.
//
// The SDL_Bgi library is quite limited, so in all likelihood I will migrate
// the Time Dilation Clock to a more capable graphics library in the future.
//------------------------------------------------------------------------------
// Speed of Light 'c' 299,792,458m/s
// The above is also called the speed of electromagnetic radiation.
// Remember that a unit of 1 meter is derived from the distance light travels
// in 1/299792458th of a second and not the other way around.



#include <stdio.h>
//#include <conio.h>
//#include <string.h>
//#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <graphics.h>

#include <limits.h>
#include <float.h>

// Turn off compiler warnings for unused variables between (Windows/Linux etc.)
#define unused(x) (x) = (x)
// The following dimentions are currently hard coded to a 1000 pixel diameter
// clock face and cannot be altered. I may add screen dimension scaling/resizing
// in the future.
#define WINDOW_X 1410
#define WINDOW_Y 1010

// To add or remove the numerals from the clock face.
#define CLOCK_NUMERALS 0 // 1 == ON | 0 == OFF

// Separate implementations of the x. y rendering functions.
// I have left both sets so you can see what I have done to
// calculate 3600 second hand tics per minute :)
// 3600 (/60 sec) == 60 frames per second for rendering the second hand.
// for 30 frames per second we would make secx/y[1800]
void Calc3600(int radius, int midx, int midy, int secx[3600], int secy[3600]);

// Calculate the x,y for the time dilation hand. 3600 possitions of circumference.
void Calc3600_td(int radius, int midx, int midy, int **secx, int **secy);
//==============================================================================
// http://see-programming.blogspot.com/2013/09/c-program-to-implement-analog-clock.html
// Calculate numbers Hours and Minutes.
void minSecCalc(int radius, int midx, int midy, int secx[60], int secy[60]);
void calcPoints(int radius, int midx, int midy, int x[12], int y[23]);

// Not currently in use. This is required as part of a standard clock with
// 1 second ticks
void Calc360(int radius, int midx, int midy, int secx[60], int secy[60]);

// Calculate the time dilation for the velocity over 1 second.
double GetTimeDilation(double v);

// Calculate the velocity in m/s for each of the 500 radius points. The outer
// tip of the second hand is traveling at 299792458m/s or C.
int Get500RadiusMeterPerSecond(void);

//http://programmertutor16.blogspot.com/2013/10/analog-clock-in-c-simplified.html
int main(int argc, char *argv[])
    {
    unused(argc);  // Turns off the compiler warning for unused argc, argv
    unused(argv);  // Turns off the compiler warning for unused argc, argv

// Note: to show greater precision in printf() use %.*f or %.17f
// Also note that double can store aprox 15 to 17 digits after the period but
// this can be platform and implementation dependent.
// The following appears to be incorrect as a 64-bit type should only hold 20
// digits in total, but DBL_MAX is defined as 1.7976931348623157E+308 in some
// instances. This differentiation is when it is held as and exponent as seen below.
//printf("DBL_MAX=%f\n", DBL_MAX);  // (double)DBL_MAX
//DBL_MAX=179769313486231570000000000000000000000000000000000000000000000000000000 _
//0000000000000000000000000000000000000000000000000000000000000000000000000000 _
//0000000000000000000000000000000000000000000000000000000000000000000000000000 _
//0000000000000000000000000000000000000000000000000000000000000000000000000000 _
//000000000.000000

//printf("DBL_MAX=%g\n", DBL_MAX);
//DBL_MAX=1.79769e+308

//printf("MAX=%d\n", INT_MAX);
//MAX=2147483647

    // Get clock times.
    int j, sec;
    int midx, midy, radius, hr, min, sec3600;
    int msec = 0;

    // Initiate Time data structures.
    time_t t1;
    struct tm *data;
    struct timeval tv;
    //struct timezone tz;  // Deprecated, use NULL.

    // Initiate arrays to hold clock graphics data.
    int x[12], y[12], minx[60], miny[60], n_minx[60], n_miny[60];
    int hrx[12], hry[12];
    int msecx[3600], msecy[3600];
    char str[256];

    // Creat the look up tables for 500 * 3600 x.y plots.
    // malloced array dimensions must be freed using the loop below.
    //int td_plotx[500][3600], td_ploty[500][3600];
    // approx 13.73291015625 MiB (x 2)
    int q = 3600;  // Circumference plots.
    int r = 500;  // Radius plots.
    int e = 0;
    int *td_plotx[r], *td_ploty[r];

    for (e = 0; e < r; e++)
        {
        td_plotx[e] = (int*)malloc(q * sizeof(int));
        td_ploty[e] = (int*)malloc(q * sizeof(int));
        }


    int Last_min = -1;  // Test for minute roll over.
    int min3600 = 0;  // Updates accumulation of drawing for each 3600 ticks.

    int time_elapsed = 0;  // Stats display.
    char Buf_time_elapsed[128] = {'\0'};  // Stats display.

    // This hold the accumulation of time up to current time for calculating the
    // time dilation for the second seconds hand.
    // Some care needs to be taken with the accumulated size of the values of
    // the array as they will continue to increase over time until the recorded
    // values exceed the size of double.
    // I have placed size limit test in the main routine for this.
    double time_accumulative = 0;
    // Calculates the current accumulation of the 3600 tick period (minutes).
    int time_accumulative_temp = 0;
    // Hold the current 3600 (Minute) adjustment amount (incitements by 3600 per minute).
    int adjust3600 = 0;
    int cnt2 = 0;  // Loop counter


    // Set the SDL windows options.
    // Note! SDL_BGI is not resizable from the window border. But can be resized
    // via internal resetwinoptions()
    setwinoptions ("Time Dilation Clock - Any key to quit", // char *title
                   SDL_WINDOWPOS_CENTERED, // int x
                   SDL_WINDOWPOS_CENTERED, // int y
                   -1); // Uint32 flags (See SDL_bgi.c setwinoptions for flags)

    // only a subset of flag is supported for now
    // From SDL_bgi.c
    /*
     if (flags & SDL_WINDOW_FULLSCREEN         ||
         flags & SDL_WINDOW_FULLSCREEN_DESKTOP ||
         flags & SDL_WINDOW_SHOWN              ||
         flags & SDL_WINDOW_HIDDEN             ||
         flags & SDL_WINDOW_BORDERLESS         ||
         flags & SDL_WINDOW_MAXIMIZED          ||
         flags & SDL_WINDOW_MINIMIZED) */

         // Window can be resized via the application, but mouse possitions must be
   // calculated for drag actions.
   // SDL_WINDOW_RESIZABLE = 0x00000020,          < window can be resized

    int Win_ID_1 = initwindow(WINDOW_X, WINDOW_Y);  // intiiate the graphics driver and window.(1280, 1024)
    // It defaults to fast, so I don't think this is needed.
    sdlbgifast();  // sdlbgiauto(void)


    // mid position in x and y -axis
    midx = getmaxx() / 2;
    midy = getmaxy() / 2;
    radius = midy - 4;  // 500
    //printf("Radius = %d\n", radius);  // DEBUG
    //int maxx = getmaxx();
    //int maxy = getmaxy();

    // Get position to locate Hr numbers in clock
    calcPoints(radius - 50, midx, midy, x, y);

    // Get position to locate MinSec numbers in clock
    minSecCalc(radius - 20, midx, midy, n_minx, n_miny);

    // Get position for hour needle
    calcPoints(radius - 100, midx, midy, hrx, hry);

    // Get position for minute needle
    minSecCalc(radius - 70, midx, midy, minx, miny);

    // Get position for seconds needle. 60 * 60 ticks per second = 3600
    Calc3600(radius - 0, midx, midy, msecx, msecy);


    // This calculates the complete x.y pixel lookup table for radius 500 times
    // 3600 ticks per minute.
    Calc3600_td(radius - 0, midx, midy, td_plotx, td_ploty);

    // Get the initial TD in m/s for 1 sec3600 (500 plots from center to radius)
    int T_Radius = 500;
    int counter = 0;
    double Velocity[500];  // m/s / 60 for 500px radius


    // This obtains our time dilation accurate to 1 sec as a fraction 1/60th of 1 sec.
    // This can also be represented as a meter per second calculation.
    for ( counter = 0; counter < T_Radius; counter++)
        {
        // The following gives meters per sec3600 for 1 second. Att 500 * radius units it should equal C (299792458)
        // C = 299792458
        // circumference = 17987547480m
        // Radius = 2862807095.5421653553357478091848m (~2862807km |~distance from Sun to Uranus)
        // Radius/500 = 5725614.1910843307106714956183696
        // Velocity[counter] = 2 * M_PI * (5725614.19108 * (counter + 1))
        //Velocity[counter] = ([2*Pi] * ([Radius/500] * (counter + 1)) / 60th of second);
        Velocity[counter] = (6.28318530717958647692 * (5725614.1910843307106714956183696 * (counter + 1)) / 60);  // == m/s == meters
        //printf("%f\n", Velocity[counter]);  // [0 +1]599584.916000 to [499 +1]299792458.000000
        }


    // Main loop to update the clock graphics.
    // kbkit() is for the console emulator, xkbhit() is for the SDL window.
    while (!xkbhit())// && !kbhit())
        {

        // Write stats.
        settextstyle(TRIPLEX_FONT, 0, 1);
        settextjustify(LEFT_TEXT, CENTER_TEXT);
        setcolor (LIGHTGRAY);
        //setbkcolor (BLACK);
        outtextxy (5, 5, "Radius step * 500: 5725614.1910843307106714956183696m" );
        outtextxy (5, 30, "Radius: 2862807095.5421653553357478091848m" );

        outtextxy (5,55, "Circumference: 17987547480m" );
        outtextxy (5, 80, "Circumference/60: 299792458 m/s" );
        outtextxy (5, 105, "circumferenc steps: 3600 (60 FPS)" );
        outtextxy (5, 130, "Scale: 1:5725614.191084331" );

        sprintf(Buf_time_elapsed, "Min elapsed: [%06d]", time_elapsed);
        outtextxy (5, 155, Buf_time_elapsed );

        // Draw the clock face (Old draw method)
        setlinestyle(SOLID_LINE, 1, 1);  // set line size for all (1|3)
        //settextstyle(TRIPLEX_FONT, 0, 3);
        settextstyle(TRIPLEX_FONT, 0, 1);
        //setcolor(DARKGRAY);

        setcolor (DARKGRAY);

        // Draws frame of the clock
        circle(midx, midy, radius +2);

#if CLOCK_NUMERALS == 1
        // To remove the hours and minutes displays, comment out from here ==>
        // Place the 60 sec numbers in clock
        for (j = 0; j < 60; j++)
            {
            if (j == 0)
                {
                sprintf(str, "%d", 60);
                }
            else
                {
                sprintf(str, "%d", j);
                }
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            moveto(n_minx[j], n_miny[j]);
            outtext(str);
            }

        // Place the 12 numbers  in clock
        for (j = 0; j < 12; j++)
            {
            if (j == 0)
                {
                sprintf(str, "%d", 12);
                }
            else
                {
                sprintf(str, "%d", j);
                }
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            moveto(x[j], y[j]);
            outtext(str);
            }
            // <== too here.
#endif

        // Get the current time using time() API
        t1 = time(NULL);
        data = localtime(&t1);
        gettimeofday(&tv, NULL);  //gettimeofday(&tv,&tz);


        // Note that the drawing order is important. Drawing starts at the back
        // layer in the Z order progressing up to the most front layer.

        // Draw the hour needle in clock
        // You can alter the colour of the hands with setcolor()
        hr = data->tm_hour % 12;
        //setcolor(LIGHTGRAY);
        line(midx, midy, hrx[hr], hry[hr]);
        //setcolor(WHITE);

        // Draw the minute needle in clock
        min = data->tm_min % 60;
        //setcolor(LIGHTGRAY);
        line(midx, midy, minx[min], miny[min]);
        //setcolor(WHITE);


        // Draw the seconds needle in clock. I am using 6 steps between each
        // Revise usec, msec, sec calculations.
        msec = (int)(tv.tv_usec /16667);  // 0 to 59 seconds (1000000 / 16667)
        //msec = (int)(tv.tv_usec *0.6 / 10000); // this will also give 1 to 59 per second.

        // Calculate seconds and microseconds to the 3600 tick position.
        sec3600 = (data->tm_sec * 60 +msec) % 3600;  //60 sec * 60 ticks/sec

        // Draw Second hand (Clock Time)
        setcolor(BLUE);  // LIGHTBLUE
        line(midx, midy, msecx[sec3600], msecy[sec3600]);
        //line(midx, midy, msecx[ticks], msecy[ticks]);
        //setcolor(WHITE);

// #############################################################################
// Time dilation calculations and plots.

        //setlinestyle(SOLID_LINE, 1, 1);  // [1|3]
        setcolor(GREEN);  // LIGHTBLUE LIGHTRED YELLOW

        // Set first minute count.
        if (Last_min == -1)
            {
            Last_min = min;
            }

        // Minute counters. Ticks over eack minute.
        if (Last_min != min)
            {
            // Accumulate each minute (3600 ticks) to calculate plot from current time.
            min3600 += 3600;  // if < INT_MAX
            Last_min = min;  // get current/new minute test.
            time_elapsed++;  // in minutes for the display stats.
            }


        // Calculate and draw the TD second hand.
        for ( cnt2 = 0; cnt2 < T_Radius; cnt2++)   // + 1 for all results
            {

            // The following gives meters per sec3600 for 1 second. At 500 * radius units it should equal C (299792458)
            // C = 299792458
            // circumference = 17987547480m
            // Radius = 2862807095.5421653553357478091848m
            // Radius/500 = 5725614.1910843307106714956183696
            //Velocity[counter] = ([2*Pi] * ([Radius/500] * (counter + 1)) / 60th of second);

            //Velocity[500] is a pre-populated look up table of the velocity for each 1/60th second.
            // I will need to change this to calculate from the real time seconds / 60.

            // 3600th division up to 3600 seconds as
            //printf("sec3600=%d\n", sec3600);
            // time_accumulative is now real clock time. Gets current time.
            time_accumulative = ((GetTimeDilation(Velocity[cnt2]) / 60.0 ) * (sec3600 + min3600));

            // The following calculates time/tick counts above 3600 and adjusts
            // so the the px are always withing the 500*3600 lookup table.

            // Get the closest integer from the double values converted to
            // 3600th steps of a circle.
            time_accumulative_temp = round(((time_accumulative) * 0.6) * 100);  // Check why I have 0.6 here. It should be 3600ths already.

            // The following steps can be combined...
            // get the number of iterations of 3600 rotations.
            adjust3600 = (int)(time_accumulative_temp / 3600);  // cast to int rounding to floor.

            // this will keep the x,y points within the 3600 and still keep accumulated error.
            if (time_accumulative_temp >= 1)
                {
                    // Remove iterations above 3600 so that plots fall within
                    // the 3600 point circle.
                time_accumulative_temp -= (3600 * adjust3600);
                }

            // Draw the actual x.y plot of the accumulated time dilation for each radius point.
            fputpixel (td_plotx[cnt2][time_accumulative_temp], td_ploty[cnt2][time_accumulative_temp] );

            }  // END Radius draw loop.


        // Some safety on type limits for MAX_(Type)
        if ((time_accumulative >= (DBL_MAX * 0.5)) || (min3600 > INT_MAX -7200))
            {
            printf( "DBL_MAX or INT_MAX Limit reached!\n");
            break;
            }

// #############################################################################


        // We can use double buffering wich is the standard method to create
        // smooth flowing animations without flicker.
        // Use void sdlbgifast (void); Mode + refresh()
        //swapbuffers(); swapbuffers is the same as the 4 lines below.
        // Use swpapbuffers or getvisualpage() etc
        int oldv = getvisualpage();
        int olda = getactivepage();
        setvisualpage(olda);
        setactivepage(oldv);


        // refresh(), event(), x|kbhit() also preforms a refresh!
        refresh();

        // Clears the display interface (background page).
        cleardevice();


        // NOTE! SDL_Delay() can interfere with the SDL_Bgi
        // Sleep() vs delay(): Sleep can sometimes interfere with the graphics
        // display and is non standard to graphics.h but has the advantage of
        // keeping the CPU at an idle state. delay is portable, but will run
        // the CPU core at close to 100% while the application is running.
        // delay wait is part of SDL-BGI
        //delay(wait); // The correct sleep function for bgi.
        // SDL_Delay() Is the native SDL equivalent to Sleep(), sleep()
        // This library runs on top of SDL2 so it is OK to use SDL functions
        // "With Care".
        SDL_Delay(1);  // Idle back the CPU usage a bit
        //Busy_Wait(wait);
        //nano_sleep_dec(0.000000099);  // 999,999,999
        }


    // deallocate memory allocated for graphic screen
    closewindow(Win_ID_1);
    closegraph();

    // loop through each static dimension and clear the dynamic allocations.
    for (e = 0; e < r; e++)
        {
        free(td_plotx[e]);
        free(td_ploty[e]);
        }

    return 0;
    }  // <== END main()


// A modification of minSecCalc() to achieve 360 x,y points for the second
// hand. (seconds * 6) + (millisecond / 166667)
// 50 * 6 + 10 <- as weird as that seams we start at 0 to 359 in the array.
void Calc3600(int radius, int midx, int midy, int secx[3600], int secy[3600])
    {
    //int i, j = 0, x, y;
    int i, j = 0;
    //char str[32];

    /* 90 position(min/sec - 12 to 3) in first quadrant of clock  */
    secx[j] = midx, secy[j++] = midy - radius;
    for (i = 901; i < 1800; i = i + 1)  // +6
        {
        secx[j] = midx - (radius * cos((i * 3.14) / 1800));
        secy[j++] = midy - (radius * sin((i * 3.14) / 1800));
        }

    /* 90 positions(min or sec - 3 to 6) in second quadrant of clock */
    secx[j] = midx + radius, secy[j++] = midy;
    for (i = 1801; i < 2700; i = i + 1)  // +6
        {
        secx[j] = midx - (radius * cos((i * 3.14) / 1800));
        secy[j++] = midy - (radius * sin((i * 3.14) / 1800));
        }

    /* 90 positions(min or sec - 6 to 9) in third quadrant of clock */
    secx[j] = midx, secy[j++] = midy + radius;
    for (i = 2701; i < 3600; i = i + 1)  // +6
        {
        secx[j] = midx - (radius * cos((i * 3.14) / 1800));
        secy[j++] = midy - (radius * sin((i * 3.14) / 1800));
        }

    /* 90 positions(min or sec - 9 to 12) in fourth quadrant of clock */
    secx[j] = midx - radius, secy[j++] = midy;
    for (i = 1; i < 900; i = i + 1)  // +6
        {
        secx[j] = midx - (radius * cos((i * 3.14) / 1800));
        secy[j++] = midy - (radius * sin((i * 3.14) / 1800));
        }

    }


// Dilation Plot version
// A modification of minSecCalc() to achieve 3600 x,y points for the second
// hand. (seconds * 60) + (millisecond / 166667)
// 3600 x,y plots are calculated for each of the 500 radius points.
void Calc3600_td(int radius, int midx, int midy, int **secx, int **secy)
    {
    //int i, j = 0, x, y;
    int i, j = 0;
    //char str[32];
    radius = radius;
    int td_count1 = 0;
    for (td_count1 = 0; td_count1 < 500; td_count1++)
        {
        j = 0;

        // 90 position(min/sec - 12 to 3) in first quadrant of clock
        secx[td_count1][j] = midx, secy[td_count1][j++] = midy - td_count1;
        for (i = 901; i < 1800; i = i + 1)  // +6
            {
            secx[td_count1][j] = midx - (td_count1 * cos((i * 3.14) / 1800));
            secy[td_count1][j++] = midy - (td_count1 * sin((i * 3.14) / 1800));
            }

        // 90 positions(min or sec - 3 to 6) in second quadrant of clock
        secx[td_count1][j] = midx + td_count1, secy[td_count1][j++] = midy;
        for (i = 1801; i < 2700; i = i + 1)  // +6
            {
            secx[td_count1][j] = midx - (td_count1 * cos((i * 3.14) / 1800));
            secy[td_count1][j++] = midy - (td_count1 * sin((i * 3.14) / 1800));
            }

        // 90 positions(min or sec - 6 to 9) in third quadrant of clock
        secx[td_count1][j] = midx, secy[td_count1][j++] = midy + td_count1;
        for (i = 2701; i < 3600; i = i + 1)  // +6
            {
            secx[td_count1][j] = midx - (td_count1 * cos((i * 3.14) / 1800));
            secy[td_count1][j++] = midy - (td_count1 * sin((i * 3.14) / 1800));
            }

        // 90 positions(min or sec - 9 to 12) in fourth quadrant of clock
        secx[td_count1][j] = midx - td_count1, secy[td_count1][j++] = midy;
        for (i = 1; i < 900; i = i + 1)  // +6
            {
            secx[td_count1][j] = midx - (td_count1 * cos((i * 3.14) / 1800));
            secy[td_count1][j++] = midy - (td_count1 * sin((i * 3.14) / 1800));
            }

        }

    }


// A modification of minSecCalc() to achieve 360 x,y points for the second
// hand. (seconds * 6) + (millisecond / 166667)
// 50 * 6 + 10 <- as weird as that seams we start at 0 to 359 in the array.
void Calc360(int radius, int midx, int midy, int secx[360], int secy[360])
    {
    //int i, j = 0, x, y;
    int i, j = 0;
    //char str[32];

    /* 90 position(min/sec - 12 to 3) in first quadrant of clock  */
    secx[j] = midx, secy[j++] = midy - radius;
    for (i = 91; i < 180; i = i + 1)  // +6
        {
        secx[j] = midx - (radius * cos((i * 3.14) / 180));
        secy[j++] = midy - (radius * sin((i * 3.14) / 180));
        }

    /* 90 positions(min or sec - 3 to 6) in second quadrant of clock */
    secx[j] = midx + radius, secy[j++] = midy;
    for (i = 181; i < 270; i = i + 1)  // +6
        {
        secx[j] = midx - (radius * cos((i * 3.14) / 180));
        secy[j++] = midy - (radius * sin((i * 3.14) / 180));
        }

    /* 90 positions(min or sec - 6 to 9) in third quadrant of clock */
    secx[j] = midx, secy[j++] = midy + radius;
    for (i = 271; i < 360; i = i + 1)  // +6
        {
        secx[j] = midx - (radius * cos((i * 3.14) / 180));
        secy[j++] = midy - (radius * sin((i * 3.14) / 180));
        }

    /* 90 positions(min or sec - 9 to 12) in fourth quadrant of clock */
    secx[j] = midx - radius, secy[j++] = midy;
    for (i = 1; i < 90; i = i + 1)  // +6
        {
        secx[j] = midx - (radius * cos((i * 3.14) / 180));
        secy[j++] = midy - (radius * sin((i * 3.14) / 180));
        }

    }



//==============================================================================
// http://see-programming.blogspot.com/2013/09/c-program-to-implement-analog-clock.html
/*
   * Calculates position for minute and second needle movement
   * Each quadrant has 90 degrees.  So, we need to split each
   * quadrant into 15 parts(6 degree each) to get the minute
   * and second needle movement
*/
void minSecCalc(int radius, int midx, int midy, int secx[60], int secy[60])
    {
    //int i, j = 0, x, y;
    int i, j = 0;
    //char str[32];

    /* 15 position(min/sec - 12 to 3) in first quadrant of clock  */
    secx[j] = midx, secy[j++] = midy - radius;

    for (i = 96; i < 180; i = i + 6)
        {
        secx[j] = midx - (radius * cos((i * 3.14) / 180));
        secy[j++] = midy - (radius * sin((i * 3.14) / 180));
        }

    /* 15 positions(min or sec - 3 to 6) in second quadrant of clock */
    secx[j] = midx + radius, secy[j++] = midy;
    for (i = 186; i < 270; i = i + 6)
        {
        secx[j] = midx - (radius * cos((i * 3.14) / 180));
        secy[j++] = midy - (radius * sin((i * 3.14) / 180));
        }

    /* 15 positions(min or sec - 6 to 9) in third quadrant of clock */
    secx[j] = midx, secy[j++] = midy + radius;
    for (i = 276; i < 360; i = i + 6)
        {
        secx[j] = midx - (radius * cos((i * 3.14) / 180));
        secy[j++] = midy - (radius * sin((i * 3.14) / 180));
        }

    /* 15 positions(min or sec - 9 to 12) in fourth quadrant of clock */
    secx[j] = midx - radius, secy[j++] = midy;
    for (i = 6; i < 90; i = i + 6)
        {
        secx[j] = midx - (radius * cos((i * 3.14) / 180));
        secy[j++] = midy - (radius * sin((i * 3.14) / 180));
        }

    return;
    }

/*
   * find the points at 0, 30, 60,.., 360 degrees
   * on the given circle.  x value corresponds to
   * radius * cos(angle) and y value corresponds
   * to radius * sin(angle).  Numbers in the clock
   * are written using the above manipulated x and
   * y values.  And the hour needle movement
   * is based on this
*/

void calcPoints(int radius, int midx, int midy, int x[12], int y[12])
    {
    int x1, y1;

    /* 90, 270, 0, 180 degrees */
    x[0] = midx, y[0] = midy - radius;
    x[6] = midx, y[6] = midy + radius;
    x[3] = midx + radius, y[3] = midy;
    x[9] = midx - radius, y[9] = midy;

    /* 30, 150, 210, 330 degrees */
    x1 = (int) ((radius / 2) * sqrt(3));
    y1 = (radius / 2);
    x[2] = midx + x1, y[2] = midy - y1;
    x[4] = midx + x1, y[4] = midy + y1;
    x[8] = midx - x1, y[8] = midy + y1;
    x[10] = midx - x1, y[10] = midy - y1;

    /* 60, 120, 210, 300 degrees */
    x1 = radius / 2;
    y1 = (int) ((radius / 2)  * sqrt(3));
    x[1] = midx + x1, y[1] = midy - y1;
    x[5] = midx + x1, y[5] = midy + y1;
    x[7] = midx - x1, y[7] = midy + y1;
    x[11] = midx - x1, y[11] = midy - y1;

    return;
    }
//==============================================================================


// Returns seconds per 1 second.
// per 1 sec in this case also equals velocity as m/s
// does this also equate to a % ? of 1 sec?
double GetTimeDilation(double v)  //double velocity, double distance)
    {

    // t'=t/sqrt(1-v^2/C^2)
    // the t/sqrt is omitted as I am using 1 second intervals for all calculations.
    //double v = 299792458 * 0.99;// 299792458 * 0.99;  // 100% C m/s

    //double distance = 17987547480 / 60.0;  // 1 sec
    //double distance = 17987547480 / 3600.0;  // 1/60th sec
    double C = 299792458; // m/s Speed of light C
    //double result1, result2, result3,
    double result4;//, result5;

    // The following is altered to keep the calculations withing the limits of
    // double. Both equate to the same values.
    //result4 = sqrt(1 - (v * v) / (C * C));  // stationary observer.
    result4 = sqrt(1 - (v / C) * (v / C));  // stationary observer.

    //This moves the time dilation forward rather than behind
    //result5 = 1 / result4;  // The moving object/observer.

    // The result is in meters per second. This means that the results
    // can be used as meters per 1 second when converting to a smaller radius.
    return result4;
    }


//==============================================================================
