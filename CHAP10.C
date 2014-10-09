/*
The programs contained herein are adapted from 

      C: Power User's Guide

by Herbert Schildt published by Osborne/McGraw-Hil, Copyright
1987, Osborne/McGraw-Hill.  Used with the permission of
Osborne/McGraw-Hill.  Program adaptations are solely the
work of Herbert Schildt and are not a publication of
Osborne/McGraw-Hill.
*/


listing 10-1

   
/* Display a bar chart. */
void bargraph(data, num, offset, min, max, width)  
double *data; /* array of data */
int num; /* number of elements in array */
int offset; /* determine spacing between sets of data */
int min, max; /* minimum and maximum values to be plotted */
int width; /* thickness of the lines */
{
  int y, t, incr;
  double norm_data, norm_ratio, spread;
  char s[80];
  static int color=0;
  int tempwidth;
 
  /* always use a different color */
  color++;
  if(color>3) color=1;

  /* determine normalization factor */
  spread = (double) max-min; 
  norm_ratio = 180/spread;  

  incr = 280/num; /* determine spacing between values */
  tempwidth = width;
  for(t=0; t<num; ++t) {
    norm_data = data[t];

    /* adjust for negative values */
    norm_data = norm_data-(double) min;  

    norm_data *= norm_ratio; /* normalize */
    y = (int) norm_data; /* type conversion */
    do { 
      line(179, ((t*incr)+20+offset+width), 179-y, 
        ((t*incr)+20+offset+width), color);
      width--;
    } while (width);
    width = tempwidth;
  }
}





listing 10-2

/* Draw the chart grid. */
void grid(min, max)
int min, max;
{
  register int t;

  goto_xy(22, 0); printf("%d", min);
  goto_xy(0, 0); printf("%d", max);
  line(180, 10, 180, 300, 1);
}





listing 10-3

/* Display labels on the screen. */
void label(str, num)
char str[][20]; /* strings to display */
int num; /* number of labels */
{
  int i, j, inc;

  inc = 38 / num;
  i = 2; /* initial horizontal starting point */
  for(j=0; j<num; j++) {
    goto_xy(23, i);
    printf(str[j]);
    i += inc;
  }
}





listing 10-4

/* Draw dotted reference lines across the screen. */
void hashlines()
{
  int i, j;

  for(i=10; i<180; i+=10) {
    for(j=10; j<300; j+=5)
      mempoint(i, j, 3); /* one point every 5 pixels */
  }
}





listing 10-5


/* Show a legend. */
void legend(names, num)
char names[][20];
int num; /* number of names */
{
  int color=1, i, j;

  goto_xy(24, 0); /* legend goes on bottom line */
  j = 0;
  for(i=0; i<num; i++) {
    /* print the label */
    printf("%s    ", names[i]);
    /* Compute where colored box goes by
       converting cursor location into graphics
       coordinates.  Each character is 8 pixels wide in
       mode 4 graphics.
    */
    j += strlen(names[i])*8+4;
    fill_box(192, j, 198, j+12, color);
    j += 28;  /* advance to next field */
    color++;
    if(color>3) color = 1;
  }
}    





listing 10-6

/* A bar graph demonstration program. */
#include "dos.h"

void bargraph(),  mode(), mempoint();
void line(), goto_xy(), grid(), label();
void hashlines(), legend(), read_cursor_xy();
void palette(), color_puts(), fill_box();

main()
{
  double widget[]={
    10.1, 20, 30, 35.34, 50
  };
  double global[]={
    19, 20, 8.8, 30, 40
  };
  double tower[]={
    25.25, 19, 17.4, 33, 29
  };

  int min, max;

  char n[][20]={
    "Widget", 
    "Global",
    "Tower"    
  };
  char lab[][20]={
    "1983",
    "1984",
    "1985",
    "1986",
    "1987"
  };

  mode(4); /* 320x200  graphics mode */
  palette(0);


  grid(0, 50); /* display the grid */
  hashlines(); /* display dotted reference lines */
  label(lab, 5); /* label the graph */
  legend(n, 3); /* show the legend */

  /* plot the stock prices of the three companies */
  bargraph(widget, 5, 0, 0, 50, 4);
  bargraph(global, 5, 10, 0, 50, 4);
  bargraph(tower, 5, 20, 0, 50, 4);

  getch();
  mode(3);
}

/* Draw the chart grid. */
void grid(min, max)
int min, max;
{
  register int t;

  goto_xy(22, 0); printf("%d", min);
  goto_xy(0, 0); printf("%d", max);
  line(180, 10, 180, 300, 1);
}

/* Display labels on the screen. */
void label(str, num)
char str[][20]; /* strings to display */
int num; /* number of labels */
{
  int i, j, inc;

  inc = 38 / num;
  i = 2; /* initial horizontal starting point */
  for(j=0; j<num; j++) {
    goto_xy(23, i);
    printf(str[j]);
    i += inc;
  }
}

/* Draw dotted reference lines across the screen. */
void hashlines()
{
  int i, j;

  for(i=10; i<180; i+=10) {
    for(j=10; j<300; j+=5)
      mempoint(i, j, 3); /* one point every 5 pixels */
  }
}

/* Show a legend. */
void legend(names, num)
char names[][20];
int num; /* number of names */
{
  int color=1, i, j;

  goto_xy(24, 0); /* legend goes on bottom line */
  j = 0;
  for(i=0; i<num; i++) {
    /* print the label */
    printf("%s    ", names[i]);
    /* Compute where colored box goes by
       converting cursor location into graphics
       coordinates.  Each character is 8 pixels wide in
       mode 4 graphics.
    */
    j += strlen(names[i])*8+4;
    fill_box(192, j, 198, j+12, color);
    j += 28;  /* advance to next field */
    color++;
    if(color>3) color = 1;
  }
}    
   
/* Display a bar chart. */
void bargraph(data, num, offset, min, max, width)  
double *data; /* array of data */
int num; /* number of elements in array */
int offset; /* determine exact screen position */
int min, max; /* minimum and maximum values to be plotted */
int width; /* thickness of the lines */
{
  int y, t, incr;
  double norm_data, norm_ratio, spread;
  char s[80];
  static int color=0;
  int tempwidth;
 
  /* always use a different color */
  color++;
  if(color>3) color=1;

  /* determine normalization factor */
  spread = (double) max-min; 
  norm_ratio = 180/spread;  

  incr = 280/num; /* determine spacing between lines */
  tempwidth = width;
  for(t=0; t<num; ++t) {
    norm_data = data[t];

    /* adjust for negative values */
    norm_data = norm_data-(double) min;  

    norm_data *= norm_ratio; /* normalize */
    y = (int) norm_data; /* type conversion */
    do { 
      line(179, ((t*incr)+20+offset+width), 179-y, 
        ((t*incr)+20+offset+width), color);
      width--;
    } while (width);
    width = tempwidth;
  }
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


/* Set the video mode. */
void mode(mode_code)
int mode_code;
{
  union REGS r;

  r.h.al = mode_code;
  r.h.ah = 0;
  int86(0x10, &r, &r);
}

/* send the cursor to x,y */
void goto_xy(x, y)
int x, y;
{
  union REGS r;

  r.h.ah = 2; /* cursor addressing function */
  r.h.dl = y; /* column coordinate */
  r.h.dh = x; /* row coordinate */
  r.h.bh = 0; /* video page */
  int86(0x10, &r, &r);
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





listing 10-7

#define MAX_SETS 3
#define MAX_ENTRIES 50
#define MAX_LABELS 20
#define MAX_NAMES 20


main()
{
  double v[MAX_SETS][MAX_ENTRIES]; /* holds the data */
  int num_entries; 
  int num_sets;
  int min, max, i;
  int lines, offset;
  char save=0; /* save graph? */
  char names[MAX_NAMES][20];
  char lab[MAX_LABELS][20];

  /* read in the information */
  enter(v, &num_entries, &num_sets);

  /* find minimum and maximum values */
  min_max(v, num_entries, num_sets, &min, &max);

  /* get the names of what is being graphed */
  get_names(names, num_sets);

  /* get the labels for the graph */
  get_labels(lab, num_entries);

  /* get line thickness */
  lines = get_line_size();
  
  /* get the spacing between sets of data */
  offset = get_offset();

  /* save to disk file ? */
  printf("save the graph to disk file? (Y/N) ");
  if(tolower(getche())=='y') save = 1;

  mode(4); /* 320x200  graphics mode */
  palette(0);

  grid(min, max); /* display the grid */
  hashlines(); /* display dotted reference lines */
  label(lab, num_entries); /* label the graph */
  legend(names, num_sets); /* show the legend */

  /* plot the values */
  for(i=0; i<num_sets; i++) 
    bargraph(v[i], num_entries, i*offset, min, max, lines);

  if(save) save_pic();
  getch();
  mode(3);
}





listing 10-8

/* Enter values to be plotted */
enter(v, entries, sets)
double v[][MAX_ENTRIES]; /* data array */
int *entries; /* number of entries in each set of data */
int *sets; /* number of sets of data */
{
  int i, j, count, num;
  char s[80];
 
  printf("How many sets of data? (1 to %d) ", MAX_SETS);
  scanf("%d%c", &count, &j);
  if(count>MAX_SETS) count = MAX_SETS; /* prevent array overrun */
  *sets = count;

  printf("How many entries? (1 to %d) ", MAX_ENTRIES);
  scanf("%d%c", &num, &j);
  if(num>MAX_SETS) num = MAX_ENTRIES; /* prevent array overrun */
  *entries = num;

  j = 0;

  /* read in the values */
  while((j<count)) {
    printf("Data set %d\n", j+1);
    for(i=0; i<num; i++) {
      printf("%d: ", i+1);
      gets(s);
      sscanf(s,"%lf", &v[j][i]);
    }
    j++;
  }
  return count;
}





listing 10-9

/* Find the smallest minimum and greatest maximum values 
   among all the sets of data.
*/
void min_max(v, entries, sets, min, max)
double v[][MAX_ENTRIES]; /* values */
int entries; /* number of entries */
int sets; /* number of sets */
int *min, *max; /* return minimum and maximum values */
{
  int i, j;
  int tmin, tmax;

  *min = *max = 0;

  for(i=0; i<sets; i++) {
      tmax = getmax(v[i], entries);
      tmin = getmin(v[i], entries);
      if(tmax > *max) *max = tmax;
      if(tmin < *min) *min = tmin;
  }
}


/* Returns the maximum value of the data. */
getmax(data, num)
double *data;
int num;
{
  int t, max;

  max = (int) data[0];
  for(t=1; t<num; ++t) 
    if(data[t]>max) max = (int) data[t];
  return max;
}

/* Returns the minimum value of the data. */
getmin(data, num)
double *data;
int num;
{
  int t, min;

  min = (int) data[0];
  for(t=1; t<num; ++t) 
    if(data[t]<min) min = (int) data[t];
  return min;
}





listing 10-10
/* A bar graph generator program. */
#include "dos.h"

#include "stdio.h"

#define MAX_SETS 3
#define MAX_ENTRIES 50
#define MAX_LABELS 20
#define MAX_NAMES 20

void bargraph(),  mode(), mempoint();
void line(), goto_xy(), grid(), label();
void hashlines(), legend(), read_cursor_xy();
void palette(), color_puts(), fill_box();
void get_labels(), get_names(), min_max();
void save_pic();

main()
{
  double v[MAX_SETS][MAX_ENTRIES]; /* holds the data */
  int num_entries; 
  int num_sets;
  int min, max, i;
  int lines, offset;
  char save=0; /* save graph? */
  char names[MAX_NAMES][20];
  char lab[MAX_LABELS][20];

  /* read in the information */
  enter(v, &num_entries, &num_sets);

  /* find minimum and maximum values */
  min_max(v, num_entries, num_sets, &min, &max);

  /* get the names of what is being graphed */
  get_names(names, num_sets);

  /* get the labels for the graph */
  get_labels(lab, num_entries);

  /* get line thickness */
  lines = get_line_size();
  
  /* get the spacing between sets of data */
  offset = get_offset();

  /* save to disk file ? */
  printf("save the graph to disk file? (Y/N) ");
  if(tolower(getche())=='y') save = 1;

  mode(4); /* 320x200  graphics mode */
  palette(0);

  grid(min, max); /* display the grid */
  hashlines(); /* display dotted reference lines */
  label(lab, num_entries); /* label the graph */
  legend(names, num_sets); /* show the legend */

  /* plot the values */
  for(i=0; i<num_sets; i++) 
    bargraph(v[i], num_entries, i*offset, min, max, lines);

  if(save) save_pic();
  getch();
  mode(3);
}

/* Enter values to be plotted */
enter(v, entries, sets)
double v[][MAX_ENTRIES]; /* data array */
int *entries; /* number of entries in each set of data */
int *sets; /* number of sets of data */
{
  int i, j, count, num;
  char s[80];
 
  printf("How many sets of data? (1 to %d) ", MAX_SETS);
  scanf("%d%c", &count, &j);
  if(count>MAX_SETS) count = MAX_SETS; /* prevent array overrun */
  *sets = count;

  printf("How many entries? (1 to %d) ", MAX_ENTRIES);
  scanf("%d%c", &num, &j);
  if(num>MAX_ENTRIES) num = MAX_ENTRIES; /* prevent array overrun */
  *entries = num;

  j = 0;

  /* read in the values */
  while((j<count)) {
    printf("Data set %d\n", j+1);
    for(i=0; i<num; i++) {
      printf("%d: ", i+1);
      gets(s);
      sscanf(s,"%lf", &v[j][i]);
    }
    j++;
  }
  return count;
}

/* Input the names of the sets. */
void get_names(n, num)
char n[][20]; /* array for the names */
int num;  /* number of sets */
{
  int i;

  for(i=0; i<num; i++) {
    printf("Enter name: ");
    gets(n[i]);
  }
}

/* Input the label of each entry. */
void get_labels(l, num)
char l[][20]; /* array for the labels */
int num;  /* number of entries */
{
  int i;

  for(i=0; i<num; i++) {
    printf("Enter label: ");
    gets(l[i]);
  }
}

/* Input distance between bars in pixels */
get_offset()
{
  int i;

  printf("Enter distance between bars in pixels: ");
  scanf("%d%*c", &i);
  return i;
}

/* Input bar thickness in pixels */
get_line_size()
{
  int i;

  printf("Enter thickness of bars in pixels: ");
  scanf("%d", &i);
  return i;
}


/* Draw the chart grid. */
void grid(min, max)
int min, max;
{
  register int t;

  goto_xy(22, 0); printf("%d", min);
  goto_xy(0, 0); printf("%d", max);
  line(180, 10, 180, 300, 1);
}

/* Display labels on the screen. */
void label(str, num)
char str[][20]; /* strings to display */
int num; /* number of labels */
{
  int i, j, inc;

  inc = 38 / num;
  i = 2; /* initial horizontal starting point */
  for(j=0; j<num; j++) {
    goto_xy(23, i);
    printf(str[j]);
    i += inc;
  }
}

/* Draw dotted reference lines across the screen. */
void hashlines()
{
  int i, j;

  for(i=10; i<180; i+=10) {
    for(j=10; j<300; j+=5)
      mempoint(i, j, 3); /* one point every 5 pixels */
  }
}

/* Show a legend. */
void legend(names, num)
char names[][20];
int num; /* number of names */
{
  int color=1, i, j;

  goto_xy(24, 0); /* legend goes on bottom line */
  j = 0;
  for(i=0; i<num; i++) {
    /* print the label */
    printf("%s    ", names[i]);
    /* Compute where colored box goes by
       converting cursor location into graphics
       coordinates.  Each character is 8 pixels wide in
       mode 4 graphics.
    */
    j += strlen(names[i])*8+4;
    fill_box(192, j, 198, j+12, color);
    j += 28;  /* advance to next field */
    color++;
    if(color>3) color = 1;
  }
}    
   
/* Display a bar chart. */
void bargraph(data, num, offset, min, max, width)  
double *data; /* array of data */
int num; /* number of elements in array */
int offset; /* determine exact screen position */
int min, max; /* minimum and maximum values to be plotted */
int width; /* thickness of the lines */
{
  int y, t, incr;
  double norm_data, norm_ratio, spread;
  char s[80];
  static int color=0;
  int tempwidth;
 
  /* always use a different color */
  color++;
  if(color>3) color=1;

  /* determine normalization factor */
  spread = (double) max-min; 
  norm_ratio = 180/spread;  

  incr = 280/num; /* determine spacing between lines */
  tempwidth = width;
  for(t=0; t<num; ++t) {
    norm_data = data[t];

    /* adjust for negative values */
    norm_data = norm_data-(double) min;  

    norm_data *= norm_ratio; /* normalize */
    y = (int) norm_data; /* type conversion */
    do { 
      line(179, ((t*incr)+20+offset+width), 179-y, 
        ((t*incr)+20+offset+width), color);
      width--;
    } while (width);
    width = tempwidth;
  }
}

/* Find the smallest minimum and greatest maximum values 
   among all the sets of data.
*/
void min_max(v, entries, sets, min, max)
double v[][MAX_ENTRIES]; /* values */
int entries; /* number of entries */
int sets; /* number of sets */
int *min, *max; /* return minimum and maximum values */
{
  int i, j;
  int tmin, tmax;

  *min = *max = 0;

  for(i=0; i<sets; i++) {
      tmax = getmax(v[i], entries);
      tmin = getmin(v[i], entries);
      if(tmax > *max) *max = tmax;
      if(tmin < *min) *min = tmin;
  }
}


/* Returns the maximum value of the data. */
getmax(data, num)
double *data;
int num;
{
  int t, max;

  max = (int) data[0];
  for(t=1; t<num; ++t) 
    if(data[t]>max) max = (int) data[t];
  return max;
}

/* Returns the minimum value of the data. */
getmin(data, num)
double *data;
int num;
{
  int t, min;

  min = (int) data[0];
  for(t=1; t<num; ++t) 
    if(data[t]<min) min = (int) data[t];
  return min;
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


/* Set the video mode. */
void mode(mode_code)
int mode_code;
{
  union REGS r;

  r.h.al = mode_code;
  r.h.ah = 0;
  int86(0x10, &r, &r);
}

/* send the cursor to x,y */
void goto_xy(x, y)
int x, y;
{
  union REGS r;

  r.h.ah = 2; /* cursor addressing function */
  r.h.dl = y; /* column coordinate */
  r.h.dh = x; /* row coordinate */
  r.h.bh = 0; /* video page */
  int86(0x10, &r, &r);
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





listing 10-11

/* A simple graphics presentation program. */

#include "stdio.h"
#include "dos.h"

void load_pic(), mode(), palette(), goto_xy();

main(argc, argv)
int argc;
char *argv[];
{
  if(argc!=2) {
    printf("usage: show <filename>");
    exit(1);
  }
  mode(4);
  palette(0);
  load_pic(argv[1]);
  getch();
  mode(3);
}

/* load the video graphics display */
void load_pic(fname)
char *fname;
{
  FILE *fp;
  register int i, j;
  char far *ptr = (char far *) 0xB8000000; /* pointer
                                              to CGA memory */
  char far *temp;
  unsigned char buf[14][80]; /* hold the contents of screen */ 

  if(!(fp=fopen(fname, "rb"))) {
    goto_xy(0, 0);
    printf("cannot open file\n");
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

/* Set the video mode. */
void mode(mode_code)
int mode_code;
{
  union REGS r;

  r.h.al = mode_code;
  r.h.ah = 0;
  int86(0x10, &r, &r);
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

/* send the cursor to x,y */
void goto_xy(x, y)
int x, y;
{
  union REGS r;

  r.h.ah = 2; /* cursor addressing function */
  r.h.dl = y; /* column coordinate */
  r.h.dh = x; /* row coordinate */
  r.h.bh = 0; /* video page */
  int86(0x10, &r, &r);
}




