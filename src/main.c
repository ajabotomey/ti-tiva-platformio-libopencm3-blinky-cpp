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

// The divisors that are looped through when SW2 is pressed
enum {
    PLL_DIV_80MHZ   = 5,
    PLL_DIV_57MHZ   = 7,
    PLL_DIV_40MHZ   = 10,
    PLL_DIV_20MHZ   = 20,
    PLL_DIV_16MHZ   = 25,
};

static const uint8_t plldiv[] = {
    PLL_DIV_80MHZ,
    PLL_DIV_57MHZ,
    PLL_DIV_40MHZ,
    PLL_DIV_20MHZ,
    PLL_DIV_16MHZ,
    0
};

// The PLL Divisor that we are currently on
static size_t ipll = 0;

// Are we bypassing the PLL?
static bool bypass = false;

/* 
    Clock Setup
    Main Crystal Oscillator runs at 16MHz, we run it through the PLL and divide the 400MHz clock to get 80MHz
*/
static void clock_setup()
{
    rcc_sysclk_config(OSCSRC_MOSC, XTAL_16M, PLL_DIV_80MHZ);
}

// GPIO Setup
static void gpio_setup()
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

// IRQ setup. If a button is pressed, trigger an interrupt
static void irq_setup()
{
    const uint32_t btnpins = USR_SW1 | USR_SW2;
    
    // Trigger interrupt on rising-edge (button release)
    gpio_configure_trigger(GPIOF, GPIO_TRIG_EDGE_RISE, btnpins);
    // Enable interrupt
    gpio_enable_interrupts(GPIOF, btnpins);
    // Enable interrupt with NVIC as well
    nvic_enable_irq(NVIC_GPIOF_IRQ);
}

#define FLASH_DELAY 800000

static void delay()
{
    int i;
    for (i = 0; i < FLASH_DELAY; i++)
        __asm__("nop");
}

int main()
{
    gpio_enable_ahb_aperture();
    clock_setup();
    gpio_setup();
    irq_setup();

    // Blink each color of the RGB LED in order
    while (1)
    {
        // Flash red
        gpio_set(RGB_PORT, LED_R);
        delay();
        gpio_clear(RGB_PORT, LED_R);
        delay();

        // Flash green
        gpio_set(RGB_PORT, LED_G);
        delay();
        gpio_clear(RGB_PORT, LED_G);
        delay();

        // Flash red
        gpio_set(RGB_PORT, LED_B);
        delay();
        gpio_clear(RGB_PORT, LED_B);
        delay();
    }

    return 0;
}

void gpiof_isr()
{
    if (gpio_is_interrupt_source(GPIOF, USR_SW1))
    {
        bypass = !bypass;
        if (bypass)
        {
            rcc_pll_bypass_enable();

            // Divisor is still applied to raw clock. Disable divisor or raw clock is divided
            SYSCTL_RCC &= ~SYSCTL_RCC_USESYSDIV;
        }
        else 
        {
            rcc_change_pll_divisor(plldiv[ipll]);
        }

        // Clear interrupt source
        gpio_clear_interrupt_flag(GPIOF, USR_SW1);
    }

    if (gpio_is_interrupt_source(GPIOF, USR_SW2))
    {
        if (!bypass)
        {
            if (plldiv[++ipll] == 0)
                ipll = 0;
            
            rcc_change_pll_divisor(plldiv[ipll]);
        }

        gpio_clear_interrupt_flag(GPIOF, USR_SW2);
    }
}