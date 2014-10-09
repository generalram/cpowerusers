/*
The programs contained herein are adapted from 

      C: Power User's Guide

by Herbert Schildt published by Osborne/McGraw-Hil, Copyright
1987, Osborne/McGraw-Hill.  Used with the permission of
Osborne/McGraw-Hill.  Program adaptations are solely the
work of Herbert Schildt and are not a publication of
Osborne/McGraw-Hill.
*/


listing 6-1

/* Initialize the port.
*/
void port_init(port, code)
int port; 
unsigned char code;
{
  union REGS r;

  r.x.dx = port; /* serial port */
  r.h.ah = 0;  /* initialize port function */
  r.h.al = code; /* initialization code - see text for details */
  int86(0x14, &r, &r);
}





listing 6-2

/* Send a character out the serial port */
void sport(port, c)
int port; /* i/o port */
char c; /* character to send */
{
  union REGS r;

  r.x.dx = port; /* serial port */
  r.h.al = c; /* char to send */
  r.h.ah = 1;  /* send character function */
  int86(0x14, &r, &r);
  if(r.h.ah & 128) { /* check bit 7 */
    printf("send error detected in serial port");
    exit(1);
  }
}





listing 6-3


/* Read a character from a port */
rport(port)
int port; /* i/o port */
{
  union REGS r;

  /* wait for a character */
  while(!(check_stat(PORT)&256)) 
    if(kbhit()) { /* abort on keypress */
      getch();
      exit(1);
    }

  r.x.dx = port; /* serial port */
  r.h.ah = 2;  /* read character function */
  int86(0x14, &r, &r);
  if(r.h.ah & 128) 
    printf("read error detected in serial port");
  return r.h.al;
}





listing 6-3A

/* Send the specified file. */
void send_file(fname)
char *fname;
{
  FILE *fp;  
  char ch;
  union {
    char c[2];
    unsigned int count;
  } cnt;

  if(!(fp=fopen(fname,"rb"))) {
    printf("cannot open input file\n");
    exit(1);
  }

  send_file_name(fname); /* send the name of the file */

  wait(PORT);  /* wait for receiver to acknowledge */
 
  /* found out the size of the file */
  cnt.count = filesize(fp);
  /* send size */
  sport(PORT, cnt.c[0]);
  wait(PORT);
  sport(PORT, cnt.c[1]);

  do {
    ch = getc(fp);    
    if(ferror(fp)) {
      printf("error reading input file");
      break;
    } 
    
    /* wait until receiver is ready */ 
    if(!feof(fp)) {
      wait(PORT);
      sport(PORT, ch);
    }
  } while(!feof(fp));
  wait(PORT);/* read the last period from port */
  fclose(fp);
}





listing 6-4

/* Send the file name */
void send_file_name(f)
char *f;
{
  printf("Transmiter waiting...\n");
  do {
     sport(PORT, '?'); 
  } while(!kbhit() && !(check_stat(PORT)&256));
  if(kbhit()) {
    getch();
    exit(1);
  }
  wait(PORT);  /* wait for receiver to acknowledge */
  printf("sending %s\n\n",f);

  /* actually send the name */
  while(*f) {
    sport(PORT, *f++);
    wait(PORT); /* wait for receiver to acknowledge */
  }
  sport(PORT,'\0'); /* null terminator */
}





listing 6-5

/* Wait for a response. */
void wait(port)
int port;
{
  if(rport(port)!='.') {
    printf("communication error\n");
    exit(1);
  }
}




listing 6-6

/* Receive a file. */
void rec_file()
{
  FILE *fp;  
  char ch;
  char fname[14];
  union {
    char c[2];
    unsigned int count;
  } cnt;

  get_file_name(fname); /* get the file name */

  printf("receiving file %s\n",fname);
  remove(fname);
  if(!(fp=fopen(fname, "wb"))) {
    printf("cannot open output file\n");
    exit(1);
  }

  /* get file length */
  sport(PORT, '.'); /* acknowledge */
  cnt.c[0] = rport(PORT);
  sport(PORT, '.'); /* acknowledge */
  cnt.c[1] = rport(PORT);
  sport(PORT, '.'); /* acknowledge */
  
  for(; cnt.count; cnt.count--) {
    ch = rport(PORT);
    putc(ch, fp);
    if(ferror(fp)) {
      printf("error writing file");
      exit(1);
    }
    sport(PORT, '.'); /* acknowledge */
  }
  fclose(fp);
}





listing 6-7

/* Receive the file name */
void get_file_name(f)
char *f;
{
  printf("receiver waiting...\n");
  while(rport(PORT)!='?') ;
  sport(PORT, '.'); /* acknowledge */
  while((*f=rport(PORT))) {
    if(*f!='?') {
      f++;
      sport(PORT, '.'); /* acknowledge */
    }
  }
}




listing 6-8

/* File transfer program using software handshaking. 

   Port initialized to
     9600 baud, 
     no parity, 
     eight data bits, 
     two stop bits.

  */

#define PORT 0

#include "dos.h"
#include "stdio.h"

unsigned int filesize();
void sport(), send_file(), rec_file(), send_file_name();
void get_file_name(), port_init(), wait();

main(argc,argv)
int argc;
char *argv[];
{
  if(argc<2) {
    printf("Usage: trans s filename OR trans r\n");
    exit(1);
  }

  printf("File transfer program in operation.  To abort,\n");
  printf("press any key.\n\n");

  port_init(PORT, 231); /* initalize the serial port */

  if(tolower(*argv[1]) == 's') send_file(argv[2]);
  else rec_file();
}

/* Send the specified file. */
void send_file(fname)
char *fname;
{
  FILE *fp;  
  char ch;
  union {
    char c[2];
    unsigned int count;
  } cnt;

  if(!(fp=fopen(fname,"rb"))) {
    printf("cannot open input file\n");
    exit(1);
  }

  send_file_name(fname); /* send the name of the file */

  wait(PORT);  /* wait for receiver to acknowledge */
 
  /* found out the size of the file */
  cnt.count = filesize(fp);
  /* send size */
  sport(PORT, cnt.c[0]);
  wait(PORT);
  sport(PORT, cnt.c[1]);

  do {
    ch = getc(fp);    
    if(ferror(fp)) {
      printf("error reading input file");
      break;
    } 
    
    /* wait until receiver is ready */ 
    if(!feof(fp)) {
      wait(PORT);
      sport(PORT, ch);
    }
  } while(!feof(fp));
  wait(PORT);/* read the last period from port */
  fclose(fp);
}

/* Receive a file. */
void rec_file()
{
  FILE *fp;  
  char ch;
  char fname[14];
  union {
    char c[2];
    unsigned int count;
  } cnt;

  get_file_name(fname); /* get the file name */

  printf("receiving file %s\n",fname);
  remove(fname);
  if(!(fp=fopen(fname, "wb"))) {
    printf("cannot open output file\n");
    exit(1);
  }

  /* get file length */
  sport(PORT, '.'); /* acknowledge */
  cnt.c[0] = rport(PORT);
  sport(PORT, '.'); /* acknowledge */
  cnt.c[1] = rport(PORT);
  sport(PORT, '.'); /* acknowledge */
  
  for(; cnt.count; cnt.count--) {
    ch = rport(PORT);
    putc(ch, fp);
    if(ferror(fp)) {
      printf("error writing file");
      exit(1);
    }
    sport(PORT, '.'); /* acknowledge */
  }
  fclose(fp);
}

/* Return the length, in bytes, of a file */
unsigned int filesize(fp)
FILE *fp;
{
  unsigned long int i;

  i = 0;
  do {
    getc(fp);
    i++;
  } while(!feof(fp));
  rewind(fp);
  return i-1; /* don't count EOF char */
}

/* Send the file name */
void send_file_name(f)
char *f;
{
  printf("Transmitter waiting...\n");
  do {
     sport(PORT, '?'); 
  } while(!kbhit() && !(check_stat(PORT)&256));
  if(kbhit()) {
    getch();
    exit(1);
  }
  wait(PORT);  /* wait for receiver to acknowledge */
  printf("sending %s\n\n",f);

  /* actually send the name */
  while(*f) {
    sport(PORT, *f++);
    wait(PORT); /* wait for receiver to acknowledge */
  }
  sport(PORT, '\0'); /* null terminator */
}

/* Receive the file name */
void get_file_name(f)
char *f;
{
  printf("receiver waiting...\n");
  while(rport(PORT)!='?') ;
  sport(PORT, '.'); /* acknowledge */
  while((*f=rport(PORT))) {
    if(*f!='?') {
      f++;
      sport(PORT, '.'); /* acknowledge */
    }
  }
}

/* Wait for a response. */
void wait(port)
int port;
{
  if(rport(port)!='.') {
    printf("communication error\n");
    exit(1);
  }
}

/* Send a character out the serial port */
void sport(port, c)
int port; /* i/o port */
char c; /* character to send */
{
  union REGS r;

  r.x.dx = port; /* serial port */
  r.h.al = c; /* char to send */
  r.h.ah = 1;  /* send character function */
  int86(0x14, &r, &r);
  if(r.h.ah & 128) {
    printf("send error detected in serial port");
    exit(1);
  }
}

/* Read a character from a port */
rport(port)
int port; /* i/o port */
{
  union REGS r;

  /* wait for a character */
  while(!(check_stat(PORT)&256)) 
    if(kbhit()) { /* abort on keypress */
      getch();
      exit(1);
    }

  r.x.dx = port; /* serial port */
  r.h.ah = 2;  /* read character function */
  int86(0x14, &r, &r);
  if(r.h.ah & 128) 
    printf("read error detected in serial port");
  return r.h.al;
}

/* Check the status of the serial port. */
check_stat(port)
int port; /* i/o port */
{
  union REGS r;

  r.x.dx = port; /* serial port */
  r.h.ah = 3;  /* read status */
  int86(0x14, &r, &r);
  return r.x.ax;
}

/* Initialize the port.
*/
void port_init(port, code)
int port; 
unsigned char code;
{
  union REGS r;

  r.x.dx = port; /* serial port */
  r.h.ah = 0;  /* initialize port function */
  r.h.al = code; /* initialization code - see text for details */
  int86(0x14, &r, &r);
}





listing 6-9

main()
{

  printf("File server in operation.\n");
  printf("To abort, press any key.\n\n");

  port_init(PORT); /* initalize the serial port */

  do {
    /* wait until a request is received */
    if(check_stat(PORT)&256) { 
      switch(rport(PORT)) {
        case 's': send_file(PORT);
          break;
        case 'r': rec_file(PORT);
          break;
      }
    }

/***********************************
Add additional workstations by checking more ports
as shown here.
    if(check_stat(PORT1)&256) { 
      switch(rport(PORT1)) {
        case 's': send_file(PORT1);
          break;
        case 'r': rec_file(PORT1);
          break;
      }
    }
 .
 .
 .
    if(check_stat(PORTn)&256) { 
      switch(rport(PORTn)) {
        case 's': send_file(PORTn);
          break;
        case 'r': rec_file(PORTn);
          break;
      }
    }
********************************/
  } while(!kbhit());
}





listing 6-10


/* Send the specified file through specified port. */
void send_file(port)
int port;
{
  FILE *fp;  
  char ch, fname[14];
  union {
    char c[2];
    unsigned int count;
  } cnt;

  sport(port, '.'); /* acknowledge */

  get_file_name(fname, PORT);
  if(!(fp=fopen(fname,"rb"))) {
    printf("cannot open input file\n");
    exit(1);
  }

  if(rport(port)!='.') {
    printf("remote file failure\n");
    exit(1);
  }

  printf("sending file %s\n", fname);
  /* found out the size of the file */
  cnt.count = filesize(fp);
  /* send size */
  sport(port, cnt.c[0]);
  wait(port);

  sport(port, cnt.c[1]);
  do {
    ch = getc(fp);    
    if(ferror(fp)) {
      printf("error reading input file");
      break;
    } 
    
    /* wait until receiver is ready */ 
    if(!feof(fp)) {
      wait(port);
      sport(port, ch);
    }
  } while(!feof(fp));
  wait(port);/* read the last period from port */
  fclose(fp);
}

/* Receive a file through the specified port. */
void rec_file(port)
int port;
{
  FILE *fp;  
  char ch;
  char fname[14];
  union {
    char c[2];
    unsigned int count;
  } cnt;

  sport(port, '.'); /* acknowledge */
  
  get_file_name(fname, PORT);

  printf("receiving file %s\n",fname);
  remove(fname);
  if(!(fp=fopen(fname, "wb"))) {
    printf("cannot open output file\n");
    exit(1);
  }

  /* get file length */
  sport(port, '.');
  cnt.c[0] = rport(port);
  sport(port, '.');
  cnt.c[1] = rport(port);
  sport(port, '.');
  
  for(; cnt.count; cnt.count--) {
    ch = rport(port);
    putc(ch, fp);
    if(ferror(fp)) {
      printf("error writing file");
      exit(1);
    }
    sport(port, '.');
  }
  fclose(fp);
}





listing 6-11

/* Poor man's LAN file server.  Port setings:

     9600 baud, 
     no parity, 
     eight data bits, 
     two stop bits.
*/

#define PORT 0

#include "dos.h"
#include "stdio.h"

unsigned int filesize();
void sport(), send_file(), rec_file(), send_file_name();
void get_file_name(), port_init(), wait();

main()
{

  printf("File server in operation.\n");
  printf("To abort, press any key.\n\n");

  port_init(PORT); /* initalize the serial port */

  do {
    /* wait until a request is received */
    if(check_stat(PORT)&256) { 
      switch(rport(PORT)) {
        case 's': send_file(PORT);
          break;
        case 'r': rec_file(PORT);
          break;
      }
    }
/***********************************
Add additional workstations by checking more ports
as shown here.
    if(check_stat(PORT1)&256) { 
      switch(rport(PORT1)) {
        case 's': send_file(PORT1);
          break;
        case 'r': rec_file(PORT1);
          break;
      }
    }
 .
 .
 .
    if(check_stat(PORTn)&256) { 
      switch(rport(PORTn)) {
        case 's': send_file(PORTn);
          break;
        case 'r': rec_file(PORTn);
          break;
      }
    }
********************************/
  } while(!kbhit());
}

/* Send the specified file through specified port. */
void send_file(port)
int port;
{
  FILE *fp;  
  char ch, fname[14];
  union {
    char c[2];
    unsigned int count;
  } cnt;

  sport(port, '.'); /* acknowledge */

  get_file_name(fname, PORT);
  if(!(fp=fopen(fname,"rb"))) {
    printf("cannot open input file\n");
    exit(1);
  }

  if(rport(port)!='.') {
    printf("remote file failure\n");
    exit(1);
  }

  printf("sending file %s\n", fname);
  /* found out the size of the file */
  cnt.count = filesize(fp);
  /* send size */
  sport(port, cnt.c[0]);
  wait(port);

  sport(port, cnt.c[1]);
  do {
    ch = getc(fp);    
    if(ferror(fp)) {
      printf("error reading input file");
      break;
    } 
    
    /* wait until receiver is ready */ 
    if(!feof(fp)) {
      wait(port);
      sport(port, ch);
    }
  } while(!feof(fp));
  wait(port);/* read the last period from port */
  fclose(fp);
}

/* Receive a file through the specified port. */
void rec_file(port)
int port;
{
  FILE *fp;  
  char ch;
  char fname[14];
  union {
    char c[2];
    unsigned int count;
  } cnt;

  sport(port, '.'); /* acknowledge */
  
  get_file_name(fname, PORT);

  printf("receiving file %s\n",fname);
  remove(fname);
  if(!(fp=fopen(fname, "wb"))) {
    printf("cannot open output file\n");
    exit(1);
  }

  /* get file length */
  sport(port, '.');
  cnt.c[0] = rport(port);
  sport(port, '.');
  cnt.c[1] = rport(port);
  sport(port, '.');
  
  for(; cnt.count; cnt.count--) {
    ch = rport(port);
    putc(ch, fp);
    if(ferror(fp)) {
      printf("error writing file");
      exit(1);
    }
    sport(port, '.');
  }
  fclose(fp);
}

/* Return the length, in bytes, of a file */
unsigned int filesize(fp)
FILE *fp;
{
  unsigned long int i;

  i = 0;
  do {
    getc(fp);
    i++;
  } while(!feof(fp));
  rewind(fp);
  return i-1; /* don't count the EOF char */
}

/* Send the file name */
void send_file_name(f, port)
char *f;
int port;
{
  do {
     sport(port, '?'); 
  } while(!kbhit() && !(check_stat(port)&256));
  if(kbhit()) {
    getch();
    exit(1);
  }
  wait(port);

  while(*f) {
    sport(port, *f++);
    wait(port);
  }
  sport(port, 0);
}

/* Receive the file name */
void get_file_name(f, port)
char *f;
int port;
{
 
  while(rport(port)!='?') printf(".");
  sport(port, '.');
  while((*f=rport(port))) {
    if(*f!='?') {
      f++;
      sport(port, '.');
    }
  }
  sport(port, '.');
}

/* Wait for a response. */
void wait(port)
int port;
{
  if(rport(port)!='.') {
    printf("communication error\n");
    exit(1);
  }
}

/* Send a character out the serial port */
void sport(port, c)
int port; /* i/o port */
char c; /* character to send */
{
  union REGS r;

  r.x.dx = port; /* serial port */
  r.h.al = c; /* byte to send */
  r.h.ah = 1;  /* send character function */
  int86(0x14, &r, &r);
  if(r.h.ah & 128) {
    printf("send error detected in serial port %d", r.h.ah);
    exit(1);
  }
}

/* Read a character from a port */
rport(port)
int port; /* i/o port */
{
  union REGS r;

  /* wait for a character */
  while(!(check_stat(port)&256)) 
    if(kbhit()) {
      getch();
      exit(1);
    }

  r.x.dx = port; /* serial port */
  r.h.ah = 2;  /* read character function */
  int86(0x14, &r, &r);
  if(r.h.ah & 128) 
    printf("read error detected in serial port");
  return r.h.al;
}

/* Check the status of the serial port. */
check_stat(port)
int port; /* i/o port */
{
  union REGS r;

  r.x.dx = port; /* serial port */
  r.h.ah = 3;  /* read status */
  int86(0x14, &r, &r);
  return r.x.ax;
}

/* Initialize port to 9600 baud, two stop-bits, 
   no parity, 8 data bits. 
*/
void port_init(port)
int port; 
{
  union REGS r;

  r.x.dx = port; /* serial port */
  r.h.ah = 0;  /* initialize port function */
  r.h.al = 231; /* initialization code - see text for details */
  int86(0x14, &r, &r);
}





listing 6-12


/* Load a file from the file server.
*/

#define PORT 0

#include "dos.h"
#include "stdio.h"

void sport(), rec_file(), send_file_name();
void get_file_name(), port_init(), wait();

main(argc,argv)
int argc;
char *argv[];
{
  if(argc!=2) {
    printf("Usage: get <filename>");
    exit(1);
  }

  port_init(PORT); /* initalize the serial port */

  rec_file(argv[1]);
}

/* Receive a file. */
void rec_file(fname)
char *fname;
{
  FILE *fp;  
  char ch;
  union {
    char c[2];
    unsigned int count;
  } cnt;

  printf("loading file %s\n",fname);
  remove(fname);
  if(!(fp=fopen(fname, "wb"))) {
    printf("cannot open output file\n");
    exit(1);
  }

  sport(PORT, 's'); /* tell server to send a file */
  wait(PORT); /* wait until server is ready */

  /* get file length */
  send_file_name(fname);

  sport(PORT, '.'); /* acknowledge */
  cnt.c[0] = rport(PORT);
  sport(PORT, '.'); /* acknowledge */
  cnt.c[1] = rport(PORT);
  sport(PORT, '.'); /* acknowledge */
  
  for(; cnt.count; cnt.count--) {
    ch = rport(PORT);
    putc(ch, fp);
    if(ferror(fp)) {
      printf("error writing file");
      exit(1);
    }
    sport(PORT, '.'); /* acknowledge */
  }
  fclose(fp);
}

/* Send the file name */
void send_file_name(f)
char *f;
{
  do {
     sport(PORT, '?');  /* wait until server is ready */
  } while(!kbhit() && !(check_stat(PORT)&256));
  if(kbhit()) {
    getch();
    exit(1);
  }
  wait(PORT);

  while(*f) {
    sport(PORT, *f++);
    wait(PORT);
  }
  sport(PORT, '\0');  /* null terminator */
  wait(PORT);
}

/* Wait for a response. */
void wait(port)
int port;
{
  if(rport(port)!='.') {
    printf("communication error\n");
    exit(1);
  }
}

/* Send a character out the serial port */
void sport(port, c)
int port; /* i/o port */
char c; /* character to send */
{
  union REGS r;

  r.x.dx = port; /* serial port */
  r.h.al = c;
  r.h.ah = 1;  /* send character function */
  int86(0x14, &r, &r);
  if(r.h.ah & 128) {
    printf("send error detected in serial port %d", r.h.ah);
    exit(1);
  }
}

/* Read a character from a port */
rport(port)
int port; /* i/o port */
{
  union REGS r;

  /* wait for a character */
  while(!(check_stat(PORT)&256)) 
    if(kbhit()) {
      getch();
      exit(1);
    }

  r.x.dx = port; /* serial port */
  r.h.ah = 2;  /* read character function */
  int86(0x14, &r, &r);
  if(r.h.ah & 128) 
    printf("read error detected in serial port");
  return r.h.al;
}

/* Check the status of the serial port. */
check_stat(port)
int port; /* i/o port */
{
  union REGS r;

  r.x.dx = port; /* serial port */
  r.h.ah = 3;  /* read status */
  int86(0x14, &r, &r);
  return r.x.ax;
}

/* Initialize port to 9600 baud, two stop-bits, 
   no parity, 8 data bits. 
*/
void port_init(port)
int port; 
{
  union REGS r;

  r.x.dx = port; /* serial port */
  r.h.ah = 0;  /* initialize port function */
  r.h.al = 231; /* initialization code - see text for details */
  int86(0x14, &r, &r);
}





listing 6-13


/* Store a file on the file server.
*/

#define PORT 0

#include "dos.h"
#include "stdio.h"

unsigned int filesize();
void sport(), send_file(),  send_file_name();
void  port_init(), wait();

main(argc,argv)
int argc;
char *argv[];
{
  if(argc!=2) {
    printf("Usage: get <filename>");
    exit(1);
  }

  port_init(PORT); /* initalize the serial port */

  send_file(argv[1]);
}

/* Send the specified file. */
void send_file(fname)
char *fname;
{
  FILE *fp;  
  char ch;
  union {
    char c[2];
    unsigned int count;
  } cnt;

  if(!(fp=fopen(fname,"rb"))) {
    printf("cannot open input file\n");
    exit(1);
  }
  printf("sending file %s\n", fname);

  /* request service */
  sport(PORT, 'r'); /* request server to receive a file */
  wait(PORT);  /* wait until server is ready */

  send_file_name(fname); /* send the file name */

  if(rport(PORT)!='.') {
    printf("remote file failure\n");
    exit(1);
  }
 
  /* found out the size of the file */
  cnt.count = filesize(fp);
  /* send size */
  sport(PORT, cnt.c[0]);
  wait(PORT); 
  sport(PORT, cnt.c[1]);
  do {
    ch = getc(fp);    

    if(ferror(fp)) {
      printf("error reading input file");
      break;
    } 
    
    /* wait until receiver is ready */ 
    if(!feof(fp)) {
      wait(PORT);
      sport(PORT, ch);
    }
  } while(!feof(fp));
  wait(PORT);/* read the last period from port */
  fclose(fp);
}

/* Return the length, in bytes, of a file */
unsigned int filesize(fp)
FILE *fp;
{
  unsigned long int i;

  i = 0;
  do {
    getc(fp);
    i++;
  } while(!feof(fp));
  rewind(fp);
  return i-1; /* don't count the EOF char */
}

/* Send the file name */
void send_file_name(f)
char *f;
{
  do {
     sport(PORT, '?'); 
  } while(!kbhit() && !(check_stat(PORT)&256));
  if(kbhit()) {
    getch();
    exit(1);
  }
  wait(PORT);

  while(*f) {
    sport(PORT, *f++);
    wait(PORT);
  }
  sport(PORT, '\0'); /* null terminator */
  wait(PORT);
}

/* Wait for a response. */
void wait(port)
int port;
{
  if(rport(port)!='.') {
    printf("communication error\n");
    exit(1);
  }
}

/* Send a character out the serial port */
void sport(port, c)
int port; /* i/o port */
char c; /* character to send */
{
  union REGS r;

  r.x.dx = port; /* serial port */
  r.h.al = c; /* byte to send */
  r.h.ah = 1;  /* send character function */
  int86(0x14, &r, &r);
  if(r.h.ah & 128) {
    printf("send error detected in serial port %d", r.h.ah);
    exit(1);
  }
}

/* Read a character from a port */
rport(port)
int port; /* i/o port */
{
  union REGS r;

  /* wait for a character */
  while(!(check_stat(PORT)&256)) 
    if(kbhit()) {
      getch();
      exit(1);
    }

  r.x.dx = port; /* serial port */
  r.h.ah = 2;  /* read character function */
  int86(0x14, &r, &r);
  if(r.h.ah & 128) 
    printf("read error detected in serial port");
  return r.h.al;
}

/* Check the status of the serial port. */
check_stat(port)
int port; /* i/o port */
{
  union REGS r;

  r.x.dx = port; /* serial port */
  r.h.ah = 3;  /* read status */
  int86(0x14, &r, &r);
  return r.x.ax;
}

/* Initialize port to 9600 baud, two stop-bits, 
   no parity, 8 data bits. 
*/
void port_init(port)
int port; 
{
  union REGS r;

  r.x.dx = port; /* serial port */
  r.h.ah = 0;  /* initialize port function */
  r.h.al = 231; /* initialization code - see text for details */
  int86(0x14, &r, &r);
}




