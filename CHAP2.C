/*
The programs contained herein are adapted from 

      C: C: Power User's Guide

by Herbert Schildt published by Osborne/McGraw-Hil, Copyright
1987, Osborne/McGraw-Hill.  Used with the permission of
Osborne/McGraw-Hill.  Program adaptations are solely the
work of Herbert Schildt and are not a publication of
Osborne/McGraw-Hill.
*/


listing 2-1

struct window_frame {
  int startx, endx, starty, endy; /* window position */
  int curx, cury; /* current cursor position in window */
  unsigned char *p; /* buffer pointer */
  char *header; /* header message */
  int border; /* border on/off */
  int active; /* on screen yes/no */
} frame[MAX_FRAME];





listing 2-2

/* Construct a pull down window frame.
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
  unsigned char *p;

  if(num>MAX_FRAME) {
    printf("Too many windows\n");
    return 0;
  }

  if((startx>24) || (startx<0) || (starty>78) || (starty<0)) {
    printf("range error");
    return 0;
  } 

  if((endx>24) || (endy>79)) {
    printf("window won't fit");
    return 0;
  } 

  /* allocate enough memory to hold it */
  p = (unsigned char *) malloc(2*(endx-startx+1)*(endy-starty+1));
  if(!p) exit(1); /* put your own error handler here */

  /* construct the frame */
  frame[num].startx = startx; frame[num].endx = endx;
  frame[num].starty = starty; frame[num].endy = endy;
  frame[num].p = p; 
  frame[num].header = header;
  frame[num].border = border;
  frame[num].active = 0;
  frame[num].curx = 0; frame[num].cury = 0;
  return 1;  
}





listing 2-3


make_window(0, " Editor [Esc to exit] ", 0, 0, 24, 78, BORDER);





listing 2-4

/* Display a pull-down window. */
void window(num)
int num; /* window number */
{
  int  vmode, choice;
  int x, y;

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
  display_header(num); /* display the window */

  x = frame[num].startx + frame[num].curx + 1;
  y = frame[num].starty + frame[num].cury + 1;
  goto_xy(x, y);
}





listing 2-5

/* Display the header message in its proper location. */
void display_header(num)
int num;
{
  register int  y, len;

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





listing 2-6

/* Deactivate a window and remove it from the screen. */
deactivate(num)
int num;
{
  /* reset the cursor postion to upper left corner */
  frame[num].curx = 0;
  frame[num].cury = 0;
  restore_video(num);
}





listing 2-7


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
  goto_xy(frame[num].startx+x+1, frame[num].starty+y+1);
  return 1;
}





listing 2-8

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
          write_char(frame[num].startx + frame[num].curx+1, 
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





listing 2-9


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





listing 2-10

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





listing 2-11

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






listing 2-12

/* Clear a window. */
void window_cls(num)
int num;
{
  register int i,j;
  char far *v, far *t;

  v = vid_mem;
  t = v;
  for(i=frame[num].starty+1; i<frame[num].endy; i++)
    for(j=frame[num].startx+1; j<frame[num].endx; j++) {
      v = t;
      v += (j*160) + i*2;
      *v++ = ' ';  /* write a space */
      *v = NORM_VID;    /* normal */
  }
  frame[num].curx = 0;
  frame[num].cury = 0;
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





listing 2-13

/* Interactively change the size of a window.
*/
void size(num)
int num;
{
  char ch;
  int x, y, startx, starty;

  /* activate if necessary */
  if(!frame[num].active) window(num);

  startx = x = frame[num].startx;
  starty = y = frame[num].starty;
  window_xy(num, 0, 0);
  
  do {
    ch = get_special();
    switch(ch) {
      case 75: /* left */
	starty--;
	break;
      case 77: /* right */
	starty++;
	break;
      case 72: /* up */
	startx--;
	break;
      case 80: /* down */
	startx++;
	break;
      case 71: /* up left */
	startx--; starty--;
	break;
      case 73: /* up right */
 	startx--; starty++;
	break;
      case 79: /* down left*/
	startx++; starty--;
	break;
      case 81: /* down right */
	startx++; starty++;
	break;
      case 60: /* F2: cancel and use original size */
        startx = x;
  	starty = y;
        ch = 59;
    }
    /* see if out-of-range */
    if(startx<0) startx++;
    if(startx>=frame[num].endx) startx--;
    if(starty<0) starty++;
    if(starty>=frame[num].endy) starty--;
    deactivate(num);
    frame[num].startx = startx;
    frame[num].starty = starty;
    window(num);
  } while(ch!=59);
  deactivate(num);
}

/* Interactively move a window 
*/
void move(num)
int num;
{
  char ch;
  int x, y, ex, ey, startx, starty, endx, endy;

  /* activate if necessary */
  if(!frame[num].active) window(num);

  startx = x = frame[num].startx;
  starty = y = frame[num].starty;
  endx = ex = frame[num].endx;
  endy = ey = frame[num].endy;
  window_xy(num, 0, 0);

  do {
    ch = get_special();
    switch(ch) {
      case 75: /* left */
	starty--;
	endy--;
	break;
      case 77: /* right */
	starty++;
	endy++;
	break;
      case 72: /* up */
	startx--;
	endx--;
	break;
      case 80: /* down */
	startx++;
	endx++;
	break;
      case 71: /* up left */
	startx--; starty--;
	endx--; endy--;
	break;
      case 73: /* up right */
 	startx--; starty++;
 	endx--; endy++;
	break;
      case 79: /* down left*/
	startx++; starty--;
	endx++; endy--;
	break;
      case 81: /* down right */
	startx++; starty++;
	endx++; endy++;
	break;
      case 60: /* F2: cancel and use original size */
        startx = x;
  	starty = y;
        endx = ex;
  	endy = ey;
        ch = 59;
    }

    /* see if out-of-range */
    if(startx<0) {
      startx++;
      endx++;
    }
    if(endx>=25) {
      startx--;
      endx--;
    }
    if(starty<0) {
      starty++;
      endy++;
    }
    if(endy>=79) {
      starty--;
      endy--;
    }
    deactivate(num);
    frame[num].startx = startx;
    frame[num].starty = starty;
    frame[num].endx = endx;
    frame[num].endy = endy;
    window(num);
  } while(ch!=59);
  deactivate(num);
}





listing 2-14
/* Decimal to hexadecimal converter. */
void dectohex()
{
  char in[80], out[80];
  int n;

  window(1);
  do {
    window_xy(1, 0, 0);  /* go to first line */
    window_cleol(1); /* clear the line */
    window_puts(1, "dec: "); /* prompt */
    window_gets(1, in); /* read the number */
    window_putchar(1, '\n'); /* go to next line */ 
    window_cleol(1); /* clear it */
    sscanf(in,"%d", &n); /* convert to internal format */
    sprintf(out, "%s%X", "hex: ",n); /* convert to hex */
    window_puts(1, out); /* output hex */
  } while(*in);
  deactivate(1);
}





listing 2-15

#define MAX 100

int *p;   /* pointer into the stack */
int *tos; /* points to top of stack */
int *bos; /* points to bottom of stack */

/* Stack based, postfix notation four-function calculator */
void calc()
{
  char in[80], out[80];
  int answer, stack[MAX];
  int a,b;

  p = stack;
  tos = p; 
  bos = p+MAX-1;

  window(2);
  do {
    window_xy(2, 0, 0); 
    window_cleol(2);
    window_puts(2, ": "); /* calc prompt */
    window_gets(2, in);
    window_puts(2, "\n ");
    window_cleol(2);
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
            window_puts("divide by 0\n");
            break;
        }
        answer = b/a;
        push(b/a);
        break;
      default:
        push(atoi(in));
 	continue;
    }
    sprintf(out, "%d", answer);
    window_puts(2, out);
  } while(*in);  
  deactivate(2);      
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





listing 2-16
#include "ctype.h"

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

  window(3);
  /* display the existing notes */
  for(i=0; i<MAX_NOTE; i++) {
    if(*notes[i]) window_puts(3, notes[i]);
    window_putchar(3, '\n');
  }

  i=0;  
  window_xy(3, 0, 0);

  for(;;) {
    c.i = window_getche(3); 	 /* read the key */
    if(tolower(c.ch[1])==59) { /* F1 to quit */
      deactivate(3);
      break;
    }

    /* if normal key read in the note */
    if(isprint(c.ch[0]) || c.ch[0]==BKSP) { 
      window_cleol(3);
      notes[i][0] = c.ch[0];
      j = 1; 
      window_putchar(3, notes[i][0]);
      do {
	ch = window_getche(3);
	if(ch==BKSP) {
          if(j>0) {
            j--;
	    window_bksp(3);
	  }
	} 
	else {
	  notes[i][j] = ch;
          j++;
        }
      } while(notes[i][j-1]!='\r');
      notes[i][j-1] = '\0';
      i++;
      window_putchar(3, '\n');
    }
    else {  /* is special key */
      switch(c.ch[1]) {
        case 72:  /* up arrow */
	  if(i>0) {
	    i--;
	    window_upline(3);
	  }
	  break;
        case 80:  /* down arrow */
	  if(i<MAX_NOTE-1) {
            i++;
 	    window_downline(3);
	  }
	  break;
      }
    }
  }
}





listing 2-17

/* Window routines with simple demostration program.
   An editor is simulated.  Three special pop-up window
   utilities illustrate both the power and magic of
   windowing software and also serve as examples of
   window programming.  The pop-up window routines
   are a four-function calculator, a decimal to hex
   converter, and a pop-up note pad.
*/

#include "stdio.h"
#include "dos.h"
#include "stdlib.h"

#define BORDER 1
#define ESC 27
#define MAX_FRAME 10
#define REV_VID 0x70
#define NORM_VID 7
#define BKSP 8

void save_video(), restore_video(), pd_driver();
void goto_xy(), cls(), write_string(), write_char();
void display_header(), draw_border();
void window_gets(), size(), move(), window_cls();
void window_cleol(), window();
void dectohex(), notepad(), calc();

char far *vid_mem;

struct window_frame {
  int startx, endx, starty, endy;
  int curx, cury; /* current cursor position in window */
  unsigned char *p; /* pointer to buffer */
  char *header; /* header message */
  int border; /* border on/off */
  int active; /* on screen yes/no */
} frame[MAX_FRAME];

main()
{
  union inkey {
    char ch[2];
    int i;
  } c;
  int i;
  char ch;

  cls();
  goto_xy(0,0);

  /* first, create the window frames */
  make_window(0, " Editor [Esc to exit] ", 0, 0, 24, 78, BORDER);
  make_window(1, " Decimal to Hex ", 7, 40, 10, 70, BORDER);
  make_window(2, " Calculator ", 8, 20, 12, 60, BORDER); 
  make_window(3, " Notepad [F1 to exit] ", 5, 20, 17, 60, BORDER); 

  /* use window() to activate the specified window */
  window(0);
  do {
    c.i = window_getche(0);
    ch = c.i; /* use only low-order byte */
    if(ch=='\r') /* must do explicit crlf */
        window_putchar(0, '\n');

    switch(c.ch[1]) { /* see if arrow or function key */
      case 59: /* F1 demostrate the window_xy() function */
        window(1);
        for(i=0; i<10; i++) 
          if(window_xy(1, i, i)) window_putchar(1,'X');
        getch();
        deactivate(1);
        break;
      case 60: /* F2 demonstrate sizing and moving a window */
        size(1);
        move(1);
        break;
      case 61: /* F3 envoke the calculator */
        calc();
        break;
      case 62: /* F4 envoke the dec to hex converter */
        dectohex();
        break;
      case 63: /* F5 envoke the note pad */
        notepad();
        break;
      case 72: /* up */
        window_upline(0);
	break;
      case 80: /* down */
	window_downline(0);
	break;
    }
  } while (ch!=ESC);
  deactivate(0); /* remove window */
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
  display_header(num); /* display the window */

  x = frame[num].startx + frame[num].curx + 1;
  y = frame[num].starty + frame[num].cury + 1;
  goto_xy(x, y);
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
  unsigned char *p;

  if(num>MAX_FRAME) {
    printf("Too many windows\n");
    return 0;
  }

  if((startx>24) || (startx<0) || (starty>78) || (starty<0)) {
    printf("range error");
    return 0;
  } 

  if((endx>24) || (endy>79)) {
    printf("window won't fit");
    return 0;
  } 

  /* allocate enough memory to hold it */
  p = (unsigned char *) malloc(2*(endx-startx+1)*(endy-starty+1));
  if(!p) exit(1); /* put your own error handler here */

  /* construct the frame */
  frame[num].startx = startx; frame[num].endx = endx;
  frame[num].starty = starty; frame[num].endy = endy;
  frame[num].p = p; 
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

/* Interactively change the size of a window.
*/
void size(num)
int num;
{
  char ch;
  int x, y, startx, starty;

  /* activate if necessary */
  if(!frame[num].active) window(num);

  startx = x = frame[num].startx;
  starty = y = frame[num].starty;
  window_xy(num, 0, 0);
  
  do {
    ch = get_special();
    switch(ch) {
      case 75: /* left */
	starty--;
	break;
      case 77: /* right */
	starty++;
	break;
      case 72: /* up */
	startx--;
	break;
      case 80: /* down */
	startx++;
	break;
      case 71: /* up left */
	startx--; starty--;
	break;
      case 73: /* up right */
 	startx--; starty++;
	break;
      case 79: /* down left*/
	startx++; starty--;
	break;
      case 81: /* down right */
	startx++; starty++;
	break;
      case 60: /* F2: cancel and use original size */
        startx = x;
  	starty = y;
        ch = 59;
    }
    /* see if out-of-range */
    if(startx<0) startx++;
    if(startx>=frame[num].endx) startx--;
    if(starty<0) starty++;
    if(starty>=frame[num].endy) starty--;
    deactivate(num);
    frame[num].startx = startx;
    frame[num].starty = starty;
    window(num);
  } while(ch!=59);
  deactivate(num);
}

/* Interactively move a window 
*/
void move(num)
int num;
{
  char ch;
  int x, y, ex, ey, startx, starty, endx, endy;

  /* activate if necessary */
  if(!frame[num].active) window(num);

  startx = x = frame[num].startx;
  starty = y = frame[num].starty;
  endx = ex = frame[num].endx;
  endy = ey = frame[num].endy;
  window_xy(num, 0, 0);

  do {
    ch = get_special();
    switch(ch) {
      case 75: /* left */
	starty--;
	endy--;
	break;
      case 77: /* right */
	starty++;
	endy++;
	break;
      case 72: /* up */
	startx--;
	endx--;
	break;
      case 80: /* down */
	startx++;
	endx++;
	break;
      case 71: /* up left */
	startx--; starty--;
	endx--; endy--;
	break;
      case 73: /* up right */
 	startx--; starty++;
 	endx--; endy++;
	break;
      case 79: /* down left*/
	startx++; starty--;
	endx++; endy--;
	break;
      case 81: /* down right */
	startx++; starty++;
	endx++; endy++;
	break;
      case 60: /* F2: cancel and use original size */
        startx = x;
  	starty = y;
        endx = ex;
  	endy = ey;
        ch = 59;
    }

    /* see if out-of-range */
    if(startx<0) {
      startx++;
      endx++;
    }
    if(endx>=25) {
      startx--;
      endx--;
    }
    if(starty<0) {
      starty++;
      endy++;
    }
    if(endy>=79) {
      starty--;
      endy--;
    }
    deactivate(num);
    frame[num].startx = startx;
    frame[num].starty = starty;
    frame[num].endx = endx;
    frame[num].endy = endy;
    window(num);
  } while(ch!=59);
  deactivate(num);
}

/* Display the header message in its proper location. */
void display_header(num)
int num;
{
  register int  y, len;

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
  goto_xy(frame[num].startx+x+1, frame[num].starty+y+1);
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

/* Clear a window. */
void window_cls(num)
int num;
{
  register int i,j;
  char far *v, far *t;

  v = vid_mem;
  t = v;
  for(i=frame[num].starty+1; i<frame[num].endy; i++)
    for(j=frame[num].startx+1; j<frame[num].endx; j++) {
      v = t;
      v += (j*160) + i*2;
      *v++ = ' ';  /* write a space */
      *v = NORM_VID;    /* normal */
  }
  frame[num].curx = 0;
  frame[num].cury = 0;
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

/* Clear the screen, */
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

/* Send the cursor to the specified X,Y position. */
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

/* Return the position code of arrow and function keys. */
get_special()
{
  union inkey {
    char ch[2];
    int i;
  } c;

 /* while(!bioskey(1)) ; /* wait for key stroke */
  c.i = bioskey(0); 	 /* read the key */

  return c.ch[1];
}

/* Returns the current video mode. */
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

#include "ctype.h"
/***************************************************/
/* pop-up window functions 			   */
/***************************************************/

#define MAX 100

int *p;   /* pointer into the stack */
int *tos; /* points to top of stack */
int *bos; /* points to bottom of stack */

/* Stack based, postfix notation four-function calculator */
void calc()
{
  char in[80], out[80];
  int answer, stack[MAX];
  int a,b;

  p = stack;
  tos = p; 
  bos = p+MAX-1;

  window(2);
  do {
    window_xy(2, 0, 0); 
    window_cleol(2);
    window_puts(2, ": "); /* calc prompt */
    window_gets(2, in);
    window_puts(2, "\n ");
    window_cleol(2);
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
            window_puts("divide by 0\n");
            break;
        }
        answer = b/a;
        push(b/a);
        break;
      default:
        push(atoi(in));
 	continue;
    }
    sprintf(out, "%d", answer);
    window_puts(2, out);
  } while(*in);  
  deactivate(2);      
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

/*****************************************************/

/* Decimal to hexadecimal converter. */
void dectohex()
{
  char in[80], out[80];
  int n;

  window(1);
  do {
    window_xy(1, 0, 0);  /* go to first line */
    window_cleol(1); /* clear the line */
    window_puts(1, "dec: "); /* prompt */
    window_gets(1, in); /* read the number */
    window_putchar(1, '\n'); /* go to next line */ 
    window_cleol(1); /* clear it */
    sscanf(in,"%d", &n); /* convert to internal format */
    sprintf(out, "%s%X", "hex: ",n); /* convert to hex */
    window_puts(1, out); /* output hex */
  } while(*in);
  deactivate(1);
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

  window(3);
  /* display the existing notes */
  for(i=0; i<MAX_NOTE; i++) {
    if(*notes[i]) window_puts(3, notes[i]);
    window_putchar(3, '\n');
  }

  i=0;  
  window_xy(3, 0, 0);

  for(;;) {
    c.i = bioskey(0); 	 /* read the key */
    if(tolower(c.ch[1])==59) { /* F1 to quit */
      deactivate(3);
      break;
    }

    /* if normal key */
    if(isprint(c.ch[0]) || c.ch[0]==BKSP) { 
      window_cleol(3);
      notes[i][0] = c.ch[0];
      j = 1; 
      window_putchar(3, notes[i][0]);
      do {
	ch = window_getche(3);
	if(ch==BKSP) {
          if(j>0) {
            j--;
	    window_bksp(3);
	  }
	} 
	else {
	  notes[i][j] = ch;
          j++;
        }
      } while(notes[i][j-1]!='\r');
      notes[i][j-1] = '\0';
      if(i<MAX_NOTE-1) i++;
      window_putchar(3, '\n');
    }
    else {  /* is special key */
      switch(c.ch[1]) {
        case 72:  /* up arrow */
	  if(i>0) {
	    i--;
	    window_upline(3);
	  }
	  break;
        case 80:  /* down arrow */
	  if(i<MAX_NOTE-1) {
            i++;
 	    window_downline(3);
	  }
	  break;
      }
    }
  }
}




