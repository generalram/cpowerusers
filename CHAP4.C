/*
The programs contained herein are adapted from 

      C: Power User's Guide

by Herbert Schildt published by Osborne/McGraw-Hil, Copyright
1987, Osborne/McGraw-Hill.  Used with the permission of
Osborne/McGraw-Hill.  Program adaptations are solely the
work of Herbert Schildt and are not a publication of
Osborne/McGraw-Hill.
*/

listing 4-1

/* set the video mode */
void mode(mode_code)
int mode_code;
{
  union REGS r;

  r.h.al = mode_code;
  r.h.ah = 0;
  int86(0x10, &r, &r);
}





listing 4-2

/* set the palette */
void palette(pnum)
int pnum;
{
  union REGS r;

  r.h.bh = 1;   /* code for mode 4 graphics */
  r.h.bl = pnum; /* palette number */
  r.h.ah = 11;  /* set palette function */
  int86(0x10, &r, &r);
}





listing 4-3

/* Write a point directly to the CGA/EGA. */
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

  bit_mask.i = 0xFF3F;	/* 11111111 00111111 in binary */

  /* check range for mode 4 */
  if(x<0 || x>199 || y<0 || y>319) return;

  xor = color_code & 128; /* see if xor mode is set */
  color_code = color_code & 127; /* mask off high bit */
 
  /* set bit_mask and color_code bits to the right location */
  bit_position = y%4;   /* compute the proper location 
			 within the byte  */
  color_code<<=2*(3-bit_position); /* shift the color_code
				      into position */
  bit_mask.i>>=2*bit_position; /* shift the bit_mask into
				  position */

 /* find the correct byte in screen memory */
  index = x*40 +(y/4);
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





listing 4-4

    
/* Draw a line in specified color 
   using Bresenham's integer based algorithm.
*/
void line(startx, starty, endx, endy, color)
int startx, starty, endx, endy, color;
{
  register int t, distance;
  int xerr=0, yerr=0, delta_x, delta_y;
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
  else incx = -1;

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
    xerr+=delta_x;
    yerr+=delta_y;  
    if(xerr>distance) {
      xerr-=distance;
      startx+=incx;
    }
    if(yerr>distance) {
      yerr-=distance;
      starty+=incy;
    }
  }
}





listing 4-5


/* draw a box */
void box(startx,starty, endx, endy, color_code)
int startx,starty,endx,endy,color_code;
{
  line(startx,starty, endx,starty, color_code);
  line(startx,starty, startx,endy, color_code);
  line(startx,endy, endx,endy, color_code);
  line(endx,starty, endx,endy, color_code);
}





listing 4-6


/* fill box with specified color */
void fill_box(startx, starty, endx, endy, color_code)
int startx, starty, endx, endy, color_code;
{
  register int i,begin, end;

  begin = startx<endx ? startx : endx;
  end = startx>endx ? startx : endx;

  for(i=begin; i<=end;i++)
    line(i,starty,i,endy,color_code);
}





listing 4-7

double asp_ratio;

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
  x=y;
  if(y) plot_circle(x, y, x_center, y_center, color_code);
}

/* plot_circle actually prints the points that
   define the circle */
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





listing 4-8

/* fill a circle by repeatedly calling circle()
   with smaller radiuses
*/
void fill_circle(x, y, r, c)
int x, y, r, c;
{
  while(r) {
    circle(x, y, r, c);
    r--;
  }
}





listing 4-9


/* A simple program to illustrate the core graphics
   functions.
 */
#include "dos.h"
#include "stdio.h"

void mode(), line(), box(),fill_box();
void mempoint(), palette(), xhairs();
void circle(), plot_circle(), fill_circle();

double asp_ratio; /* holds aspect ratio for cirlces */

main()
{
  mode(4);
  palette(0);

  line(0, 0, 100, 100, 1);
  box(50, 50, 80, 90, 2);
  fill_box(100, 0, 120, 40, 3);
  circle(100, 160, 30, 2);
  fill_circle(150, 250, 20, 1);

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






listing 4-10

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





listing 4-11

/* Copy one region to another location. */
void copy(startx, starty, endx, endy, x, y)
int startx, starty; /* upper left coordinate */
int endx, endy;  /* lower right coordinate of region to copy */
int x, y; /* upper left coordinate of region to copy to */
{ 
  int i, j;
  unsigned char c;

  for(; startx<=endx; startx++, x++)
    for(i=starty, j=y; i<=endy; i++, j++) {	
      c = read_point(startx, i); /* read point */
      mempoint(x, j, c); /* write it to new location */
    }
}





listing 4-12

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





listing 4-13

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





listing 4-14

double object[10][4];





listing 4-15

object[0][0] = 0; object[0][1] = 0;
object[0][0] = 0; object[0][3] = 10;

object[1][0] = 0; object[1][1] = 10;
object[1][0] = 10; object[1][3] = 10;

object[2][0] = 10; object[2][1] = 10;
object[2][0] = 10; object[2][3] = 0;

object[3][0] = 10; object[3][1] = 0;
object[3][0] = 0; object[3][3] = 0;





listing 4-16


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





listing 4-17

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





listing 4-18


/* An example of object rotation using the 
   CGA/EGA in graphics mode 4 */

#include "dos.h"
#include "stdio.h"
#include "math.h"

void mode(), line();
void mempoint(), palette();
void rotate_point(), rotate_object();
void display_object();

/* house array */
double house[][4] ={
/* startx, starty, endx, endy */
  120, 120, 120, 200, /* house */
  120, 200, 80, 200,
  80, 120, 80, 200,
  80, 120, 120, 120,
  60, 160, 80, 120, /* roof */
  60, 160, 80, 200,
  120, 155, 100, 155, /* door */
  100, 155, 100, 165,
  100, 165, 120, 165,
  90, 130, 100, 130, /* windows */
  90, 130, 90, 140,
  100, 130, 100, 140,
  90, 140, 100, 140,
  90, 180, 100, 180, 
  90, 180, 90, 190,
  100, 180, 100, 190,
  90, 190, 100, 190
};

main()
{
  union k{
    char c[2];
    int i;
  } key;

  mode(4);  /* mode 4 graphics, palette 0 */
  palette(0);
  
  /* draw a box around the house to give perspective */
  line(30, 70, 30, 260, 2);
  line(160, 70, 160, 260, 2);
  line(30, 70, 160, 70, 2);
  line(30, 260, 160, 260, 2); 

  display_object(house, 17);
  getche();
  rotate_object(house, 0.025, 90, 160, 17);
  mode(3);
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
  delta_x=endx-startx;
  delta_y=endy-starty;

  /* Compute the direction of the increment,
     an increment of 0 means either a vertical or horizontal
     line.
  */
  if(delta_x>0) incx=1;
  else if(delta_x==0) incx=0;
  else incx=-1;

  if(delta_y>0) incy=1;
  else if(delta_y==0) incy=0;
  else incy=-1;

  /* determine which distance is greater */
  delta_x=abs(delta_x);
  delta_y=abs(delta_y);
  if(delta_x>delta_y) distance=delta_x;
  else distance=delta_y;

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

/* Write a point directly to the CGA/EGA. */
void mempoint(x,y,color_code)
int x,y,color_code;
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
  color_code=color_code & 127; /* mask off high bit */
 
  /* set bit_mask and color_code bits to the right location */
  bit_position=y%4;
  color_code<<=2*(3-bit_position);
  bit_mask.i>>=2*bit_position;


 /* find the correct byte in screen memory */
  index=x*40 +(y >> 2);
  if(x % 2) index += 8152; /* if odd use 2nd bank */

  /* write the color */
  if(!xor) { /* overwrite mode */
    t=*(ptr+index) & bit_mask.c[0];
    *(ptr+index)=t | color_code;
  }
  else { /* xor mode */
    t=*(ptr+index) | (char)0;
    *(ptr+index)=t ^ color_code;
  }
}

/* Rotate a point around the origin, specified by
   x_org and y_org, by angle theta. */
void rotate_point(theta, x, y, x_org, y_org)
double theta, *x, *y;
int x_org, y_org;
{
  double tx, ty;

  /* normalize x and y */
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
      case 'l':  /* counterclockwise */
	theta = theta < 0 ? -theta : theta;
  	break;
      case 'r':  /* clockwise */
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





listing 4-19

/* display crosshair locator */
void xhairs(x,y)
int x,y;
{
  line(x-4, y, x+3, y, 1 | 128);
  line(x, y+4, x, y-3, 1 | 128);
}





listing 4-20

main()
{
  union k{
    char c[2];
    int i;
  } key;

  int x=10, y=10; /* current screen position */
  int cc=2; /* current color */
  int on_flag=1; /* pen on or off */
  int pal_num=1; /* palette number */
  /* the end points of a defined line, circle, or box */
  int startx=0, starty=0, endx=0, endy=0, first_point=1;
  int inc=1; /* movement increment */
  int  sides=0; /* number of sides of a defined object */
  int i;

  mode(4);  /* switch to mode 4 CGA/EGA graphics */
  palette(0); /* palette 0 */

  xhairs(x, y); /* show the crosshairs */
  do {
    key.i = bioskey(0);
    xhairs(x, y);   /* plot the crosshairs */
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
	  startx = x, starty = y;
	}
	else {
	  endx = x; endy = y;
	}
	first_point = !first_point;
	break;
      case 'p': pal_num = pal_num==1 ? 2:1;
        palette(pal_num);
    }
    xhairs(x, y);
  } while (key.c[0]!='q');
  getchar();
  mode(2);
}





listing shown later.
Once the object is defined, the object can be rotated by
pressing the 'a' key and using the 'l' or 'r' keys
to rotate the object about an origin defined by  the current
position of the locator.  To stop the rotate procedure,
press any key other than 'l' or 'a'.
 

The program is
terminated by typing a "q".  

The entire paint program is shown here.  You might find it
fun to add other high-level commands or to interface it to a
mouse. Sample output is shown in figure 4-4.




listing 4-21
/* A paint program for the CGA/EGA that allows
   lines, boxes, and cirlces to be drawn.  You may
   define an object and rotate it in either a clockwise
   or counterclockwise direction.  Also, the 
   graphic image may be saved to disk and loaded
   at a later date.  
 */

#define NUM_SIDES 20 /* Number of sides an object may
                        have.  Enlarge as needed */

#include "dos.h"
#include "stdio.h"
#include "math.h"

void mode(), line(), box(),fill_box();
void mempoint(), palette(), xhairs();
void circle(), plot_circle(), fill_circle();
void rotate_point(), rotate_object(), goto_xy();
void display_object(), copy(), move();
void save_pic(), load_pic();
unsigned char read_point();

/* This array will hold the coordinates of an object 
   defined dynamcially.
*/
double object[NUM_SIDES][4];

double asp_ratio; /* holds aspect ratio for cirlces */

main()
{
  union k{
    char c[2];
    int i;
  } key;

  int x=10, y=10; /* current screen position */
  int cc=2; /* current color */
  int on_flag=1; /* pen on or off */
  int pal_num=1; /* palette number */
  /* the end points of a defined line, circle, or box */
  int startx=0, starty=0, endx=0, endy=0, first_point=1;
  int inc=1; /* movement increment */
  int  sides=0; /* number of sides of a defined object */
  int i;

  mode(4);  /* switch to mode 4 CGA/EGA graphics */
  palette(0); /* palette 0 */

  xhairs(x, y); /* show the crosshairs */
  do {
    key.i = bioskey(0);
    xhairs(x, y);   /* plot the crosshairs */
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
	  startx = x, starty = y;
	}
	else {
	  endx = x; endy = y;
	}
	first_point = !first_point;
	break;
      case 'p': pal_num = pal_num==1 ? 2:1;
        palette(pal_num);
    }
    xhairs(x, y);
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

/* Define an object by specifying its endpoints */
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
  xhairs(x, y);
  do {
    goto_xy(0, 0);
    printf("Define side %d,", sides+1);
    if(i==0) printf(" enter first endpoint");
    else printf(" enter second endpoint");

    key.i = bioskey(0);
    xhairs(x, y);   /* plot the crosshairs */
    if(key.c[0]==13) { 
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




