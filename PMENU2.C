/* M_HANDLE */
/************/
/************/


#include <string.h>	/* strcat(), strcpy() */
#include <alloc.h>	/* farmalloc() */
#include <conio.h>	/* wherex(), gotoxy() */
#include <dir.h>		/* getdisk(), getcurdir() */
#include <dos.h>		/* REGS */
#include "stru.h"
#include "keys.h"
#include "ints.h"

extern void menubuild(void);
extern void error();

void menuinitalize(void);
void menureconstruction(void);
void menu();
int pause();
void shiftstatus();
void f_NULL(void) {;};
void drawkeybar();

void light();   	/* M_TOOLS */
#include <stdarg.h>	/* va_list, va_start */
#include <stdio.h>	/* vsprintf(), printf() */
void delight();
void mopen();
void mclose();
char *wopen();
void wclose();
void settextcolor();
void frame();
void message();
void inc();
void dec();
int bhit1(void);
void bclr1(void);
int bget1(void);
void bunget1();
void bput1();
int bhit2(void);
void bclr2(void);
int bget2(void);
void bunget2();
void bput2();
#define BUFSIZE 500
int b1[BUFSIZE], b2[BUFSIZE];
int b1b,b1e,b2b,b2e;
int key_delay;
unsigned val_delay;


int string();			/* M_STRING */
extern int strw;
extern int strf;
extern int strn;
extern int stro;
extern int strs;
extern int strhp;
extern int strhi;
int rctop, rcbot;

char far *load();			/* M_LOAD */
void csqsort(); 	/* A QUICSORT FOR FILE NAMES */
int fcmp();				/* COMPARE TWO FILE NAMES    */
extern struct menus search;
char curdir[60];		/* CURRENT DIR */
char path[60];			/* CURRENT DIR + DIR OF MASKSTR */
char dir[4]="*.*";
char fulldirs[60];   /* PATH + *.*			*/
char mask[60];	     /* MASK OF MASKSTR 	*/
char fullmask[60];   /* PATH + MASK		*/
char str[60];
char chosen[60];		/* CHOSEN PATH + FILE NAME */
char probadir[60];
int succs;
int n_nevek,elso,ahany,act;
struct ffblk fblock[1];

#include <stdlib.h>		/* random, NULL */   /* M_LOAD */
#include <ctype.h>		/* toupper() */

void f_help(void);		/* M_HELP */
void f_lasthelp(void);

extern char helpfname[];
extern int mac[];
extern struct menus *m;
extern struct menus keybarw;
extern struct keybars h[];
extern int showfreememory;
extern int showshiftstatus;
extern int ssp,ssi,ssx,ssy,fmp,fmi,fmx,fmy;

int defcursor_x, defcursor_y, defcursor_top, defcursor_bottom;
char *defscreen_ptr;
int defdisk;
char defcurdir[60];
unsigned long cleft;
int mouse;
int mx, my;
char helpstr[20];
char helpfilename[60];
struct keybars *keybarptr;
struct menust *dhp;
int si,oldsh;
int b;
int i,j;
union REGS inregs,outregs;
int AX,BX,CX,DX;
int was_macro;
int gorg, gorg2;
int menulevel;
int to_mainmenu;
int quit_menu;
char f_to_main;
struct keybars *keybar_in_main;
void (*f_in_main)();
int choice;
struct menust *prev;
char *nevek[128];
char blankline[90];
char *loadresult;
int bol;
int cur_posx, cur_posy;


/* ллл				other functions 				ллл */

void menuinitalize(void)
{
  int i;

  for(i=0;i<90;i++) blankline[i]=' ';
  defcursor_x=wherex(); defcursor_y=wherey();
  GET_CUR_SIZE(defcursor_top,defcursor_bottom);
  if(defcursor_top<16) then { rctop=16; rcbot=16; }
  rctop=16; rcbot=0;
  defdisk=getdisk(); defcurdir[0]='\\'; getcurdir(0,&defcurdir[1]);
  helpfilename[0]=defdisk+'A'; helpfilename[1]=':';
  strcat(helpfilename,defcurdir);
  if(defcurdir[1]!='\x0') then strcat(helpfilename,"\\");
  strcat(helpfilename,helpfname);
  defscreen_ptr=farmalloc(2*80*25);
  if (defscreen_ptr==NULL) then error("No memory for the screen storage");
  gettext(1,1,80,25,defscreen_ptr);
  CUR_SIZE(rctop,rcbot);
  EGER(0,0,0,0);
  mouse=(AX==0)?0:1;
  if (mouse) then { EGER(1,0,0,0); EGER(4,0,640,200); }
  bclr1(); bclr2();
  gorg=0;
  to_mainmenu=0;
  quit_menu=0;
  f_to_main=0;
  choice=0;
  key_delay=0;
  for (i=0;i<150;i++) {
    nevek[i]=farcalloc(15,sizeof(char));
    if (nevek[i]==NULL) then error("No room for the file names");
  }
  menubuild();
  menu(m);
  menureconstruction();
}


void menureconstruction(void)
{
  if (mouse) then EGER(2,0,0,0);
  puttext(1,1,80,25,defscreen_ptr);
  free(defscreen_ptr);
  settextcolor(15,0); BORDER(0);
  gotoxy(defcursor_x, defcursor_y);
  CUR_SIZE(defcursor_top,defcursor_bottom);
  setdisk(defdisk); chdir(defcurdir);
}


void menu(m)
struct menus *m;
{
  int i,esc;
  int mmp;
  struct menust *mt;

  if (m->bef!=NULL) then m->bef();
  mt=m->st;
  mmp=m->act;
  mopen(m);
  light(m,mt,mmp);
  while(!to_mainmenu) {
    menulevel=m->le;
    if(menulevel=='0' and f_to_main==1) then {
      keybarptr=keybar_in_main;
      (*f_in_main)();
      f_to_main=0;
    }
    keybarptr=m->kbar;
    if (menulevel=='1') then gorg=1;
    if (menulevel>'1') then gorg=0;
    b=0;
    esc=pause(m->l,m->t,m->w,m->h);
    if (esc==2) then {
      for (i=0;i<m->living;++i)
      if (mx>=(mt+i)->l and mx<=(mt+i)->r and
	my==(mt+i)->t) then {
	delight(m,mt,mmp); mmp=i; light(m,mt,mmp); goto cim2;
      }
    }
    if (bhit2()) then b=bget2();
    if (b==256*KEY_F10) then {to_mainmenu=1; gorg=0;}
    if ((b==ESC) and m->le!='0') then goto cim1;
    if (b==KEY_ENTER) then goto cim2;
    for (i=0;i<m->living;++i) {
      if ((mt+i)->c1==b or (mt+i)->c2==b) then {
	delight(m,mt,mmp);
	mmp=i;
	light(m,mt,mmp);
	goto cim2;
      }
    }
    if (b==256*KEY_RIGHT and m->ty=='m') then {
      delight(m,mt,mmp); inc(&mmp,m->living); light(m,mt,mmp);
    }
    else if (b==256*KEY_LEFT and m->ty=='m') then {
      delight(m,mt,mmp); dec(&mmp,m->living); light(m,mt,mmp);
    }
    else if (b==256*KEY_DOWN and m->ty!='m') then {
      delight(m,mt,mmp); inc(&mmp,m->living); light(m,mt,mmp);
    }
    else if (b==256*KEY_UP and m->ty!='m') then {
      delight(m,mt,mmp); dec(&mmp,m->living); light(m,mt,mmp);
    }
    else if (b==256*CTRL_F10) then {
      if ((mt+mmp)->ty=='m') then goto cim2;
    }
    if (m->le=='0') then to_mainmenu=0;
    continue;

cim2:
    if ((mt+mmp)->ty=='f') then {
      gorg2=gorg; gorg=0;
      if ((mt+mmp)->olt=='o') then delight(m,mt,mmp);
      ((mt+mmp)->next.f)();
      if((mt+mmp)->aft!=NULL) then (mt+mmp)->aft();
      light(m,mt,mmp);
      gorg=gorg2;
    }
    else if((mt+mmp)->ty=='i') then {
      keybar_in_main=(mt+mmp)->kbar;
      f_in_main=(mt+mmp)->next.f;
      to_mainmenu=1; gorg=0;
      f_to_main=1;
    }
    else if((mt+mmp)->ty=='q') then {
      quit_menu=1;
      to_mainmenu=1;
    }
    else if((mt+mmp)->ty=='m') then {
      if ((mt+mmp)->olt=='o') then delight(m,mt,mmp);
      menu((mt+mmp)->next.m);
      if((mt+mmp)->aft!=NULL) then (mt+mmp)->aft();
      light(m,mt,mmp);
    }
    else if((mt+mmp)->ty=='t') then {
      if(++(mt+mmp)->next.tr->act>=(mt+mmp)->next.tr->db) then
      (mt+mmp)->next.tr->act=0;
      *(mt+mmp)->next.tr->var=(mt+mmp)->next.tr->act;
      if((mt+mmp)->aft!=NULL) then (mt+mmp)->aft();
      light(m,mt,mmp);
      continue;
    }
    else if((mt+mmp)->ty=='c') then {
      choice=1;
      menu((mt+mmp)->next.m);
      *(mt+mmp)->next.m->var=(mt+mmp)->next.m->act;
      if((mt+mmp)->aft!=NULL) then (mt+mmp)->aft();
      light(m,mt,mmp);
      choice=0;
      continue;
    }
#define Ll (mt+mmp)->next.ldstr->
    else if((mt+mmp)->ty=='l') then {
      gorg2=gorg; gorg=0;
      loadresult=(char far *)load(Ll mask, Ll n, Ll x, Ll y, Ll w, Ll sw);
      if(loadresult!=NULL and loadresult[0]!='\x0') then
	strcpy(Ll str,loadresult);
      if((mt+mmp)->aft!=NULL) then (mt+mmp)->aft();
      light(m,mt,mmp); gorg=gorg2;
      continue;
    }
    else if((mt+mmp)->ty=='s') then {
      gorg2=gorg; gorg=0;
      bol=string(Ll n, Ll str, Ll x, Ll y, Ll w, Ll sw);
      if((mt+mmp)->aft!=NULL) then (mt+mmp)->aft();
      light(m,mt,mmp); gorg=gorg2;
      continue;
    }
    else if(choice==1) then {
      m->act=mmp;
      goto cim1;
    }
    if (m->le=='0' and quit_menu==0) then to_mainmenu=0;
  }

cim1:
   if(choice!=1) then m->act=mmp;
   mclose(m);
}


int pause(l,t,w,h)   /*  0:semmi, 1:bill van, 2:mx,my van  */
int l,t,w,h;
{
  int c1,sh,sh2,db,x,y;
  struct menust *ht;

  cur_posx=wherex(); cur_posy=wherey();
  si=0;
  GETSHIFT(sh2); sh=sh2&0x0f;
  if (sh>0 and sh<4) then si=1;		/* some shift */
  if (sh==4) then si=3;		/* ctrl */
  if (sh==8) then si=2;		/* alt */
  sh=sh2&0x70;
  if (keybarptr!=NULL) then {
    ht=keybarptr->ht[si]; db=keybarptr->db[si];
    if (dhp!=ht or sh!=oldsh) then {oldsh=1;drawkeybar(db,ht);dhp=ht;}
  }
  if (showfreememory==1) then if (cleft!=farcoreleft()) then oldsh=1;
  if (sh!=oldsh) then {
    oldsh=sh;
    if (sh>63) then shiftstatus(1); /* caps */
    sh=sh&0x30;  if (sh>31) then shiftstatus(2);	/* num */
    sh=sh&0x10;  if (sh>15) then shiftstatus(3);	/* scroll */
    if (showfreememory==1) then cleft=farcoreleft();
    x=wherex(); y=wherey();
    gotoxy(fmx,fmy);
    settextcolor(fmi,fmp);
    if (mouse) then EGER(2,0,0,0);
    if (showfreememory==1) then cprintf("%lu",cleft);
    if (mouse) then EGER(1,0,0,0);
    gotoxy(x,y);
  }

  if (mouse) then {
    EGER(5,0,0,0);
    if (BX>=1 or AX==2) then {
      EGER(3,0,0,0);
      mx=CX/8;my=DX/8;
      for (i=0; i<m[0].living; i++) {
	if (mx>=m->st[i].l and mx<=m->st[i].r and
	  my==m->st[i].t) then {
	  bput1(256*KEY_F10); bput1(m->st[i].c1); goto cim5;
	}
      }
      if (keybarptr!=NULL) then
	for (i=0; i<keybarptr->db[si]; i++)
	  if (mx>=ht[i].l and mx<=ht[i].r and
	    my==ht[i].t) then {
	      bput1(ht[i].c1); goto cim5;
	  }
      if (mx<l or mx>l+w-1 or
	my<t or my>t+h-1) then {
	bput1(ESC); goto cim5;
      }
      gotoxy(cur_posx, cur_posy); return(2);
    }
  }

  if (kbhit()) then {
    c1=getch(); if (c1==KEY_JUMP) then c1=256*getch();
    if(c1==27) then { bclr1(); bclr2(); key_delay=0; }
    bput1(c1);
  }
cim5:
  if (bhit2()) then {
    c1=bget2();
    if (c1==256*KEY_F1 and helpstr!=NULL) then { f_help(); gotoxy(cur_posx, cur_posy); return(0); }
    if (c1==256*ALT_F1) then { f_lasthelp(); gotoxy(cur_posx, cur_posy); return(0); }
    if (c1==256*131) then exit(0); /* <alt => emergency exit here */
    if (c1==256*130) then magic(); /* <alt -> some kind of magic  */
    if (keybarptr!=NULL) then
      for (i=0;i<keybarptr->db[si];i++)
	if ((ht+i)->c1==c1 or (ht+i)->c2==c1) then
	  if ((ht+i)->ty=='f') then { ((ht+i)->next.f)(); gotoxy(cur_posx, cur_posy); return(0); }
    bunget2(c1);
    gotoxy(cur_posx, cur_posy); return(1);
  }
  else { gotoxy(cur_posx, cur_posy); return(0); }
}


void shiftstatus(what)
int what;
{
  if (showshiftstatus==1) then {
    if (mouse) then EGER(2,0,0,0);
    settextcolor(ssi,ssp);
    if (what==3) then {
      gotoxy(ssx+9,ssy);
      cprintf("%s"," SCROLL");
    }
    if (what==2) then {
      gotoxy(ssx+5,ssy);
      cprintf("%s"," NUM ");
    }
    if (what==1) then {
      gotoxy(ssx,ssy);
      cprintf("%s"," CAPS ");
    }
    if (mouse) then EGER(1,0,0,0);
  }
}


void drawkeybar(db,ht)
int db;
struct menust *ht;
{
  int i;

  if (mouse) then EGER(2,0,0,0);
  frame(keybarw.l,keybarw.t,keybarw.w,keybarw.h,keybarw.n,
	keybarw.c_w,keybarw.c_f,keybarw.c_n,keybarw.lt);
  for(i=0;i<db;++i) {
    settextcolor(keybarw.c_o,keybarw.c_w);
    gotoxy((ht+i)->l+1,(ht+i)->t+1);
    cprintf("%s",(ht+i)->n);
    settextcolor(keybarw.c_s,keybarw.c_w);
    gotoxy((ht+i)->l+(ht+i)->sb,(ht+i)->t+1);
    for(j=(ht+i)->sb-1;j<(ht+i)->se;j++) cprintf("%c",(ht+i)->n[j]);
  }
  if (mouse) then EGER(1,0,0,0);
}


/* M_TOOLS */
/***********/
/***********/

#define FALSE 0
#define TRUE 1

char lty[][7] = {
  'к','Ф','П','Г','Р','Ф','й',
  'е','Э','И','Г','Р','Ф','й',
  'Щ','Э','Л','К','Ш','Э','М',
  ' ',' ',' ',' ',' ',' ',' '
};


void light(m,mt,i)
struct menus *m;
struct menust *mt;
int i;
{
  struct trimmerst *tr;
  int k,j,len;

  strcpy(helpstr,"`");
  if ((mt+i)->helpstr!=NULL) then strcat(helpstr,(mt+i)->helpstr);
  strcat(helpstr,"`\n");
  if (mouse) EGER(2,0,0,0);
  settextcolor(m->c_hi,m->c_hp);
  gotoxy((mt+i)->l+1,(mt+i)->t+1);
  if((mt+i)->t!=0) then cprintf("%c",' ');
  cprintf("%s",(mt+i)->n);
  if((mt+i)->t!=0) then cprintf("%c",' ');
  if((mt+i)->ty=='t') then {
    tr=(mt+i)->next.tr;
    tr->act=*tr->var;
    gotoxy((mt+i)->el,(mt+i)->t+1);
    cprintf("%s",&tr->ts[tr->ml*tr->act]);
  }
  if((mt+i)->ty=='c') then {
    (mt+i)->next.m->act=*(mt+i)->next.m->var;
    gotoxy((mt+i)->el,(mt+i)->t+1);
    cprintf("%s",((mt+i)->next.m->st+(mt+i)->next.m->act)->n);
  }
  if((mt+i)->ty=='l') then
    if((mt+i)->next.ldstr->str!=NULL and
      (mt+i)->next.ldstr->str[0]!='\x0') then {
      gotoxy((mt+i)->el,(mt+i)->t+1);
      cprintf("%c%c",(mt+i)->next.ldstr->str[0],(mt+i)->next.ldstr->str[1]);
      len=strlen((mt+i)->next.ldstr->str); k=0;
      for(j=0;j<len;j++) if((mt+i)->next.ldstr->str[j]=='\\') then k=j+1;
      cprintf("%s",&((mt+i)->next.ldstr->str[k]));
    }
  if((mt+i)->ty=='s') then
    if((mt+i)->next.ldstr->str!=NULL and
      (mt+i)->next.ldstr->str[0]!='\x0') then {
      gotoxy((mt+i)->el,(mt+i)->t+1);
      cprintf("%s",((mt+i)->next.ldstr->str));
    }
  if (mouse) EGER(1,0,0,0);
}

void delight(m,mt,i)
struct menus *m;
struct menust *mt;
int i;
{
  struct trimmerst *tr;
  int j,sx,k,len;

  strcpy(helpstr,"`MAIN`\n");
  if (mouse) EGER(2,0,0,0);
  settextcolor(m->c_o,m->c_w);
  gotoxy((mt+i)->l+1,(mt+i)->t+1);
  if((mt+i)->t!=0) then cprintf("%c",' ');
  cprintf("%s",(mt+i)->n);
  if((mt+i)->t!=0) then cprintf("%c",' ');
  settextcolor(m->c_s,m->c_w);
  sx=(mt+i)->sb; if((mt+i)->t!=0) then sx++;
  gotoxy((mt+i)->l+sx,(mt+i)->t+1);
  for(j=(mt+i)->sb-1;j<(mt+i)->se;j++) cprintf("%c",(mt+i)->n[j]);
  settextcolor(m->c_o,m->c_w);
  if((mt+i)->ty=='t') then {
    tr=(mt+i)->next.tr;
    tr->act=*tr->var;
    gotoxy((mt+i)->el,(mt+i)->t+1);
    cprintf("%s",&tr->ts[tr->ml*tr->act]);
  }
  if((mt+i)->ty=='c') then {
    (mt+i)->next.m->act=*(mt+i)->next.m->var;
    gotoxy((mt+i)->el,(mt+i)->t+1);
    cprintf("%s",((mt+i)->next.m->st+(mt+i)->next.m->act)->n);
  }
  if((mt+i)->ty=='l') then
    if((mt+i)->next.ldstr->str!=NULL and
      (mt+i)->next.ldstr->str[0]!='\x0') then {
      gotoxy((mt+i)->el,(mt+i)->t+1);
      cprintf("%c%c",(mt+i)->next.ldstr->str[0],(mt+i)->next.ldstr->str[1]);
      len=strlen((mt+i)->next.ldstr->str); k=0;
      for(j=0;j<len;j++) if((mt+i)->next.ldstr->str[j]=='\\') then k=j+1;
      cprintf("%s",&((mt+i)->next.ldstr->str[k]));
    }
  if((mt+i)->ty=='s') then
    if((mt+i)->next.ldstr->str!=NULL and
      (mt+i)->next.ldstr->str[0]!='\x0') then {
      gotoxy((mt+i)->el,(mt+i)->t+1);
      cprintf("%s",((mt+i)->next.ldstr->str));
    }
  if (mouse) EGER(1,0,0,0);
}


void mopen(m)
struct menus *m;
{
  int i;
  struct menust *mt;

  mt=m->st;
  if (mouse) EGER(2,0,0,0);
  m->ptr=wopen(m->l,m->t,m->w,m->h); /* save */
  frame(m->l,m->t,m->w,m->h,m->n,m->c_w,m->c_f,m->c_n,m->lt);
  for(i=0;i<m->db;++i)
    if (i==m->act) then light(m,mt,i); else delight(m,mt,i);
  if (mouse) EGER(1,0,0,0);
}

void mclose(m)
struct menus *m;
{
  wclose(m->ptr,m->l,m->t,m->w,m->h); /* save */
}


char *wopen(l,t,w,h)
int l,t,w,h;
{
  char far *p, far *p1;

  if (mouse) EGER(2,0,0,0);
  if ((p=farmalloc(2*w*h))==NULL) then
    message("No memory for a window","nmfaw");
  p1=p;
  if(p!=NULL) then gettext(l+1,t+1,l+w,t+h,p);
  if (mouse) EGER(1,0,0,0);
  return(p1);
}

void wclose(p,l,t,w,h)
char *p;
int l,t,w,h;
{
  char *p1;

  if (mouse) EGER(2,0,0,0);
  p1=p;
  if(p!=NULL) then puttext(l+1,t+1,l+w,t+h,p);
  free(p1);
  if (mouse) EGER(1,0,0,0);
}

void settextcolor(color,backg)
int color,backg;
{
  textcolor(color);
  textbackground(backg);
}


void frame(l,t,w,h,str,c_w,c_f,c_n,lt)
int l,t,w,h,c_w,c_f,c_n,lt;
char *str;
{
  int l_str;
  int str_begin;
  int i;
  int col;

  if (mouse) EGER(2,0,0,0);
  l_str=strlen(str);
  str_begin=(w-l_str)/2;
  settextcolor(c_f,c_w);
  col=c_w*16+c_f; col&=0x07f;
  gotoxy(l+1,t+1);
  cprintf("%c",lty[lt][0]);
  CHARS(lty[lt][1],col,w-2);
  gotoxy(l+w,t+1);
  cprintf("%c",lty[lt][2]);
  if (l_str!=0 and str!=NULL) then {
    gotoxy(l+str_begin,t+1);
    settextcolor(c_n,c_w);
    cprintf("%c",' ');
    cprintf("%s",str);
    cprintf("%c",' ');
    settextcolor(c_f,c_w);
  }

  for (i=1;i<h-1;++i) {
    gotoxy(l+1,t+1+i);
    cprintf("%c",lty[lt][3]);
    CHARS(' ',col,w-2);
    gotoxy(l+w,t+1+i);
    cprintf("%c",lty[lt][3]);
  }

  gotoxy(l+1,t+h);
  cprintf("%c",lty[lt][4]);
  CHARS(lty[lt][5],col,w-2);
  gotoxy(l+w,t+h);
  cprintf("%c",lty[lt][6]);
  if (mouse) EGER(1,0,0,0);
}


void message(s,h)
char *s,*h;
{
  char *ptr;
  char hsave[30];
  int c;

  bclr1(); bclr2();
  strcpy(hsave,helpstr);
  strcpy(helpstr,"`"); strcat(helpstr,h); strcat(helpstr,"`\n");
  ptr=(char far *)wopen(20,10,40,3);
  frame(20,10,40,3,"Error Message",4,15,15,2);
  settextcolor(15,4); gotoxy(22,12);
  cprintf("%s",s);
  c=getch(); if(c==0) then c=256*getch();
  if(c==256*KEY_F1) then f_help();
  wclose(ptr,20,10,40,3);
  strcpy(helpstr,hsave);
}

void inc(a,n)
int *a,n;
{
  *a=(++*a<n)?(*a):0;
}

void dec(a,n)
int *a,n;
{
  *a=(--*a>=0)?(*a):(n-1);
}

int bhit1(void)
{
  if (b1e!=b1b) then return(1); else return(0);
}

void bclr1(void)
{
  b1e=b1b=0;
}

int bget1(void)
{
  int c;
  if (b1e!=b1b) then { c=b1[b1b]; b1b=(b1b+1)%BUFSIZE; return(c); }
  else return(-1);
}

void bunget1(ch)
int ch;
{
  b1b=(b1b-1)%BUFSIZE; b1[b1b]=ch;
}

void bput1(ch)
int ch;
{
  int was_macro;
  int c1,i;

  if ((b1e+1)%BUFSIZE!=b1b) then { b1[b1e]=ch; b1e=(b1e+1)%BUFSIZE;}

  if (bhit1()) then {
    was_macro=FALSE;
    c1=bget1();
    if (c1==ESC and menulevel=='1') then gorg=0;
    if(c1==256*KEY_RIGHT and gorg==1) then {
      was_macro=TRUE;
      bput2(ESC);bput2(256*KEY_RIGHT);bput2(256*CTRL_F10);
    }
    else if(c1==256*KEY_LEFT and gorg==1) then {
      was_macro=TRUE;
      bput2(ESC);bput2(256*KEY_LEFT);bput2(256*CTRL_F10);
    }
    else {
      i=0;
      while(mac[i]!=257)
      if(c1==mac[i]) then {
	was_macro=TRUE;
	while(mac[i++]!=258) bput2(mac[i]);
      }
      else while(mac[i++]!=258);
    }
    if (!was_macro) then bput2(c1);
  }
}

int bhit2(void)
{
  if (b2e!=b2b) then return(1); else return(0);
}

void bclr2(void)
{
  b2e=b2b=0;
}

int bget2(void)
{
  int c,i;
  if (b2e!=b2b) then {
    c=b2[b2b];
    b2b=(b2b+1)%BUFSIZE;
    if (c==300) then key_delay=1;
    if (c==400) then key_delay=0;
    if (key_delay==1) then
      for(i=0;i<100;i++)
	delay(val_delay);
    return(c);
  }
  else return(-1);
}

void bunget2(ch)
int ch;
{
  b2b=(b2b-1)%BUFSIZE; b2[b2b]=ch;
}

void bput2(ch)
int ch;
{
  if ((b2e+1)%BUFSIZE!=b2b) then { b2[b2e]=ch; b2e=(b2e+1)%BUFSIZE;}
}


/* M_STRING */
/************/
/************/




int string(n,s,l,t,w,m)
char *n, *s;
int l,t,w,m;
{
  int ch;
  int ret;
  int x,y,xe,cur;
  int elsobill;
  int cha, attr;
  int i;
  char *ptr;

  if (mouse) then EGER(2,0,0,0);
  ptr=wopen(l,t,w,3);
  frame(l,t,w,3,n,strw,strf,strn,0);
  settextcolor(strhi,strhp);
  x=l+2; y=t+2;
  gotoxy(x,y); if(s!=NULL) then cprintf("%s",s);
  xe=wherex(); y=wherey(); cur=xe;
  if (mouse) then EGER(1,0,0,0);
  if (s[0]=='\x0') then bput2(8); /* backspace */
  ch=0;
  while(!bhit2() and ch!=2) ch=pause(l,t,w,3);
  elsobill=1;
  if (ch==2) then
    if (mx>=l and mx<=l+w-1 and
      my>=t and my<=t+2) then {
      ret=2; goto cim4;
    }
    else { ret=1; goto cim4; }
  else while(1) {
    while (!bhit2())
      pause(l,t,w,3);
    b=bget2();
    if (b==ESC) then {ret=1; goto cim4;}
    if (b==256*KEY_F10) then {ret=1; bunget2(256*KEY_F10); goto cim4;}
    if (b==KEY_ENTER) then {
      for (i=x; i<xe; i++) {
	gotoxy(i,y);
	CHIN(cha, attr);
	s[i-x]=cha;
      }
      s[xe-x]='\x0';
      ret=2; goto cim4;
    }

    if (elsobill) then switch (b) {
      case 256*KEY_HOME:
      case 256*KEY_END:
      case 256*KEY_LEFT:
      case 256*KEY_RIGHT:
      case 8 /* backspace */:
      CUR_SIZE(defcursor_top,defcursor_bottom);
      settextcolor(stro,strw);
      gotoxy(x,y);
      cprintf("%s",s);
      elsobill=0;
      break;

      default: break;
    }

    if (b==256*KEY_HOME) then {
      cur=x;
      gotoxy(cur,y);
    }
    else if (b==256*KEY_END) then {
      gotoxy(xe,y);
      cur=xe;
      gotoxy(cur,y);
    }
    else if (b==256*KEY_LEFT) then {
      cur--; if (cur<x) then cur=x;
      gotoxy(cur,y);
    }
    else if (b==256*KEY_RIGHT) then {
      cur++; if (cur>xe) then cur=xe;
      gotoxy(cur,y);
    }
    else if (b==256*KEY_DEL) then {
      if (cur<xe) then {
	for (i=cur; i<xe; i++) {
	  gotoxy(i+1,y);
	  CHIN(cha, attr);
	  gotoxy(i,y);
	  CHARS(cha, attr, 1);
	}
	xe--;
	gotoxy(cur,y);
      }

    }
    else if (b==8 /* backspace */) then {
      if (cur>x) then {
	for (i=cur; i<=xe; i++) {
	  gotoxy(i,y);
	  CHIN(cha, attr);
	  gotoxy(i-1,y);
	  CHARS(cha, attr, 1);
	}
	cur--; xe--;
	gotoxy(cur,y);
      }
    }
    else if (b>=31 and b<=128) then {
      if (xe-x<m) then {
	if (elsobill) then {
	  CUR_SIZE(defcursor_top,defcursor_bottom);
	  gotoxy(x-1,y);
	  CHIN(cha, attr);
	  gotoxy(x,y);
	  CHARS(' ', attr, w-2);
	  gotoxy(x,y);
	  cur=x; xe=x;
	  elsobill=0;
	}
	for (i=xe; i>cur; i--) {
	  gotoxy(i-1,y);
	  CHIN(cha, attr);
	  gotoxy(i,y);
	  CHARS(cha, attr, 1);
	}
	xe++;
	settextcolor(stro,strw);
	gotoxy(cur,y);
	cprintf("%c",b);
	cur++;
      }
    }
  }

cim4:
  CUR_SIZE(rctop,rcbot);
  if (mouse) then EGER(2,0,0,0);
  wclose(ptr,l,t,w,3);
  if (mouse) then EGER(1,0,0,0);
  return(ret);
}


/* M_LOAD */
/**********/
/**********/



char far *load(maskstr,name,xpos,ypos,width,swidth)
char *maskstr, *name;
int xpos, ypos, width, swidth;
{
  int esc,escape;
  int i,x,y,j,k,l,len,len2;
  int succ, curdrv;
  struct menus *s;
  int bol;

  str[0]='\x0';
  bol=string(name,maskstr,xpos,ypos,width,swidth);
  if (bol==1) then return(NULL);
  curdir[0]='A'+getdisk(); curdir[1]='\x0';
  strcat(curdir,":\\"); getcurdir(0,curdir+3);
  if (*(curdir+3)!='\x0') then strcat(curdir,"\\");
  strcpy(path,curdir);
  len=strlen(maskstr); len2=strlen(path);
  if(maskstr[1]==':')  then { len2=0; path[0]='\x0'; }
  if(maskstr[0]=='\\') then { len2=2; path[2]='\x0'; }
  k=0; for(j=0;j<=len;j++) if(maskstr[j]=='\\') then k=j+1;
  if(k==0) then strcpy(mask,maskstr);
  else {
    for(j=0;j<k;j++) path[len2++]=maskstr[j]; path[len2]='\x0';
    for(j=k;j<=len;j++) mask[j-k]=maskstr[j]; mask[len-k+1]='\x0';
  }

  s=&search;
  s->ptr=wopen(s->l,s->t,s->w,s->h);

   while(1) {
     strcpy(fulldirs,path); strcat(fulldirs,dir);
     strcpy(fullmask,path); strcat(fullmask,mask);
     strcpy(nevek[0],"[A]"); strcpy(nevek[1],"[B]");
     n_nevek=2;
     curdrv=getdisk();
     for (i=2; i<27; i++) {
       setdisk(i);
       succ=getcurdir(i+1,str);
       if (succ==0 and i<8) then {
	 nevek[n_nevek][0]='[';
	 nevek[n_nevek][1]='A'+i;
	 nevek[n_nevek][2]=']';
	 nevek[n_nevek][3]='\x0';
	 n_nevek++;
       }
     }
     setdisk(curdrv);
     succs=findfirst(fulldirs,fblock,FA_DIREC);
     while (succs==0) {
       if (fblock->ff_attrib=='\x10' and
	 (fblock->ff_name[0]!='.' or fblock->ff_name[1]!='\0')) then {
	 nevek[n_nevek][0]='\\';
	 strcpy(&nevek[n_nevek][1],fblock->ff_name);
	 n_nevek+=1;
       }
       succs=findnext(fblock);
     }
     succs=findfirst(fullmask,fblock,0);
     while (succs==0) {
       strcpy(nevek[n_nevek],fblock->ff_name);
       n_nevek++;
       succs=findnext(fblock);
     }
     csqsort(0,n_nevek-1);
     elso=0;ahany=36;act=0;

     s->n=fullmask;
     window(1,1,80,25);
     frame(s->l, s->t, s->w, s->h, s->n, s->c_w, s->c_f, s->c_n, s->lt);
     window(s->l+3, s->t+2, s->l+s->w-2, s->t+s->h-1);

     while(1) {
       if (mouse) then EGER(2,0,0,0);
       gotoxy(1,1);
       settextcolor(s->c_o,s->c_w);
       for (i=elso,j=0; i<n_nevek and j<ahany; i++,j++) {
	 if (j==act) then settextcolor(s->c_hi,s->c_hp);
	 cprintf("%-14s",*(nevek+i));
	 if (j==act) then settextcolor(s->c_o,s->c_w);
       }
       cprintf("%-55s"," ");
       if (mouse) then EGER(1,0,0,0);
       window(1,1,80,25);
       b=0; esc=0 ;
       while (!bhit2() and esc!=2)
	 esc=pause(s->l,s->t,s->w,s->h);
       window(s->l+3, s->t+2, s->l+s->w-2, s->t+s->h-1);
       if (bhit2()) then b=bget2();
       escape=0;
       if (b==ESC)	    then { b=KEY_ENTER; escape=1; }
       if (b==256*KEY_F10) then { b=KEY_ENTER; escape=2; }
       if (esc==2) then {
	 mx-=s->l; my-=s->t;
	 x=1; y=1; i=0;
	 while(1) {
	   if (my==0) then {b=256*KEY_UP; break;}
	   if (my==s->h-1 or my==s->h-2) then {b=256*KEY_DOWN; break;}
	   if (mx==0) then {b=256*KEY_HOME; break;}
	   if (mx==1) then {b=256*KEY_LEFT; break;}
	   if (mx==s->w-1) then {b=256*KEY_END; break;}
	   if (mx==s->w-2) then {b=256*KEY_RIGHT; break;}
	   if (mx>x and mx<x+15 and my==y) then
	   if (act==i) then {b=KEY_ENTER; break;}
	   else if (i<n_nevek-elso) then {act=i; break;}
	   i++;
	   x+=14;
	   if (x>s->w-10) then {
	     x=1; y+=1; if (y>s->h) then break;
	   }
	 }
       }
       if (b==256*KEY_RIGHT) then {
	 act++;
	 if (elso+act>=n_nevek) then act--;
	 if (act>=ahany) then { cprintf("\n"); elso+=4; act-=4; }
       }
       else if (b==256*KEY_LEFT) then {
	 act=act-1;
	 if (act<0) then {elso-=4; act+=4;}
	 if (elso<0) then { elso=0; act=0; }
       }
       else if (b==256*KEY_HOME) then { elso=0; act=0;}
       else if (b==256*KEY_END) then {
	 if (n_nevek>ahany) then {
	   elso=n_nevek-ahany; act=ahany-1;
	 }
	 else {
	   elso=0; act=n_nevek-1;
	 }
       }
       else if (b==256*KEY_DOWN) then
	 for (i=0;i<4;i++) {
	   act++;
	   if (elso+act>=n_nevek) then act--;
	   if (act>=ahany) then { cprintf("\n"); elso+=4; act-=4; }
	 }
       else if (b==256*KEY_UP) then
	 for (i=0;i<4;i++) {
	   act=act-1;
	   if (act<0) then {elso-=4; act+=4;}
	   if (elso<0) then { elso=0; act=0; }
	 }
       else if (b==KEY_ENTER) then {
	 if (escape==0) then {
	   if (nevek[act+elso][0]=='\\') then {
	     if (nevek[act+elso][1]=='.') then {
	       j=strlen(path)-1;
	       while(path[--j]!='\\') path[j]='\x0';
	     }
	     else {
	       strcat(path,&nevek[act+elso][1]);
	       strcat(path,"\\");
	     }
	     break;
	   }
	   if (nevek[act+elso][0]=='[') then {
	     j=getcurdir(nevek[act+elso][1]-'A'+1,probadir);
	     if(j==-1) then {
	       message(" Something is wrong somewhere","siws");
	       break;
	     }
	     path[0]=nevek[act+elso][1];
	     curdrv=path[0]-'a';
	     path[3]='\x0'; strcat(path,probadir);
	     if (path[3]!='\x0') then strcat(path,"\\");
	     break;
	   }
	 }
	 window(1,1,80,25);
	 wclose(s->ptr,s->l,s->t,s->w,s->h);

	 len=strlen(path); len2=strlen(curdir);
	 l=0; for(k=0;k<len2-1;k++) if(path[k]!=curdir[k]) then l=1;
	 if(l==0) then
	   if(len==len2) then strcpy(maskstr,mask);
	 else {
	   k=len2;
	   for(j=k;j<len;j++) maskstr[j-k]=path[j];
	   maskstr[len-k]='\x0'; strcat(maskstr,mask);
	 }
	 else strcpy(maskstr,fullmask);

	 strcpy(chosen,path); strcat(chosen,nevek[act+elso]);

	 if (escape==0) then return(chosen);
	 if (escape==2) then bunget2(256*KEY_F10);
	 return(NULL);

       }  /* ENTER */
       else for(i=0;i<n_nevek;i++) {
	 if (nevek[i][0]==toupper(b)) then {
	   if (mouse) then EGER(2,0,0,0);
	   settextcolor(s->c_o, s->c_w);
	   clrscr();
	   if (mouse) then EGER(1,0,0,0);
	   elso=i; act=0; break;
	 }
       }
     }  /* WHILE(1) */
   }  /* WHILE(1) */


}

void csqsort(b,j)
int b,j;
{
   int e,v,r;
   char *n1;

   if (j-b<1) then return;
   r=b+random(j-b+1);
   for(e=b,v=j;e<v;) {
     while (fcmp(&nevek[e],&nevek[r])<0) e++;
     while (fcmp(&nevek[r],&nevek[v])<0) v--;
     n1=nevek[e]; nevek[e]=nevek[v]; nevek[v]=n1; e++; v--;
     if (r==e-1) then {v++;r=v;} else if(r==v+1) then {e--;r=e;}
   }
   csqsort(b,r-1);
   csqsort(r+1,j);
}


int fcmp(a,b)
char **a,**b;
{
   if ((*a)[0]=='\\' and (*a)[1]=='.') then return(1);
   else if ((*b)[0]=='\\' and (*b)[1]=='.') then return(-1);
   else if ((*a)[0]=='[' and (*b)[0]!='[') then return(-1);
   else if ((*a)[0]!='[' and (*b)[0]=='[') then return(1);
   else return(strcmp(*a,*b));
}

/* M_HELP */
/**********/
/**********/


extern struct menus helpw;

int helpis=0, helpwindowis=0, lasthelpis=0, lasthelp=0;
int nohelpinfo=1;
char lasthelpstr[32][20];
char helpstr_save[20];
struct help_topic h_top[40];

int f_help2(void);


void f_lasthelp(void)
{
  if (helpis==0) then {
    bput2(256*KEY_F1);
    bput2(256*KEY_F1);
    bput2(256*ALT_F1);
  }
  else bunget2(256*ALT_F1);
}


void f_help(void)
{
  strcpy(helpstr_save,helpstr);
  while (f_help2());
  strcpy(helpstr,helpstr_save);
}



int f_help2(void)
{
  int b,e,c,i,i1,db,in_topic,ret;
  char s[80];
  FILE *helpf;

  db=0; in_topic=0; i=0;
  if ((helpis==0 and helpstr[2]!='\n') or lasthelpis==1) then {
    if (lasthelp==0 or (lasthelpis==0 and
      strcmp(lasthelpstr[lasthelp-1],helpstr)!=0)) then
      if (lasthelp<30) then strcpy(lasthelpstr[lasthelp++],helpstr);
      lasthelpis=0;
      if (helpwindowis==0) then {
	helpw.ptr=wopen(helpw.l,helpw.t,helpw.w,helpw.h);
	frame(helpw.l,helpw.t,helpw.w,helpw.h,helpw.n,
	helpw.c_w,helpw.c_f,helpw.c_n,helpw.lt);
	helpwindowis=1;
      }
      window(helpw.l+3,helpw.t+2,
      helpw.l+helpw.w-2,helpw.t+helpw.h-1);
      if (mouse) then EGER(2,0,0,0);
      settextcolor(helpw.c_f,helpw.c_w);
      clrscr();
      if (mouse) then EGER(1,0,0,0);
      helpis=1; strcpy(s,"cini"); c=' ';
      helpf=fopen(helpfilename,"rt");
      if (helpf==NULL) then {
	printf("Can't find helpfile %s...",helpfilename); exit(0);
      }
      while(strcmp(s,helpstr)!=0 and s[0]!='л')
      fgets(s,80,helpf);
      if (s[0]=='л') then goto hlp4;
      if (mouse) then EGER(2,0,0,0);
      c=' '; settextcolor(helpw.c_f,helpw.c_w);
      while(c!='|' and c!=EOF) {
      c=getc(helpf);
      if (in_topic==1) then h_top[db].topic[i++]=c;
      if(c=='\n') then cprintf("%c%c",10,13);
      else if(c=='\t') then
	cprintf("%c%c%c%c%c%c%c%c",32,32,32,32,32,32,32,32);
      else if(c=='~') then {
	if (in_topic==0) then {
	  in_topic=1;
	  i=0;
	  h_top[db].l=wherex(); h_top[db].t=wherey();
	  settextcolor(helpw.c_o,helpw.c_s);
	}
	else if (in_topic==1) then {
	  in_topic=0;
	  h_top[db].topic[i-1]='\x0';
	  h_top[db].r=wherex()-1;
	  settextcolor(helpw.c_f,helpw.c_w);
	  db++;
	}
      }
      else if(c!='|') then cprintf("%c",c);
    }
    if (mouse) then EGER(1,0,0,0);
hlp4:	fclose(helpf);
    i=0; i1=0;
hlp1:	if (db>0) then {
      if (mouse) then EGER(2,0,0,0);
      gotoxy(h_top[i1].l,h_top[i1].t);
      settextcolor(helpw.c_o,helpw.c_s);
      cprintf("%s",h_top[i1].topic);
      gotoxy(h_top[i].l,h_top[i].t);
      settextcolor(helpw.c_hi,helpw.c_hp);
      cprintf("%s",h_top[i].topic);
      if (mouse) then EGER(1,0,0,0);
    }
hlp3: b=0; e=0;
    while(!bhit2() and e!=2) {
      window(1,1,80,25);
      e=pause(helpw.l,helpw.t,helpw.w,helpw.h);
      window(helpw.l+3,helpw.t+2,
      helpw.l+helpw.w-2,helpw.t+helpw.h-1);
    }
    if (bhit2()) then b=bget2();
    if (b==256*KEY_F10) then {
      wclose(helpw.ptr,helpw.l,helpw.t,helpw.w,helpw.h);
      helpwindowis=0;
      bunget2(b); ret=0; goto hlp2;
    }
    if (b==256*KEY_RIGHT) then { i1=i; inc(&i,db); goto hlp1; }
    if (b==256*KEY_LEFT) then { i1=i; dec(&i,db); goto hlp1; }
    if (b==ESC) then {
      wclose(helpw.ptr,helpw.l,helpw.t,helpw.w,helpw.h);
      helpwindowis=0;
      ret=0; goto hlp2;
    }
    if (b==KEY_ENTER and db>0) then {
      strcpy(helpstr,"`");
      strcat(helpstr,h_top[i].topic);
      strcat(helpstr,"`\n");
      ret=1; goto hlp2;
    }
    if (b==256*ALT_F1) then {
      if (lasthelp>1) then {
	strcpy(helpstr,lasthelpstr[lasthelp-2]);
	lasthelp--;
	lasthelpis=1;
	ret=1; goto hlp2;
      }
    }
    if (e==2) then
      if (db>0) then
	for(i=0;i<db;i++)
	  if (mx>=h_top[i].l+helpw.l+1 and
	    mx<=h_top[i].r+helpw.l+1 and
	    my==h_top[i].t+helpw.t) then {
	    strcpy(helpstr,"`");
	    strcat(helpstr,h_top[i].topic);
	    strcat(helpstr,"`\n");
	    ret=1; goto hlp2;
	  }
    goto hlp3;

hlp2: window(1,1,80,25);
    helpis=0;
    return(ret);
  }
  else {
    return(0);
  }
}

