/*
The programs contained herein are adapted from 

      C: Power User's Guide

by Herbert Schildt published by Osborne/McGraw-Hil, Copyright
1987, Osborne/McGraw-Hill.  Used with the permission of
Osborne/McGraw-Hill.  Program adaptations are solely the
work of Herbert Schildt and are not a publication of
Osborne/McGraw-Hill.
*/


listing 5-1

sprite[0][0] = 0;  /* start_x */
sprite[0][1] = 0;  /* start_y */
sprite[0][2] = 0;  /* end_x */
sprite[0][3] = 10; /* end_y */





listing 5-2
/* Display an object using XOR write mode. */
void display_object(ob, sides, cc)
int ob[][4]; /* object */
int sides; /* number of sides */
int cc; /* color of object */
{
  register int i;

  for(i=0; i<sides; i++)
    line((int) ob[i][0], (int) ob[i][1],
      (int) ob[i][2], (int) ob[i][3], cc | 128);
}





listing 5-3

/* Animated sprite example. */

#include "dos.h"
#include "stdio.h"

void mode(), line();
void mempoint(), palette();
void display_object(), update_object();
unsigned char read_point();


int sprite[2][4] = {
  3, 0, 3, 5,
  0, 3, 5, 3
};

main()
{
  union k{
    char c[2];
    int i;
  } key;

  int deltax=0, deltay=0; /* direction of movement */
 
  mode(4);  /* switch to mode 4 CGA/EGA graphics */
  palette(0); /* palette 0 */
  
  display_object(sprite, 2, 1);
  do {
    deltay=deltax=0;
    key.i = bioskey(0);
    if(!key.c[0]) switch(key.c[1]) {
      case 75: /* left */
        deltay = -1;
  	break;
      case 77: /* right */
	deltay = 1;
	break;
      case 72: /* up */
	deltax = -1;
	break;
      case 80: /* down */
	deltax = 1;
	break;
      case 71: /* up left */
	deltax = -1; deltay = -1;
	break;
      case 73: /* up right */
	deltax = -1; deltay = 1;
	break;
      case 79: /* down left*/
	deltax = 1; deltay = -1;
	break;
      case 81: /* down right */
	deltax = 1; deltay = 1;
	break;
    }
    /* erase sprite's current position */
    display_object(sprite, 2, 1);
    /* if move is legal, update object definition */
    if(is_legal(sprite, deltax, deltay, 2))
      update_object(sprite, deltax, deltay, 2);
    /* redisplay sprite in new position */
    display_object(sprite, 2, 1);
  } while (key.c[0]!='q');
  getchar();
  mode(2);
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
  char far *ptr = (char far *) 0xB8000000; /* pointer to CGA memory */

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
  char far *ptr = (char far *) 0xB8000000; /* pointer to CGA memory */

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

/* Display an object using XOR write mode. */
void display_object(ob, sides, cc)
int ob[][4]; /* object */
int sides; /* number of sides */
int cc; /* color of object */
{
  register int i;

  for(i=0; i<sides; i++)
    line((int) ob[i][0], (int) ob[i][1],
      (int) ob[i][2], (int) ob[i][3], cc | 128);
}

/* Update an object by specified x, y */
void update_object(ob, x, y, sides)
int ob[][4]; /* object */
int x, y; /* increment */
register int sides; /* number of sides */
{
  sides--;
  for( ; sides>=0; sides--) {
    ob[sides][0] += x;
    ob[sides][1] += y;
    ob[sides][2] += x;
    ob[sides][3] += y;
  }
}

/* Return 1 if prospective move is legal; 0 otherwise */
is_legal(ob, x, y, sides)
int ob[][4]; /* object */
int x, y; /* increment to move */
int sides; /* number of sides the object has */
{
  if(x==0 && y==0) return 1;
  sides--;
  for( ; sides>=0; sides--) { 
    if(ob[sides][0]+x>199 || ob[sides][1]+y >319) return 0;
    if(ob[sides][2]+x<0 || ob[sides][3]+y<0) return 0;
  }
  return 1;
}





listing 5-4

int sprite2[2][4] = {
  0, 0, 5, 5,
  0, 5, 5, 0
};

main()
{
  union k{ /* holds both scan and key codes */
    char c[2];
    int i;
  } key;

  int deltax=0, deltay=0; /* direction of movement */
  int swap=0; /* controls which sprite is used. */

  mode(4);  /* switch to mode 4 CGA/EGA graphics */
  palette(0); /* palette 0 */
  

  display_object(sprite, 2, 1);
  do {
    deltay = deltax = 0;
    key.i = bioskey(0);
    if(!key.c[0]) switch(key.c[1]) {
      case 75: /* left */
        deltay = -1;
  	break;
      case 77: /* right */
	deltay = 1;
	break;
      case 72: /* up */
	deltax = -1;
	break;
      case 80: /* down */
	deltax = 1;
	break;
      case 71: /* up left */
	deltax = -1; deltay = -1;
	break;
      case 73: /* up right */
	deltax = -1; deltay = 1;
	break;
      case 79: /* down left*/
	deltax = 1; deltay = -1;
	break;
      case 81: /* down right */
	deltax = 1; deltay = 1;
	break;
    }
    /* erase sprite's current position */
    if(!swap) display_object(sprite, 2, 1);
    else display_object(sprite2, 2, 1);
    /* change the object database if move is legal */
    if(is_legal(sprite, deltax, deltay, 2)) {
      update_object(sprite, deltax, deltay, 2);
      update_object(sprite2, deltax, deltay, 2);
    }
    swap = !swap; /* switch between sprites */
    /* redisplay sprite in new location */
    if(!swap) display_object(sprite, 2, 1);
    else display_object(sprite2, 2, 1);
  } while (key.c[0]!='q');
  getchar();
  mode(2);
}





listing 5-5

/* See if a prospective move is legal. 
   Returns 1 if legal; 0 otherwise. 
*/
is_legal(ob, x, y, sides)
int ob[][4]; /* object */
int x, y; /* increment to move */
int sides; /* number of sides the object has */
{
  if(x==0 && y==0) return 1;
  sides--;
  for( ; sides>=0; sides--) {
    /* check for out of range */
    if(ob[sides][0]+x>199 || ob[sides][1]+y >319) return 0;
    if(ob[sides][2]+x<0 || ob[sides][3]+y<0) return 0;
    /* check for obstacle */
    if(read_point(ob[sides][0]+x, ob[sides][1] + y)==2) return 0;
    if(read_point(ob[sides][2]+x, ob[sides][3] + y)==2) return 0;
  }
  return 1;
}






listing 5-6


int human[4][4] = { /* your sprites */
  1, 6, 6, 6,
  4, 2, 3, 9,
  9, 1, 6, 6,
  9, 11, 6, 6
};

int human2[4][4] = {
  1, 6, 6, 6,
  4, 2, 3, 9,
  9, 3, 6, 6,
  9, 9, 6, 6
};

int computer[4][4] = { /* computer's sprites */
  180, 6, 185, 6,
  183, 2, 182, 9,
  188, 1, 185, 6,
  188, 11, 185, 6
};

int computer2[4][4] = {
  180, 6, 185, 6,
  183, 2, 182, 9,
  188, 3, 185, 6,
  188, 9, 185, 6
};





listing 5-7


int directx, directy; /* direction of human */

main()
{
  union k{ /* holds both scan and key codes */
    char c[2];
    int i;
  } key;

  int deltax=0, deltay=0; /* direction of movement */
  int swaph=0, swapc=0;
  int it=COMPUTER;
  long htime, ctime, starttime, curtime; /* score timers */
  int count; /* used to let the players separate slightly
	        after a tag */ 

  mode(4);  /* switch to mode 4 CGA/EGA graphics */
  palette(0); /* palette 0 */
  
  load_pic(); /* get the game board */

  time(&starttime); /* setup the clocks */
  htime = ctime = 0;
  
  display_object(human, 4, 1);
  display_object(computer, 4, 3);
  count = 0;

  /* main loop of the game */
  do {
    /* update the score counters */
    time(&curtime);
    if(it==COMPUTER) htime += curtime-starttime;
    else ctime += curtime-starttime;
    time(&starttime); 
    show_score(it, htime, ctime);
  
    if(bioskey(1)) { /* if keypressed */
      directx = directy = IDLE; /* reset direction for each move */
      key.i = bioskey(0); /* read the key */
      deltax = 0; deltay = 0;
      if(!key.c[0]) switch(key.c[1]) {
        case 75: /* left */
	  deltay = -1;
	  directy = LEFT;
  	  break;
        case 77: /* right */
	  deltay = 1;
	  directy = RIGHT;
	  break;
        case 72: /* up */
	  deltax = -1;
   	  directx = UP;
	  break;
        case 80: /* down */
	  deltax = 1;
	  directx = DOWN;
	  break;
        case 71: /* up left */
	  deltax = -1; deltay = -1;
	  directx = UP; directy = LEFT;
	  break;
        case 73: /* up right */
	  deltax = -1; deltay = 1;
	  directx = UP; directy = RIGHT;
	  break;
        case 79: /* down left*/
	  deltax = 1; deltay = -1;
	  directx = DOWN; directy = LEFT;
	  break;
        case 81: /* down right */
	  deltax = 1; deltay = 1;
	  directx = DOWN; directy = RIGHT;
	  break;
      }
    }
    /* turn off player's person */
    if(!swaph) display_object(human, 4, 1);
    else display_object(human2, 4, 1);
    if(is_legal(human, deltax, deltay, 4)) {
      update_object(human, deltax, deltay, 4);
      update_object(human2, deltax, deltay, 4);
    }
    /* see if a tag has occured */
    if(!count && tag(human, computer)) {
      it = !it;  /* switch who is "it" */
      count = 6; /* don't allow an instant re-tag */
    }
    swaph = !swaph; /* swap figures to simulate running */
    /* redisplay person in new position */ 
    if(!swaph) display_object(human, 4, 1);
    else display_object(human2, 4, 1);

    /* turn off computer's person */
    if(!swapc) display_object(computer, 4, 3);
    else display_object(computer2, 4, 3);
    /* generate the computer's move */
    if(it==COMPUTER) it_comp_move(computer, computer2, human, 4);
    else not_it_comp_move(computer, computer2, directx, directy, 4);
    if(!count && tag(human, computer)) {
      it = !it; /* switch who is "it" */
      count = 6; /* don't allow an instant re-tag */
      /* if computer tag's person, shift computer's  X
         position by 2 so that it is not trivial to re-tag.
      */
      if(is_legal(computer, 2, 0, 4)) { 
	update_object(computer, 2, 0, 4);
	update_object(computer2, 2, 0, 4);
      }
      else {
        update_object(computer, -2, 0, 4);
        update_object(computer2, -2, 0, 4);
      }

    }
    swapc = !swapc; /* swap figures to simulate running */
    /* display computer's person */
    if(!swapc) display_object(computer, 4, 3);
    else display_object(computer2, 4, 3);
    if(count) count--;
  } while (key.c[0]!='q' && htime<999 && ctime<999);
  mode(2);
  if(ctime>htime) printf("Computer wins!");
  else printf("You win!");
}





listing 5-8
/* Generate the computer's move when it is "it". */
void it_comp_move(ob1, ob2,  human, sides)
int ob1[][4], ob2[][4], human[][4], sides;
{
  register int  x, y, direction;
  static skip = 0;

  skip++;
  if(skip==3) {
    skip = 0;
    return; /* skip every other time to slow computer */
  }
  x = 0; y = 0;

  /* move toward the human */
  if(human[0][0]<ob1[0][0]) x = -1;
  else if(human[0][0]>ob1[0][0]) x = 1;

  if(human[0][1]<ob1[0][1]) y = -1;
  else if(human[0][1]>ob1[0][1]) y = 1;

  if(is_legal(ob1, x, y, sides)) {
    update_object(ob1, x, y, sides);
    update_object(ob2, x, y, sides);   
  }
  else { /* if not legal, try to go around */
    if(x && is_legal(ob1, x, 0, sides)) {
      update_object(ob1, x, 0, sides);
      update_object(ob2, x, 0, sides);   
    }
    else if(is_legal(ob1, 0, y, sides)) {
      update_object(ob1, 0, y, sides);
      update_object(ob2, 0, y, sides);   
    }
  }
}





listing 5-9

/* Generate the computer's move when it is not "it". */
void not_it_comp_move(ob1, ob2, directx, directy, sides)
int ob1[][4], ob2[][4];
int directx, directy; /* direction of human's last move */
int sides;
{
  register int  x, y, direction;
  static skip = 1;

  skip++;
  if(skip==3) {
    skip = 0;
    return; /* skip every other time to slow computer */
  }
  x = 0; y = 0;

  /* move in opposite direction as human */
  x = -directx;
  y = -directy;

  if(is_legal(ob1, x, y, sides)) {
    update_object(ob1, x, y, sides);
    update_object(ob2, x, y, sides);   
  }
  else { /* if not legal, try to go around */
    if(x && is_legal(ob1, x, 0, sides)) {
      update_object(ob1, x, 0, sides);
      update_object(ob2, x, 0, sides);   
    }
    else if(is_legal(ob1, 0, y, sides)) {
      update_object(ob1, 0, y, sides);
      update_object(ob2, 0, y, sides);   
    }
  }
}





listing 5-10


/* See if a tag has taken place. */
tag(ob1, ob2)
int ob1[][4], ob2[][4];
{
  register int i;
 
  /* To tag, one figure must be within one pixel of being 
     of being directly on top of the other.
  */
  for(i=-1; i<2; i++) {
    if(ob1[0][0]==ob2[0][0]+i && ob1[0][1]==ob2[0][1]+i) {
      return 1;
    }
  }
  return 0;
}





listing 5-11

/* A simple animated video game of TAG.

   The object of the game is for your "person" to
   tag the other "person" by running into them.  

   Your person is green, the computer's is yellow. No
   red boundary may be crossed.

   For the tag to count, the two "people" must be within
   one pixel of being directly on top of one another.
*/

#define COMPUTER 0
#define HUMAN 1

#define IDLE 0
#define DOWN 1
#define UP -1
#define LEFT -1
#define RIGHT 1

#include "dos.h"
#include "stdio.h"
#include "math.h"
#include "time.h"

void mode(), line();
void mempoint(), palette(), xhairs();
void goto_xy(), show_score();
void display_object(), update_object();
void save_pic(), load_pic();
void it_comp_move(), not_it_comp_move();
unsigned char read_point();


int human[4][4] = { /* your sprites */
  1, 6, 6, 6,
  4, 2, 3, 9,
  9, 1, 6, 6,
  9, 11, 6, 6
};

int human2[4][4] = {
  1, 6, 6, 6,
  4, 2, 3, 9,
  9, 3, 6, 6,
  9, 9, 6, 6
};

int computer[4][4] = { /* computer's sprites */
  180, 6, 185, 6,
  183, 2, 182, 9,
  188, 1, 185, 6,
  188, 11, 185, 6
};

int computer2[4][4] = {
  180, 6, 185, 6,
  183, 2, 182, 9,
  188, 3, 185, 6,
  188, 9, 185, 6
};

int directx, directy; /* direction of human */

main()
{
  union k{ /* holds both scan and key codes */
    char c[2];
    int i;
  } key;

  int deltax=0, deltay=0; /* direction of movement */
  int swaph=0, swapc=0;
  int it=COMPUTER;
  long htime, ctime, starttime, curtime;
  int count; /* used to let the players separate slightly
	        after a tag */ 

  mode(4);  /* switch to mode 4 CGA/EGA graphics */
  palette(0); /* palette 0 */
  
  load_pic(); /* get the game board */

  time(&starttime); /* setup the clocks */
  htime = ctime = 0;
  
  display_object(human, 4, 1);
  display_object(computer, 4, 3);
  count = 0;

  /* main game loop */
  do {
    /* update the score counters */
    time(&curtime);
    if(it==COMPUTER) htime += curtime-starttime;
    else ctime += curtime-starttime;
    time(&starttime); 
    show_score(it, htime, ctime);
  
    if(bioskey(1)) { /* if keypressed */
      directx = directy = IDLE; /* reset direction for each move */
      key.i = bioskey(0); /* read the key */
      deltax = 0; deltay = 0;
      if(!key.c[0]) switch(key.c[1]) {
        case 75: /* left */
	  deltay = -1;
	  directy = LEFT;
  	  break;
        case 77: /* right */
	  deltay = 1;
	  directy = RIGHT;
	  break;
        case 72: /* up */
	  deltax = -1;
   	  directx = UP;
	  break;
        case 80: /* down */
	  deltax = 1;
	  directx = DOWN;
	  break;
        case 71: /* up left */
	  deltax = -1; deltay = -1;
	  directx = UP; directy = LEFT;
	  break;
        case 73: /* up right */
	  deltax = -1; deltay = 1;
	  directx = UP; directy = RIGHT;
	  break;
        case 79: /* down left*/
	  deltax = 1; deltay = -1;
	  directx = DOWN; directy = LEFT;
	  break;
        case 81: /* down right */
	  deltax = 1; deltay = 1;
	  directx = DOWN; directy = RIGHT;
	  break;
      }
    }
    /* turn off player's person */
    if(!swaph) display_object(human, 4, 1);
    else display_object(human2, 4, 1);
    if(is_legal(human, deltax, deltay, 4)) {
      update_object(human, deltax, deltay, 4);
      update_object(human2, deltax, deltay, 4);
    }
    /* see if a tag has occured */
    if(!count && tag(human, computer)) {
      it = !it;  /* switch who is "it" */
      count = 6; /* don't allow an instant re-tag */
    }
    swaph = !swaph; /* swap figures to simulate running */
    /* redisplay person in new position */ 
    if(!swaph) display_object(human, 4, 1);
    else display_object(human2, 4, 1);

    /* turn off computer's person */
    if(!swapc) display_object(computer, 4, 3);
    else display_object(computer2, 4, 3);
    /* generate the computer's move */
    if(it==COMPUTER) it_comp_move(computer, computer2, human, 4);
    else not_it_comp_move(computer, computer2, directx, directy, 4);
    if(!count && tag(human, computer)) {
      it = !it; /* switch who is "it" */
      count = 6; /* don't allow an instant re-tag */
      /* if computer tag's person, shift computer's  X
         position by 2 so that it is not trivial to re-tag.
      */
      if(is_legal(computer, 2, 0, 4)) { 
	update_object(computer, 2, 0, 4);
	update_object(computer2, 2, 0, 4);
      }
      else {
        update_object(computer, -2, 0, 4);
        update_object(computer2, -2, 0, 4);
      }

    }
    swapc = !swapc; /* swap figures to simulate running */
    /* display computer's person */
    if(!swapc) display_object(computer, 4, 3);
    else display_object(computer2, 4, 3);
    if(count) count--;
  } while (key.c[0]!='q' && htime<999 && ctime<999);
  getchar();
  mode(2);
  if(ctime>htime) printf("Computer wins!");
  else printf("You win!");
}

/* Display the score. 
   When the YOU or ME is in caps, points 
   are scored.
*/
void show_score(it, htime, ctime)
int it;
long htime, ctime;
{
  goto_xy(24,6); 
  if(it==COMPUTER) printf("YOU:%ld", htime);
  else printf("you:%ld", htime);
  goto_xy(24,26); 
  if(it==HUMAN) printf("ME:%ld", ctime);
  else printf("me:%ld", ctime);
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
  char far *ptr = (char far *) 0xB8000000; /* pointer to CGA memory */

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
  char far *ptr = (char far *) 0xB8000000; /* pointer to CGA memory */

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

/* load the video graphics display */
void load_pic()
{
  char fname[80];
  FILE *fp;
  register int i, j;
  char far *ptr = (char far *) 0xB8000000; /* pointer to CGA memory */
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
  printf("Game board?: ");
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


/* Display an object. */
void display_object(ob, sides, cc)
int ob[][4]; /* object */
int sides; /* number of sides */
int cc; /* color of object */
{
  register int i;

  for(i=0; i<sides; i++)
    line((int) ob[i][0], (int) ob[i][1],
      (int) ob[i][2], (int) ob[i][3], cc | 128);
}

/* Update an object's position as specified in x, y. */
void update_object(ob, x, y, sides)
int ob[][4]; /* object */
int x, y; /* amount to update */
register int sides; /* number of sides */
{
  sides--;
  for( ; sides>=0; sides--) {
    ob[sides][0] += x;
    ob[sides][1] += y;
    ob[sides][2] += x;
    ob[sides][3] += y;
  }
}

/* See if a prospective move is legal. 
   Returns 1 if legal; 0 otherwise. 
*/
is_legal(ob, x, y, sides)
int ob[][4]; /* object */
int x, y; /* increment to move */
int sides; /* number of sides the object has */
{
  if(x==0 && y==0) return 1;
  sides--;
  for( ; sides>=0; sides--) {
    /* check for out of range */
    if(ob[sides][0]+x>199 || ob[sides][1]+y >319) return 0;
    if(ob[sides][2]+x<0 || ob[sides][3]+y<0) return 0;
    /* check for obstacle */
    if(read_point(ob[sides][0]+x, ob[sides][1] + y)==2) return 0;
    if(read_point(ob[sides][2]+x, ob[sides][3] + y)==2) return 0;
  }
  return 1;
}

/* Generate the computer's move when it is "it". */
void it_comp_move(ob1, ob2,  human, sides)
int ob1[][4], ob2[][4], human[][4], sides;
{
  register int  x, y, direction;
  static skip = 0;

  skip++;
  if(skip==3) {
    skip = 0;
    return; /* skip every other time to slow computer */
  }
  x = 0; y = 0;

  /* move toward the human */
  if(human[0][0]<ob1[0][0]) x = -1;
  else if(human[0][0]>ob1[0][0]) x = 1;

  if(human[0][1]<ob1[0][1]) y = -1;
  else if(human[0][1]>ob1[0][1]) y = 1;

  if(is_legal(ob1, x, y, sides)) {
    update_object(ob1, x, y, sides);
    update_object(ob2, x, y, sides);   
  }
  else { /* if not legal, try to go around */
    if(x && is_legal(ob1, x, 0, sides)) {
      update_object(ob1, x, 0, sides);
      update_object(ob2, x, 0, sides);   
    }
    else if(is_legal(ob1, 0, y, sides)) {
      update_object(ob1, 0, y, sides);
      update_object(ob2, 0, y, sides);   
    }
  }
}

/* Generate the computer's move when it is not "it". */
void not_it_comp_move(ob1, ob2, directx, directy, sides)
int ob1[][4], ob2[][4];
int directx, directy; /* direction of human's last move */
int sides;
{
  register int  x, y, direction;
  static skip = 1;

  skip++;
  if(skip==3) {
    skip = 0;
    return; /* skip every other time to slow computer */
  }
  x = 0; y = 0;

  /* move in opposite direction as human */
  x = -directx;
  y = -directy;

  if(is_legal(ob1, x, y, sides)) {
    update_object(ob1, x, y, sides);
    update_object(ob2, x, y, sides);   
  }
  else { /* if not legal, try to go around */
    if(x && is_legal(ob1, x, 0, sides)) {
      update_object(ob1, x, 0, sides);
      update_object(ob2, x, 0, sides);   
    }
    else if(is_legal(ob1, 0, y, sides)) {
      update_object(ob1, 0, y, sides);
      update_object(ob2, 0, y, sides);   
    }
  }
}

/* See if a tag has taken place. */
tag(ob1, ob2)
int ob1[][4], ob2[][4];
{
  register int i;
 
  /* To tag, one figure must be within one pixel of being 
     of being directly on top of the other.
  */
  for(i=-1; i<2; i++) {
    if(ob1[0][0]==ob2[0][0]+i && ob1[0][1]==ob2[0][1]+i) {
      return 1;
    }
  }
  return 0;
}




