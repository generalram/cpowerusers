/*
The programs contained herein are adapted from 

      C: Power User's Guide

by Herbert Schildt published by Osborne/McGraw-Hil, Copyright
1987, Osborne/McGraw-Hill.  Used with the permission of
Osborne/McGraw-Hill.  Program adaptations are solely the
work of Herbert Schildt and are not a publication of
Osborne/McGraw-Hill.
*/


listing 8-1
/* Read the current cursor position. */
void read_cursor_xy(x, y)
char *x, *y;
{
  union REGS r;
  
  r.h.ah = 3; /* read cursor postion */
  r.h.bh = 0; /* vidio page */
  int86(0x10, &r, &r);
  *y = r.h.dl;
  *x = r.h.dh;
}




listing 8-2

/* Send the cursor to the specified X,Y position. */
void goto_xy(x, y)
int x, y;
{
  union REGS r;

  r.h.ah = 2; /* cursor addressing function */
  r.h.dl = x; /* column coordinate */
  r.h.dh = y; /* row coordinate */
  r.h.bh = 0; /* video page */
  int86(0x10, &r, &r);
}




listing 8-3

/* Print a string in color. */
void color_puts(s, color)
char *s; /* string */
char color; /* color of string */
{
  union REGS r;
  char x, y;

  read_cursor_xy(&x, &y); /* get current cursor position */  
  while(*s) {
    if(*s=='\n') { /* process a newline */
      printf("\n");
      s++;
      x = 0; y++; /* advance to next line */
      continue;
    }

    r.h.ah = 9; /* write character and attribute */
    r.h.al = *s++; /* character to write */
    r.h.bl = color; /* color attribute */
    r.h.bh = 0; /* video page 0 */
    r.x.cx = 1; /* write it one time */
    int86(0x10, &r, &r);
    x++;
    goto_xy(x, y); /* advance the cursor */
  }
}




listing 8-4

#define BLUE 	   1
#define GREEN      2
#define RED 	   4
#define INTENSE    8
#define BLUE_BACK  16
#define GREEN_BACK 32
#define RED_BACK   64
#define BLINK 	   128




listing 8-5

color_puts("this is a test", GREEN | RED | INTENSE);




listing 8-6

/* Set the size of the cursor */
void size_cursor(start, end)
char start, end; /* starting and ending scan lines */
{
  union REGS r;

  r.h.ah = 1; /* cursor addressing function */
  r.h.ch = start;
  r.h.cl = end;
  int86(0x10, &r, &r);
}




listing 8-7

size_cursor(0,2);




listing 8-8

/* Scroll a window up or down. */
void scroll_window(startx, starty, endx, endy, lines, direct)
char startx, starty; /* upper left corner */
char endx, endy; /* lower right corner */
char lines; /* number of lines to scroll */
char direct; /* up or down */
{
  union REGS r;

  if(direct==UP) r.h.ah = 6; /* scroll up */
  else r.h.ah = 7; /* scroll down */

  r.h.al = lines; 
  r.h.ch = starty;
  r.h.cl = startx;
  r.h.dh = endy;
  r.h.dl = endx;
  r.h.bh = 0; /* display attribute */
  int86(0x10, &r, &r);
}




listing 8-9

/* Demonstration program for printing text in color, sizing 
   sizing the cursor, and scrolling a window.
*/
#include "dos.h"

#define BLUE 	   1
#define GREEN      2
#define RED 	   4
#define INTENSE    8
#define BLUE_BACK  16
#define GREEN_BACK 32
#define RED_BACK   64
#define BLINK 	   128

#define UP   0
#define DOWN 1

void mode(), color_puts(), palette(), read_cursor_xy();
void goto_xy(), size_cursor(), scroll_window();

main()
{
  int i, j;

  mode(3); /* color text */

  size_cursor(4, 7);
  goto_xy(0, 0);
  color_puts("this is a test\n", BLUE | RED | INTENSE);
  
  for(i=0; i<22; i++ ) {
    for(j=0; j<79; j++) 
      printf("%c", i+'a');
    printf("\n");
  }
  getche();
  scroll_window(10, 10, 50, 15, 3, DOWN);
  getche();
}

/* Print a string in color. */
void color_puts(s, color)
char *s; /* string */
char color; /* color of string */
{
  union REGS r;
  char x, y;

  read_cursor_xy(&x, &y); /* get current cursor position */  
  while(*s) {
    if(*s=='\n') { /* process a newline */
      printf("\n");
      s++;
      x = 0; y++; /* advance to next line */
      continue;
    }

    r.h.ah = 9; /* write character and attribute */
    r.h.al = *s++; /* character to write */
    r.h.bl = color; /* color attribute */
    r.h.bh = 0; /* video page 0 */
    r.x.cx = 1; /* write it one time */
    int86(0x10, &r, &r);
    x++;
    goto_xy(x, y); /* advance the cursor */
  }
}

/* Read the current cursor position. */
void read_cursor_xy(x, y)
char *x, *y;
{
  union REGS r;
  
  r.h.ah = 3; /* read cursor postion */
  r.h.bh = 0; /* vidio page */
  int86(0x10, &r, &r);
  *y = r.h.dl;
  *x = r.h.dh;
}

/* Set the palette. */
void palette(pnum)
int pnum;
{
  union REGS r;

  r.h.bh = 1;   /* code for mode 4 graphics */
  r.h.bl = pnum;
  r.h.ah = 11;  /* set palette function */
  int86(0x10, &r, &r);
}

/* Set the video mode. */
void mode(mode_code)
int mode_code;
{
  union REGS r;

  r.h.al = mode_code;
  r.h.ah = 0;
  int86(0x10, &r, &r);
}

/* Send the cursor to the specified X,Y position. */
void goto_xy(x, y)
int x, y;
{
  union REGS r;

  r.h.ah = 2; /* cursor addressing function */
  r.h.dl = x; /* column coordinate */
  r.h.dh = y; /* row coordinate */
  r.h.bh = 0; /* video page */
  int86(0x10, &r, &r);
}

/* Set the size of the cursor */
void size_cursor(start, end)
char start, end; /* starting and ending scan lines */
{
  union REGS r;

  r.h.ah = 1; /* cursor addressing function */
  r.h.ch = start;
  r.h.cl = end;
  int86(0x10, &r, &r);
}

/* Scroll a window up or down. */
void scroll_window(startx, starty, endx, endy, lines, direct)
char startx, starty; /* upper left corner */
char endx, endy; /* lower right corner */
char lines; /* number of lines to scroll */
char direct; /* up or down */
{
  union REGS r;

  if(direct==UP) r.h.ah = 6; /* scroll up */
  else r.h.ah = 7; /* scroll down */

  r.h.al = lines; 
  r.h.ch = starty;
  r.h.cl = startx;
  r.h.dh = endy;
  r.h.dl = endx;
  r.h.bh = 0; /* display attribute */
  int86(0x10, &r, &r);
}




listing 8-10

C>screen scr.sav




listing 8-11

/* This program saves the what is currently on the video 
   display to the disk file specified on the command line.
*/

#include "dos.h"
#include "stdio.h"

void save_screen(), goto_xy();

main(argc, argv)
int argc;
char *argv[];
{
  if(argc!=2) {
    printf("usage: screen <filename>");
    exit(1);
  }
  save_screen(argv[1]);
}

/* Save the contents of the screen to a disk file. */
void save_screen(fname)
char *fname;
{
  FILE *fp;
  union REGS r;
  register char x, y;

  if(!(fp=fopen(fname, "w"))) {
    printf("cannot open file");
    exit(1);
  }

  for(y=0; y<25; y++)
    for(x=0; x<80; x++) {
      goto_xy(x, y);
      r.h.ah = 8; /* read a character */  
      r.h.bh = 0; /* video page */
      int86(0x10, &r, &r);
      putc(r.h.al, fp); /* write the character */
    }
  fclose(fp);
}

/* Send the cursor to the specified X,Y position. */
void goto_xy(x, y)
int x, y;
{
  union REGS r;

  r.h.ah = 2; /* cursor addressing function */
  r.h.dl = x; /* column coordinate */
  r.h.dh = y; /* row coordinate */
  r.h.bh = 0; /* video page */
  int86(0x10, &r, &r);
}




listing 8-12
/* Beep the speaker using the specified frequency. */
void sound(freq)
int freq;
{
  unsigned i;
  union {
    long divisor;
    unsigned char c[2];
  } count;

  unsigned char p;

  count.divisor = 1193280 / freq;  /* compute the proper count */
  outportb(67, 182); /* tell 8253 that a count is coming */
  outportb(66, count.c[0]); /* send low-order byte */
  outportb(66, count.c[1]); /* send high-order byte */
  p = inportb(97); /* get existing bit pattern */
  outportb(97, p | 3); /* turn on bits 0 and 1 */

  for(i=0; i<64000; ++i) ; /* delay loop */

  outportb(97, p); /* restore original bits to turn off speaker */
}




listing 8-13

/* A simple hearing tester. */
#include "dos.h"

void sound();

main()
{
  int freq;

  do {
    printf("enter frequency (0 to exit): ");
    scanf("%d", &freq);
    if(freq) sound(freq);
  } while(freq);
}




listing 8-14
#define RATE 100
#define DELAY 10000

/* Create a siren effect. */
void siren()
{
  unsigned i, freq;
  union {
    long divisor;
    unsigned char c[2];
  } count;

  unsigned char p;

  p = inportb(97); /* get existing bit pattern */
  outportb(97, p | 3); /* turn on bits 0 and 1 */
  /* ascending siren */
  for(freq=1000; freq<3000; freq+=RATE) {
    count.divisor = 1193280 / freq;  /* compute the proper count */
    outportb(67, 182); /* tell 8253 that a count is coming */
    outportb(66, count.c[0]); /* send low-order byte */
    outportb(66, count.c[1]); /* send high-order byte */

    for(i=0; i<DELAY; ++i) ;
  }
  
  /* descending siren */
  for( ; freq>1000; freq-=RATE) {
    count.divisor = 1193280 / freq;  /* compute the proper count */
    outportb(67, 182); /* tell 8253 that a count is coming */
    outportb(66, count.c[0]); /* send low-order byte */
    outportb(66, count.c[1]); /* send high-order byte */

    for(i=0; i<DELAY; ++i) ; 
  }
  outportb(97, p); /* restore original bits to turn off speaker */
}




listing 8-15
#define RATE 100
#define DELAY 10000

/* Create a laser blast effect. */
void siren()
{
  unsigned i, freq;
  union {
    long divisor;
    unsigned char c[2];
  } count;

  unsigned char p;

  p = inportb(97); /* get existing bit pattern */
  outportb(97, p | 3); /* turn on bits 0 and 1 */
  /* laser blast */
  for(freq=5000 ; freq>1000; freq-=RATE) {
    count.divisor = 1193280 / freq;  /* compute the proper count */
    outportb(67, 182); /* tell 8253 that count is coming */
    outportb(66, count.c[0]); /* send low-order byte */
    outportb(66, count.c[1]); /* send high-order byte */

    for(i=0; i<DELAY; ++i) ; 
  }
  outportb(97, p); /* restore original bits to turn off speaker */
}




listing 8-16

/* Celestial Music of the Cosmic Spheres. */

#define DELAY 64000

#include "dos.h"

void hssound();

main()
{
  int freq;

  do {
    do {
      freq = rand();
    } while(freq>5000); /* alter to your personal taste */
    hssound(freq);
  } while(!kbhit());
}

/* Beep the speaker using the specified frequency. */
void hssound(freq)
int freq;
{
  unsigned i;
  union {
    long divisor;
    unsigned char c[2];
  } count;

  unsigned char p;

  count.divisor = 1193280 / freq;  /* compute the proper count */
  outportb(67, 182); /* tell 8253 that count is comming */
  outportb(66, count.c[0]); /* send low-order byte */
  outportb(66, count.c[1]); /* send high-order byte */
  p = inportb(97); /* get existing bit pattern */
  outportb(97, p | 3); /* turn on bits 0 and 1 */

  for(i=0; i<DELAY; ++i) ; /* delay 64000 for 10+ mhz computers
			      32000 for 6 mhz ATs
			      20000 for standard PCs and XTs */

  outportb(97, p); /* restore original bits to turn off speaker */
}



