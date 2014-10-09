/*
The programs contained herein are adapted from 

      C: Power User's Guide

by Herbert Schildt published by Osborne/McGraw-Hil, Copyright
1987, Osborne/McGraw-Hill.  Used with the permission of
Osborne/McGraw-Hill.  Program adaptations are solely the
work of Herbert Schildt and are not a publication of
Osborne/McGraw-Hill.
*/


listing 1-2

union REGS in, out;

in.h.ah = 5;
int86(16, &in, &out);





listing 1-3

/* send the cursor to x,y */
void goto_xy(x,y)
int x,y;
{
  union REGS r;

  r.h.ah=2; /* cursor addressing function */
  r.h.dl=y; /* column coordinate */
  r.h.dh=x; /* row coordinate */
  r.h.bh=0; /* video page */
  int86(0x10, &r, &r);
}





listing 1-4

/* save a portion of the screen */
void save_video(startx, endx, starty, endy, buf_ptr)
int startx, endx, starty, endy;
unsigned int *buf_ptr; /* buffer where screen will be stored */
{
  union REGS r;
  register int i,j;
  for(i=starty; i<endy; i++)
    for(j=startx; j<endx; j++) {
      goto_xy(j, i); 
      r.h.ah = 8; /* read character function */
      r.h.bh = 0; /* assume active display page is 0 */
      *buf_ptr++ = int86(0x10, &r, &r);
      putchar(' '); /* clear the screen */
    }
}





listing 1-5

/* restore a portion of the screen */
void restore_video(startx, endx, starty, endy, buf_ptr)
int startx, endx, starty, endy;
unsigned char *buf_ptr; /* buffer containing screen info */
{
  union REGS r;
  register int i,j;

  for(i=starty; i<endy; i++)
    for(j=startx; j<endx; j++) {
      goto_xy(j, i);
      r.h.ah = 9; /* write character function */
      r.h.bh = 0; /* assume active display page is 0 */
      r.x.cx = 1; /* number of times to write the character */
      r.h.al = *buf_ptr++; /* character */
      r.h.bl = *buf_ptr++; /* attribute */
      int86(0x10, &r, &r);
  }
}





listing 1-6

/* display a pop-up menu and return selection */
int popup(menu, keys, count, x, y, border)
char *menu[];  	/* menu text */
char *keys;    	/* hot keys */
int count;	/* number of menu items */
int x, y;	/* X,Y coordinates of left hand corner */
int border;	/* no border if 0 */





listing 1-7

char *fruit[]= {
  "Apple",
  "Orange",
  "Pear",
  "Grape",
  "Raspberry",
  "Strawberry"
};






listing 1-8

void draw_border(startx, starty, endx, endy)
int startx, starty, endx, endy;
{
  register int i;

  for(i=startx+1; i<endx; i++) {
     goto_xy(i, starty);
     putchar(179);
     goto_xy(i, endy);
     putchar(179);
  }

  for(i=starty+1; i<endy; i++) {
    goto_xy(startx, i);
    putchar(196);
    goto_xy(endx, i);
    putchar(196);
  }
  goto_xy(startx, starty); putchar(218);
  goto_xy(startx, endy); putchar(191);
  goto_xy(endx, starty); putchar(192);
  goto_xy(endx, endy); putchar(217);

}





listing 1-9


/* input user's selection */
get_resp(x, y, count, menu, keys)
int x, y, count;
char *menu[];
char *keys;
{
  union inkey {
    char ch[2];
    int i;
  } c;
  int arrow_choice=0, key_choice;

  y++;

  /* highlight the first selection */
  goto_xy(x, y);
  write_video(x, y, menu[0], REV_VID); /* reverse video */

  for(;;) {
    while(!bioskey(1)) ; /* wait for key stroke */
    c.i = bioskey(0);    /* read the key */
 
    /* reset the selection to normal video */
    goto_xy(x+arrow_choice, y);
    write_video(x+arrow_choice, y, 
      menu[arrow_choice], NORM_VID); /* redisplay */

    if(c.ch[0]) { /* is normal key */
      /* see if it is a hot key */
      key_choice = is_in(keys, tolower(c.ch[0]));
      if(key_choice) return key_choice-1;
      /* check for ENTER or space bar */
      switch(c.ch[0]) {
	case '\r': return arrow_choice;
	case ' ' : arrow_choice++;
	  break;
	case ESC : return -1; /* cancel */
      }
    }
    else {  /* is special key */
      switch(c.ch[1]) {
	case 72: arrow_choice--; /* up arrow */
	  break;
 	case 80: arrow_choice++; /* down arrow */
	  break;
      }
    }
    if(arrow_choice==count) arrow_choice=0;
    if(arrow_choice<0) arrow_choice=count-1;

    /* highlight the next selection */
    goto_xy(x+arrow_choice, y);
    write_video(x+arrow_choice, y, menu[arrow_choice], REV_VID);
  }
}





listing 1-10

/* emulate part of the Turbo C bioskey() function */
bioskey(c)
int c;
{
  switch(c) {
    case 0: return get_key();
    case 1: return kbhit();
  }
}

/* read the 16-bit scan code of a key */
get_key()
{
  union REGS r;

  r.h.ah = 0;
  return int86(0x16, &r, &r);
}





listing 1-11

/* display a string with specified attribute */
void write_video(x, y, p, attrib)
int x, y;
char *p;
int attrib;
{
  union REGS r;
  register int i,j;

  for(i=y; *p; i++) {
      goto_xy(x, i);
      r.h.ah = 9; /* write character function */
      r.h.bh = 0; /* assume active display page is 0 */
      r.x.cx = 1; /* number of times to write the character */
      r.h.al = *p++; /* character */
      r.h.bl = attrib; /* attribute */
      int86(0x10, &r, &r);
  }
}





listing 1-12


is_in(s, c)
char *s, c;
{
  register int i;

  for(i=0; *s; i++) if(*s++==c) return i+1;
  return 0;
}





listing 1-13


/* display a pop-up menu and return selection
   returns -2 if menu cannot be constructed
   returns -1 if user hits escape key
   otherwise the item number is returned starting
   with 0 as the first (top most) entry
*/
int popup(menu, keys, count, x, y, border)
char *menu[];  	/* menu text */
char *keys;    	/* hot keys */
int count;	/* number of menu items */
int x, y;	/* X,Y coordinates of left hand corner */
int border;	/* no border if 0 */
{
  register int i, len;
  int endx, endy, choice;
  unsigned int *p;

  if((x>24) || (x<0) || (y>79) || (y<0)) {
    printf("range error");
    return -2;
  } 

  /* compute the dimensions */
  len = 0;
  for(i=0; i<count; i++) 
   if(strlen(menu[i]) > len) len = strlen(menu[i]);
  endy = len + 2 + y;
  endx = count + 1 + x;
  if((endx+1>24) || (endy+1>79)) {
    printf("menu won't fit");
    return -2;
  } 
  
  /* allocate enough memory for it */
  p = (unsigned int *) malloc((endx-x+1) * (endy-y+1));
  if(!p) exit(1);  /* install your own error handler here */

  /* save the current screen data */
  save_video(x, endx+1, y, endy+1, p);

  if(border) draw_border(x, y, endx, endy);

  /* display the menu */
  display_menu(menu, x+1, y+1, count);

  /* get the user's response */
  choice = get_resp(x+1, y, count, menu, keys);

  /* restore the original screen */
  restore_video(x, endx+1, y, endy+1, (char *) p);
  free(p);
  return choice;
}





listing 1-14

/* Pop-up menu routines for text mode operation. */

#include "stdio.h"
#include "dos.h"
#include "stdlib.h"

#define BORDER 1
#define ESC 27
#define REV_VID 0x70
#define NORM_VID 7

void save_video(), restore_video();
void goto_xy(), cls(), write_video();
void display_menu(), draw_border();

char *fruit[]= {
  "Apple",
  "Orange",
  "Pear",
  "Grape",
  "Raspberry",
  "Strawberry"
};

char *color[]= {
  "Red",
  "Yellow",
  "Orange",
  "Green",
};

char *apple_type[]= {
  "Red delicious",
  "Jonathan",
  "Winesap",
  "Rome"
};

main()
{
  int i;

  cls();
  goto_xy(0,0);
  for(i=0; i<25; i++)
    printf("This is a test of the pop-up window routines.\n");
  popup(fruit, "aopgrs", 6, 1, 3, BORDER); 
  popup(color, "ryog", 4, 5, 10, BORDER); 
  popup(apple_type, "rjwr", 4, 10, 18, BORDER); 
}

/* display a pop-up menu and return selection 
   returns -2 if menu cannot be constructed
   returns -1 if user hits escape key
   otherwise the item number is returned starting
   with 0 as the first (top most) entry
*/
int popup(menu, keys, count, x, y, border)
char *menu[];  	/* menu text */
char *keys;    	/* hot keys */
int count;	/* number of menu items */
int x, y;	/* X,Y coordinates of left hand corner */
int border;	/* no border if 0 */
{
  register int i, len;
  int endx, endy, choice;
  unsigned int *p;

  if((x>24) || (x<0) || (y>79) || (y<0)) {
    printf("range error");
    return -2;
  } 

  /* compute the dimensions */
  len = 0;
  for(i=0; i<count; i++) 
   if(strlen(menu[i]) > len) len = strlen(menu[i]);
  endy = len + 2 + y;
  endx = count + 1 + x;
  if((endx+1>24) || (endy+1>79)) {
    printf("menu won't fit");
    return -2;
  } 
  
  /* allocate enough memory for it */
  p = (unsigned int *) malloc((endx-x+1) * (endy-y+1));
  if(!p) exit(1);  /* install your own error handler here */

  /* save the current screen data */
  save_video(x, endx+1, y, endy+1, p);

  if(border) draw_border(x, y, endx, endy);

  /* display the menu */
  display_menu(menu, x+1, y+1, count);

  /* get the user's response */
  choice = get_resp(x+1, y, count, menu, keys);

  /* restore the original screen */
  restore_video(x, endx+1, y, endy+1, (char *) p);
  free(p);
  return choice;
}

/* display the menu in its proper location */
void display_menu(menu, x, y, count)
char *menu[];
int x, y, count;
{
  register int i;

  for(i=0; i<count; i++, x++) {
    goto_xy(x, y);
    printf(menu[i]);
  }
}

void draw_border(startx, starty, endx, endy)
int startx, starty, endx, endy;
{
  register int i;

  for(i=startx+1; i<endx; i++) {
     goto_xy(i, starty);
     putchar(179);
     goto_xy(i, endy);
     putchar(179);
  }

  for(i=starty+1; i<endy; i++) {
    goto_xy(startx, i);
    putchar(196);
    goto_xy(endx, i);
    putchar(196);
  }
  goto_xy(startx, starty); putchar(218);
  goto_xy(startx, endy); putchar(191);
  goto_xy(endx, starty); putchar(192);
  goto_xy(endx, endy); putchar(217);

}

/* input user's selection */
get_resp(x, y, count, menu, keys)
int x, y, count;
char *menu[];
char *keys;
{
  union inkey {
    char ch[2];
    int i;
  } c;
  int arrow_choice=0, key_choice;

  y++;

  /* highlight the first selection */
  goto_xy(x, y);
  write_video(x, y, menu[0], REV_VID); /* reverse video */

  for(;;) {
    while(!bioskey(1)) ; /* wait for key stroke */
    c.i = bioskey(0);    /* read the key */
 
    /* reset the selection to normal video */
    goto_xy(x+arrow_choice, y);
    write_video(x+arrow_choice, y, 
      menu[arrow_choice], NORM_VID); /* redisplay */

    if(c.ch[0]) { /* is normal key */
      /* see if it is a hot key */
      key_choice = is_in(keys, tolower(c.ch[0]));
      if(key_choice) return key_choice-1;
      /* check for ENTER or space bar */
      switch(c.ch[0]) {
	case '\r': return arrow_choice;
	case ' ' : arrow_choice++;
	  break;
	case ESC : return -1; /* cancel */
      }
    }
    else {  /* is special key */
      switch(c.ch[1]) {
	case 72: arrow_choice--; /* up arrow */
	  break;
 	case 80: arrow_choice++; /* down arrow */
	  break;
      }
    }
    if(arrow_choice==count) arrow_choice=0;
    if(arrow_choice<0) arrow_choice=count-1;

    /* highlight the next selection */
    goto_xy(x+arrow_choice, y);
    write_video(x+arrow_choice, y, menu[arrow_choice], REV_VID);
  }
}

/* display a string with specified attribute */
void write_video(x, y, p, attrib)
int x, y;
char *p;
int attrib;
{
  union REGS r;
  register int i,j;

  for(i=y; *p; i++) {
      goto_xy(x, i);
      r.h.ah = 9; /* write character function */
      r.h.bh = 0; /* assume active display page is 0 */
      r.x.cx = 1; /* number of times to write the character */
      r.h.al = *p++; /* character */
      r.h.bl = attrib; /* attribute */
      int86(0x10, &r, &r);
  }
}


/* save a portion of the screen */
void save_video(startx, endx, starty, endy, buf_ptr)
int startx, endx, starty, endy;
unsigned int *buf_ptr;
{
  union REGS r;
  register int i,j;
  for(i=starty; i<endy; i++)
    for(j=startx; j<endx; j++) {
      goto_xy(j, i); 
      r.h.ah = 8; /* read character function */
      r.h.bh = 0; /* assume active display page is 0 */
      *buf_ptr++ = int86(0x10, &r, &r);
      putchar(' '); /* clear the screen */
    }
}

/* restore a portion of the screen */
void restore_video(startx, endx, starty, endy, buf_ptr)
int startx, endx, starty, endy;
unsigned char *buf_ptr;
{
  union REGS r;
  register int i,j;

  for(i=starty; i<endy; i++)
    for(j=startx; j<endx; j++) {
      goto_xy(j, i);
      r.h.ah = 9; /* write character function */
      r.h.bh = 0; /* assume active display page is 0 */
      r.x.cx = 1; /* number of times to write the character */
      r.h.al = *buf_ptr++; /* character */
      r.h.bl = *buf_ptr++; /* attribute */
      int86(0x10, &r, &r);
  }
}


/* clear the screen */
void cls()
{
  union REGS r;

  r.h.ah=6; /* screen scroll code */
  r.h.al=0; /* clear screen code */
  r.h.ch=0; /* start row */
  r.h.cl=0; /* start column */
  r.h.dh=24; /* end row */
  r.h.dl=79; /* end column */
  r.h.bh=7;  /* blank line is blank */
  int86(0x10, &r, &r);
}

/* send the cursor to x,y */
void goto_xy(x,y)
int x,y;
{
  union REGS r;

  r.h.ah=2; /* cursor addressing function */
  r.h.dl=y; /* column coordinate */
  r.h.dh=x; /* row coordinate */
  r.h.bh=0; /* video page */
  int86(0x10, &r, &r);
}

is_in(s, c)
char *s, c;
{
  register int i;

  for(i=0; *s; i++) if(*s++==c) return i+1;
  return 0;
}





listing 1-15

  vmode = video_mode();
  if((vmode!=2) && (vmode!=3) && (vmode!=7)) {
    printf("video must be in 80 column text mode");
    exit(1);
  }
  /* set proper address of video RAM */
  if(vmode==7) vid_mem = (char far *) 0xB0000000;
  else vid_mem = (char far *) 0xB8000000;





listing 1-16

/* save a portion of the screen using direct
   video RAM accessing
*/
void save_video(startx, endx, starty, endy, buf_ptr)
int startx, endx, starty, endy;
unsigned char *buf_ptr;
{
  register int i,j;
  char far *v, far *t;

  v = vid_mem;
  for(i=starty; i<endy; i++)
    for(j=startx; j<endx; j++) {
      t = v + (j*160) + i*2; /* compute the address */
      *buf_ptr++ = *t++; /* read the character */
      *buf_ptr++ = *t;   /* read the attribute */
      *(t-1) = ' ';  /* clear the window */
    }
}

/* restore a portion of the screen using direct
   video RAM accessing
*/
void restore_video(startx, endx, starty, endy, buf_ptr)
int startx, endx, starty, endy;
unsigned char *buf_ptr;
{
  register int i,j;
  char far *v, far *t;

  v = vid_mem;
  t = v;
  for(i=starty; i<endy; i++)
    for(j=startx; j<endx; j++) {
      v = t;
      v += (j*160) + i*2; /* compute the address */
      *v++ = *buf_ptr++;  /* write the character */
      *v = *buf_ptr++;    /* write the attribute */
  }
}





listing 1-17

/* write character with specified attribute */
void write_char(x, y, ch, attrib)
int x, y;
char ch;
int attrib;
{
  register int i;
  char far *v;
  
  v = vid_mem;
  v += (x*160) + y*2; /* compute the address */
  *v++ = ch;  /* write the character */
  *v = attrib;    /* write the attribute */
}





listing 1-18

/* Pop-up menu routines for text mode operation
   using direct video RAM reads and writes. */

#include "stdio.h"
#include "dos.h"
#include "stdlib.h"

#define BORDER 1
#define ESC 27
#define REV_VID 0x70
#define NORM_VID 7

void save_video(), restore_video();
void goto_xy(), cls(), write_string(), write_char();
void display_menu(), draw_border();
char far *vid_mem;

char *fruit[]= {
  "Apple",
  "Orange",
  "Pear",
  "Grape",
  "Raspberry",
  "Strawberry"
};

char *color[]= {
  "Red",
  "Yellow",
  "Orange",
  "Green",
};

char *apple_type[]= {
  "Red delicious",
  "Jonathan",
  "Winesap",
  "Rome"
};

main()
{
  int i;

  cls();
  goto_xy(0,0);
  for(i=0; i<25; i++)
    printf("This is a test of the pop-up window routines.\n");

  popup(fruit, "aopgrs", 6, 1, 3, BORDER); 
  popup(color, "ryog", 4, 5, 10, BORDER); 
  popup(apple_type, "rjwr", 4, 10, 18, BORDER); 
}

/* display a pop-up menu and return selection 
   returns -2 if menu cannot be constructed
   returns -1 if user hits escape key
   otherwise the item number is returned starting
   with 0 as the first (top most) entry
*/
int popup(menu, keys, count, x, y, border)
char *menu[];  	/* menu text */
char *keys;    	/* hot keys */
int count;	/* number of menu items */
int x, y;	/* X,Y coordinates of left hand corner */
int border;	/* no border if 0 */
{
  register int i, len;
  int endx, endy, choice, vmode;
  unsigned char *p;

  if((x>24) || (x<0) || (y>79) || (y<0)) {
    printf("range error");
    return -2;
  } 

  vmode = video_mode();
  if((vmode!=2) && (vmode!=3) && (vmode!=7)) {
    printf("video must be in 80 column text mode");
    exit(1);
  }
  /* set proper address of video RAM */
  if(vmode==7) vid_mem = (char far *) 0xB0000000;
  else vid_mem = (char far *) 0xB8000000;

  /* compute the dimensions */
  len = 0;
  for(i=0; i<count; i++) 
   if(strlen(menu[i]) > len) len = strlen(menu[i]);
  endy = len + 2 + y;
  endx = count + 1 + x;
  if((endx+1>24) || (endy+1>79)) {
    printf("menu won't fit");
    return -2;
  } 

  /* allocate enough memory for it */
  p = (unsigned char *) malloc(2 * (endx-x+1) * (endy-y+1));
  if(!p) exit(1); /* install your own error handler here */

  /* save the current screen data */
  save_video(x, endx+1, y, endy+1, p);

  if(border) draw_border(x, y, endx, endy);

  /* display the menu */
  display_menu(menu, x+1, y+1, count);

  /* get the user's response */
  choice = get_resp(x+1, y, count, menu, keys);

  /* restore the original screen */
  restore_video(x, endx+1, y, endy+1,  p);
  free(p);
  return choice;
}

/* display the menu in its proper location */
void display_menu(menu, x, y, count)
char *menu[];
int x, y, count;
{
  register int i;

  for(i=0; i<count; i++, x++) 
    write_string(x, y, menu[i], NORM_VID);
}

void draw_border(startx, starty, endx, endy)
int startx, starty, endx, endy;
{
  register int i;
  char far *v, far *t;

  v = vid_mem;
  t = v;
  for(i=startx+1; i<endx; i++) {
     v += (i*160) + starty*2;
     *v++ = 179;
     *v = NORM_VID;
     v = t;
     v += (i*160) + endy*2;
     *v++ = 179;
     *v = NORM_VID;
     v = t;
  }
  for(i=starty+1; i<endy; i++) {
     v += (startx*160) + i*2;
     *v++ = 196;
     *v = NORM_VID;
     v = t;
     v += (endx*160) + i*2;
     *v++ = 196;
     *v = NORM_VID;
     v = t;
  }
  write_char(startx, starty, 218, NORM_VID);
  write_char(startx, endy, 191, NORM_VID);
  write_char(endx, starty, 192, NORM_VID);
  write_char(endx, endy, 217, NORM_VID);
}

/* input user's selection */
get_resp(x, y, count, menu, keys)
int x, y, count;
char *menu[];
char *keys;
{
  union inkey {
    char ch[2];
    int i;
  } c;
  int arrow_choice=0, key_choice;

  y++;

  /* highlight the first selection */
  goto_xy(x, y);
  write_string(x, y, menu[0], REV_VID); /* reverse video */

  for(;;) {
    while(!bioskey(1)) ; /* wait for key stroke */
    c.i = bioskey(0);    /* read the key */

    /* reset the selection to normal video */
    goto_xy(x+arrow_choice, y);
    write_string(x+arrow_choice, y, 
      menu[arrow_choice], NORM_VID); /* redisplay */

    if(c.ch[0]) { /* is normal key */
      /* see if it is a hot key */
      key_choice = is_in(keys, tolower(c.ch[0]));
      if(key_choice) return key_choice-1;
      /* check for ENTER or space bar */
      switch(c.ch[0]) {
	case '\r': return arrow_choice;
	case ' ' : arrow_choice++;
	  break;
	case ESC : return -1;  /* cancel */
      }
    }
    else {  /* is special key */
      switch(c.ch[1]) {
	case 72: arrow_choice--; /* up arrow */
	  break;
 	case 80: arrow_choice++; /* down arrow */
	  break;
      }
    }
    if(arrow_choice==count) arrow_choice=0;
    if(arrow_choice<0) arrow_choice=count-1;

    /* highlight the next selection */
    goto_xy(x+arrow_choice, y);
    write_string(x+arrow_choice, y, menu[arrow_choice], REV_VID);
  }
}

/* display a string with specified attribute */
void write_string(x, y, p, attrib)
int x, y;
char *p;
int attrib;
{
  register int i;
  char far *v;

  v = vid_mem;
  v += (x*160) + y*2;  /* compute the address */
  for(i=y; *p; i++) {
    *v++ = *p++;  /* write the character */
    *v++ = attrib;    /* write the attribute */
   }
}

/* write character with specified attribute */
void write_char(x, y, ch, attrib)
int x, y;
char ch;
int attrib;
{
  register int i;
  char far *v;
  
  v = vid_mem;
  v += (x*160) + y*2; /* compute the address */
  *v++ = ch;  /* write the character */
  *v = attrib;    /* write the attribute */
}

/* save a portion of the screen using direct
   video RAM accessing
*/
void save_video(startx, endx, starty, endy, buf_ptr)
int startx, endx, starty, endy;
unsigned char *buf_ptr;
{
  register int i,j;
  char far *v, far *t;

  v = vid_mem;
  for(i=starty; i<endy; i++)
    for(j=startx; j<endx; j++) {
      t = v + (j*160) + i*2; /* compute the address */
      *buf_ptr++ = *t++; /* read the character */
      *buf_ptr++ = *t;   /* read the attribute */
      *(t-1) = ' ';  /* clear the window */
    }
}

/* restore a portion of the screen using direct
   video RAM accessing
*/
void restore_video(startx, endx, starty, endy, buf_ptr)
int startx, endx, starty, endy;
unsigned char *buf_ptr;
{
  register int i,j;
  char far *v, far *t;

  v = vid_mem;
  t = v;
  for(i=starty; i<endy; i++)
    for(j=startx; j<endx; j++) {
      v = t;
      v += (j*160) + i*2; /* compute the address */
      *v++ = *buf_ptr++;  /* write the character */
      *v = *buf_ptr++;    /* write the attribute */
  }
}


/* clear the screen */
void cls()
{
  union REGS r;

  r.h.ah=6; /* screen scroll code */
  r.h.al=0; /* clear screen code */
  r.h.ch=0; /* start row */
  r.h.cl=0; /* start column */
  r.h.dh=24; /* end row */
  r.h.dl=79; /* end column */
  r.h.bh=7;  /* blank line is blank */
  int86(0x10, &r, &r);
}

/* send the cursor to x,y */
void goto_xy(x,y)
int x,y;
{
  union REGS r;

  r.h.ah=2; /* cursor addressing function */
  r.h.dl=y; /* column coordinate */
  r.h.dh=x; /* row coordinate */
  r.h.bh=0; /* video page */
  int86(0x10, &r, &r);
}

/* returns the current video mode */
video_mode()
{
  union REGS r;

  r.h.ah = 15;  /* get video mode */
  return int86(0x10, &r, &r) & 255;
}

is_in(s, c)
char *s, c;
{
  register int i;

  for(i=0; *s; i++) if(*s++==c) return i+1;
  return 0;
}





listing 1-19

struct menu_frame {
  int startx, endx, starty, endy;
  unsigned char *p; /* pointer to screen info */
  char **menu; /* pointer to menu strings */
  char *keys;  /* pointer to hot keys */
  int border;  /* border on/off */
  int count;   /* number of selections */
  int active;  /* is menu already active */
} frame[MAX_FRAME];





listing 1-20

/* construct a pull-down menu frame 
   1 is returned if menu frame can be constructed;
   otherwise 0 is returned.
*/
make_menu(num, menu, keys, count, x, y, border)
int num;	/* menu number */
char *menu[];  	/* menu text */
char *keys;    	/* hot keys */
int count;	/* number of menu items */
int x, y;	/* X,Y coordinates of left hand corner */
int border;	/* no border if 0 */
{
  register int i, len;
  int endx, endy, choice, vmode;
  unsigned char *p;

  if(num>MAX_FRAME) {
    printf("Too many menus\n");
    return 0;
  }

  if((x>24) || (x<0) || (y>79) || (y<0)) {
    printf("range error");
    return 0;
  } 

  /* compute the size */
  len = 0;
  for(i=0; i<count; i++) 
   if(strlen(menu[i]) > len) len = strlen(menu[i]);
  endy = len + 2 + y;
  endx = count + 1 + x;
  if((endx+1>24) || (endy+1>79)) {
    printf("menu won't fit");
    return 0;
  } 

  /* allocate enough memory to hold it */
  p = (unsigned char *) malloc(2 * (endx-x+1) * (endy-y+1));
  if(!p) exit(1); /* put your own error handler here */

  /* construct the frame */
  frame[num].startx = x; frame[num].endx = endx;
  frame[num].starty = y; frame[num].endy = endy;
  frame[num].p = p; 
  frame[num].menu = (char **) menu;
  frame[num].border = border;
  frame[num].keys = keys;
  frame[num].count = count;
  frame[num].active = 0;
  return 1;  
}





listing 1-21

/* Display a pull-down menu and return selection.
   Returns -1 if user press the escape key;
   othwerwise the number of the selection is 
   returned beginning with 0.
*/
int pulldown(num)
int num; /* frame number */
{
  int  vmode, choice;

  vmode = video_mode();
  if((vmode!=2) && (vmode!=3) && (vmode!=7)) {
    printf("video must be in 80 column text mode");
    exit(1);
  }
  /* set proper address of video RAM */
  if(vmode==7) vid_mem = (char far *) 0xB0000000;
  else vid_mem = (char far *) 0xB8000000;

  /* get active window */
  if(!frame[num].active) { /* not currently in use */ 
    save_video(num);       /* save the current screen */
    frame[num].active = 1; /* set active flag */
  }

  if(frame[num].border) draw_border(num);

  display_menu(num);	/* display the menu */
  return get_resp(num); /* return response */
}






listing 1-22


/* restore a portion of the screen */
void restore_video(num)
int num;
{
  register int i,j;
  char far *v, far *t;
  char *buf_ptr;

  buf_ptr = frame[num].p;
  v = vid_mem;
  t = v;
  for(i=frame[num].starty; i<frame[num].endy+1; i++)
    for(j=frame[num].startx; j<frame[num].endx+1; j++) {
      v = t;
      v += (j*160) + i*2; /* compute the address */
      *v++ = *buf_ptr++;  /* write the character */
      *v = *buf_ptr++;    /* write the attribute */
  }
  frame[num].active = 0; /* deactivate */
}





listing 1-23
/* Pull-down menu routines for text mode operation and
   short sample program.
*/

#include "stdio.h"
#include "dos.h"
#include "stdlib.h"

#define BORDER 1
#define ESC 27
#define MAX_FRAME 10
#define REV_VID 0x70
#define NORM_VID 7

void save_video(), restore_video(), pd_driver();
void goto_xy(), cls(), write_string(), write_char();
void display_menu(), draw_border();
char far *vid_mem;

struct menu_frame {
  int startx, endx, starty, endy;
  unsigned char *p;
  char **menu;
  char *keys;
  int border, count;
  int active;
} frame[MAX_FRAME], i;

char *fruit[]= {
  "Apple",
  "Orange",
  "Pear",
  "Grape",
  "Raspberry",
  "Strawberry"
};

char *color[]= {
  "Red",
  "Yellow",
  "Orange",
  "Green",
};

char *apple_type[]= {
  "Red delicious",
  "Jonathan",
  "Winesap",
  "Rome"
};

char *grape_type[]= {
  "Concord",
  "cAnadice",
  "Thompson",
  "Red flame"
};

main()
{
  int i;

  cls();
  goto_xy(0,0);

  /* first, create the menu frames */
  make_menu(0, fruit, "aopgrs", 6, 5, 20, BORDER); 
  make_menu(1, color, "ryog", 4, 9, 28, BORDER);
  make_menu(2, apple_type, "rjwr", 4, 12, 32, BORDER);
  make_menu(3, grape_type, "catr", 4, 9, 10, BORDER);

  printf("Select your fruit:");

  pd_driver(); /* activate the menu system */
}

void pd_driver()
{
  int choice1, choice2, selection;

  /* now, activate as needed */
  while((choice1=pulldown(0)) != -1) {
    switch(choice1) {
      case 0:  /* wants an apple */
   	while((choice2=pulldown(1)) != -1) { 
  	  if(choice2==0) {
            selection = pulldown(2); /* red apple */
  	    restore_video(2);
          }
 	}
        restore_video(1);
        break;
      case 1:
      case 2: goto_xy(1,0);
	printf("out of that selection");
	break;
      case 3:  /* wants a grape */
	selection = pulldown(3);
        restore_video(3);
        break;
      case 4:
      case 5: goto_xy(1,0);
	printf("out of that selection");
	break;
    }
  }
  restore_video(0);
}

/* display a pull-down menu and return selection */
int pulldown(num)
int num; /* menu number */
{
  int  vmode, choice;

  vmode = video_mode();
  if((vmode!=2) && (vmode!=3) && (vmode!=7)) {
    printf("video must be in 80 column text mode");
    exit(1);
  }
  /* set proper address of video RAM */
  if(vmode==7) vid_mem = (char far *) 0xB0000000;
  else vid_mem = (char far *) 0xB8000000;

  /* get active window */
  if(!frame[num].active) { /* not currently in use */ 
    save_video(num);       /* save the current screen */
    frame[num].active = 1; /* set active flag */
  }

  if(frame[num].border) draw_border(num);

  display_menu(num);	/* display the menu */
  return get_resp(num); /* return response */
}


/* construct a pull down menu frame 
   1 is returned if menu frame can be constructed
   otherwise 0 is returned.
*/
make_menu(num, menu, keys, count, x, y, border)
int num;	/* menu number */
char *menu[];  	/* menu text */
char *keys;    	/* hot keys */
int count;	/* number of menu items */
int x, y;	/* X,Y coordinates of left hand corner */
int border;	/* no border if 0 */
{
  register int i, len;
  int endx, endy, choice, vmode;
  unsigned char *p;

  if(num>MAX_FRAME) {
    printf("Too many menus\n");
    return 0;
  }

  if((x>24) || (x<0) || (y>79) || (y<0)) {
    printf("range error");
    return 0;
  } 

  /* compute the size */
  len = 0;
  for(i=0; i<count; i++) 
   if(strlen(menu[i]) > len) len = strlen(menu[i]);
  endy = len + 2 + y;
  endx = count + 1 + x;
  if((endx+1>24) || (endy+1>79)) {
    printf("menu won't fit");
    return 0;
  } 

  /* allocate enough memory to hold it */
  p = (unsigned char *) malloc(2 * (endx-x+1) * (endy-y+1));
  if(!p) exit(1); /* put your own error handler here */

  /* construct the frame */
  frame[num].startx = x; frame[num].endx = endx;
  frame[num].starty = y; frame[num].endy = endy;
  frame[num].p = p; 
  frame[num].menu = (char **) menu;
  frame[num].border = border;
  frame[num].keys = keys;
  frame[num].count = count;
  frame[num].active = 0;
  return 1;  
}

/* display the menu in its proper location */
void display_menu(num)
int num;
{
  register int i, x;
  char **m;

  x = frame[num].startx+1;
  m = frame[num].menu;

  for(i=0; i<frame[num].count; i++, x++)
    write_string(x, frame[num].starty+1,
                 m[i], NORM_VID);
}

void draw_border(num)
int num;
{
  register int i;
  char far *v, far *t;

  v = vid_mem;
  t = v;
  for(i=frame[num].startx+1; i<frame[num].endx; i++) {
     v += (i*160) + frame[num].starty*2;
     *v++ = 179;
     *v = NORM_VID;
     v = t;
     v += (i*160) + frame[num].endy*2;
     *v++ = 179;
     *v = NORM_VID;
     v = t;
  }
  for(i=frame[num].starty+1; i<frame[num].endy; i++) {
     v += (frame[num].startx*160) + i*2;
     *v++ = 196;
     *v = NORM_VID;
     v = t;
     v += (frame[num].endx*160) + i*2;
     *v++ = 196;
     *v = NORM_VID;
     v = t;
  }
  write_char(frame[num].startx, frame[num].starty, 218, NORM_VID);
  write_char(frame[num].startx, frame[num].endy, 191, NORM_VID);
  write_char(frame[num].endx, frame[num].starty, 192, NORM_VID);
  write_char(frame[num].endx, frame[num].endy, 217, NORM_VID);
}

/* input user's selection */
get_resp(num)
int num;
{
  union inkey {
    char ch[2];
    int i;
  } c;
  int arrow_choice=0, key_choice;
  int x, y;

  x = frame[num].startx+1;
  y = frame[num].starty+1;

  /* highlight the first selection */
  goto_xy(x, y);
  write_string(x, y, frame[num].menu[0], REV_VID); 

  for(;;) {
    while(!bioskey(1)) ; /* wait for key stroke */
    c.i = bioskey(0); 	 /* read the key */

    /* reset the selection to normal video */
    goto_xy(x+arrow_choice, y);
    write_string(x+arrow_choice, y, 
      frame[num].menu[arrow_choice], NORM_VID); /* redisplay */

    if(c.ch[0]) { /* is normal key */
      /* see if it is a hot key */
      key_choice = is_in(frame[num].keys, tolower(c.ch[0]));
      if(key_choice) return key_choice-1;
      /* check for ENTER or space bar */
      switch(c.ch[0]) {
	case '\r': return arrow_choice;
	case ' ' : arrow_choice++;
	  break;
	case ESC : return -1; /* cancel */
      }
    }
    else {  /* is special key */
      switch(c.ch[1]) {
	case 72: arrow_choice--; /* up arrow */
	  break;
 	case 80: arrow_choice++; /* down arrow */
	  break;
      }
    }
    if(arrow_choice==frame[num].count) arrow_choice=0;
    if(arrow_choice<0) arrow_choice = frame[num].count-1;

    /* highlight the next selection */
    goto_xy(x+arrow_choice, y);
    write_string(x+arrow_choice, y, 
      frame[num].menu[arrow_choice], REV_VID);
  }
}

/* display a string with specified attribute */
void write_string(x, y, p, attrib)
int x, y;
char *p;
int attrib;
{
  register int i;
  char far *v;

  v = vid_mem;
  v += (x*160) + y*2; /* compute the address */
  for(i=y; *p; i++) {
    *v++ = *p++;  /* write the character */
    *v++ = attrib;    /* write the attribute */
   }
}

/* write character with specified attribute */
void write_char(x, y, ch, attrib)
int x, y;
char ch;
int attrib;
{
  register int i;
  char far *v;
  
  v = vid_mem;
  v += (x*160) + y*2; /* compute the address */
  *v++ = ch;  /* write the character */
  *v = attrib;    /* write the attribute */
}

/* save a portion of the screen */
void save_video(num)
int num;
{
  register int i,j;
  char *buf_ptr;
  char far *v, far *t;

  buf_ptr = frame[num].p;
  v = vid_mem;
  for(i=frame[num].starty; i<frame[num].endy+1; i++)
    for(j=frame[num].startx; j<frame[num].endx+1; j++) {
      t = (v + (j*160) + i*2);
      *buf_ptr++ = *t++;
      *buf_ptr++ = *t;
      *(t-1) = ' ';  /* clear the window */
    }
}

/* restore a portion of the screen */
void restore_video(num)
int num;
{
  register int i,j;
  char far *v, far *t;
  char *buf_ptr;

  buf_ptr = frame[num].p;
  v = vid_mem;
  t = v;
  for(i=frame[num].starty; i<frame[num].endy+1; i++)
    for(j=frame[num].startx; j<frame[num].endx+1; j++) {
      v = t;
      v += (j*160) + i*2; /* compute the address */
      *v++ = *buf_ptr++;  /* write the character */
      *v = *buf_ptr++;    /* write the attribute */
  }
  frame[num].active = 0; /* deactivate */
}


/* clear the screen */
void cls()
{
  union REGS r;

  r.h.ah=6; /* screen scroll code */
  r.h.al=0; /* clear screen code */
  r.h.ch=0; /* start row */
  r.h.cl=0; /* start column */
  r.h.dh=24; /* end row */
  r.h.dl=79; /* end column */
  r.h.bh=7;  /* blank line is blank */
  int86(0x10, &r, &r);
}

/* send the cursor to x,y */
void goto_xy(x,y)
int x,y;
{
  union REGS r;

  r.h.ah=2; /* cursor addressing function */
  r.h.dl=y; /* column coordinate */
  r.h.dh=x; /* row coordinate */
  r.h.bh=0; /* video page */
  int86(0x10, &r, &r);
}

/* returns the current video mode */
video_mode()
{
  union REGS r;

  r.h.ah = 15;  /* get video mode */
  return int86(0x10, &r, &r) & 255;
}

is_in(s, c)
char *s, c;
{
  register int i;

  for(i=0; *s; i++) if(*s++==c) return i+1;
  return 0;
}





