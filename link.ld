
MEMORY
{
  /* Define each memory region */
  Flash (rx) : ORIGIN = 0x0, LENGTH = 0xC000 /* 48K bytes */
  RAM_main (rwx) : ORIGIN = 0x10000000, LENGTH = 0x2000 /* 8K bytes */
  RAM_USB (rwx) : ORIGIN = 0x20004000, LENGTH = 0x800 /* 2K bytes */


}

/* TODO: lpc_usb_lib references RAM2, maybe rename to a more descriptive name*/


  /* Define a symbol for the top of each memory region */
  __top_Flash = 0x0 + 0xC000;
  __top_RAM_main = 0x10000000 + 0x2000;
  __top_RAM_USB = 0x20004000 + 0x800;

