#include "pico_display.hpp"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <string>
using namespace pimoroni;

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay picoDisplay(buffer);

uint8_t timeBetweenColourUpdates = 5;
const uint16_t millisecondsInSeconds = 1000;

uint8_t red = random() % 255;
uint8_t green = random() % 255;
uint8_t blue = random() % 255;

bool bPressed = false;
bool yPressed = false;

uint32_t counter = 0;
uint8_t buttonTimeout = 0;
uint8_t buttonTimeoutOnPress = 1;

void count()
{
    while(true)
    {
        if(counter < UINT32_MAX)
        {
            counter ++;
            //printf("Pushing: ");
            //printf(std::to_string(counter).c_str());
            //printf("\n");

            //multicore_fifo_push_blocking(counter);
            //multicore_fifo_drain();
        }

        if(buttonTimeout > 0)
        {
            buttonTimeout --;
        }

        sleep_ms(millisecondsInSeconds);
    }
}

void setTextAndLed()
{
    // Randomise pen colour
    red = random() % 255;
    green = random() % 255;
    blue = random() % 255;
    picoDisplay.set_pen(red, green, blue);

    // Write text to display
    picoDisplay.text("Hello World", Point(PicoDisplay::WIDTH / 100 * 30, PicoDisplay::HEIGHT / 2), PicoDisplay::WIDTH);
    picoDisplay.update();

    // Set RGB led to match pen colour
    picoDisplay.set_led(red, green, blue);
}

int main() {
    stdio_init_all();
    // Launch counter thread
    multicore_launch_core1(count);

    // display init
    picoDisplay.init();
    picoDisplay.set_backlight(100);
    setTextAndLed();

    //add_repeating_timer_us()

    while(true) {

        /*if(multicore_fifo_rvalid())
        {
            counter = (uint32_t)multicore_fifo_pop_blocking();
            
            printf("Got: ");
            printf(std::to_string(counter).c_str());
            printf("\n");
        }*/

        // Check if buttons have been pressed and adjust time between colour change
        if(picoDisplay.is_pressed(picoDisplay.B) && buttonTimeout == 0) 
        {
            if(timeBetweenColourUpdates >= 1)
            {
                timeBetweenColourUpdates --;
            }

            printf("Time between updates: ");
            printf(std::to_string(timeBetweenColourUpdates).c_str());
            printf("\n");

            buttonTimeout = buttonTimeoutOnPress;

        } else if(picoDisplay.is_pressed(picoDisplay.Y) && buttonTimeout == 0)
        {
            if(timeBetweenColourUpdates < UINT8_MAX)
            {
                timeBetweenColourUpdates ++;
            }
            printf("Time between updates: ");
            printf(std::to_string(timeBetweenColourUpdates).c_str());
            printf("\n");

            buttonTimeout = buttonTimeoutOnPress;

        }

        if(counter >= timeBetweenColourUpdates) 
        {
            setTextAndLed();
            counter = 0;
        }
        
        // sleep_ms(sleep_time_seconds * millisecondsInSeconds);
    }
}