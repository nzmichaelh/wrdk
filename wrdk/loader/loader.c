/** A simple serial based boot loader.

    @author Michael Hope
*/
#include "loader.h"

/* Protocol specification:

   The protocol is a synchronous command/response system where the
   loader receives a command from the client, executes it, then returns
   the response.

   Commands and responses are sent in frames.  A frame starts with the
   first character received and ends with a \n (0x0A).  \r (0x0D) is
   ignored.

   Sending Ctrl-C at any time will interrupt the loader, cause any
   current command to be discarded, and generate a '!' response.

   Most frames are printable ASCII.  Some frames such as memory store
   have a binary variant.

   Special characters are escaped by sending the escape character '}'
   (0x7D) then the actual character XORed with 0x20.  \n, \r, Ctrl-C,
   and } must be escaped.  Note that this is only needed on binary
   frames.
   
   Commands are made up of a command ID and then optional space
   separated arguments.  Numbers are in hex with lower case for the
   alpha values.

   'Gets' use lower case command IDs.  'Sets' use upper case. See
   dispatch() for the list of commands.

   Responses may be either the data, 'k' for success, 'e' for error,
   or '?' for unrecognised command.
*/

/* Design notes:

   There's no checksum as a serial link is actually very reliable.
   Having no checksum means you can also talk to the loader manually
   over a terminal.

   There's no value or bounds checking.  This keeps the loader small
   and, as you're already using a system with direct memory and
   program access, there's no extra security risk.

   'uint' is used instead of more specific types.  One architecture
   the author is interested in doesn't have good casting or signed
   comparisons instructions.
*/

/** Stringify a macro */
#define STR(_x) #_x

/** All of the hex characters.  Used in encoding */
static const uint8_t nibbles[] = "0123456789abcdef";

/** Shared state across the module */
struct
{
    /** Current position in the input stream */
    uint8_t* pat;
    /** End of the command.  Used for binary packets. */
    uint8_t* pend;
} q;

/** The input buffer.  Done outside of q to allow putting it in a
    different segment */
uint8_t command[LOADER_INPUT_SIZE];

/** Decode a hex nibble */
static uint denibble(uint ch)
{
    /* Note that there's no range checking */
    if (ch >= 'a')
    {
        return ch - 'a' + 10;
    }
    else if (ch >= 'A')
    {
        return ch - 'A' + 10;
    }
    else
    {
        return ch - '0';
    }
}

/** Writes out the given character */
void loader_putch(uint ch)
{
    port_putch(ch);
}

/** Writes out the given null terminated string */
void loader_puts(const char* pmsg)
{
    for (; *pmsg != '\0'; pmsg++)
    {
        loader_putch(*pmsg);
    }
}

/** Writes out the given value as a hex byte */
void loader_putb(uint byte)
{
    loader_putch(nibbles[(byte >> 4) & 0x0F]);
    loader_putch(nibbles[(byte >> 0) & 0x0F]);
}

/** Start the response */
static void start()
{
    /* Any future extensions such as framing or checksums go here */
}

/** Finish up the response */
static void end()
{
    /* Write out the end of message */
    port_putch('\n');
}

/** Respond with OK */
static void ok()
{
    loader_putch('k');
}

/** Get the next hex value from the command */
uint loader_get()
{
    uint v = 0;

    while (1)
    {
        uint got = *q.pat++;

        /* Values are separated by space or end of string */
        if (got == ' ' || got == '\0')
        {
            return v;
        }

        v <<= 4;
        v |= denibble(got);
    }
}

static uint receive()
{
    uint8_t* pat = command;
    uint escaped = 0;

    /* Read in the whole of the packet */
    while (1)
    {
        uint ch = port_getch();

        if (ch == '\003')
        {
            /* Ctrl-C.  Discard everything */
            return ch;
        }
        else if (ch == '\r')
        {
            q.pend = pat;
            q.pat = command + 1;

            /* Null terminate to make things easier */
            *pat = '\0';

            /* Note that a zero length command has \0 as the first
               byte */
            return command[0];
        }
        else if (ch == '\n')
        {
            /* Eat so that \r\n from a terminal is fine */
        }
        else if (ch == '}')
        {
            /* PPP style escaping.  Most of the commands are plain
               text but there's no harm in de-escaping all types of
               command.
            */
            escaped = 0x20;
        }
        else
        {
            *pat++ = ch ^ escaped;
            escaped = 0;

            /* Note that there's no buffer overflow checking */
        }
    }
}

void dispatch(uint id)
{
    switch (id)
    {
    case '\003':
        /* Ctrl-C.  Discard the frame and re-sync
           Response: '!'
        */
        loader_putch('!');
        break;

    case 'm':
    {
        /* Read memory.
           Arguments: start address, count in bytes
           Response: bytes in hex
        */
        uint8_t* p = (uint8_t*)loader_get();
        uint8_t* pend = p + loader_get();

        for (; p != pend; p++)
        {
            loader_putb(*p);
        }

        break;
    }
    case 'M':
    {
        /* Write memory
           Arguments: start address, data as hex bytes
           Response: k
        */
        uint8_t* p = (uint8_t*)loader_get();
        uint8_t* pend = p + loader_get();

        for (; p != pend; p++)
        {
            *p = (denibble(*q.pat++) << 4) | denibble(*q.pat++);
        }

        ok();
        break;
    }
    case 'X':
    {
        /* Write memory in binary mode
           Arguments: start address, rest as binary data bytes
           Response: k
        */
        uint8_t* p = (uint8_t*)loader_get();
        const uint8_t* pat;

        for (pat = q.pat; pat != q.pend; pat++)
        {
            *p++ = *pat;
        }

        ok();
        break;
    }
    case 'u':
    {
        /* Checksum a block of memory
           Arguments: start address, count in bytes
           Response: 16 bit modulo 65536 checksum
        */
        uint8_t* p = (uint8_t*)loader_get();
        uint8_t* pend = p + loader_get();
        uint sum = 0;

        for (; p != pend; p++)
        {
            sum += *p;
        }
        loader_putb((sum >> 8) & 0xFF);
        loader_putb((sum >> 0) & 0xFF);
        break;
    }
    case 'c':
    {
        /* Change execution
           Argument: address to start executing at
           Response: k
        */
        void (*pstart)(void) = (void (*)(void))loader_get();
        (*pstart)();
        ok();
        break;
    }
    case 'B':
    {
        /* Set the baud rate in a device specific way
           Argument: New value for the divisor
           Response: k
        */
        port_set_baud(loader_get());
        ok();
        break;
    }
    case '?':
        /* Get capabilities
           Response: Space separated capabilities.  First character of
           each is the capability type.

           v: protocol version.  Always the first field
           t: host CPU type
           c: command capacity in decimal
        */
        loader_puts("v1 t" STR(LOADER_TYPE) " c" STR(LOADER_INPUT_SIZE));
        break;

    default:
        loader_putch('?');
        break;
    }
}

/** Main entry point */
void loader()
{
    for (;;)
    {
        uint code = receive();

        start();
        dispatch(code);
        end();

        port_progress();
    }
}
