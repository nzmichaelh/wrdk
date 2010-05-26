/** A very simple splash screen.
    Draws an image on the screen and waits for a second.

    Only here as init.app needs something to auto-boot.
*/

#include <grifo.h>

typedef unsigned int uintf;

/** Basic image type */
typedef struct
{
    uint width;
    uint height;

    uint8_t bytes[];
} Image;

#define LCD_ImageType static const Image

#include "build/splash.xpm.h"

/** Main must be first in the file */
int main(void)
{
    lcd_clear(LCD_WHITE);

    lcd_bitmap(lcd_get_framebuffer(),
               LCD_BUFFER_WIDTH_BYTES,
               (LCD_WIDTH - splash_image.width)/2,
               (LCD_HEIGHT - splash_image.height)/2,
               splash_image.width,
               splash_image.height,
               0,
               splash_image.bytes);

    delay_us(500000);

    return 0;
}
