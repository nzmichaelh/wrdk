/** WikiReader specific startup and port code

    @author Michael Hope
*/
#include "loader.h"
#include <grifo.h>
#include <regs.h>
#include <samo.h>

/** Represents a bitmap image */
struct Image
{
    uint width;
    uint height;
    uint8_t bytes[];
};

/* Pull in the bitmaps */
#define LCD_ImageType static const struct Image
#include "build/icon.xpm.h"
#include "build/dot.xpm.h"
#include "build/filled.xpm.h"

/** X positions of the progress dots */
static const uint dot_x[] =
{
    LCD_WIDTH / 2 - 5*8,
    LCD_WIDTH / 2 - 2*8,
    LCD_WIDTH / 2 + 1*8,
    LCD_WIDTH / 2 + 4*8
};

/** Local data */
static struct
{
    /** Current step.  Sets the highlighted dot. */
    int step;
    /** Time the dots were last animated */
    uint32_t last;
} q;

static void go(void);

/** Entry point.  Must be first in the file.  Hand off to go().
 */
int main(void)
{
    go();
    return 0;
}

/** Draw the progress dots with the given index highlighted */
static void draw_dots(int idx)
{
    int x;

    for (x = 0; x < SizeOfArray(dot_x); x++)
    {
        lcd_bitmap(lcd_get_framebuffer(),
                   LCD_BUFFER_WIDTH_BYTES,
                   dot_x[x],
                   (LCD_HEIGHT - icon_image.height) / 2 + icon_image.height + 4,
                   dot_image.width, dot_image.height,
                   0,
                   x == idx ? filled_image.bytes : dot_image.bytes);
    }
}

/** Main entry point.  Set up the serial port, draw the screen, and
    run the loader.
*/
static void go(void)
{
    /* Jump up to 115200 baud */
    SET_BRTRD(0, CALC_BAUD(PLL_CLK, 1, SERIAL_DIVMD, 115200));


    lcd_clear(LCD_WHITE);

    lcd_bitmap(lcd_get_framebuffer(), LCD_BUFFER_WIDTH_BYTES,
               (LCD_WIDTH - icon_image.width) / 2,
               (LCD_HEIGHT - icon_image.height) / 2 - 8,
               icon_image.width, icon_image.height,
               0,
               icon_image.bytes);

    /* Draw the dots with none highlighted */
    draw_dots(-1);
    q.step = 0;

    port_putch('!');
    loader();
}

/** Tick along the progress dots now and again */
void port_progress(void)
{
    uint32_t now = timer_get();
    int32_t elapsed = now - q.last;

    if (elapsed < 0 || elapsed > 500000*TIMER_CountsPerMicroSecond)
    {
        q.last = now;
        draw_dots(q.step);
        q.step++;

        if (q.step >= SizeOfArray(dot_x))
        {
            q.step = 0;
        }
    }
}

void port_set_baud(uint div)
{
    REG_EFSIF0_BRTRUN = 0;
    REG_EFSIF0_BRTRDM = div >> 8;
    REG_EFSIF0_BRTRDL = div & 0xff;
    REG_EFSIF0_BRTRUN = BRTRUNx;
}

uint port_getch(void)
{
    event_t event;
    event_get(&event);
    watchdog(WATCHDOG_KEY);

    return serial_getchar();
}

void port_putch(uint ch)
{
    serial_putchar(ch);
}
