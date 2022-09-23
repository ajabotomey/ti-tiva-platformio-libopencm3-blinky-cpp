#include "main.h"
#include <unity.h>

// The divisors that are looped through when SW2 is pressed
enum {
    PLL_DIV_80MHZ   = 5,
    PLL_DIV_57MHZ   = 7,
    PLL_DIV_40MHZ   = 10,
    PLL_DIV_20MHZ   = 20,
    PLL_DIV_16MHZ   = 25,
};

// The PLL Divisor that we are currently on
static size_t ipll = 0;

// Are we bypassing the PLL?
static bool bypass = false;

/* 
    Clock Setup
    Main Crystal Oscillator runs at 16MHz, we run it through the PLL and divide the 400MHz clock to get 80MHz
*/
static void clock_setup(void)
{
    rcc_sysclk_config(OSCSRC_MOSC, XTAL_16M, PLL_DIV_80MHZ);
}

// GPIO Setup
static void gpio_setup(void)
{
    // Configure GPIOF
    periph_clock_enable(RCC_GPIOF);
    const uint32_t outpins = (LED_R | LED_G | LED_B);

    gpio_mode_setup(RGB_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, outpins);
    gpio_set_output_config(RGB_PORT, GPIO_OTYPE_PP, GPIO_DRIVE_2MA, outpins);

    // Button setup
    const uint32_t btnpins = USR_SW1 | USR_SW2;

    // PF0 is locked by default, we need to unlock before being able to use it
    gpio_unlock_commit(GPIOF, USR_SW2);
    gpio_mode_setup(GPIOF, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, btnpins);
}

#define FLASH_DELAY 800000
#define HARDWARE_DELAY 2000000

static void delay(int delay)
{
    int i;
    for (i = 0; i < delay; i++)
        __asm__("nop");
}

void setUp(void)
{
    gpio_enable_ahb_aperture();
    clock_setup();
    gpio_setup();
}

void tearDown(void)
{
    
}

void test_led_turn_red(void)
{
    // Flash red
    gpio_set(RGB_PORT, LED_R);
    delay(FLASH_DELAY);
    TEST_ASSERT_EQUAL(0xff, gpio_read(RGB_PORT, LED_R));
    gpio_clear(RGB_PORT, LED_R);
    delay(FLASH_DELAY);
}

void test_led_turn_green(void)
{
    // Flash green
    gpio_set(RGB_PORT, LED_G);
    delay(FLASH_DELAY);
    TEST_ASSERT_EQUAL(0xff, gpio_read(RGB_PORT, LED_G));
    gpio_clear(RGB_PORT, LED_G);
    delay(FLASH_DELAY);
}

void test_led_turn_blue(void)
{
    // Flash blue
    gpio_set(RGB_PORT, LED_B);
    delay(FLASH_DELAY);
    TEST_ASSERT_EQUAL(0xff, gpio_read(RGB_PORT, LED_B));
    gpio_clear(RGB_PORT, LED_B);
    delay(FLASH_DELAY);
}

int main()
{
    delay(HARDWARE_DELAY);

    UNITY_BEGIN();

    RUN_TEST(test_led_turn_red);
    RUN_TEST(test_led_turn_green);
    RUN_TEST(test_led_turn_blue);

    UNITY_END();
}