/*
The programs contained herein are adapted from 

      C: Power User's Guide

by Herbert Schildt published by Osborne/McGraw-Hil, Copyright
1987, Osborne/McGraw-Hill.  Used with the permission of
Osborne/McGraw-Hill.  Program adaptations are solely the
work of Herbert Schildt and are not a publication of
Osborne/McGraw-Hill.
*/


listing 3-1

void interrupt test(bp, di, si, ds, es, dx, cx, bx,
		    ax, ip, cs, flags)
unsigned bp, di, si, ds, es, dx, cx, bx, ax, ip, cs, flags;
{
  .
  .
  .
}





listing 3-2

void interrupt tsr_ap(); /* entry to application */

main()
{
  struct address {
    char far *p;
  } ;
  /* address of the print screen interrupt */
  struct address far *addr = (struct address far *) 20;
 
  addr->p = (char far *) tsr_ap;
  set_vid_mem();
  tsr(2000);
}





listing 3-3


set_vid_mem()
{
  int vmode;

  vmode = video_mode();
  if((vmode!=2) && (vmode!=3) && (vmode!=7)) {
    printf("video must be in 80 column text mode");
    exit(1);
  }
  /* set proper address of video RAM */
  if(vmode==7) vid_mem = (char far *) 0xB0000000;
  else vid_mem = (char far *) 0xB8000000;
}





listing 3-4

/* terminate but keep resident */
tsr(size)
unsigned size;
{
  union REGS r;

  r.h.ah = 49;  /* terminate and stay resident */
  r.h.al = 0;   /* return code */
  r.x.dx = size;
  int86(0x21, &r, &r);
}





listing 3-5

/* This is the entry point into the TSR application code. */
void interrupt tsr_ap()
{
  if(!busy) {
    busy = !busy;
    window_main();
    busy = !busy;
  }
}





listing 3-6

/* Terminate and stay resident using the print screen
   interrupt.
*/
#include "dos.h"
#include "stdlib.h"

#define BORDER 1
#define ESC 27
#define MAX_FRAME 1
#define REV_VID 0x70
#define NORM_VID 7
#define BKSP 8

void interrupt tsr_ap();
void save_video(), restore_video();
void write_string(), write_char();
void display_header(), draw_border();
void window_gets();
void window_cleol(), window();
void calc();

char far *vid_mem;

struct window_frame {
  int startx, endx, starty, endy;
  int curx, cury; /* current cursor position in window */
  unsigned char *p; /* pointer to buffer */
  char *header; /* header message */
  int border; /* border on/off */
  int active; /* on screen yes/no */
} frame[MAX_FRAME];

char wp[4000]; /* buffer to hold current contents of the screen */

/* busy is set to 1 when the program is active, 0 otherwise */ 
char busy = 0; 

main()
{
  struct address {
    char far *p;
  } ;
  /* address of the print screen interrupt */
  struct address far *addr = (struct address far *) 20;
 
  addr->p = (char far *) tsr_ap;
  set_vid_mem();
  tsr(2000);
}

set_vid_mem()
{
  int vmode;

  vmode = video_mode();
  if((vmode!=2) && (vmode!=3) && (vmode!=7)) {
    printf("video must be in 80 column text mode");
    exit(1);
  }
  /* set proper address of video RAM */
  if(vmode==7) vid_mem = (char far *) 0xB0000000;
  else vid_mem = (char far *) 0xB8000000;
}

/* this is the entry point into the TSR application code */
void interrupt tsr_ap()
{
  if(!busy) {
    busy = !busy;
    window_main();
    busy = !busy;
  }
}

/* terminate but keep resident */
tsr(size)
unsigned size;
{
  union REGS r;

  r.h.ah = 49;  /* terminate and stay resident */
  r.h.al = 0;   /* return code */
  r.x.dx = size;
  int86(0x21, &r, &r);
}


window_main()
{
  /* first, create the window frames */
  make_window(0, " Calculator ", 8, 20, 12, 60, BORDER); 
  /* use window() to activate the specified window */
  calc();
}

/*********************************************************/
/* Window functions           				 */
/*********************************************************/

/* Display a pull-down window. */
void window(num)
int num; /* window number */
{
  int  vmode, choice;
  int x, y;

  /* get active window */
  if(!frame[num].active) { /* not currently in use */ 
    save_video(num);       /* save the current screen */
    frame[num].active = 1; /* set active flag */
  }

  if(frame[num].border) draw_border(num);
  display_header(num); /* display the window */
}

/* Construct a pull down window frame 
   1 is returned if window frame can be constructed;
   otherwise 0 is returned.
*/
make_window(num, header, startx, starty, endx, endy, border)
int num;	/* window number */
char *header;  	/* header text */
int startx, starty; /* X,Y coordinates of upper left corner */
int endx, endy; /* X,Y coordinates of lower right corner */
int border;	/* no border if 0 */
{
  register int i;
  int choice, vmode;
  unsigned char *p;

  if(num>MAX_FRAME) {
    window_puts(0, "Too many windows\n");
    return 0;
  }

  if((startx>24) || (startx<0) || (starty>78) || (starty<0)) {
    window_puts(0, "range error");
    return 0;
  } 

  if((endx>24) || (endy>79)) {
    window_puts(0, "window won't fit");
    return 0;
  } 

  /* construct the frame */
  frame[num].startx = startx; frame[num].endx = endx;
  frame[num].starty = starty; frame[num].endy = endy;
  frame[num].p = wp; 
  frame[num].header = header;
  frame[num].border = border;
  frame[num].active = 0;
  frame[num].curx = 0; frame[num].cury = 0;
  return 1;  
}

/* Deactivate a window and remove it from the screen. */
deactivate(num)
int num;
{
  /* reset the cursor postion to upper left corner */
  frame[num].curx = 0;
  frame[num].cury = 0;
  restore_video(num);
}

/* Display the header message in its proper location. */
void display_header(num)
int num;
{
  register int i, y, len;

  y = frame[num].starty;

  /* Calculate the correct starting position to center
     the header message - if negative, message won't
     fit.
  */
  len = strlen(frame[num].header);
  len = (frame[num].endy - y - len) / 2;
  if(len<0) return; /* don't display it */
  y = y +len;

  write_string(frame[num].startx, y, 
               frame[num].header, NORM_VID);
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

/*************************************************************/
/* Window I/O functions					     */
/*************************************************************/

/* Write a string at the current cursor position
   in the specified window.
   Returns 0 if window not active;
   1 otherwise.
*/
window_puts(num, str)
int num;
char *str;
{
   /* make sure window is active */
  if(!frame[num].active) return 0;

  for( ; *str;  str++) 
    window_putchar(num, *str);
  return 1;
}

/* Write a character at the current cursor position
   in the specified window.
   Returns 0 if window not active;
   1 otherwise.
*/
window_putchar(num, ch)
int num;
char ch;
{
  register int x, y;
  char far *v;

  /* make sure window is active */
  if(!frame[num].active) return 0;

  x = frame[num].curx + frame[num].startx + 1;
  y = frame[num].cury + frame[num].starty + 1;

  v = vid_mem;
  v += (x*160) + y*2; /* compute the address */
  if(y>=frame[num].endy) {
    return 1;
  }
  if(x>=frame[num].endx) {
    return 1;
  }

  if(ch=='\n') { /* newline char */
    x++;
    y = frame[num].startx+1;
    v = vid_mem;
    v += (x*160) + y*2; /* compute the address */
    frame[num].curx++;  /* increment X */
    frame[num].cury = 0; /* reset Y */ 
  }
  else {
    frame[num].cury++;
    *v++ = ch;  /* write the character */
    *v++ = NORM_VID;    /* normal video attribute */
  }
  window_xy(num, frame[num].curx, frame[num].cury);
  return 1;
}

/* Position cursor in a window at specified location.
   Returns 0 if out of range;
   non-zero otherwise.
*/
window_xy(num, x, y)
int num, x, y;
{
  if(x<0 || x+frame[num].startx>=frame[num].endx-1)
    return 0;
  if(y<0 || y+frame[num].starty>=frame[num].endy-1)
    return 0;
  frame[num].curx = x;
  frame[num].cury = y;
  return 1;
}

/* Read a string from a window. */
void window_gets(num, s)
int num;
char *s;
{
  char ch, *temp;

  temp = s;
  for(;;) {
    ch = window_getche(num);
    switch(ch) {
      case '\r':  /* the ENTER key is pressed */  
        *s='\0';
        return;
      case BKSP: /* backspace */
        if(s>temp) {
          s--;
          frame[num].cury--;
          if(frame[num].cury<0) frame[num].cury = 0;
            window_xy(num, frame[num].curx, frame[num].cury);
              write_char(frame[num].startx+ frame[num].curx+1, 
            frame[num].starty+frame[num].cury+1, ' ', NORM_VID);
        }
        break;
      default: *s = ch;
	s++;
    }
  }
}


/* Input keystrokes inside a window.
   Returns full 16 bit scan code.
*/
window_getche(num)
int num;
{
  union inkey {
    char ch[2];
    int i;
  } c;
  
  if(!frame[num].active) return 0; /* window not active */

  window_xy(num, frame[num].curx, frame[num].cury);

  c.i = bioskey(0); 	 /* read the key */

  if(c.ch[0]) {
    switch(c.ch[0]) {
      case '\r': /* the ENTER key is pressed */
        break;
      case BKSP: /* back space */
        break;
      default: 
        if(frame[num].cury+frame[num].starty < frame[num].endy-1) {
        write_char(frame[num].startx+ frame[num].curx+1, 
          frame[num].starty+frame[num].cury+1, c.ch[0], NORM_VID);
          frame[num].cury++;
        }
    }
    if(frame[num].curx < 0) frame[num].curx = 0;
    if(frame[num].curx+frame[num].startx > frame[num].endx-2) 
      frame[num].curx--;
    window_xy(num, frame[num].curx, frame[num].cury);
  }
  return c.i;
}

/* Clear to end of line. */
void window_cleol(num)
int num;
{
  register int i, x, y;

  x = frame[num].curx;
  y = frame[num].cury;
  window_xy(num, frame[num].curx, frame[num].cury);

  for(i=frame[num].cury; i<frame[num].endy-1; i++)
    window_putchar(num,' ');
  window_xy(num, x, y);
}

/* Move cursor up one line.
   Returns non-zero if successful;
   0 otherwise.
*/
window_upline(num)
int num;
{
  if(frame[num].curx>0) {
    frame[num].curx--;
    window_xy(num, frame[num].curx, frame[num].cury);
    return 1;
  }
  return 0;
}

/* Move cursor down one line.
   Returns non-zero if successful;
   0 otherwise.
*/
window_downline(num)
int num;
{
  if(frame[num].curx<frame[num].endx-frame[num].startx-1) {
    frame[num].curx++;
    window_xy(num, frame[num].curx, frame[num].cury);
    return 1;
  }
  return 1;
}

/* back up one character. */
window_bksp(num)
int num;
{
  if(frame[num].cury>0) {
    frame[num].cury--;
    window_xy(num, frame[num].curx, frame[num].cury);
    window_putchar(num, ' ');
    frame[num].cury--;
    window_xy(num, frame[num].curx, frame[num].cury); 
 }
}

/*****************************************************/
/* Misc. functions				     */
/*****************************************************/

/* Display a string with specifed attribute. */
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

/* Write character with specified attribute. */
void write_char(x, y, ch, attrib)
int x, y;
char ch;
int attrib;
{
  register int i;
  char far *v;
  
  v = vid_mem;
  v += (x*160) + y*2;
  *v++ = ch;  /* write the character */
  *v = attrib;    /* write the attribute */
}

/* Save a portion of the screen. */
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

/* Restore a portion of the screen. */
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
      v += (j*160) + i*2;
      *v++ = *buf_ptr++;  /* write the character */
      *v = *buf_ptr++;    /* write the attribute */
  }
  frame[num].active = 0; /* restore_video */
}

/* Returns the current video mode. */
video_mode()
{
  union REGS r;

  r.h.ah = 15;  /* get video mode */
  return int86(0x10, &r, &r) & 255;
}
/***********************************************************
calculator 
************************************************************/

#define MAX 100

int *p;   /* pointer into the stack */
int *tos; /* points to top of stack */
int *bos; /* points to bottom of stack */
char in[80], out[80];
int stack[MAX];

/* Stack based, postfix notation four-function calculator */
void calc()
{
  int answer;
  int a,b;

  p = stack;
  tos = p; 
  bos = p+MAX-1;

  window(0);
  do {
    window_xy(0, 0, 0); 
    window_cleol(0);
    window_puts(0, ": "); /* calc prompt */
    window_gets(0, in);
    window_puts(0, "\n ");
    window_cleol(0);
    switch(*in) {
      case '+':
        a = pop();
        b = pop();
        answer = a+b;
        push(a+b);
        break;
      case '-':
        a = pop();
        b = pop();
        answer = b-a;
        push(b-a);
        break;
      case '*':
        a = pop();
        b = pop();
        answer = b*a;
        push(b*a);
        break;
      case '/':
        a = pop();
        b=pop();
        if(a==0) {
            window_puts(0, "divide by 0\n");
            break;
        }
        answer = b/a;
        push(b/a);
        break;
      default:
        push(atoi(in));
 	continue;
    }
    itoa(answer, out, 10);
    window_puts(0, out);
  } while(*in);  
  deactivate(0);      
}

/* Place a number on the stack.
   Returns 1 if successful;
   0 if stack is full.
*/
push(i)  
int i;   
{
  if(p>bos) return 0;
 
  *p=i;
  p++;
  return 1;
}

/* Retrieve top element from the stack.
   Returns 0 on stack underflows.
*/ 
pop()   
{
  p--;
  if(p<tos) {
    p++;
    return 0;
  }
  return *p;
}





listing 3

main()
{
  struct address {
    char far *p;
  } temp;

  /* pointer to interrupt 9's address */
  struct address far *addr = (struct address far *) 36;
  /* pointer to interrupt 60's address */
  struct address far *int9 = (struct address far *) 240;

  /* Move the keyboard interrupt routine's address to int 60.
     If int 60 and int 61 contain the same addresses, then
     the TSR program has not been installed.
  */
  if(int9->p == (int9+1)->p) {
    int9->p = addr->p;
    addr->p = (char far *) tsr_ap;
    printf("tsr installed - F2 for note pad, F3 for calculator");
  } else {
    printf("tsr application already initialized\n");
    exit(1);
  }
 
  set_vid_mem();
  tsr(2000);
}





listing 3-7

/* This is the entry point into the TSR application code. */
void interrupt tsr_ap()
{
  char far *t = (char far *) 1050; /* address of head pointer */

  geninterrupt(60); 

  if(*t != *(t+2)) { /* if not empty */
    t += *t-30+5; /* advance to the character position */
    if(*t == 60 || *t == 61) {
      bioskey(0); /* clear the F2/F3 key */
      if(!busy) {
	busy = !busy;
	window_main(*t);
        busy = !busy;
      }
    }
  }
}





listing 3-8

/* create the windows */
window_main(which)
int which;
{
  union inkey {
    char ch[2];
    int i;
  } c;
  int i;
  char ch;

  /* first, create the window frames */
  make_window(0, " Notepad [F1 to exit] ", 5, 20, 17, 60, BORDER); 
  make_window(1, " Calculator ", 8, 20, 12, 60, BORDER); 

  /* use window() to activate the specified window */
  switch(which) {
    case 60:  
      notepad();
      break;
    case 61: 
      calc();
      break;
  }
}





listing 3-9

/* Terminate and stay resident using keyboard interrupt 9.
*/
#include "dos.h"
#include "stdlib.h"
#include "ctype.h"

#define BORDER 1
#define ESC 27
#define MAX_FRAME 2
#define REV_VID 0x70
#define NORM_VID 7
#define BKSP 8

void interrupt tsr_ap();
void save_video(), restore_video();
void write_string(), write_char();
void display_header(), draw_border();
void window_gets();
void window_cleol(), window();
void notepad(), calc();

char far *vid_mem;
char wp[4000]; /* buffer to hold current contents of the screen */

struct window_frame {
  int startx, endx, starty, endy;
  int curx, cury; /* current cursor position in window */
  unsigned char *p; /* pointer to buffer */
  char *header; /* header message */
  int border; /* border on/off */
  int active; /* on screen yes/no */
} frame[MAX_FRAME];
char in[80], out[80];

/* busy is set to 1 when the program is active, 0 otherwise */ 
char busy = 0; 

main()
{
  struct address {
    char far *p;
  } temp;

  /* pointer to interrupt 9's address */
  struct address far *addr = (struct address far *) 36;
  /* pointer to interrupt 60's address */
  struct address far *int9 = (struct address far *) 240;

  /* Move the keyboard interrupt routine's address to int 60.
     If int 60 and int 61 contain the same addresses, then
     the TSR program has not been installed.
  */
  if(int9->p == (int9+1)->p) {
    int9->p = addr->p;
    addr->p = (char far *) tsr_ap;
    printf("tsr installed - F2 for note pad, F3 for calculator");
  } else {
    printf("tsr application already initialized\n");
    exit(1);
  }
 
  set_vid_mem();
  tsr(800);
}

set_vid_mem()
{
  int vmode;

  vmode = video_mode();
  if((vmode!=2) && (vmode!=3) && (vmode!=7)) {
    printf("video must be in 80 column text mode");
    exit(1);
  }
  /* set proper address of video RAM */
  if(vmode==7) vid_mem = (char far *) 0xB0000000;
  else vid_mem = (char far *) 0xB8000000;
}

/* This is the entry point into the TSR application code. */
void interrupt tsr_ap()
{
  char far *t = (char far *) 1050; /* address of head pointer */

  geninterrupt(60); /* read the character */

  if(*t != *(t+2)) {/* if not empty */
    t += *t-30+5; /* advance to the character position */
    if(*t == 60 || *t == 61) {
      bioskey(0); /* clear the F2/F3 key */
      if(!busy) {
	busy = !busy;
	window_main(*t);
        busy = !busy;
      }
    }
  }
}

/* terminate but keep resident */
tsr(size)
unsigned size;
{
  union REGS r;

  r.h.ah = 49;  /* terminate and stay resident */
  r.h.al = 0;   /* return code */
  r.x.dx = size; /* size of program/16 */
  int86(0x21, &r, &r);
}

/* create the windows */
window_main(which)
int which;
{
  union inkey {
    char ch[2];
    int i;
  } c;
  int i;
  char ch;

  /* first, create the window frames */
  make_window(0, " Notepad [F1 to exit] ", 5, 20, 17, 60, BORDER); 
  make_window(1, " Calculator ", 8, 20, 12, 60, BORDER); 

  /* use window() to activate the specified window */
  switch(which) {
    case 60:  
      notepad();
      break;
    case 61: 
      calc();
      break;
  }
}

/*********************************************************/
/* Window functions           				 */
/*********************************************************/

/* Display a pull-down window. */
void window(num)
int num; /* window number */
{
  /* get active window */
  if(!frame[num].active) { /* not currently in use */ 
    save_video(num);       /* save the current screen */
    frame[num].active = 1; /* set active flag */
  }

  if(frame[num].border) draw_border(num);
  display_header(num); /* display the window */
}

/* Construct a pull down window frame 
   1 is returned if window frame can be constructed;
   otherwise 0 is returned.
*/
make_window(num, header, startx, starty, endx, endy, border)
int num;	/* window number */
char *header;  	/* header text */
int startx, starty; /* X,Y coordinates of upper left corner */
int endx, endy; /* X,Y coordinates of lower right corner */
int border;	/* no border if 0 */
{
  register int i;
  int choice, vmode;
  unsigned char *p;

  if(num>MAX_FRAME) {
    window_puts(0, "Too many windows\n");
    return 0;
  }

  if((startx>24) || (startx<0) || (starty>78) || (starty<0)) {
    window_puts(0, "range error");
    return 0;
  } 

  if((endx>24) || (endy>79)) {
    window_puts(0, "window won't fit");
    return 0;
  } 

  /* allocate enough memory to hold it */
/*  p = (unsigned char *) malloc(2*(endx-startx+1)*(endy-starty+1));
  if(!p) exit(1); /* put your own error handler here */


  /* construct the frame */
  frame[num].startx = startx; frame[num].endx = endx;
  frame[num].starty = starty; frame[num].endy = endy;
  frame[num].p = wp; 
  frame[num].header = header;
  frame[num].border = border;
  frame[num].active = 0;
  frame[num].curx = 0; frame[num].cury = 0;
  return 1;  
}

/* Deactivate a window and remove it from the screen. */
deactivate(num)
int num;
{
  /* reset the cursor postion to upper left corner */
  frame[num].curx = 0;
  frame[num].cury = 0;
  restore_video(num);
}

/* Display the header message in its proper location. */
void display_header(num)
int num;
{
  register int i, y, len;

  y = frame[num].starty;

  /* Calculate the correct starting position to center
     the header message - if negative, message won't
     fit.
  */
  len = strlen(frame[num].header);
  len = (frame[num].endy - y - len) / 2;
  if(len<0) return; /* don't display it */
  y = y +len;

  write_string(frame[num].startx, y, 
               frame[num].header, NORM_VID);
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

/*************************************************************/
/* Window I/O functions					     */
/*************************************************************/

/* Write a string at the current cursor position
   in the specified window.
   Returns 0 if window not active;
   1 otherwise.
*/
window_puts(num, str)
int num;
char *str;
{
   /* make sure window is active */
  if(!frame[num].active) return 0;

  for( ; *str;  str++) 
    window_putchar(num, *str);
  return 1;
}

/* Write a character at the current cursor position
   in the specified window.
   Returns 0 if window not active;
   1 otherwise.
*/
window_putchar(num, ch)
int num;
char ch;
{
  register int x, y;
  char far *v;

  /* make sure window is active */
  if(!frame[num].active) return 0;

  x = frame[num].curx + frame[num].startx + 1;
  y = frame[num].cury + frame[num].starty + 1;

  v = vid_mem;
  v += (x*160) + y*2; /* compute the address */
  if(y>=frame[num].endy) {
    return 1;
  }
  if(x>=frame[num].endx) {
    return 1;
  }

  if(ch=='\n') { /* newline char */
    x++;
    y = frame[num].startx+1;
    v = vid_mem;
    v += (x*160) + y*2; /* compute the address */
    frame[num].curx++;  /* increment X */
    frame[num].cury = 0; /* reset Y */ 
  }
  else {
    frame[num].cury++;
    *v++ = ch;  /* write the character */
    *v++ = NORM_VID;    /* normal video attribute */
  }
  window_xy(num, frame[num].curx, frame[num].cury);
  return 1;
}

/* Position cursor in a window at specified location.
   Returns 0 if out of range;
   non-zero otherwise.
*/
window_xy(num, x, y)
int num, x, y;
{
  if(x<0 || x+frame[num].startx>=frame[num].endx-1)
    return 0;
  if(y<0 || y+frame[num].starty>=frame[num].endy-1)
    return 0;
  frame[num].curx = x;
  frame[num].cury = y;
  return 1;
}

/* Read a string from a window. */
void window_gets(num, s)
int num;
char *s;
{
  char ch, *temp;
  char out[10];


  temp = s;
  for(;;) {
    ch = window_getche(num);
    switch(ch) {
      case '\r':  /* the ENTER key is pressed */  
        *s='\0';
        return;
      case BKSP: /* backspace */
        if(s>temp) {
          s--;
          frame[num].cury--;
          if(frame[num].cury<0) frame[num].cury = 0;
            window_xy(num, frame[num].curx, frame[num].cury);
              write_char(frame[num].startx+ frame[num].curx+1, 
            frame[num].starty+frame[num].cury+1, ' ', NORM_VID);
        }
        break;
      default: *s = ch;
	s++;
    }
  }
}


/* Input keystrokes inside a window.
   Returns full 16 bit scan code.
*/
window_getche(num)
int num;
{
  union inkey {
    char ch[2];
    int i;
  } c;
  
  if(!frame[num].active) return 0; /* window not active */

  window_xy(num, frame[num].curx, frame[num].cury);

  c.i = bioskey(0); 	 /* read the key */

  if(c.ch[0]) {
    switch(c.ch[0]) {
      case '\r': /* the ENTER key is pressed */
        break;
      case BKSP: /* back space */
        break;
      default: 
        if(frame[num].cury+frame[num].starty < frame[num].endy-1) {
        write_char(frame[num].startx+ frame[num].curx+1, 
          frame[num].starty+frame[num].cury+1, c.ch[0], NORM_VID);
          frame[num].cury++;
        }
    }
    if(frame[num].curx < 0) frame[num].curx = 0;
    if(frame[num].curx+frame[num].startx > frame[num].endx-2) 
      frame[num].curx--;
    window_xy(num, frame[num].curx, frame[num].cury);
  }
  return c.i;
}

/* Clear to end of line. */
void window_cleol(num)
int num;
{
  register int i, x, y;

  x = frame[num].curx;
  y = frame[num].cury;
  window_xy(num, frame[num].curx, frame[num].cury);

  for(i=frame[num].cury; i<frame[num].endy-1; i++)
    window_putchar(num,' ');
  window_xy(num, x, y);
}

/* Move cursor up one line.
   Returns non-zero if successful;
   0 otherwise.
*/
window_upline(num)
int num;
{
  if(frame[num].curx>0) {
    frame[num].curx--;
    window_xy(num, frame[num].curx, frame[num].cury);
    return 1;
  }
  return 0;
}

/* Move cursor down one line.
   Returns non-zero if successful;
   0 otherwise.
*/
window_downline(num)
int num;
{
  if(frame[num].curx<frame[num].endx-frame[num].startx-1) {
    frame[num].curx++;
    window_xy(num, frame[num].curx, frame[num].cury);
    return 1;
  }
  return 1;
}

/* back up one character. */
window_bksp(num)
int num;
{
  if(frame[num].cury>0) {
    frame[num].cury--;
    window_xy(num, frame[num].curx, frame[num].cury);
    window_putchar(num, ' ');
    frame[num].cury--;
    window_xy(num, frame[num].curx, frame[num].cury); 
 }
}

/*****************************************************/
/* Misc. functions				     */
/*****************************************************/

/* Display a string with specifed attribute. */
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

/* Write character with specified attribute. */
void write_char(x, y, ch, attrib)
int x, y;
char ch;
int attrib;
{
  register int i;
  char far *v;
  
  v = vid_mem;
  v += (x*160) + y*2;
  *v++ = ch;  /* write the character */
  *v = attrib;    /* write the attribute */
}

/* Save a portion of the screen. */
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

/* Restore a portion of the screen. */
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
      v += (j*160) + i*2;
      *v++ = *buf_ptr++;  /* write the character */
      *v = *buf_ptr++;    /* write the attribute */
  }
  frame[num].active = 0; /* restore_video */
}


/* Returns the current video mode. */
video_mode()
{
  union REGS r;

  r.h.ah = 15;  /* get video mode */
  return int86(0x10, &r, &r) & 255;
}

/***************************************************/
/* pop-up window functions 			   */
/***************************************************/

#define MAX 100

int *p;   /* pointer into the stack */
int *tos; /* points to top of stack */
int *bos; /* points to bottom of stack */
int stack[MAX];

/* Stack based, postfix notation four-function calculator */
void calc()
{
  int answer;
  int a,b;

  p = stack;
  tos = p; 
  bos = p+MAX-1;

  window(1);
  do {
    window_xy(1, 0, 0); 
    window_cleol(1);
    window_puts(1, ": "); /* calc prompt */
    window_gets(1, in);
    window_puts(1, "\n ");
    window_cleol(1);
    switch(*in) {
      case '+':
        a = pop();
        b = pop();
        answer = a+b;
        push(a+b);
        break;
      case '-':
        a = pop();
        b = pop();
        answer = b-a;
        push(b-a);
        break;
      case '*':
        a = pop();
        b = pop();
        answer = b*a;
        push(b*a);
        break;
      case '/':
        a = pop();
        b=pop();
        if(a==0) {
            window_puts(0, "divide by 0\n");
            break;
        }
        answer = b/a;
        push(b/a);
        break;
      default:
        push(atoi(in));
 	continue;
    }
    itoa(answer, out, 10);
    window_puts(1, out);
  } while(*in);  
  deactivate(1);      
}

/* Place a number on the stack.
   Returns 1 if successful;
   0 if stack is full.
*/
push(i)  
int i;   
{
  if(p>bos) return 0;
 
  *p=i;
  p++;
  return 1;
}

/* Retrieve top element from the stack.
   Returns 0 on stack underflows.
*/ 
pop()   
{
  p--;
  if(p<tos) {
    p++;
    return 0;
  }
  return *p;
}

/****************************************************/

/* Pop-up note pad. */
#define MAX_NOTE 10 
#define BKSP 8
char notes[MAX_NOTE][80];

void notepad()
{
  static first=1;
  register int i, j;
  union inkey {
    char ch[2];
    int i;
  } c;
  char ch;

  /* initialize notes array if necessary */
  if(first) {
    for(i=0; i<MAX_NOTE; i++)
      *notes[i] = '\0';
    first = !first;
  }

  window(0);
  /* display the existing notes */
  for(i=0; i<MAX_NOTE; i++) {
    if(*notes[i]) window_puts(0, notes[i]);
    window_putchar(0, '\n');
  }

  i=0;  
  window_xy(0, 0, 0);

  for(;;) {
    c.i = bioskey(0); 	 /* read the key */
    if(tolower(c.ch[1])==59) { /* F1 to quit */
      deactivate(0);
      break;
    }

    /* if normal key */
    if(isprint(c.ch[0]) || c.ch[0]==BKSP) { 
      window_cleol(0);
      notes[i][0] = c.ch[0];
      j = 1; 
      window_putchar(0, notes[i][0]);
      do {
	ch = window_getche(0);
	if(ch==BKSP) {
          if(j>0) {
            j--;
	    window_bksp(0);
	  }
	} 
	else {
	  notes[i][j] = ch;
          j++;
        }
      } while(notes[i][j-1]!='\r');
      notes[i][j-1] = '\0';
      i++;
      window_putchar(0, '\n');
    }
    else {  /* is special key */
      switch(c.ch[1]) {
        case 72:  /* up arrow */
	  if(i>0) {
	    i--;
	    window_upline(0);
	  }
	  break;
        case 80:  /* down arrow */
	  if(i<MAX_NOTE-1) {
            i++;
 	    window_downline(0);
	  }
	  break;
      }
    }
  }
}





