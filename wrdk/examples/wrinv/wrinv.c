/** WikiReader Invaders.  Strangely familiar.

    Michael Hope 2010
*/
#include <grifo.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int uintf;

#define FALSE 0
#define TRUE  1

/** Basic image type */
typedef struct
{
    uint width;
    uint height;

    const uint8_t *mask;
    uint8_t bytes[];
} Image;

/** A sprite such as an invader, player, or shot. */
typedef struct
{
    int x;
    int y;
    /** The row the invader belongs to */
    int row;
    bool active;
    bool explode;
} Sprite;

/** A rectangle.  Used in collision detection. */
typedef struct
{
    int top;
    int right;
    int bottom;
    int left;
} Rectangle;

/** Current game state */
typedef enum { Playing, GameOver } State;

/** All of the global values */
struct
{
    /** All invaders */
    Sprite invaders[11*5];
    /** The player */
    Sprite player;
    /** The player's one and only shot */
    Sprite shot;

    /** Current animation frame */
    int frame;
    /** Next invader to update */
    int at;
    int dx;
    int dy;
    /** Set if the invaders are currently moving down */
    bool down;
    
    /** Bitwise OR of the buttons that are down */
    int buttons;
    /** Game state */
    State state;
    /** Player score */
    int score;
} q;

static uint8_t fb[LCD_BUFFER_WIDTH_BYTES*LCD_HEIGHT];

#define LCD_ImageType static const Image

#include "build/background.img"
#include "build/special.img"
#include "build/top1.img"
#include "build/top2.img"
#include "build/mid1.img"
#include "build/mid2.img"
#include "build/bottom1.img"
#include "build/bottom2.img"
#include "build/player.img"
#include "build/shot.img"
#include "build/explode.img"
#include "build/gameover.img"

/** The invader images where the columns are the animation frame and
    the rows match the invader rows
 */
static const Image* images[5][2] =
{
    { &top1_image, &top2_image },
    { &mid1_image, &mid2_image },
    { &mid1_image, &mid2_image },
    { &bottom1_image, &bottom2_image },
    { &bottom1_image, &bottom2_image },
};

/** Points for the invaders at each row */
static const int points[] =
{
    30,
    20,
    20,
    10,
    10
};

static void run(void);

/** Main must be first in the file */
int main(void)
{
    run();
    return 0;
}

/** Unaligned blit */
static void blit1(void *framebuffer, size_t stride,
           int x, int y, size_t width, size_t height,
           bool reverse,
           const uint8_t *bits, const uint8_t *mask)
{
    uint8_t *p = (uint8_t *)framebuffer + (x >> 3) + (stride * y);
    size_t h;
    int shift = x % 8;

    width >>= 3;

    int cols = width + 1;
            
    for (h = 0; h < height; ++h) {
        size_t w;

        for (w = 0; w < cols; ++w) {
            if (w == 0)
            {
                uint8_t m = (0xFF00 >> shift) | (mask[w] >> shift);
                uint8_t img = bits[w] >> shift;
                p[w] = (p[w] & m) | img;
            }
            else if (w == cols - 1)
            {
                uint8_t m = (mask[w-1] << (8 - shift)) | (0xFF >> shift);
                uint8_t img = bits[w-1] << (8 - shift);
                p[w] = (p[w] & m) | img;
            }
            else
            {
                uint8_t m = (mask[w-1] << (8 - shift)) | (mask[w] >> shift);
                uint8_t img = (bits[w-1] << (8 - shift)) | (bits[w] >> shift);
                p[w] = (p[w] & m) | img;
            }
        }
        p += stride;
        bits += width;
        mask += width;
    }
}

/** Aligned blit */
static void blit2(void *framebuffer, size_t stride,
           int x, int y, size_t width, size_t height,
           bool reverse,
           const uint8_t *bits, const uint8_t *mask)
{
    uint8_t *p = (uint8_t *)framebuffer + (x >> 3) + (stride * y);
    size_t h;

    width >>= 3;

    int cols = width;
            
    for (h = 0; h < height; ++h) {
        size_t w;

        for (w = 0; w < cols; ++w) {
            p[w] = (p[w] & mask[w]) | bits[w];
        }

        p += stride;
        bits += width;
        mask += width;
    }
}

/** Blit an image onto the screen at the given position.  Hands off to
    the aligned or unaligned blit.
 */
static void blit(const Image* image, uintf x, uintf y)
{
    if ((x % 8) == 0)
    {
        blit2(fb, LCD_BUFFER_WIDTH_BYTES,
              x, y,
              (image->width + 7) & ~7,
              image->height,
              FALSE,
              image->bytes,
              image->mask);
    }
    else
    {
        blit1(fb, LCD_BUFFER_WIDTH_BYTES,
              x, y,
              (image->width + 7) & ~7,
              image->height,
              FALSE,
              image->bytes,
              image->mask);
    }
}

/** Flip between the different frame buffers */
static void flip(void)
{
    /* We pretend here.  Render into an off-screen buffer then
       memcpy() into the real framebuffer
    */
    uint32_t *pdest = lcd_get_framebuffer();
    uint32_t *psrc = fb;
    uint32_t *pend = fb + sizeof(fb);

    while (psrc != pend)
    {
        *pdest++ = *psrc++;
        *pdest++ = *psrc++;
        *pdest++ = *psrc++;
        *pdest++ = *psrc++;
    }
}

/** Initialise the game */
static void init(void)
{
    q.state = GameOver;
    q.score = 0;

    q.frame = 0;
    q.at = 0;
    q.dx = 5;
    q.dy = 5;
    q.down = 0;

    /* Reset the player */
    q.player.x = 0;
    q.player.y = LCD_HEIGHT - 10 - player_image.height;

    /* Place all of the invaders */
    for (int row = 0; row < 5; row++)
    {
        for (int col = 0; col < 11; col++)
        {
            int id = row*11 + col;
            int x = col*18 + 5;
            int y = row*20 + 10;
            Sprite *invader = q.invaders + id;

            invader->x = x;
            invader->y = y;
            invader->row = row;
            invader->active = 1;
            invader->explode = 0;
        }
    }
}

/** Draw the scene */
static void draw(void)
{
    /* Draws the whole scene every time.  Inefficient but fast enough */

    /* Background image */
    blit(&background_image, 0, 0);

    /* Score */
    lcd_set_framebuffer(fb);
    lcd_at_xy(0, 0);
    lcd_printf("Score %u", q.score);
    lcd_set_default_framebuffer();

    /* All of the invaders */
    for (int i = 0; i < SizeOfArray(q.invaders); i++)
    {
        Sprite *invader = q.invaders + i;

        if (invader->active)
        {
            if (invader->explode)
            {
                blit(&explode_image, invader->x, invader->y);
            }
            else
            {
                blit(images[invader->row][q.frame], invader->x, invader->y);
            }
        }
    }

    /* The player */
    blit(&player_image, q.player.x, q.player.y);

    /* The shot, if any */
    if (q.shot.active)
    {
        blit(&shot_image, q.shot.x, q.shot.y);
    }

    /* The game over text */
    if (q.state == GameOver)
    {
        blit(&gameover_image, (LCD_WIDTH - gameover_image.width)/2, 30);
    }
}

static int min(int a, int b)
{
    return a < b ? a : b;
}

static int max(int a, int b)
{
    return a > b ? a : b;
}

/** Get the bounding rectangle of one invader */
static void get_bounds2(Sprite *invader, Rectangle *bounds)
{
    bounds->top = invader->y;
    bounds->right = invader->x + mid1_image.width;
    bounds->bottom = invader->y + mid1_image.height;
    bounds->left = invader->x;
}

/** Get the bounds of all invaders */
static void get_bounds(Rectangle *bounds)
{
    bounds->top = LCD_HEIGHT;
    bounds->right = 0;
    bounds->bottom = 0;
    bounds->left = LCD_WIDTH;

    for (int i = 0; i < SizeOfArray(q.invaders); i++)
    {
        Sprite *invader = q.invaders + i;

        if (invader->active)
        {
            Rectangle inner;

            get_bounds2(invader, &inner);

            bounds->top = min(inner.top, bounds->top);
            bounds->right = max(inner.right, bounds->right);
            bounds->bottom = max(inner.bottom, bounds->bottom);
            bounds->left = min(inner.left, bounds->left);
        }
    }
}

/** Updates when we hit the last invader */
static void update_on_last(void)
{
    if (q.down)
    {
        /* Finished moving down */
        q.down = 0;
    }
    else
    {
        /* Switch between the two frames */
        if (++q.frame == 2)
        {
            q.frame = 0;
        }

        /* See if we've hit the edge and change direction if we did */
        Rectangle bounds;

        get_bounds(&bounds);
            
        if (q.dx < 0 && bounds.left < 5)
        {
            q.dx = -q.dx;
            q.down = 1;
        }
        else if (q.dx > 0 && bounds.right > 240 - 5)
        {
            q.dx = -q.dx;
            q.down = 1;
        }

        /* Has the lowest invader landed? */
        if (bounds.bottom >= LCD_HEIGHT - 30)
        {
            q.state = GameOver;
        }
    }
}

/** Update the single shot */
static void update_shot(void)
{
    int any = 0;

    /* Move it on up */
    q.shot.y -= 2;

    /* Collied with any invaders? */
    for (int i = 0; i < SizeOfArray(q.invaders); i++)
    {
        Sprite *invader = q.invaders + i;

        if (invader->active)
        {
            Rectangle bounds;
            get_bounds2(invader, &bounds);

            if (q.shot.y > bounds.bottom || q.shot.y < bounds.top)
            {
                /* Too high or low */
            }
            else if (q.shot.x < bounds.left - shot_image.width || q.shot.x > bounds.right)
            {
                /* Too far left or right */
            }
            else
            {
                /* A hit! */
                invader->explode = 1;
                q.score += points[invader->row];
                q.shot.active = 0;
            }
        }

        if (invader->active && !invader->explode)
        {
            any = TRUE;
        }
    }

    if (!any)
    {
        /* No invaders left */
        q.state = GameOver;
    }

    if (q.shot.y < 5)
    {
        /* Went off the top of the screen */
        q.shot.active = 0;
    }
}

/** Update all non-player sprites */
static void update(void)
{
    if (q.state != Playing)
    {
        return;
    }

    while (1)
    {
        /* We only update one active invader at one time.  While
           terribly inefficient, this emulates the look of the
           original where the invaders shimmy to the side one at a
           time and slide down one at a time
        */
        Sprite *invader = q.invaders + q.at;

        if (invader->explode)
        {
            invader->active = 0;
        }
        else
        {
            if (q.down)
            {
                invader->y += q.dy;
            }
            else
            {
                invader->x += q.dx;
            }
        }

        if (++q.at == SizeOfArray(q.invaders))
        {
            q.at = 0;
            update_on_last();
        }

        if (invader->active)
        {
            break;
        }
    }

    if (q.shot.active)
    {
        update_shot();
    }
}

/** Check for buttons and move the player */
static void move(void)
{
    event_t event;
    int button = -1;

    switch (event_get(&event))
    {
    case EVENT_BUTTON_DOWN:
        q.buttons |= 1 << event.button.code;
        button = event.button.code;
        break;

    case EVENT_BUTTON_UP:
        q.buttons &= ~(1 << event.button.code);
        break;

    default:
        break;
    }

    if (q.state == Playing)
    {
        if ((q.buttons & (1 << BUTTON_SEARCH)) != 0)
        {
            /* Move left */
            q.player.x -= 1;
            q.player.x = max(0, q.player.x);
        }

        if ((q.buttons & (1 << BUTTON_HISTORY)) != 0)
        {
            /* Move right */
            q.player.x += 1;
            q.player.x = min(LCD_WIDTH - player_image.width, q.player.x);
        }

        if ((q.buttons & (1 << BUTTON_RANDOM)) != 0)
        {
            /* Fire! */
            if (!q.shot.active)
            {
                q.shot.x = q.player.x + (player_image.width - shot_image.width)/2;
                q.shot.y = q.player.y - shot_image.height;
                q.shot.active = 1;
            }
        }
    }
    else
    {
        if (button == BUTTON_RANDOM)
        {
            /* Start the game */
            init();
            q.state = Playing;
        }
    }
}

/** Limit to a certain number of frames per second */
static void fps(int count)
{
    static uint32_t last;
    uint32_t now = timer_get();

    int32_t delta = now - last;
    int32_t want = TIMER_CountsPerMicroSecond * 1000000 / count;
    int32_t remain = want - delta;

    if (last == 0 || remain <= 0)
    {
        /* Can't keep up */
        last = now;
    }
    else
    {
        last += remain;
        delay_us(remain / TIMER_CountsPerMicroSecond);
    }
}

static void run(void)
{
#ifdef WRDK_HOST
    grifo_init();
#endif

    init();

    for (;;)
    {
        draw();
        flip();

        update();
        move();

        fps(30);

        watchdog(WATCHDOG_KEY);
    }
}
