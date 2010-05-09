/* loader.lds - loader linker script

   Note that the loader is built near the top of kernel
   memory and is copied in by stub.c 
*/

OUTPUT_FORMAT("elf32-c33")

OUTPUT_ARCH(c33)

MEMORY
{
    /* sdram must be first or linker gives errors */
    sdram   (rwx): ORIGIN = 0x10010000 + 236k, LENGTH = 20k
    ivram   (rwx): ORIGIN = 0x00080000,        LENGTH = 12k
}

SECTIONS
{
    .lcd (NOLOAD) : {
        __START_FrameBuffer = . ;
        . = . + 12k /* LENGTH(ivram) */ ;
        __END_FrameBuffer = . ;
    } > ivram

    /* the compiler/linker assumes r15 is preset to this value at the start */
    __dp = 0x10000000 + 256k /* ORIGIN(sdram) */ ;

    .text : {
         __START_text = . ;
         *(.text)
    } > sdram
    __END_text = . ;

    . = ALIGN(4);

    .rodata : {
         __START_rodata = . ;
         *(.rodata)
    } > sdram
    __END_rodata = . ;

    .bss : {
         __START_bss = . ;
         *(.bss)
    } > sdram
     __END_bss = . ;

    .data : {
         __START_data = . ;
         *(.data)
    } > sdram
    __END_data = . ;

    . = ALIGN(4);

    __END_program = . ;

    __START_text = ADDR(.text);
    __END_text = __START_text + SIZEOF(.text);

    __START_rodata = ADDR(.rodata);
    __END_rodata = __START_rodata + SIZEOF(.rodata);

    __START_data = ADDR(.data);
    __END_data = __START_data + SIZEOF(.data);

    __START_bss = ADDR(.bss);
    __END_bss = __START_bss + SIZEOF(.bss);

}
