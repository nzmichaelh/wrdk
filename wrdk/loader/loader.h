/** A simple serial based boot loader.

    @author Michael Hope
*/
#ifndef __LOADER_H__
#define __LOADER_H__

#include <sys/types.h>

/** Size of the input buffer in bytes.  The maximum number of bytes
    per packet is slightly less than this.
*/
#define LOADER_INPUT_SIZE  (512+20)

/** Write a character */
void port_putch(uint ch);
/** Blocking get a character */
uint port_getch(void);
/** Called when a packet is received to update any progress indication
 */
void port_progress(void);
/** Change the baud rate in a device specific way */
void port_set_baud(uint div);

/** Main loader entry point */
void loader(void);

#endif
