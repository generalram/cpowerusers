/*
The programs contained herein are adapted from 

      C: Power User's Guide

by Herbert Schildt published by Osborne/McGraw-Hil, Copyright
1987, Osborne/McGraw-Hill.  Used with the permission of
Osborne/McGraw-Hill.  Program adaptations are solely the
work of Herbert Schildt and are not a publication of
Osborne/McGraw-Hill.
*/


listing 9-1

device=mouse.sys




listing 9-2

mouse




listing 9-3

/* Reset the mouse. */
void mouse_reset()
{
  int fnum, arg2, arg3, arg4;

  fnum = 0; /* reset the mouse */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  if(fnum!=-1) {
    printf("mouse hardware or software not installed");
    exit(1);
  }
  if(arg2!=2) {
   printf("two-button mouse required");
   exit(1);
  }
}




listing 9-4

/* Turn on the mouse cursor. */
void cursor_on()
{
  int fnum;

  fnum = 1; /* show the cursor */
  cmouses(&fnum, &fnum, &fnum, &fnum);
}

/* Turn off the mouse cursor. */
void cursor_off()
{
  int fnum;

  fnum = 2; /* erase the cursor */
  cmouses(&fnum, &fnum, &fnum, &fnum);
}




listing 9-5


/* Return true if right button is pressed;
   false otherwise. */
rightb_pressed()
{
  int fnum, arg2, arg3, arg4;

  fnum = 3; /* get position and button status */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  return arg2 & 2;
}

/* Return true if left button is pressed;
   false otherwise. */
leftb_pressed()
{
  int fnum, arg2, arg3, arg4;

  fnum = 3; /* get position and button status */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  return arg2 & 1;
}




listing 9-6

/* Return the direction of travel. */
void mouse_motion(deltax, deltay)
char *deltax, *deltay;
{
  int fnum, arg2, arg3, arg4;

  fnum = 11; /* get direction of motion */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  if(arg3>0) *deltax = RIGHT;
  else if(arg3<0) *deltax = LEFT;
  else *deltax = NOT_MOVED;

  if(arg4>0) *deltay = DOWN;
  else if(arg4<0) *deltay = UP;
  else *deltay = NOT_MOVED;





listing 9-7

#define NOT_MOVED 0
#define RIGHT 	  1
#define LEFT      2
#define UP        3
#define DOWN      4




listing 9-8


/* Set mouse cursor coordinates. */
void set_mouse_position(x, y)
int x, y;
{
  int fnum, arg2;

  fnum = 4; /* set position */
  cmouses(&fnum, &arg2, &x, &y);
}  

/* Return mouse cursor coordinates. */
void mouse_position(x, y)
int *x, *y;
{
  int fnum, arg2, arg3, arg4;

  fnum = 3; /* get position and button status */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  *x = arg3;
  *y = arg4;
}  




listing 9-9

/* Interfacing to the Microsoft/IBM mouse. */

#include "dos.h"

#define NOT_MOVED 0
#define RIGHT 	  1
#define LEFT      2
#define UP        3
#define DOWN      4

void mouse_position(), mode(), goto_xy(), mouse_motion();
void cursor_on(), cursor_off(), mouse_reset();

main(argc, argv)
int argc;
char *argv[];
{
  char deltax, deltay, x, y;

  if(argc!=2) {
    printf("usage:mouser <video mode>");
    exit(1);
  }

  mode(atoi(argv[1]));

  mouse_reset(); /* initialize the mouse */
  cursor_on(); /* turn on the cursor */

  do {
    goto_xy(0, 0);
    if(leftb_pressed()) printf("left button ");
    if(rightb_pressed()) {
      printf("right button");
      /* show mouse location */
      mouse_position(&x, &y);
      printf("%d %d - ", x, y);
    }

    /* see if change in position */
    mouse_motion(&deltax, &deltay);
    if(deltax || deltay) {
      printf("moving ");
      switch(deltax) {
    	case NOT_MOVED: break;
 	case RIGHT: printf("right ");
	  break;
	case LEFT: printf("left  ");
	  break;
      }
      switch(deltay) {
    	case NOT_MOVED: break;
 	case UP: printf("up    ");
	  break;
	case DOWN: printf("down  ");
	  break;
      }
    }
  /* loop until both buttons are pressed at the same time */
  } while(!(leftb_pressed() && rightb_pressed()));
  mode(3);
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

  r.h.ah=2; /* cursor addressing function */
  r.h.dl = y; /* column coordinate */
  r.h.dh = x; /* row coordinate */
  r.h.bh = 0; /* video page */
  int86(0x10, &r, &r);
}

/************************************************/
/* Mouse interface functons. 			*/
/************************************************/

/* Turn off the mouse cursor. */
void cursor_off()
{
  int fnum;

  fnum = 2; /* erase the cursor */
  cmouses(&fnum, &fnum, &fnum, &fnum);
}

/* Turn on the mouse cursor. */
void cursor_on()
{
  int fnum;

  fnum = 1; /* show the cursor */
  cmouses(&fnum, &fnum, &fnum, &fnum);
}


/* Return true if right button is pressed;
   false otherwise. */
rightb_pressed()
{
  int fnum, arg2, arg3, arg4;

  fnum = 3; /* get position and button status */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  return arg2 & 2;
}

/* Return true if left button is pressed;
   false otherwise. */
leftb_pressed()
{
  int fnum, arg2, arg3, arg4;

  fnum = 3; /* get position and button status */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  return arg2 & 1;
}

/* Set mouse cursor coordinates. */
void set_mouse_position(x, y)
int x, y;
{
  int fnum, arg2;

  fnum = 4; /* set position */
  cmouses(&fnum, &arg2, &x, &y);
}  

/* Return mouse cursor coordinates. */
void mouse_position(x, y)
int *x, *y;
{
  int fnum, arg2, arg3, arg4;

  fnum = 3; /* get position and button status */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  *x = arg3;
  *y = arg4;
}  

/* Return the direction of travel. */
void mouse_motion(deltax, deltay)
char *deltax, *deltay;
{
  int fnum, arg2, arg3, arg4;

  fnum = 11; /* get direction of motion */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  if(arg3>0) *deltax = RIGHT;
  else if(arg3<0) *deltax = LEFT;
  else *deltax = NOT_MOVED;

  if(arg4>0) *deltay = DOWN;
  else if(arg4<0) *deltay = UP;
  else *deltay = NOT_MOVED;
}

/* Reset the mouse. */
void mouse_reset()
{
  int fnum, arg2, arg3, arg4;

  fnum = 0; /* reset the mouse */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  if(fnum!=-1) {
    printf("mouse hardware or software not installed");
    exit(1);
  }
  if(arg2!=2) {
   printf("two-button mouse required");
   exit(1);
  }
}




listing 9-10

/* Return 1 when specified button released. */
void wait_on(button)
int button;
{
  if(button==LEFTB)
    while(leftb_pressed()) ;
  else
    while(rightb_pressed()) ;
}



listing 9-11

#define LEFTB   1
#define RIGHTB  2




listing 9-12
/* Display a one line mouse menu and return selection. */
mouse_menu(count, item, x, y)
int count; /* number of menu items */
char item[][20]; /* menu items */
int x, y; /* display position */
{
  int i, len[MENU_MAX][2], t;
  int mousex, mousey;

  goto_xy(x, y);
  t = 0;
  for(i=0; i<count; i++) { 
   printf("%s  ", item[i]);
   len[i][0] = t;
   /* each character is 16 pixels wide */
   len[i][1] = t + strlen(item[i])*16;
   t = len[i][1] + 32; /* add 2 for the spaces between items */
 }

  /* wait until user makes a selection */
  do {
    if(rightb_pressed() || leftb_pressed()) break;
  } while(!kbhit());
  /* wait until button not pressed */
  while(rightb_pressed() || leftb_pressed()) ;
  
  /* get the current mouse position */
  mouse_position(&mousex, &mousey);

  /* check to see if that position is on a menu selection */
  if(mousey>=0 && mousey<8) /* chars are 8 units tall */
    for(i=0; i<count; i++) {
      if(mousex>len[i][0] && mousex<len[i][1])
        return i;
    }
  return -1; /* no selection made */
}




listing 9-13

main()
{
  char done=0;

  mode(4);  /* switch to mode 4 CGA/EGA graphics */
  palette(0); /* palette 0 */

  mouse_reset(); /* initialize the mouse */

  xhairs(x, y); /* show the crosshairs */
  set_mouse_position(y*2, x); /* set initial mouse position */
  do {     
    /* see if mouse has been moved */
    mouse_motion(&deltax, &deltay); 
    if(deltax || deltay) read_mouse();
    /* check for button press */
    if(leftb_pressed() || rightb_pressed())
      read_mouse();
    if(kbhit()) {
      done = read_kb();
      /* reposition mouse to match new cross hairs location */
      set_mouse_position(y*2, x);
    }
  } while (!done);
  mode(2);
}




listing 9-14

/* Read and process a keyboard command. */
read_kb()
{
  union k{
    char c[2];
    int i;
  } key;

  key.i = bioskey(0);
  xhairs(x, y);   /* erase the crosshairs */
  if(!key.c[0]) switch(key.c[1]) {
    case 75: /* left */
      if(on_flag) line(x, y, x, y-inc, cc); 
      y -= inc;
      break;
    case 77: /* right */
      if(on_flag) line(x, y, x, y+inc, cc); 
      y += inc;
      break; 
    case 72: /* up */
      if(on_flag) line(x, y, x-inc, y, cc); 
      x -= inc;
      break;
    case 80: /* down */
      if(on_flag) line(x, y, x+inc, y, cc); 
      x += inc;
      break;
    case 71: /* up left */
      if(on_flag) line(x, y, x-inc, y-inc, cc); 
      x -= inc; y -= inc;
      break;
    case 73: /* up right */
      if(on_flag) line(x, y, x-inc, y+inc, cc); 
      x -= inc; y += inc;
      break;
    case 79: /* down left*/
      if(on_flag) line(x, y, x+inc, y-inc, cc); 
      x += inc; y -= inc;
      break;
    case 81: /* down right */
      if(on_flag) line(x, y, x+inc, y+inc, cc); 
      x += inc; y += inc;
      break;
    case 59: inc = 1;  /* F1 - slow speed */
      break;
    case 60: inc = 5;  /* F2 - fast speed */
      break;
  }
  else switch(tolower(key.c[0])) {
    case 'o': on_flag = !on_flag; /* toggle brush */
      break;
    case '1': cc = 1; /* color 1 */
      break;
    case '2': cc = 2; /* color 2 */
      break;
    case '3': cc = 3; /* color 3 */
      break;
    case '0': cc = 0; /* color 0 */
      break;
    case 'b': box(startx, starty, endx, endy, cc);
      break;
    case 'f': fill_box(startx, starty, endx, endy, cc);
      break;
    case 'l': line(startx, starty, endx, endy, cc);
      break;
    case 'c': circle(startx, starty, endy-starty, cc);
      break;
    case 'h': fill_circle(startx, starty, endy-starty, cc);
      break;
    case 's': save_pic();
      break;
    case 'r': load_pic();
      break;
    case 'm': /* move a region */
      move(startx, starty, endx, endy, x, y);
      break;
    case 'x': /* copy a region */
      copy(startx, starty, endx, endy, x, y);
      break;
    case 'd':  /* define an object to rotate */
      sides = define_object(object, x, y);
      break;
    case 'a': /* rotate the object */
      rotate_object(object, 0.05, x, y, sides);
        break;
    case '\r': /* set endpoints for line, circle, or box */
      if(first_point) {
        startx = x; starty = y;
      }
      else {
        endx = x; endy = y;
      }
      first_point = !first_point;
      break;
    case 'p': pal_num = pal_num==1 ? 2:1;
      palette(pal_num);
  }
  xhairs(x, y); /* redisplay the cross hairs */

  if(tolower(key.c[0])=='q') return 1;
  return 0;
}




listing 9-15

/* Read and process mouse input. */
read_mouse()
{
  int oldx, oldy;
  int choice;

  oldx = x; oldy = y; 
  xhairs(x, y); /* erase from current position */

  /* press both buttons to activate mouse menu */
  if(rightb_pressed() && leftb_pressed()) {
    choice = menu(); /* get mouse menu selection */
    switch(choice) {
      case 0: box(startx, starty, endx, endy, cc);
        break;
      case 1: circle(startx, starty, endy-starty, cc);
	break;
      case 2: line(startx, starty, endx, endy, cc);
	break;
      case 3: fill_box(startx, starty, endx, endy, cc);
	break;
      case 4: fill_circle(startx, starty, endy-starty, cc);
        break;
         
    }
  }
  /* right botton defines endpoints for shapes */
  else if(rightb_pressed()) {
    if(first_point) {
      startx = x; starty = y;
    }
    else {
      endx = x; endy = y;
    }
    first_point = !first_point;
    wait_on(RIGHTB); /* wait until button released */
  }
  
  if(deltax || deltay) { 
    mouse_position(&y, &x); 
    y = y / 2; /* normalize vitual screen coordinates */

    /* press left button to draw */
    if(leftb_pressed()) mouse_on_flag = 1;
    else mouse_on_flag = 0;
    if(mouse_on_flag) line(oldx, oldy, x, y, cc); 
  }
  xhairs(x, y); /* redisplay cross hairs */
}




listing 9-16


/* Display a menu. */
menu()
{
  register int i, j;
  char far *ptr = (char far *) 0xB8000000; /* pointer
                                              to CGA memory */
  char far *temp;
  unsigned char buf[14][80]; /* hold the contents of screen */ 
  int x, y, choice;
  char items[][20] = {
    "BOX",
    "CIRCLE",
    "LINE",
    "FILL BOX",
    "FILL CIRCLE"
  };

  temp = ptr;
  /* save the top of the current screen */
  for(i=0; i<14; i++)
    for(j=0; j<80; j+=2) {
      buf[i][j] = *temp; /* even byte */
      buf[i][j+1] = *(temp+8152); /* odd byte */
      *temp = 0; *(temp+8152) = 0;  /* clear top of screen */
      temp++;
    }
      
  goto_xy(0, 0);
  /* wait until last button press has cleared */
  while(rightb_pressed() || leftb_pressed()) ;

  cursor_on();
  
  choice = mouse_menu(5, items, 0, 0);

  cursor_off();  
  temp = ptr;
  /* restore the top of the current screen */
  for(i=0; i<14; i++)
    for(j=0; j<80; j+=2) {
      *temp = buf[i][j];
      *(temp+8152) = buf[i][j+1];
      temp++;
    }
  return choice;
}




listing 9-17

/* Define an object by specifying its endpoints using
   either the mouse or the keyboard. */
define_object(ob, x, y)
double ob[][4];
int x, y;
{

  union k{
    char c[2];
    int i;
  } key;
  register int i, j;
  char far *ptr = (char far *) 0xB8000000; /* pointer
                                              to CGA memory */
  char far *temp;
  unsigned char buf[14][80]; /* hold the contents of screen */ 
  int sides=0;
  int deltax, deltay, oldx, oldy;

  temp = ptr;
  /* save the top of the current screen */
  for(i=0; i<14; i++)
    for(j=0; j<80; j+=2) {
      buf[i][j] = *temp;
      buf[i][j+1] = *(temp+8152);
      *temp = 0; *(temp+8152) = 0; /* clear the top of the screen */
      temp++;
    }

  i = 0;      
  key.i = 0;
  xhairs(x, y);
  do {
    goto_xy(0, 0);
    printf("Define side %d,", sides+1);
    if(i==0) printf(" enter first endpoint");
    else printf(" enter second endpoint");

    do {
      /* mouse additions *******************************/
      /* see if mouse has moved */
      mouse_motion(&deltax, &deltay);
      /* use left button to define a point */
      if(leftb_pressed()) {
        xhairs(x, y);   /* erase the crosshairs */
	/* store coordinates of the point */
        ob[sides][i++] = (double) x;
        ob[sides][i++] = (double) y;
        if(i==4) {
          i = 0;
          sides++;
        }
        break;
      } 
    } while(!kbhit() && !deltax && !deltay);
    if(leftb_pressed()) wait_on(LEFTB); 

    if(deltax || deltay) {
      /* if mouse moved, update position */ 
      oldx = x; oldy = y;
      mouse_position(&y, &x); 
      y = y / 2; /* normalize vitual screen coordinates */
      xhairs(oldx, oldy);   /* erase the crosshairs */
    }
    /* end of mouse code ********************************/
    else if(kbhit()) {
      key.i = bioskey(0);
      xhairs(x, y);   /* plot the crosshairs */
      if(key.c[0]==13) { 
        /* use RETURN to define a point */
        ob[sides][i++] = (double) x;
        ob[sides][i++] = (double) y;
        if(i==4) {
          i = 0;
          sides++;
        }
      }
 
      /* if arrow key, move the crosshairs */
      if(!key.c[0]) switch(key.c[1]) {
        case 75: /* left */
          y-=1;
          break;
        case 77: /* right */
          y+=1;
          break;
        case 72: /* up */
          x-=1;
          break;
        case 80: /* down */
          x+=1;
          break;
        case 71: /* up left */
          x-=1; y-=1;
          break;
        case 73: /* up right */
          x-=1; y+=1;
          break;
        case 79: /* down left*/
          x+=1;y-=1;
          break;
        case 81: /* down right */
          x+=1;y+=1;
          break;
      }
     }
   if(key.c[1]!=59) xhairs(x, y);
 } while(key.c[1]!=59); /* F1 to stop */
  
  temp = ptr;
  /* restore the top of the current screen */
  for(i=0; i<14; i++)
    for(j=0; j<80; j+=2) {
      *temp = buf[i][j];
      *(temp+8152) = buf[i][j+1];
      temp++;
    }
  return sides;
}




listing 9-18

/* Ths version of the paint program for the CGA/EGA allows
   the use of a Microsoft/IBM mouse as an alternate input
   device.
 */

#define NUM_SIDES 20 /* Number of sides an object may
                        have.  Enlarge as needed */
#define NOT_MOVED 0
#define RIGHT 	  1
#define LEFT      2
#define UP        3
#define DOWN      4

#define LEFTB   1
#define RIGHTB  2

#define MENU_MAX 20 /* number of mouse menu items */

#include "dos.h"
#include "stdio.h"
#include "math.h"

void mode(), line(), box(),fill_box();
void mempoint(), palette(), xhairs();
void circle(), plot_circle(), fill_circle();
void rotate_point(), rotate_object(), goto_xy();
void display_object(), copy(), move();
void save_pic(), load_pic();
void set_mouse_position(), mouse_position(), mouse_motion();
void cursor_on(), cursor_off(), wait_on(), mouse_reset();

unsigned char read_point();

/* This array will hold the coordinates of an object 
   defined dynamcially.
*/
double object[NUM_SIDES][4];

double asp_ratio; /* holds aspect ratio for cirlces */

  int x=10, y=10; /* current screen position */
  int cc=2; /* current color */
  int on_flag=1, mouse_on_flag=0; /* pen on or off */
  int pal_num=1; /* palette number */
  /* the end points of a defined line, circle, or box */
  int startx=0, starty=0, endx=0, endy=0, first_point=1;
  int inc=1; /* movement increment */
  int  sides=0; /* number of sides of a defined object */
  int deltax, deltay; /* mouse change in position indicators */

main()
{
  char done=0;

  mode(4);  /* switch to mode 4 CGA/EGA graphics */
  palette(0); /* palette 0 */

  mouse_reset(); /* initialize the mouse */

  xhairs(x, y); /* show the crosshairs */
  set_mouse_position(y*2, x); /* set initial mouse position */
  do {     
    /* see if mouse has been moved */
    mouse_motion(&deltax, &deltay); 
    if(deltax || deltay) read_mouse();
    /* check for button press */
    if(leftb_pressed() || rightb_pressed())
      read_mouse();
    if(kbhit()) {
      done = read_kb();
      /* reposition mouse to match new cross hairs location */
      set_mouse_position(y*2, x);
    }
  } while (!done);
  mode(2);
}

/* Read and process mouse input. */
read_mouse()
{
  int oldx, oldy;
  int choice;

  oldx = x; oldy = y; 
  xhairs(x, y); /* erase from current position */

  /* press both buttons to activate mouse menu */
  if(rightb_pressed() && leftb_pressed()) {
    choice = menu(); /* get mouse menu selection */
    switch(choice) {
      case 0: box(startx, starty, endx, endy, cc);
        break;
      case 1: circle(startx, starty, endy-starty, cc);
	break;
      case 2: line(startx, starty, endx, endy, cc);
	break;
      case 3: fill_box(startx, starty, endx, endy, cc);
	break;
      case 4: fill_circle(startx, starty, endy-starty, cc);
        break;
         
    }
  }
  /* right botton defines endpoints for shapes */
  else if(rightb_pressed()) {
    if(first_point) {
      startx = x; starty = y;
    }
    else {
      endx = x; endy = y;
    }
    first_point = !first_point;
    wait_on(RIGHTB); /* wait until button released */
  }
  
  if(deltax || deltay) { 
    mouse_position(&y, &x); 
    y = y / 2; /* normalize vitual screen coordinates */

    /* press left button to draw */
    if(leftb_pressed()) mouse_on_flag = 1;
    else mouse_on_flag = 0;
    if(mouse_on_flag) line(oldx, oldy, x, y, cc); 
  }
  xhairs(x, y); /* redisplay cross hairs */
}

/* Read and process a keyboard command. */
read_kb()
{
  union k{
    char c[2];
    int i;
  } key;

  key.i = bioskey(0);
  xhairs(x, y);   /* erase the crosshairs */
  if(!key.c[0]) switch(key.c[1]) {
    case 75: /* left */
      if(on_flag) line(x, y, x, y-inc, cc); 
      y -= inc;
      break;
    case 77: /* right */
      if(on_flag) line(x, y, x, y+inc, cc); 
      y += inc;
      break; 
    case 72: /* up */
      if(on_flag) line(x, y, x-inc, y, cc); 
      x -= inc;
      break;
    case 80: /* down */
      if(on_flag) line(x, y, x+inc, y, cc); 
      x += inc;
      break;
    case 71: /* up left */
      if(on_flag) line(x, y, x-inc, y-inc, cc); 
      x -= inc; y -= inc;
      break;
    case 73: /* up right */
      if(on_flag) line(x, y, x-inc, y+inc, cc); 
      x -= inc; y += inc;
      break;
    case 79: /* down left*/
      if(on_flag) line(x, y, x+inc, y-inc, cc); 
      x += inc; y -= inc;
      break;
    case 81: /* down right */
      if(on_flag) line(x, y, x+inc, y+inc, cc); 
      x += inc; y += inc;
      break;
    case 59: inc = 1;  /* F1 - slow speed */
      break;
    case 60: inc = 5;  /* F2 - fast speed */
      break;
  }
  else switch(tolower(key.c[0])) {
    case 'o': on_flag = !on_flag; /* toggle brush */
      break;
    case '1': cc = 1; /* color 1 */
      break;
    case '2': cc = 2; /* color 2 */
      break;
    case '3': cc = 3; /* color 3 */
      break;
    case '0': cc = 0; /* color 0 */
      break;
    case 'b': box(startx, starty, endx, endy, cc);
      break;
    case 'f': fill_box(startx, starty, endx, endy, cc);
      break;
    case 'l': line(startx, starty, endx, endy, cc);
      break;
    case 'c': circle(startx, starty, endy-starty, cc);
      break;
    case 'h': fill_circle(startx, starty, endy-starty, cc);
      break;
    case 's': save_pic();
      break;
    case 'r': load_pic();
      break;
    case 'm': /* move a region */
      move(startx, starty, endx, endy, x, y);
      break;
    case 'x': /* copy a region */
      copy(startx, starty, endx, endy, x, y);
      break;
    case 'd':  /* define an object to rotate */
      sides = define_object(object, x, y);
      break;
    case 'a': /* rotate the object */
      rotate_object(object, 0.05, x, y, sides);
        break;
    case '\r': /* set endpoints for line, circle, or box */
      if(first_point) {
        startx = x; starty = y;
      }
      else {
        endx = x; endy = y;
      }
      first_point = !first_point;
      break;
    case 'p': pal_num = pal_num==1 ? 2:1;
      palette(pal_num);
  }
  xhairs(x, y); /* redisplay the cross hairs */

  if(tolower(key.c[0])=='q') return 1;
  return 0;
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


/* Draw a box. */
void box(startx, starty, endx, endy, color_code)
int startx, starty, endx, endy, color_code;
{
  line(startx, starty, endx, starty, color_code);
  line(startx, starty, startx, endy, color_code);
  line(startx, endy, endx, endy, color_code);
  line(endx, starty, endx, endy, color_code);
}
 

/* Draw a line in specified color
   using Bresenham's integer based algorithm.
*/
void line(startx, starty, endx, endy, color)
int startx, starty, endx, endy, color;
{
  register int t, distance;
  int x=0, y=0, delta_x, delta_y;
  int incx, incy;

  /* compute the distances in both directions */
  delta_x = endx-startx;
  delta_y = endy-starty;

  /* Compute the direction of the increment,
     an increment of 0 means either a vertical or horizontal
     line.
  */
  if(delta_x>0) incx = 1;
  else if(delta_x==0) incx = 0;
  else incx=-1;

  if(delta_y>0) incy = 1;
  else if(delta_y==0) incy = 0;
  else incy = -1;

  /* determine which distance is greater */
  delta_x = abs(delta_x);
  delta_y = abs(delta_y);
  if(delta_x>delta_y) distance = delta_x;
  else distance = delta_y;

  /* draw the line */
  for(t=0; t<=distance+1; t++) {
    mempoint(startx, starty, color);
    x+=delta_x;
    y+=delta_y;  
    if(x>distance) {
      x-=distance;
      startx+=incx;
    }
    if(y>distance) {
      y-=distance;
      starty+=incy;
    }
  }
}

/* Fill box with specified color. */
void fill_box(startx, starty, endx, endy, color_code)
int startx, starty, endx, endy, color_code;
{
  register int i, begin, end;

  begin = startx<endx ? startx : endx;
  end = startx>endx ? startx : endx;

  for(i=begin; i<=end;i++)
    line(i, starty, i, endy, color_code);
}

/* Draw a circle using Bresenham's integer based Algorithm. */
void circle(x_center, y_center, radius,  color_code)
int x_center, y_center, radius, color_code;
{
  register int x, y, delta;

  asp_ratio = 1.0;  /* for different aspect ratios, alter 
		   this number */

  y = radius;
  delta = 3 - 2 * radius;

  for(x=0; x<y; ) {
    plot_circle(x, y, x_center, y_center, color_code);

    if (delta < 0)
      delta += 4*x+6;
    else {
      delta += 4*(x-y)+10;
      y--;
    }
    x++;
  }
  x = y;
  if(y) plot_circle(x, y, x_center, y_center, color_code);
}

/* Plot_circle actually prints the points that
   define the circle. */
void plot_circle(x, y, x_center, y_center, color_code)
int x, y, x_center, y_center, color_code;
{
  int startx, endx, x1, starty, endy, y1;

  starty = y*asp_ratio;
  endy = (y+1)*asp_ratio;
  startx = x*asp_ratio;
  endx = (x+1)*asp_ratio;

  for (x1=startx; x1<endx; ++x1)  {
    mempoint(x1+x_center, y+y_center, color_code);
    mempoint(x1+x_center, y_center-y, color_code);
    mempoint(x_center-x1, y_center-y, color_code);
    mempoint(x_center-x1, y+y_center, color_code);
  }

  for (y1=starty; y1<endy; ++y1) {
    mempoint(y1+x_center, x+y_center, color_code);
    mempoint(y1+x_center, y_center-x, color_code);
    mempoint(x_center-y1, y_center-x, color_code);
    mempoint(x_center-y1, x+y_center, color_code);
  }
}

/* Fill a circle by repeatedly calling circle()
   with smaller radius.
*/
void fill_circle(x, y, r, c)
int x, y, r, c;
{
  while(r) {
    circle(x, y, r, c);
    r--;
  }
}

/* Display crosshair locator. */
void xhairs(x,y)
int x, y;
{
  line(x-4, y, x+3, y, 1 | 128);
  line(x, y+4, x, y-3, 1 | 128);
}

/* Write a point directly to the CGA/EGA */
void mempoint(x, y, color_code)
int x, y, color_code;
{
  union mask {
    char c[2];
    int i;
  } bit_mask;
  int i, index, bit_position;
  unsigned char t;
  char xor; /* xor color in or overwrite */
  char far *ptr = (char far *) 0xB8000000; /* pointer
                                              to CGA memory */

  bit_mask.i=0xFF3F;	/* 11111111 00111111 in binary */

  /* check range for mode 4 */
  if(x<0 || x>199 || y<0 || y>319) return;

  xor=color_code & 128; /* see if xor mode is set */
  color_code = color_code & 127; /* mask off high bit */
 
  /* set bit_mask and color_code bits to the right location */
  bit_position = y%4;
  color_code<<=2*(3-bit_position);
  bit_mask.i>>=2*bit_position;


  /* find the correct byte in screen memory */
  index = x*40 +(y >> 2);
  if(x % 2) index += 8152; /* if odd use 2nd bank */

  /* write the color */
  if(!xor) { /* overwrite mode */
    t = *(ptr+index) & bit_mask.c[0];
    *(ptr+index) = t | color_code;
  }
  else { /* xor mode */
    t = *(ptr+index) | (char)0;
    *(ptr+index) = t ^ color_code;
  }
}

/* Read byte directly from the CGA/EGA in mode 4. */
unsigned char read_point(x, y)
int x, y;
{
  union mask {
    char c[2];
    int i;
  } bit_mask;
  int i, index, bit_position;
  unsigned char t;
  char xor; /* xor color in or overwrite */
  char far *ptr = (char far *) 0xB8000000; /* pointer
                                              to CGA memory */

  bit_mask.i=3;	/* 11111111 00111111 in binary */

  /* check range for mode 4 */
  if(x<0 || x>199 || y<0 || y>319) return 0;

  /* set bit_mask and color_code bits to the right location */
  bit_position = y%4;
  bit_mask.i<<=2*(3-bit_position);
  /* find the correct byte in screen memory */
  index = x*40 +(y >> 2);
  if(x % 2) index += 8152; /* if odd use 2nd bank */

  /* read the color */
  t = *(ptr+index) & bit_mask.c[0];
  t >>=2*(3-bit_position);
  return t;
}

/* save the video graphics display */
void save_pic()
{
  char fname[80];
  FILE *fp;
  register int i, j;
  char far *ptr = (char far *) 0xB8000000; /* pointer
                                              to CGA memory */
  char far *temp;
  unsigned char buf[14][80]; /* hold the contents of screen */ 

  temp = ptr;
  /* save the top of the current screen */
  for(i=0; i<14; i++)
    for(j=0; j<80; j+=2) {
      buf[i][j] = *temp; /* even byte */
      buf[i][j+1] = *(temp+8152); /* odd byte */
      *temp = 0; *(temp+8152) = 0;  /* clear top of screen */
      temp++;
    }
      
  goto_xy(0, 0);
  printf("Filename: ");
  gets(fname);
  if(!(fp=fopen(fname, "wb"))) {
    printf("cannot open file\n");
    return;
  }

  temp = ptr;
  /* restore the top of the current screen */
  for(i=0; i<14; i++)
    for(j=0; j<80; j+=2) {
      *temp = buf[i][j];
      *(temp+8152) = buf[i][j+1];
      temp++;
    }

 /* save image to file */ 
 for(i=0; i<8152; i++) { 
    putc(*ptr, fp); /* even byte */
    putc(*(ptr+8152), fp); /* odd byte */
    ptr++;
  }
 
  fclose(fp);
}

/* load the video graphics display */
void load_pic()
{
  char fname[80];
  FILE *fp;
  register int i, j;
  char far *ptr = (char far *) 0xB8000000; /* pointer
                                              to CGA memory */
  char far *temp;
  unsigned char buf[14][80]; /* hold the contents of screen */ 

  temp = ptr;
  /* save the top of the current screen */
  for(i=0; i<14; i++)
    for(j=0; j<80; j+=2) {
      buf[i][j] = *temp;
      buf[i][j+1] = *(temp+8152);
      *temp = 0; *(temp+8152) = 0; /* clear the top of the screen */
      temp++;
    }
      
  goto_xy(0, 0);
  printf("Filename: ");
  gets(fname);
  if(!(fp=fopen(fname, "rb"))) {
    goto_xy(0, 0);
    printf("cannot open file\n");
    temp = ptr;
    /* restore the top of the current screen */
    for(i=0; i<14; i++)
      for(j=0; j<80; j+=2) {
        *temp = buf[i][j];
        *(temp+8152) = buf[i][j+1];
        temp++;
      }
    return;
  }

  /* load image from file */
  for(i=0; i<8152; i++) { 
    *ptr = getc(fp); /* even byte */
    *(ptr+8152) = getc(fp); /* odd byte */
    ptr++;
  }

  fclose(fp);
}

/* Send the cursor to the specified X,Y position. */
void goto_xy(x, y)
int x, y;
{
  union REGS r;

  r.h.ah=2; /* cursor addressing function */
  r.h.dl = y; /* column coordinate */
  r.h.dh = x; /* row coordinate */
  r.h.bh = 0; /* video page */
  int86(0x10, &r, &r);
}

/* Move one region to another location. */
void move(startx, starty, endx, endy, x, y)
int startx, starty; /* upper left coordinate */
int endx, endy; /* lower right coordinate of region to move */
int x, y; /* upper left of region recieving the image */
{ 
  int i, j;
  unsigned char c;

  for(; startx<=endx; startx++, x++)
    for(i=starty, j=y; i<=endy; i++, j++) {	
      c = read_point(startx, i); /* read point */
      mempoint(startx, i, 0); /* erase old image */
      mempoint(x, j, c); /* write it to new location */
    }
}

/* Copy one region to another location. */
void copy(startx, starty, endx, endy, x, y)
int startx, starty; /* upper left coordinate */
int endx, endy; /* lower right coordinate of region to copy */
int x, y; /* upper left of region recieving the image */
{ 
  int i, j;
  unsigned char c;

  for(; startx<=endx; startx++, x++)
    for(i=starty, j=y; i<=endy; i++, j++) {	
      c = read_point(startx, i); /* read point */
      mempoint(x, j, c); /* write it to new location */
    }
}


/* Rotate a point around the origin, specified by
   x_org and y_org,  by angle theta. */
void rotate_point(theta, x, y, x_org, y_org)
double theta, *x, *y;
int x_org, y_org;
{
  double tx, ty;

  /* normalize x and y to origin */
  tx = *x - x_org;  
  ty = *y - y_org;
 
  /* rotate */
  *x = tx * cos(theta) - ty * sin(theta);
  *y = tx * sin(theta) + ty * cos(theta);

  /* return to PC coordinate values */
  *x += x_org;
  *y += y_org;

}

/* Rotate the specified object. */
void rotate_object(ob, theta, x, y, sides)
double ob[][4]; /* object definition */
double theta; /* angle of rotation in radians */
int x, y; /* location of origin */
int sides;
{
  register int i, j;
  double tempx, tempy;  /* these help with the type conversions */
  char ch;

  for(;;) {
    ch = getch(); /* see which direction to rotate */
    switch(tolower(ch)) {
      case 'l': /* rotate counterclockwise */
	theta = theta < 0 ? -theta : theta;
  	break;
      case 'r': /* rotate clockwise */
	theta = theta > 0 ? -theta : theta;
        break;
      default: return;
    }

    for(j=0; j<sides; j++) {     
      /* erase old line */
      line((int) ob[j][0], (int) ob[j][1],
        (int) ob[j][2], (int) ob[j][3], 0);
 
      rotate_point(theta, &ob[j][0], 
        &ob[j][1], x, y);

      rotate_point(theta, &ob[j][2], 
        &ob[j][3], x, y);

      line((int)ob[j][0], (int) ob[j][1],
        (int) ob[j][2], (int) ob[j][3], 2);
    }
  }
}

/* Display an object. */
void display_object(ob, sides)
double ob[][4];
int sides;
{
  register int i;

  for(i=0; i<sides; i++)
    line((int) ob[i][0], (int) ob[i][1],
      (int) ob[i][2], (int) ob[i][3], 2);
}

/* Define an object by specifying its endpoints using
   either the mouse or the keyboard. */
define_object(ob, x, y)
double ob[][4];
int x, y;
{

  union k{
    char c[2];
    int i;
  } key;
  register int i, j;
  char far *ptr = (char far *) 0xB8000000; /* pointer
                                              to CGA memory */
  char far *temp;
  unsigned char buf[14][80]; /* hold the contents of screen */ 
  int sides=0;
  int deltax, deltay, oldx, oldy;

  temp = ptr;
  /* save the top of the current screen */
  for(i=0; i<14; i++)
    for(j=0; j<80; j+=2) {
      buf[i][j] = *temp;
      buf[i][j+1] = *(temp+8152);
      *temp = 0; *(temp+8152) = 0; /* clear the top of the screen */
      temp++;
    }

  i = 0;      
  key.i = 0;
  xhairs(x, y);
  do {
    goto_xy(0, 0);
    printf("Define side %d,", sides+1);
    if(i==0) printf(" enter first endpoint");
    else printf(" enter second endpoint");

    do {
      /* mouse additions *******************************/
      /* see if mouse has moved */
      mouse_motion(&deltax, &deltay);
      /* use left button to define a point */
      if(leftb_pressed()) {
        xhairs(x, y);   /* erase the crosshairs */
	/* store coordinates of the point */
        ob[sides][i++] = (double) x;
        ob[sides][i++] = (double) y;
        if(i==4) {
          i = 0;
          sides++;
        }
        break;
      } 
    } while(!kbhit() && !deltax && !deltay);
    if(leftb_pressed()) wait_on(LEFTB); 

    if(deltax || deltay) {
      /* if mouse moved, update position */ 
      oldx = x; oldy = y;
      mouse_position(&y, &x); 
      y = y / 2; /* normalize vitual screen coordinates */
      xhairs(oldx, oldy);   /* erase the crosshairs */
    }
    /* end of mouse code ********************************/
    else if(kbhit()) {
      key.i = bioskey(0);
      xhairs(x, y);   /* plot the crosshairs */
      if(key.c[0]==13) { 
        /* use RETURN to define a point */
        ob[sides][i++] = (double) x;
        ob[sides][i++] = (double) y;
        if(i==4) {
          i = 0;
          sides++;
        }
      }
 
      /* if arrow key, move the crosshairs */
      if(!key.c[0]) switch(key.c[1]) {
        case 75: /* left */
          y-=1;
          break;
        case 77: /* right */
          y+=1;
          break;
        case 72: /* up */
          x-=1;
          break;
        case 80: /* down */
          x+=1;
          break;
        case 71: /* up left */
          x-=1; y-=1;
          break;
        case 73: /* up right */
          x-=1; y+=1;
          break;
        case 79: /* down left*/
          x+=1;y-=1;
          break;
        case 81: /* down right */
          x+=1;y+=1;
          break;
      }
     }
   if(key.c[1]!=59) xhairs(x, y);
 } while(key.c[1]!=59); /* F1 to stop */
  
  temp = ptr;
  /* restore the top of the current screen */
  for(i=0; i<14; i++)
    for(j=0; j<80; j+=2) {
      *temp = buf[i][j];
      *(temp+8152) = buf[i][j+1];
      temp++;
    }
  return sides;
}

/* Display a menu. */
menu()
{
  register int i, j;
  char far *ptr = (char far *) 0xB8000000; /* pointer
                                              to CGA memory */
  char far *temp;
  unsigned char buf[14][80]; /* hold the contents of screen */ 
  int x, y, choice;
  char items[][20] = {
    "BOX",
    "CIRCLE",
    "LINE",
    "FILL BOX",
    "FILL CIRCLE"
  };

  temp = ptr;
  /* save the top of the current screen */
  for(i=0; i<14; i++)
    for(j=0; j<80; j+=2) {
      buf[i][j] = *temp; /* even byte */
      buf[i][j+1] = *(temp+8152); /* odd byte */
      *temp = 0; *(temp+8152) = 0;  /* clear top of screen */
      temp++;
    }
      
  goto_xy(0, 0);
  /* wait until last button press has cleared */
  while(rightb_pressed() || leftb_pressed()) ;
  cursor_on();
  
  choice = mouse_menu(5, items, 0, 0);

  cursor_off();  
  temp = ptr;
  /* restore the top of the current screen */
  for(i=0; i<14; i++)
    for(j=0; j<80; j+=2) {
      *temp = buf[i][j];
      *(temp+8152) = buf[i][j+1];
      temp++;
    }
  return choice;
}

/************************************************/
/* Mouse interface functons. 			*/
/************************************************/

/* Turn off the mouse cursor. */
void cursor_off()
{
  int fnum;

  fnum = 2; /* erase the cursor */
  cmouses(&fnum, &fnum, &fnum, &fnum);
}

/* Turn on the mouse cursor. */
void cursor_on()
{
  int fnum;

  fnum = 1; /* show the cursor */
  cmouses(&fnum, &fnum, &fnum, &fnum);
}


/* Return true if right button is pressed;
   false otherwise. */
rightb_pressed()
{
  int fnum, arg2, arg3, arg4;

  fnum = 3; /* get position and button status */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  return arg2 & 2;
}

/* Return true if left button is pressed;
   false otherwise. */
leftb_pressed()
{
  int fnum, arg2, arg3, arg4;

  fnum = 3; /* get position and button status */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  return arg2 & 1;
}

/* Set mouse cursor coordinates. */
void set_mouse_position(x, y)
int x, y;
{
  int fnum, arg2;

  fnum = 4; /* set position */
  cmouses(&fnum, &arg2, &x, &y);
}  

/* Return mouse cursor coordinates. */
void mouse_position(x, y)
int *x, *y;
{
  int fnum, arg2, arg3, arg4;

  fnum = 3; /* get position and button status */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  *x = arg3;
  *y = arg4;
}  

/* Return the direction of travel. */
void mouse_motion(deltax, deltay)
char *deltax, *deltay;
{
  int fnum, arg2, arg3, arg4;

  fnum = 11; /* get direction of motion */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  if(arg3>0) *deltax = RIGHT;
  else if(arg3<0) *deltax = LEFT;
  else *deltax = NOT_MOVED;

  if(arg4>0) *deltay = DOWN;
  else if(arg4<0) *deltay = UP;
  else *deltay = NOT_MOVED;
}

/* Display a one line mouse menu and return selection. */
mouse_menu(count, item, x, y)
int count; /* number of menu items */
char item[][20]; /* menu items */
int x, y; /* display position */
{
  int i, len[MENU_MAX][2], t;
  int mousex, mousey;

  goto_xy(x, y);
  t = 0;
  for(i=0; i<count; i++) { 
   printf("%s  ", item[i]);
   len[i][0] = t;
   len[i][1] = t + strlen(item[i])*16;
   t = len[i][1] + 32;
 }

  do {
    if(rightb_pressed() || leftb_pressed()) break;
  } while(!kbhit());
  /* wait until button not pressed */
  while(rightb_pressed() || leftb_pressed()) ;
  
  mouse_position(&mousex, &mousey);

  if(mousey>=0 && mousey<8)
    for(i=0; i<count; i++) {
      if(mousex>len[i][0] && mousex<len[i][1])
        return i;
    }
  return -1; /* no selection made */
}

/* Return 1 when specified button released. */
void wait_on(button)
int button;
{
  if(button==LEFTB)
    while(leftb_pressed()) ;
  else
    while(rightb_pressed()) ;
}

/* Reset the mouse. */
void mouse_reset()
{
  int fnum, arg2, arg3, arg4;

  fnum = 0; /* reset the mouse */
  cmouses(&fnum, &arg2, &arg3, &arg4);
  if(fnum!=-1) {
    printf("mouse hardware or software not installed");
    exit(1);
  }
  if(arg2!=2) {
   printf("two-button mouse required");
   exit(1);
  }
}


