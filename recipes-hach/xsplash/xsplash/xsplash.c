/*
 * xsplash.c 
 *
 * Copyright (C) 2018 by Hach Company.
 * All rights reserved.
 *
 * Description: a lightweight framebuffer splashscreen for embedded devices.
 * Parts of this file ( fifo handling ) based on 'psplash' copyright 
 * Matthew Allum.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include "xsplash-config.h"

#define MWINCLUDECOLORS
#include <microwin/nano-X.h>	/* GrOpen */

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef int      bool;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define XSPLASH_FIFO "psplash_fifo"

#define CLAMP(x, low, high) \
   (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#define DEBUG 0

#if DEBUG
#define DBG(x, a...) \
   { printf ( __FILE__ ":%d,%s() " x "\n", __LINE__, __func__, ##a); }
#else
#define DBG(x, a...) do {} while (0)
#endif

#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

/* This is the overall background color */
#define XSPLASH_BACKGROUND_COLOR GR_RGB(0x00,0x00,0x00)

/* This is the color of any text output */
#define XSPLASH_TEXT_COLOR GR_RGB(0xff,0xff,0xff)

/* This is the color of the progress bar indicator */
#define XSPLASH_BAR_COLOR GR_RGB(0xff,0xff,0xff)

/* This is the color of the progress bar background */
#define XSPLASH_BAR_BACKGROUND_COLOR GR_RGB(0x00,0x00,0x00)

#define LOGO_IMAGE_FILE "/usr/bin/hachLogo.gif"

#ifdef S_SPLINT_S
typedef /*@abstract@ */ MWCOLORVAL;
typedef /*@abstract@ */ GR_EVENT;
typedef /*@abstract@ */ GR_EVENT_EXPOSURE;
typedef /*@abstract@ */ GR_WINDOW_ID;
typedef /*@abstract@ */ GR_GC_ID;
#endif


static GR_SCREEN_INFO l_xSIP;

static int dX = 0;
static int dY = 0;
#define SPLIT_LINE_POS                                 \
	(  l_xSIP.vs_height                                     \
	 - ((  XSPLASH_IMG_SPLIT_DENOMINATOR                \
	     - XSPLASH_IMG_SPLIT_NUMERATOR)                 \
	    * l_xSIP.vs_height / XSPLASH_IMG_SPLIT_DENOMINATOR) \
	)

void
xsplash_exit (int UNUSED(signum))
{
  DBG("mark");
  GrClose();
  unlink(XSPLASH_FIFO);
}

void
xsplash_draw_msg (GR_WINDOW_ID wid, GR_GC_ID gc,char *msg)
{

  int w,h,b;


  GrGetGCTextSize(gc, msg, -1,
			GR_TFASCII | GR_TFTOP, &w,
			&h,&b);
  h += b;
  DBG("displaying '%s' %ix%i\n", msg, w, h);

  GrSetGCForeground(gc, XSPLASH_BAR_BACKGROUND_COLOR);

  GrFillRect (wid,gc, 
			0, 
			SPLIT_LINE_POS - h, 
			l_xSIP.vs_width,
			h);
  GrSetGCForeground(gc, XSPLASH_TEXT_COLOR);
  GrText (wid,gc,
			(l_xSIP.vs_width-w)/2, 
			SPLIT_LINE_POS - h,msg, -1, GR_TFASCII | GR_TFTOP);

}

void
xsplash_draw_progress ( GR_WINDOW_ID wid,GR_GC_ID gc,int value)
{


  int x, y, width, height, barwidth;

  //4 pix border
  x      = ((l_xSIP.vs_width  - BAR_IMG_WIDTH)/2) + 4 ;
  y      = SPLIT_LINE_POS + 4;
  width  = BAR_IMG_WIDTH - 8; 
  height = BAR_IMG_HEIGHT - 8;

  if (value > 0)
    {
      barwidth = (CLAMP(value,0,100) * width) / 100;
	  GrSetGCForeground(gc, XSPLASH_BAR_BACKGROUND_COLOR);
      GrFillRect(wid,gc, x + barwidth, y, 
    			width - barwidth, height);
	  GrSetGCForeground(gc, XSPLASH_BAR_COLOR);
      GrFillRect (wid,gc, x, y, barwidth,
			    height);
    }
  else
    {
      barwidth = (CLAMP(-value,0,100) * width) / 100;
	  GrSetGCForeground(gc, XSPLASH_BAR_BACKGROUND_COLOR);
      GrFillRect(wid,gc, x, y, 
    			width - barwidth, height);
	GrSetGCForeground(gc, XSPLASH_BAR_COLOR);
      GrFillRect(wid,gc, x + width - barwidth,
			    y, barwidth, height);
    }

  DBG("value: %i, width: %i, barwidth :%i\n", value, 
		width, barwidth);

}

static int 
parse_command (GR_WINDOW_ID w,GR_GC_ID gc,char *string)
{
  char *command;

  DBG("got cmd %s", string);
	
  if (strcmp(string,"QUIT") == 0)
    return 1;

  command = strtok(string," ");

  if (!strcmp(command,"PROGRESS")) 
    {
      xsplash_draw_progress (w, gc,atoi(strtok(NULL,"\0")));
    } 
  else if (!strcmp(command,"MSG")) 
    {
      xsplash_draw_msg (w,gc,strtok(NULL,"\0"));
    } 
  else if (!strcmp(command,"QUIT")) 
    {
      	GrDestroyGC(gc);
  		GrClose();
		return 1;
    }

  return 0;
}

void 
xsplash_main ( GR_WINDOW_ID w,GR_GC_ID gc,int pipe_fd, int timeout) 
{
  int            err;
  ssize_t        length = 0;
  fd_set         descriptors;
  struct timeval tv;
  char          *end;
  char           command[2048];

  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  FD_ZERO(&descriptors);
  FD_SET(pipe_fd, &descriptors);

  end = command;

  while (1) 
    {
      if (timeout != 0) 
	err = select(pipe_fd+1, &descriptors, NULL, NULL, &tv);
      else
	err = select(pipe_fd+1, &descriptors, NULL, NULL, NULL);
      
      if (err <= 0) 
	{
	  /*
	  if (errno == EINTR)
	    continue;
	  */
	  return;
	}
      
      length += read (pipe_fd, end, sizeof(command) - (end - command));

      if (length == 0) 
	{
	  /* Reopen to see if there's anything more for us */
	  close(pipe_fd);
	  pipe_fd = open(XSPLASH_FIFO,O_RDONLY|O_NONBLOCK);
	  goto out;
	}
      
      if (command[length-1] == '\0') 
	{
	  if (parse_command(w,gc,command))
	    return;
	  length = 0;
	} 
      else if (command[length-1] == '\n') 
	{
	  command[length-1] = '\0';
	  if (parse_command(w,gc,command))
	    return;
	  length = 0;
	} 


    out:
      end = &command[length];
    
      tv.tv_sec = timeout;
      tv.tv_usec = 0;
      
      FD_ZERO(&descriptors);
      FD_SET(pipe_fd,&descriptors);
    }

  return;
}

int main(int argc, char *argv[])
{

	GR_WINDOW_ID w;
	GR_GC_ID gc;
	GR_IMAGE_ID iid;
	GR_IMAGE_INFO iif;
  	char      *tmpdir;
  	int        pipe_fd;


  	signal(SIGHUP, xsplash_exit);
  	signal(SIGINT, xsplash_exit);
  	signal(SIGQUIT, xsplash_exit);

	printf("xsplash: 1.0 $ " __TIME__ "\n");

	if ((argc != 1) && (argc != 3)) {
		fprintf(stderr, "Usage: %s [dX dY]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Disable framebuffer console cursor */
	int fd = open("/sys/class/graphics/fbcon/cursor_blink", O_WRONLY);
	if (fd >= 0) {
		write(fd, "0", 1);
		close(fd);
	}

	if (3 == argc) {
		/* dX and dY is for testing the display to see that lines
		 * vanish from display */
		dX = atoi(argv[1]);
		dY = atoi(argv[2]);
	}
  tmpdir = "/tmp";
  chdir(tmpdir);

  if (mkfifo(XSPLASH_FIFO, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP))
    {
      if (errno!=EEXIST) 
	    {
	      perror("mkfifo");
	      exit(-1);
	    }
    }

  pipe_fd = open (XSPLASH_FIFO,O_RDONLY|O_NONBLOCK);
  
  if (pipe_fd==-1) 
    {
      perror("pipe open");
      exit(-2);
    }

	if (GrOpen() < 0) {
		fprintf(stderr, "cannot open graphics\n");
 		unlink(XSPLASH_FIFO);
		exit(EXIT_FAILURE);
	}

	GrGetScreenInfo(&l_xSIP);
	/* create window, center it in display */
	w = GrNewWindowEx(GR_WM_PROPS_NOAUTOMOVE,
			  (unsigned char *)"nanox_mini", GR_ROOT_WINDOW_ID,
			  dX, dY, l_xSIP.vs_width, l_xSIP.vs_height, XSPLASH_BACKGROUND_COLOR);

	GrMapWindow(w);
	gc = GrNewGC();

	GrSetGCForeground(gc, XSPLASH_BACKGROUND_COLOR);

	//GrFillRect(w, gc, 0, 0,l_xSIP.vs_width, l_xSIP.vs_height);
	GrFillRect(w, gc, (l_xSIP.vs_width  - BAR_IMG_WIDTH)/2,
			 SPLIT_LINE_POS, BAR_IMG_WIDTH, BAR_IMG_HEIGHT);

	if(!(iid = GrLoadImageFromFile(LOGO_IMAGE_FILE, 0))) {
		fprintf(stderr, "Failed to load image file \"%s\"\n", LOGO_IMAGE_FILE);
		return 0;
	}
	GrGetImageInfo(iid, &iif);

	GrDrawImageToFit(w,gc,(l_xSIP.vs_width  - iif.width)/2, 
			 (l_xSIP.vs_height * XSPLASH_IMG_SPLIT_NUMERATOR
			  / XSPLASH_IMG_SPLIT_DENOMINATOR - iif.height)/2,
			 iif.width,
			 iif.height, iid);

	xsplash_main(w,gc,pipe_fd,0);
    
	GrDestroyGC(gc);
    GrClose();
  	unlink(XSPLASH_FIFO);

}

