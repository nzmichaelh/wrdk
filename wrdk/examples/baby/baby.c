/** Bouncing Babies:  A simple 'save the baby from the burning' game.

    Based on the the 1984 DOS game by Dave Baskin.
*/
#include <grifo.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int uintf;

#define FALSE  0
#define TRUE   1

/** Chance of a flame going out and being replaced */
#define FLAME_MAX_AGE        10
/** Longest time between a flame changing state */
#define FLAME_MAX_FLICKER   700
/** Shortest time between a flame changing state */
#define FLAME_MIN_FLICKER   200

/** Time until we spin the baby onto the next frame */
#define BABY_SPIN_TIME      200
/** How quickly the baby accelerates */
#define BABY_ACCELRATION      5
/** Chance of a new baby appearing */
#define BABY_CHANCE         100

/** Basic time stemp */
#define DT               100

/** Main font */
static const uint8_t font[][8] =
{
#include "lat16.h"
};

/** A single image */
typedef struct
{
    uint width;
    uint height;
    const uint8_t* mask;

    uint8_t bytes[];
} Image;

/** A sprite.  Mainly used for the babies. */
typedef struct
{
    int x;
    int y;

    const Image* image;
} Sprite;

/** A single flame */
typedef struct
{
    Sprite sprite;
    bool on;
    int until;
} Flame;

/** A single baby */
typedef struct
{
    Sprite sprite;
    /** Current frame in the animation */
    uint frame;
    /** How long until the baby animates */
    int until;
    /** X velocity */
    int vx;
    /** Y velocity */
    int vy;
    bool running;
} Baby;

/** The overall game state */
typedef enum
{
    Title,
    Playing,
    GameOver
} State;


#define LCD_ImageType static const Image

#include "build/flame.xpm.h"
#include "build/building.xpm.h"
#include "build/baby.xpm.h"
#include "build/player.xpm.h"
#include "build/ambulance.xpm.h"
#include "build/title.xpm.h"
#include "build/gameover.xpm.h"
#include "build/baby0.xpm.h"
#include "build/baby90.xpm.h"
#include "build/baby180.xpm.h"
#include "build/baby270.xpm.h"

/** The different images used in the baby animation */
static const Image* baby_frames[] =
{
    &baby0_image,
    &baby90_image,
    &baby180_image,
    &baby270_image,
};

/** The player sprite */
Sprite player =
{
    .image = &player_image,
};

/** All global variables */
static struct
{
    Baby babies[5];
    Flame flames[5];

    State state;

    uintf score;
    uintf highscore;
    uintf lives;
    uintf phase;
    uintf spawned;

    uint seed;
} q;

static void run(void);

/** Main entry point.  Must be the first function in the file. */
int main(void)
{
    run();

    return 0;
}

/** Make a new random number */
static uint random(void)
{
    uint k;

    if (q.seed == 0)
        q.seed = 0x12345987;
    k = q.seed / 127773;
    q.seed = 16807 * (q.seed - k * 127773) - 2836 * k;
    if (q.seed < 0)
        q.seed += 2147483647;

    return q.seed & RAND_MAX;
}

/** Put a character on the screen */
static void putch(uintf x, uintf y, char ch)
{
    lcd_bitmap(lcd_get_framebuffer(), LCD_BUFFER_WIDTH_BYTES,
                 x, y, 8, 8, FALSE,
                 font[ch - ' ']);
}

/** Write a string to the screen */
static void print(int x, int y, const char* pmsg)
{
    if (x < 0)
    {
        x = -x + (LCD_WIDTH - strlen(pmsg) * 8)/2;
    }

    for (; *pmsg != '\0'; pmsg++)
    {
        putch(x, y, *pmsg);
        x += 8;
    }
}

/** Simple blit function that copies an image onto the screen */
static void blit(const Image* image, uintf x, uintf y)
{
    lcd_bitmap(lcd_get_framebuffer(), LCD_BUFFER_WIDTH_BYTES,
               x, y,
               (image->width + 7) & ~7,
               image->height,
               FALSE,
               image->bytes);
}

/** Main image drawing routine.  XORs the image onto the screen which
    allows us to draw without a mask and also, through a double XOR,
    easily erase.
*/
static void xor(const Image* image, uintf x, uintf y)
{
    uint8_t* framebuffer = lcd_get_framebuffer();
    uint8_t* p = framebuffer + (x >> 3) + (LCD_BUFFER_WIDTH_BYTES * y);
    uint8_t* bits = image->bytes;

    uintf width = image->width;
    uintf height = image->height;
    uintf shift = x % 8;

    size_t h;

    width += 7;
    width >>= 3;

    uintf cols = (shift == 0) ? width : width + 1;

    for (h = 0; h < height; ++h)
    {
        size_t w;

        for (w = 0; w < cols; ++w)
        {
            if (w == 0)
            {
                p[w] ^= bits[w] >> shift;
            }
            else if (w == cols - 1)
            {
                p[w] ^= bits[w-1] << (8 - shift);
            }
            else
            {
                p[w] ^= (bits[w-1] << (8 - shift)) | (bits[w] >> shift);
            }
        }

        p += LCD_BUFFER_WIDTH_BYTES;
        bits += width;
    }
}

/** Draw the main background */
static void draw_background(void)
{
    lcd_clear(LCD_WHITE);

    blit(&building_image,
         0,
         LCD_HEIGHT - building_image.height - 2);

    blit(&ambulance_image,
         LCD_WIDTH - ambulance_image.width,
         LCD_HEIGHT - ambulance_image.height - 2);
}

/** Draw the title screen */
static void draw_title(void)
{
    int y = LCD_HEIGHT * 2 / 10;

    blit(&title_image,
         10 + (LCD_WIDTH - title_image.width) / 2,
         y);

    y += title_image.height + 3;
    print(-10, y, "by");
    y += 8 + 3;
    print(-10, y, "Michael Hope");
    y += 8 + 3;
    print(-10, y, "based on the version by");
    y += 8 + 3;
    print(-10, y, "Dave Baskin");
    y += 8 + 3;
}

/** Draw the score and other state */
static void draw_score(void)
{
    char buffer[50];
    sprintf(buffer, "Phase %u Left %u Score %u Hi %u   ",
            q.phase, q.lives, q.score, q.highscore);
    print(0, 8, buffer);
}

/** Decide on how long until the flame changes state */
static void new_age(Flame* flame)
{
    flame->until = FLAME_MIN_FLICKER + random() % FLAME_MAX_FLICKER;
}

/** Create a new flame and draw it */
static void new_flame(Flame* flame)
{
    flame->sprite.image = &flame_image;
    flame->sprite.x = random() % 16;
    flame->sprite.y = LCD_HEIGHT - building_image.height 
        + random() % (building_image.height - 2*flame_image.height);
    flame->on = TRUE;

    xor(flame->sprite.image, flame->sprite.x, flame->sprite.y);
}

/** Check an individual flame and update or replace it if required */
static void tick_flame(Flame* flame)
{
    if (flame->until > 0)
    {
        flame->until -= DT;
    }

    if (flame->until <= 0)
    {
        flame->on = !flame->on;
        new_age(flame);

        xor(flame->sprite.image, flame->sprite.x, flame->sprite.y);

        if (!flame->on)
        {
            if ((random() % FLAME_MAX_AGE) == 0)
            {
                new_flame(flame);
            }
        }
    }
}

static void go_game_over(void);

/** Create a new baby and draw it */
static void new_baby(Baby* baby)
{
    baby->sprite.x = building_image.width;
    baby->sprite.y = LCD_HEIGHT - building_image.height + 10;
    baby->running = TRUE;
    baby->vx = 5;
    baby->vy = 0;

    baby->frame = 0;
    baby->until = BABY_SPIN_TIME;
    xor(baby_frames[baby->frame], baby->sprite.x, baby->sprite.y);
}

/** Update an individual baby by animating it, moving it, and bouncing
    it as required
 */
static void tick_baby(Baby* baby)
{
    if (!baby->running)
    {
        return;
    }

    if (baby->until > 0)
    {
        baby->until -= DT;
    }

    if (baby->until <= 0)
    {
        /* Move on a frame */
        baby->until = BABY_SPIN_TIME;
        xor(baby_frames[baby->frame], baby->sprite.x, baby->sprite.y);
        baby->frame++;

        if (baby->frame == SizeOfArray(baby_frames))
        {
            baby->frame = 0;
        }

        /* Move the baby */
        baby->sprite.x += baby->vx;
        baby->vy += BABY_ACCELRATION;
        baby->sprite.y += baby->vy;

        /* See if we've hit bottom */
        int platform = LCD_HEIGHT - player_image.height - 2 + 10 - baby_image.height;

        if (baby->vy > 0 && baby->sprite.y > platform)
        {
            int dx = baby->sprite.x - player.x - player_image.width/2;

            if (dx < 0)
            {
                dx = -dx;
            }

            /* See if the platform is under the baby */
            if (q.state == Playing && dx > player_image.width/2)
            {
                /* Dropped */
                baby->running = FALSE;
                q.lives--;
                draw_score();

                if (q.lives == 0)
                {
                    go_game_over();
                }
            }
            else
            {
                /* Bounce! */
                baby->vy = -6*BABY_ACCELRATION;
                baby->sprite.y = platform;
            }
        }

        /* See if we've bounced into the ambulance */
        if (baby->sprite.x > LCD_WIDTH - baby_image.width)
        {
            baby->running = FALSE;

            if (q.state == Playing)
            {
                q.score++;

                if (q.score > q.highscore)
                {
                    q.highscore = q.score;
                }

                draw_score();
            }
        }

        /* Draw the next frame, if any */
        if (baby->running)
        {
            xor(baby_frames[baby->frame], baby->sprite.x, baby->sprite.y);
        }
    }
}

/** Create a new baby and start it off falling from the building */
static void spawn_baby(void)
{
    uintf running = 0;

    for (int i = 0; i < SizeOfArray(q.babies); i++)
    {
        if (q.babies[i].running)
        {
            running++;
        }
    }

    if (running == 0 || (random() % BABY_CHANCE) < q.phase)
    {
        for (int i = 0; i < SizeOfArray(q.babies); i++)
        {
            Baby* baby = q.babies + i;

            if (!baby->running)
            {
                new_baby(baby);
                q.spawned++;

                /* Spawned enough for this phase.  Make things harder */
                if (q.spawned >= 10)
                {
                    q.spawned = 0;
                    q.phase++;
                    draw_score();
                }

                break;
            }
        }
    }
}

/** Clear all babies */
static void clear_babies(void)
{
    for (int i = 0; i < SizeOfArray(q.babies); i++)
    {
        q.babies[i].running = FALSE;
    }
}

/** Change to the game over state */
static void go_game_over(void)
{
    q.state = GameOver;
    clear_babies();

    int y = LCD_HEIGHT * 2 / 10;
    blit(&gameover_image,
         10 + (LCD_WIDTH - gameover_image.width) / 2,
         y);
}

/** Change to the title state */
static void go_title(void)
{
    q.state = Title;
    q.phase = 5;

    draw_background();
    draw_score();
    draw_title();

    clear_babies();
}

/** Start a new game */
static void start_game(void)
{
    q.state = Playing;
    q.score = 0;
    q.lives = 5;
    q.phase = 1;
    draw_background();
    draw_score();
    clear_babies();
    xor(player.image, player.x, player.y);
}

/** Main loop */
static void run(void)
{
#ifdef WRDK_HOST
    grifo_init();
#endif

    int xstep = (LCD_WIDTH - building_image.width - ambulance_image.width - player_image.width - 5) / 2;

    /* Calculate the X position of the three player positions */
    int xs[] =
    {
        building_image.width + 8 + xstep * 0,
        building_image.width + 8 + xstep * 1,
        building_image.width + 8 + xstep * 2
    };

    /* Map a button to player position */
    int buttons[10] =
    {
        [BUTTON_SEARCH] = 0,
        [BUTTON_HISTORY] = 1,
        [BUTTON_RANDOM] = 2,
        [BUTTON_POWER] = -1,
    };

    /* Start the player out under the building */
    player.y = LCD_HEIGHT - player_image.height - 2;
    player.x = xs[0];
    player.image = &player_image;

    xor(player.image, player.x, player.y);

    q.seed = timer_get();

    /* Move to the title screen */
    go_title();

    /* Create all of the flames */
    for (int i = 0; i < SizeOfArray(q.flames); i++)
    {
        Flame* flame = q.flames + i;
        new_flame(flame);
    }

    uint next = timer_get() + (DT*1000*TIMER_CountsPerMicroSecond);

    for (;;)
    {
        event_t event;

        switch (event_get(&event))
        {
        case EVENT_BUTTON_DOWN:
        {
            /* Button press */
            int position = buttons[event.key.code];

            if (position >= 0)
            {
                xor(player.image, player.x, player.y);
                player.x = xs[position];
                xor(player.image, player.x, player.y);
            }

            if (q.state == Title)
            {
                start_game();
            }
            else if (q.state == GameOver)
            {
                go_title();
            }
            else
            {
            }
                
            break;
        }

        case EVENT_NONE:
        {
            /* Idle.  Animate */
            uint now = timer_get();
            int remain = next - now;

            if (remain <= 0)
            {
                for (int i = 0; i < SizeOfArray(q.flames); i++)
                {
                    tick_flame(q.flames + i);
                }

                for (int i = 0; i < SizeOfArray(q.babies); i++)
                {
                    tick_baby(q.babies + i);
                }

                if (q.state != GameOver)
                {
                    spawn_baby();
                }

                next = now + (DT*1000*TIMER_CountsPerMicroSecond);
            }

            watchdog(WATCHDOG_KEY);
            delay_us(1000);
            break;
        }

        default:
            break;
        }
    }
}
