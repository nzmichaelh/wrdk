/** Stub file that pulls in, relocates, and runs the loader.
 */
#include <string.h>

/** Address to relocate to */
#define BASE   0x1004b000

static const uint8_t loader[] =
{
#include "build/loader.inc"
};

int main(void)
{
    memcpy((void*)BASE, loader, sizeof(loader));

    int (*start)(void) = (void*)BASE;

    return (*start)();
}

