#include <libopencm3/cm3/nvic.h>
#include <libopencm3/lm4f/systemcontrol.h>
#include <libopencm3/lm4f/rcc.h>
#include <libopencm3/lm4f/gpio.h>
#include <libopencm3/lm4f/nvic.h>

#include <stdbool.h>
#include <stdio.h>

#define RGB_PORT GPIOF

// RGB Connection on Launchpad
enum {
    LED_R = GPIO1,
    LED_G = GPIO3,
    LED_B = GPIO2,
};

// Connect the User Switches
enum {
    USR_SW1 = GPIO4,
    USR_SW2 = GPIO0,
};