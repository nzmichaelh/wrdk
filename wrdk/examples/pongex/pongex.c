/** WikiReader Invaders.  Strangely familiar.

    Michael Hope 2010
*/
#include <grifo.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int uintf;

#define FALSE 0
#define TRUE  1

#define PLAYER_VMAX       8

#define COMPUTER_VMAX     4
#define COMPUTER_ERROR    14

#define BALL_VX           2
#define BALL_VY           3

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
    const Image* image;
    int x;
    int y;
} Sprite;

/** A rectangle.  Used in collision detection. */
typedef struct
{
    int top;
    int right;
    int bottom;
    int left;
} Rectangle;

typedef enum { Title, Playing } State;

#define LCD_ImageType static const Image

#include "build/leftbumper.img"
#include "build/rightbumper.img"
#include "build/ball.img"
#include "build/player1.img"
#include "build/player2.img"
#include "build/title.img"

/** All of the global values */
struct
{
    Sprite players[2];
    Sprite ball;

    int vx;
    int vy;

    int scores[2];
    uint buttons;
    int stopped;
    uint seed;

    State state;
} q;

static const Sprite leftbumper =
{
    &leftbumper_image, 0, 0
};

static const Sprite rightbumper =
{
    &rightbumper_image, LCD_WIDTH - rightbumper_image_Width, 0
};

/** All of the sprites in the scene in Z order */
static const Sprite* const scene[] =
{
    &leftbumper,
    &rightbumper,
    &q.players[0],
    &q.players[1],
    &q.ball
};

static uint8_t fb[LCD_BUFFER_WIDTH_BYTES*LCD_HEIGHT];

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
static void blit(const Image* image, int x, int y)
{
    if (x < 0)
    {
        x = (LCD_WIDTH - image->width) / 2;
    }

    if (y < 0)
    {
        y = (LCD_HEIGHT - image->height) / 2;
    }

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

static void blit_sprite(const Sprite* sprite)
{
    blit(sprite->image, sprite->x, sprite->y);
}

/** Flip between the different frame buffers */
static void flip(void)
{
    /* We pretend here.  Render into an off-screen buffer then
       memcpy() into the real framebuffer
    */
    uint32_t *pdest = (uint32_t *)lcd_get_framebuffer();
    uint32_t *psrc = (uint32_t *)fb;
    uint32_t *pend = (uint32_t *)(fb + sizeof(fb));

    while (psrc != pend)
    {
        *pdest++ = *psrc++;
        *pdest++ = *psrc++;
        *pdest++ = *psrc++;
        *pdest++ = *psrc++;
    }
}

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

static int randrange(int low, int high)
{
    int v;

    do
    {
        v = low + random() % (high - low);
    } while (v == 0);

    return v;
}

static void new_ball()
{
    q.ball.x = (LCD_WIDTH - ball_image.width) / 2;
    q.ball.y = (LCD_HEIGHT - ball_image.height) / 2;

    q.ball.x += randrange(-20, 20);

    q.vx = BALL_VX;
    q.vy = BALL_VY;

    uint v = random();

    if ((v & 1) != 0)
    {
        q.vx += 1;
    }

    if ((v & 2) == 0)
    {
        q.vx = -q.vx;
    }

    if ((v & 4) == 0)
    {
        q.vy = -q.vy;
    }

    q.stopped = 15;
}

/** Initialise the game */
static void init(void)
{
    q.players[0].image = &player1_image;
    q.players[0].x = (LCD_WIDTH - player1_image.width) / 2;
    q.players[0].y = 16;

    q.players[1].image = &player2_image;
    q.players[1].x = (LCD_WIDTH - player2_image.width) / 2;
    q.players[1].y = LCD_HEIGHT - player2_image.height - 16;

    q.ball.image = &ball_image;
    q.ball.x = (LCD_WIDTH - ball_image.width) / 2;
    q.ball.y = (LCD_HEIGHT - ball_image.height) / 2;

    new_ball();
}

static void new_game()
{
    q.scores[0] = 0;
    q.scores[1] = 0;
    q.state = Playing;

    new_ball();
}

/** Draw the scene */
static void draw(void)
{
    /* Draws the whole scene every time.  Inefficient but fast enough */

    /* Background */
    memset(fb, 0, sizeof(fb));

    /* Score */
    lcd_set_framebuffer((uint32_t *)fb);
    lcd_at_xy(12, 0);
    lcd_printf("%u / %u", q.scores[0], q.scores[1]);
    lcd_set_default_framebuffer();

    for (uint i = 0; i < SizeOfArray(scene); i++)
    {
        blit_sprite(scene[i]);
    }

    if (q.state == Title)
    {
        blit(&title_image, -1, -1);
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

static int abs(int v)
{
    return v > 0 ? v : -v;
}

/** Get the bounding rectangle of one invader */
static void get_bounds(const Sprite *sprite, Rectangle *bounds)
{
    bounds->top = sprite->y;
    bounds->right = sprite->x + sprite->image->width;
    bounds->bottom = sprite->y + sprite->image->height;
    bounds->left = sprite->x;
}

static int intersects(const Rectangle* r1, const Rectangle* r2)
{
    return r2->bottom >= r1->top
        && r2->top <= r1->bottom
        && r2->right >= r1->left
        && r2->left <= r1->right;
}

static void bounce_bumper(int* x, int* v, int low, int high)
{
    if (*v < 0)
    {
        *x += *v;

        if (*x <= low)
        {
            *v = -*v;
            *x = low;
        }
    }
    else if (*v > 0)
    {
        *x += *v;

        if (*x >= high)
        {
            *v = -*v;
            *x = high;
        }
    }
}

static void bounce_puck(const Rectangle* puck, const Rectangle* ball, bool direction)
{
    if (intersects(puck, ball) && direction)
    {
        int x1 = (puck->right + puck->left) / 2;
        int x2 = (ball->right + ball->left) / 2;
        /* Difference between the centres of the puck and ball */
        int offset = x2 - x1;
        /* Velocity depends on where we hit the puck */
        q.vx = offset / 4;

        q.vy = -q.vy;
    }
}

static void missed(int* score)
{
    if (q.state == Playing)
    {
        *score += 1;

        if (*score == 5)
        {
            q.state = Title;
        }
    }

    new_ball();
}

static void update(void)
{
    Sprite* player = q.players + 0;
    Sprite* computer = q.players + 1;
    Sprite* ball = &q.ball;

    if (q.stopped > 0)
    {
        q.stopped -= 1;
    }
    else
    {
        bounce_bumper(&q.ball.x, &q.vx, 
               leftbumper_image.width,
               LCD_WIDTH - rightbumper_image.width - ball_image.width);

        bounce_bumper(&q.ball.y, &q.vy, 
               0,
               LCD_HEIGHT - ball_image.height);
    }

    Rectangle player_rect;
    Rectangle computer_rect;
    Rectangle ball_rect;

    get_bounds(player, &player_rect);
    get_bounds(computer, &computer_rect);
    get_bounds(ball, &ball_rect);

    bounce_puck(&computer_rect, &ball_rect, q.vy > 0);
    bounce_puck(&player_rect, &ball_rect, q.vy < 0);

    Rectangle out1 = { -100, LCD_WIDTH + 100, 0, -100 };

    if (intersects(&ball_rect, &out1))
    {
        /* Player missed ! */
        missed(&q.scores[1]);
    }

    Rectangle out2 = { LCD_HEIGHT, LCD_WIDTH + 100, LCD_HEIGHT + 100, -100 };

    if (intersects(&ball_rect, &out2))
    {
        /* Computer missed ! */
        missed(&q.scores[0]);
    }

    /* Have the computer chase the ball */
    int err = ball->x - computer->x - (computer->image->width - ball->image->width)/2;

    if (abs(err) < COMPUTER_ERROR)
    {
        /* Close enough */
    }
    else if (err < 0)
    {
        err = max(-COMPUTER_VMAX, err);

        computer->x += err;
        computer->x = max(computer->x, leftbumper_image.width);
        
    }
    else if (err > 0)
    {
        err = min(COMPUTER_VMAX, err);

        computer->x += err;
        computer->x = min(computer->x, LCD_WIDTH - computer->image->width - rightbumper_image.width);
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

        if (q.state == Title)
        {
            new_game();
        }

        break;

    case EVENT_BUTTON_UP:
        q.buttons &= ~(1 << event.button.code);
        break;

    default:
        break;
    }

    Sprite* player = q.players + 0;

    if ((q.buttons & (1 << BUTTON_SEARCH)) != 0)
    {
        player->x -= PLAYER_VMAX;

        player->x = max(player->x, leftbumper_image.width);
    }

    if ((q.buttons & (1 << BUTTON_HISTORY)) != 0)
    {
        player->x += PLAYER_VMAX;

        player->x = min(player->x, LCD_WIDTH - player->image->width - rightbumper_image.width);
    }
}

/** Limit to a certain number of frames per second */
static void fps(int count)
{
    static uint32_t last;
    uint32_t now = timer_get();

    int32_t elapsed = now - last;
    int32_t want = TIMER_CountsPerMicroSecond * 1000000 / count;
    int32_t remain = want - elapsed;

    if (last == 0 || remain < 0)
    {
        /* Can't keep up */
        last = now;
    }
    else
    {
        last += want;
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
