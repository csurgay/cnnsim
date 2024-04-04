#include "m_incl.h"		/* pause, bhit2, bget2, message, mouse, REGS,
						EGER, settextcolor */

#include <alloc.h>		/* farrealloc(), farmalloc(), farfree() */
#include <conio.h>		/* clrscr(), window(), gettext(), puttext() */
#include <string.h>		/* strcat() */
#include <stdlib.h>		/* itoa() */

#define MAXLEVEL 8

#define minx   1  /* Area of Free Movement for Miki */
#define miny   2
#define maxx  80
#define maxy  24
#define attr_border 23 /* white on blue */
#define attr_data 0   /* black on black */

/***************************************************************************/
/************************* Declaration of functions ************************/
extern void status(void);

void miki(void);
void miki_config(void);

static void win_border();           		      /*ok*/
static void win_data_generate();                      /*ok*/
static void win_display();                            /*ok*/
static void win_info_generate(int,int,int,int,int);   /*ok*/
static void win_manipulate();			      /*ok*/
static void win_row(char,char,char);		      /*ok*/

/***************************************************************************/
/******************* Definition of structures and types ********************/

struct {int ln,        /* the number of the corresponding level (0=input) */
	    lstx,lsty, /* the upper-left corner of the area displayed
			  (in neurons, (1,1) is the minimum) */
	    lsix,lsiy, /* the size of the area displayed (in neurons) */
	    wstx,wsty, /* the upper-left corner of the window displayed
			  (in characters, (1,1) is the minimum) */
	    wenx,weny, /* the lower-right corner of the window displayed
			  (in charaters, (80,25) is the maximum) */
	    wsix,wsiy, /* the size of the window displayed (in characters) */
	    wlen;      /* the amount of memory needed to store the window */
	float huge *laddr; /* the address of the first neuron */
	char *waddr,   /* the address of the copy of the window displayed */
	     *saddr;   /* the address of the saved area hidden by the
			  window displayed */
	} win_info[MAXLEVEL+1];

typedef char string[81];

/***************************************************************************/
/************************* Declaration of variables ************************/

extern int defc,defg;
extern int level;
extern int rt;
extern int lev[];
extern float huge *o[];
extern float sigm;
extern float cent;
extern int aw;
extern int awin;
extern int al;
extern int alin;
extern char ns,is,ws;
extern int first_status;

int first_mikiconfig=1;
int ch, attr;
float nco, range, ci, cp;

unsigned char col[8][8]={ { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 },
			{ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 },
			{ 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27 },
			{ 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37 },
			{ 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47 },
			{ 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57 },
			{ 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67 },
			{ 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77 } };

static int i,j,win_no;
static string ki1,ki2;

/***************************************************************************/
/************************* Definition of functions *************************/

void miki(void)
{
	int i;

  win_no=0;

	if(first_mikiconfig==1) then
		if(is=='n') then {
			message(" I can't show anything","icsa");
			return;
		}
		else {
			win_info_generate(0,1,1,awin,alin);
			win_border();
			win_data_generate();
			win_display();
			status();
			return;
		}

	for (i=0;i<level+1;i++) {
		if (lev[i]!=0) {
			win_data_generate();
			win_display();
			win_no++;
		} /* endif */
	} /* endfor */
	status();
}

void miki_config(void)
{
	int i;

	if (ns=='n') then { message(" The net is not initalized","nini"); return; }

  win_no=0;
	if(mouse) then EGER(2,0,0,0);
	window(minx,miny,maxx,maxy);
	settextcolor(defc,defg);
	clrscr();
	window(1,1,80,25);
	if(mouse) then EGER(1,0,0,0);
	first_status=0;
	first_mikiconfig=0;

	for (i=0;i<level+1;i++) {
		if (lev[i]!=0) {
			win_info_generate(i,1,1,awin,alin);
			win_border();
			win_data_generate();
			win_display();
			win_manipulate();
			win_no++;
		} /* endif */
	} /* endfor */
	ws='y';
	status();
}

/*************************** Manipulate a window ***************************/

static void win_manipulate()
{
	int oper2;

further_manipulate:

		while(!bhit2()) pause(2,2,78,23);
		oper2=bget2();

    puttext(win_info[win_no].wstx,win_info[win_no].wsty,
	    win_info[win_no].wenx,win_info[win_no].weny,win_info[win_no].saddr);
		if ((oper2==256*77)&&(win_info[win_no].wenx<maxx)) { /* right */
      win_info[win_no].wstx++;
      win_info[win_no].wenx++;
		} else if ((oper2==256*75)&&(win_info[win_no].wstx>minx))  { /* left */
      win_info[win_no].wstx--;
      win_info[win_no].wenx--;
		} else if ((oper2==256*80)&&(win_info[win_no].weny<maxy))  { /* down */
      win_info[win_no].wsty++;
      win_info[win_no].weny++;
		} else if ((oper2==256*72)&&(win_info[win_no].wsty>miny))  { /* up */
      win_info[win_no].wsty--;
		win_info[win_no].weny--;
	 } else if (oper2==256*73)  {										/* pg up */
      win_info[win_no].wsty=miny;
      win_info[win_no].weny=miny+win_info[win_no].wsiy-1;
	 } else if (oper2==256*81)  {                             /* pg down */
      win_info[win_no].weny=maxy;
      win_info[win_no].wsty=maxy-win_info[win_no].wsiy+1;
		} else if (oper2==256*71)  {                               /* home */
      win_info[win_no].wstx=minx;
      win_info[win_no].wenx=minx+win_info[win_no].wsix-1;
		} else if (oper2==256*79)  {                               /* end */
      win_info[win_no].wenx=maxx;
      win_info[win_no].wstx=maxx-win_info[win_no].wsix+1;
		} else if ((oper2=='6')&&(win_info[win_no].lstx>1))  { /* pleft */
      win_info[win_no].lstx--;
      win_info[win_no].laddr--;
      win_data_generate();
		} else if ((oper2=='2')&&(win_info[win_no].lsty>1))  { /* pup */
      win_info[win_no].lsty--;
      win_info[win_no].laddr-=aw;
      win_data_generate();
		} else if ((oper2=='8')&&(win_info[win_no].lsty+
			      win_info[win_no].lsiy-1<alin))  { /* pdown */
      win_info[win_no].lsty++;
      win_info[win_no].laddr+=aw;
      win_data_generate();
		} else if ((oper2=='4')&&(win_info[win_no].lstx+
			      win_info[win_no].lsix-1<awin))  { /* pright */
      win_info[win_no].lstx++;
      win_info[win_no].laddr++;
      win_data_generate();
		} else if ((oper2==256*115)&&(win_info[win_no].lsix>0))  { /* rleft */
      win_info[win_no].lsix--;
			win_info[win_no].wsix--;
      win_info[win_no].wenx--;
      win_info[win_no].wlen-=2*win_info[win_no].wsiy;
      win_info[win_no].waddr=farrealloc(win_info[win_no].waddr,win_info[win_no].wlen);
      win_info[win_no].saddr=farrealloc(win_info[win_no].saddr,win_info[win_no].wlen);
      win_border();
      win_data_generate();
		} else if ((oper2==256*132)&&(win_info[win_no].lsiy>1))  { /* rup */
      win_info[win_no].lsiy-=2;
      win_info[win_no].wsiy--;
      win_info[win_no].weny--;
      win_info[win_no].wlen-=2*win_info[win_no].wsix;
      win_info[win_no].waddr=farrealloc(win_info[win_no].waddr,win_info[win_no].wlen);
      win_info[win_no].saddr=farrealloc(win_info[win_no].saddr,win_info[win_no].wlen);
      win_border();
      win_data_generate();
		} else if ((oper2==256*118)&&(win_info[win_no].lsty+win_info[win_no].lsiy<alin)
			  &&(win_info[win_no].weny<maxy))  { /* rdown */
      win_info[win_no].lsiy+=2;
      win_info[win_no].wsiy++;
      win_info[win_no].weny++;
      win_info[win_no].wlen+=2*win_info[win_no].wsix;
      win_info[win_no].waddr=farrealloc(win_info[win_no].waddr,win_info[win_no].wlen);
      win_info[win_no].saddr=farrealloc(win_info[win_no].saddr,win_info[win_no].wlen);
      win_border();
      win_data_generate();
		} else if ((oper2==256*116)&&(win_info[win_no].lstx+win_info[win_no].lsix-1<awin)
			  &&(win_info[win_no].wenx<maxx))  { /* rright */
      win_info[win_no].lsix++;
      win_info[win_no].wsix++;
      win_info[win_no].wenx++;
      win_info[win_no].wlen+=2*win_info[win_no].wsiy;
      win_info[win_no].waddr=farrealloc(win_info[win_no].waddr,win_info[win_no].wlen);
      win_info[win_no].saddr=farrealloc(win_info[win_no].saddr,win_info[win_no].wlen);
      win_border();
      win_data_generate();
    } /* endif */
    win_display();
		if (oper2==13 or oper2==27) then goto exit_win_manipulate;

	goto further_manipulate;

exit_win_manipulate: ;
} /* end win_manipulate */

/************************ Generate info on a window ************************/

static void win_info_generate(level_no,start_x,start_y,size_x,size_y)
       int level_no,start_x,start_y,size_x,size_y;
{
    win_info[win_no].ln=level_no;
    win_info[win_no].lstx=start_x;
    win_info[win_no].lsty=start_y;
    win_info[win_no].lsix=size_x;
    win_info[win_no].lsiy=size_y;
    win_info[win_no].laddr=o[level_no]+(rt+start_y-1)*aw+rt+start_x-1;
    win_info[win_no].wsix=size_x+2;
		win_info[win_no].wsiy=(size_y+5)/2;
    win_info[win_no].wstx=(maxx+minx-win_info[win_no].wsix+1)/2;
		if (first_mikiconfig==1) then win_info[win_no].wstx=minx;
    win_info[win_no].wsty=(maxy+miny-win_info[win_no].wsiy+1)/2;
		if (first_mikiconfig==1) then win_info[win_no].wsty=miny;
    win_info[win_no].wenx=win_info[win_no].wstx+win_info[win_no].wsix-1;
    win_info[win_no].weny=win_info[win_no].wsty+win_info[win_no].wsiy-1;
    win_info[win_no].wlen=2*win_info[win_no].wsix*win_info[win_no].wsiy;
		if(win_info[win_no].waddr!=NULL) then farfree(win_info[win_no].waddr);
		win_info[win_no].waddr=farmalloc(win_info[win_no].wlen);
		if(win_info[win_no].waddr==NULL) then error("mikike");
		if(win_info[win_no].saddr!=NULL) then farfree(win_info[win_no].saddr);
		win_info[win_no].saddr=farmalloc(win_info[win_no].wlen);
		if(win_info[win_no].saddr==NULL) then error("mikike2");
} /* end win_info_generate */

/************************ Generate data on a window ************************/

static void win_data_generate()
{
  nco=7.9; /* ch=0; attr=ch; */
  range=(float)nco/(2.0*sigm);

  for (j=0;(j<win_info[win_no].lsiy);j+=2)
    for (i=0;i<win_info[win_no].lsix;i++) {
	gotoxy(20,20);
	ci=(win_info[win_no].laddr[i+ j   *aw]-cent+sigm)*range;
	cp=(win_info[win_no].laddr[i+(j+1)*aw]-cent+sigm)*range;

/* 	settextcolor((int)ci,(int)cp);		*/
/* 	gotoxy(80,1); cprintf("%c",'ß');   */
/* 	gotoxy(80,1); CHIN(ch,attr);        */
/* 	cprintf("%d,%d,%d.",cp,ci,attr);     */

	win_info[win_no].waddr[(2+j)*win_info[win_no].wsix+(1+i)*2+1]=16*(int)cp+(int)ci;
	win_info[win_no].waddr[(2+j)*win_info[win_no].wsix+(1+i)*2]='ß';
    }
}

/***************************** Display a window ****************************/

static void win_display()
{
	if(mouse) then EGER(2,0,0,0);
  gettext(win_info[win_no].wstx,win_info[win_no].wsty,win_info[win_no].wenx,
	  win_info[win_no].weny,win_info[win_no].saddr);
  puttext(win_info[win_no].wstx,win_info[win_no].wsty,win_info[win_no].wenx,
	  win_info[win_no].weny,win_info[win_no].waddr);
	if(mouse) then EGER(1,0,0,0);
} /* end win_display() */

/*********************** Draw the border of a window ***********************/

static void win_border()
{
	int i;

	j=0;
	win_row('Õ','Í','¸');
	for (i=2;i<win_info[win_no].wsiy;i++) {
		win_row('³',' ','³');
  } /* endfor */
	win_row('À','Ä','Ù');

	strcpy(ki1,"");
	if (win_info[win_no].lsix>2) {
		strcat(ki1,"µ");
		if (win_info[win_no].lsix>8) {
			strcat(ki1,"level ");
		} /* endif */
		itoa(win_info[win_no].ln,ki2,10);
		strcat(ki1,ki2);
		strcat(ki1,"Æ");
	} /* endif */
  for (i=0;i<strlen(ki1);i++) {
		win_info[win_no].waddr[2*(i+(win_info[win_no].wsix-strlen(ki1))/2)]=ki1[i];
		win_info[win_no].waddr[2*(i+(win_info[win_no].wsix-strlen(ki1))/2)+1]=attr_border;
  } /* endfor */
} /* end win_border */

/************************* Draw a row of the border ************************/

static void win_row(c1,c2,c3)
       char c1,c2,c3;
{
  int i;

  i=0;
  win_info[win_no].waddr[j]=c1;
  win_info[win_no].waddr[j+1]=attr_border;
  if (c2!=' ') {
    for (i=1;i<win_info[win_no].wsix-1;i++) {
      win_info[win_no].waddr[j+2*i]=c2;
      win_info[win_no].waddr[j+2*i+1]=attr_border;
    } /* endfor */
  } /* endif */
  j+=2*win_info[win_no].wsix;
  win_info[win_no].waddr[j-2]=c3;
  win_info[win_no].waddr[j-1]=attr_border;
} /* end win_row() */
