#include "m_incl.h"

#include <alloc.h>    /* farcalloc(), farcoreleft(), farfree() */
#include <conio.h>    /* clrscr(), puttext(), gotoxy(), getch() */
#include <string.h>   /* strlen(), strcpy() */
#include <stdlib.h>   /* atof() */

int defc=7;
int defg=1;

/* ллл		function prototypes	     ллл */
void main(void);
extern void miki(void);
extern void miki_config(void);

int akosmain();
void setdefault(void);
void f_quit(void);
void net_init(void);
extern void base_iter(void);
extern void iter2(void);
extern void iterall(void);
extern void del_iter(void);
extern void del_iterall(void);
void (* iter[])(void) = { base_iter,iter2,iterall,del_iter,del_iterall };
extern void initinf(void);
extern void inittf(void);
void f_fluently(void);
void f_stepbystep(void);
void run(void);
void status(void);
void inp_set(void);
void tem_set(void);
void f_chstatus(void);
void f_statusconfig(void);
void before_main(void);

void (*fun_array[])(void) = {
  f_NULL,f_NULL,inp_set,status,tem_set,f_chstatus,f_statusconfig,
  net_init,miki,miki_config,run,before_main
};

int run_mode=0, rt=0;
int lev[]={1,1,0,0,0,0,0,0,0,0}; int sts[]={1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,1};
int in[]={1,2,0,0,0,0,0,0,0};
int *int_array[]={ &run_mode,&run_mode,&rt,
  &lev[0],&lev[1],&lev[2],&lev[3],&lev[4],&lev[5],&lev[6],&lev[7],&lev[8],
  &sts[0],&sts[1],&sts[2],&sts[3],&sts[4],&sts[5],&sts[6],&sts[7],&sts[8],
  &sts[9],&sts[10],&sts[11],&sts[12],&sts[13],&sts[14],
  &in[1],&in[2],&in[3],&in[4],&in[5],&in[6],&in[7],&in[8]
};

char s_tem[60]="",
  s_inp[60]="",
  s_net[]="Initialized", s_win[]="Configured";
char s_no_level[30]="", s_resis[30]="1.000", s_capac[30]="1.000",
  s_tst[30]="1.000", s_curre[30]="0.000";
char *str_array[]={ s_no_level, s_resis, s_capac, s_tst, s_tem, s_inp,
  s_curre };

/********************	      CONSTANTS        **************************/

#define MAXLEVEL 8 /* max. no. of level */
#define MAXDELAY 5 /* max. delay in time
			for example :
			delay=3 --> you can use the state and the output
			array you've define 3 'minutes' before */

/**********************     GLOBAL VARIABLES	 *************************/

int first_status=0;
int st_l, st_t, st_w, st_h;
struct status_struct {
	int db;
	int l,t,w,h;
	char *n;
	int c_w,c_f,c_n,lt;
	char *ptr;
} st = { 15,54,11,26,13,"Status",1,7,15,2,NULL };


char
  fname[60],      /*?*/
  ns='n',    /* if the net is initialized then ns='y' else ns='n' */
  is='n',    /* if the input level is set then is='y' else is='n' */
  ts='n',    /* if any template is set then ts='y' else ts='n' */
  ws='n';

unsigned long
  psw,   /* place for setting window -> Peter + Miki*/
  sumn,  /* the actual size of the net */
  sumi,  /* the actual size of the input */
  sumt,  /* the actual size of the templates (all) */
  sumact,/* this is my free memory
	  at the beginning it means the whole
	  memory I've allocated */
  sum;   /* the memory for me */

int
  level, /* the no. of levels */
  step,
  int_e,
  act_iter,  /* this is the code of the actual iteration */
  al=0, /* actual length of the input and the other levels */
  alin=0 ,     /* the length from the input file */
  aw=0,  /* actual width of the input and the other levels */
  awin=0,     /* the width from the input file */
  rt1,   /* rt1=2*rt+1 ,because this value is used so much */
  change[MAXDELAY+1],   /* this arrays help us to remember which delo
			array contains the values of any delay-time
			for example change[3]==2 means :delo[2]contains
			the values iterated 3 time_step earlier */
  del=0; /* actual delay --> if del=3 then you can
		use the previous values of the s,o arrays
		that was defined (1,2,3)*time step earlier */

float
  float_e,
  defo[MAXLEVEL+1],/* the values we must initialize the arrays with */
  defs[MAXLEVEL+1],
  mps1,mps2, /* mps1,mps2 are the number we must multiplicate the previous
			states and the other parameters with */
  tst,   /* time step */
  huge *thloc, /* the pointer --> the last free byte+1*/
  huge *hmain, /* the pointer --> the start of my place in memory */
  huge * t,  /* template - max. radius: 2 */
  /* huge * dels[MAXDELAY+1][MAXLEVEL+1], */
  huge * delo[MAXDELAY+1][MAXLEVEL+1],
  huge * s[MAXLEVEL+1], /* levelpointers of the state array */
  huge * o[MAXLEVEL+1], /* levelpointers of the output array
			   and of the input array */
  huge * it[MAXDELAY+1][MAXLEVEL+1][MAXLEVEL+1],
		 /* it[0][i][j] is a pointer,it[0][i][j]<>NULL
		       means that the template between level
		       i and j is set,and this pointer shows
			 the first element of this template */
  cent,  /* the y-coordinate of the center of the function sigmoid */
  sigm, /* the max value on the function sigmoid */
  R,  /*  resistance */
  C,  /* capacity */
  II;  /* current */

/************************     FUNCTIONS     ******************************/
void set_default(void)
{
  int x,y,z;

  st.ptr=farcalloc(st.w*(st.db+3)*2, sizeof(char));
  if (st.ptr==NULL) then message("No memory for Status window","nmfsw");

  psw=120000l;  /* Miki 70Kbyte */
  sigm=1;
  cent=0;
  R=1;
  C=1;
  II=0;
  rt=1;   /* it's needed because it can be that at first we load the
		 input , and we must know the rt to do the frame */
  rt1=3;
  tst=1.0;
  level=0;
  for (x=0;x!=MAXLEVEL+1;x++) {
    defo[x]=-1.0;
    defs[x]=-1.0;
  }
  for (z=0;z!=MAXDELAY+1;z++)
    for (x=0;x!=MAXLEVEL+1;x++)
      for (y=0;y!=MAXLEVEL+1;y++)
	it[z][x][y]=NULL;
  del=0;

  sumact=farcoreleft();
  if(sumact<psw) then error("Not enough memory for CNNBME...");
  sumact-=psw;
  sumact-=000;
  hmain=(float huge *)(farmalloc(sumact));
  if (hmain==NULL) then error("Not enough memory for CNNBME...");
  sumact/=sizeof(float);
  sum=sumact;
  sumn=0;
  sumt=0;
  sumi=0;
  thloc=hmain+sumact;
  o[0]=hmain;
}

void net_init(void)
{
  int x,y,i,z;
  int n_i_error;

  n_i_error=0;

  if (is=='n') {
    message(" Input is not set ","iins");
    n_i_error=1;
  }
  if (ts=='n') {
    message(" Template is not set ","tins");
    n_i_error=1;
  }
  int_e=atoi(s_no_level);
  if(int_e==0 and s_no_level[0]!='0') then {
    message (" 'Layers' is not int","lini"); n_i_error=1;
  }
  else if (int_e==0) then {
    message(" 'Layers' is zero","liz"); n_i_error=1;
  }
  else level=int_e;
  float_e=atof(s_resis);
  if(float_e==0 and s_resis[0]!='0') then {
    message (" 'Resistance' is not float","rinf"); n_i_error=1;
  }
  else if (float_e==0) then {
    message(" 'Resistance' is zero","riz"); n_i_error=1;
  }
  else R=float_e;
  float_e=atof(s_capac);
  if(float_e==0 and s_capac[0]!='0') then {
    message (" 'Capacity' is not float","cinf"); n_i_error=1;
  }
  else if (float_e==0) then {
    message(" 'Capacity' is zero","ciz"); n_i_error=1;
  }
  else C=float_e;
  float_e=atof(s_tst);
  if(float_e==0 and s_tst[0]!='0') then {
    message (" 'Time step' is not float","tsinf"); n_i_error=1;
  }
  else if (float_e==0) then {
    message(" 'Time step' is zero","tsiz"); n_i_error=1;
  }
  else tst=float_e;
  float_e=atof(s_curre);
  if(float_e==0 and s_curre[0]!='0') then {
    message (" 'Current' is not float","cuinf"); n_i_error=1;
  }
  else II=float_e;

  if (n_i_error==1) then return;

  step=0;
  rt1=rt+rt+1;
  aw=awin+2*rt; /* the net has got a frame ,the width of the frame : rt */
  al=alin+2*rt;
  act_iter=akosmain();
  if ((act_iter==2)||(act_iter==4)) {
    mps1=1-(tst/(R*C));
    mps2=tst/C;
  }

	/************** set the level pointers *****************/

  if (ns=='y') {
    sumact+=sumn;
    ns='n';
  }
  sumn=((2.0+del)*(unsigned long)level)*aw*al;
  if (sumact<sumn) {
    message(" No memory to set the net ","nmsn");
    return;
  }

  o[1]=hmain+aw*al;
  for (x=2;x!=level+1;x++) {
    o[x]=o[x-1]+aw*al;
  }
  s[0]=s[1]=o[level]+aw*al;   /* s[0]=s[1] !!! we are not to load the
				the input array twice */
  for (x=2;x!=level+1;x++) {
    s[x]=s[x-1]+aw*al;
  }
  if (del>=1) {
    for (x=0;x!=level+1;x++)
    delo[0][x]=o[x];
    for (x=1;x!=del+1;x++)
    delo[x][0]=hmain;
    delo[1][1]=s[level]+aw*al;
    for (x=2;x!=level+1;x++) {
      delo[1][x]=delo[1][x-1]+aw*al;
    }
    for (y=2;y!=del+1;y++) {
      delo[y][1]=delo[y-1][level]+aw*al;
      for (x=2;x!=level+1;x++) {
	delo[y][x]=delo[y][x-1]+aw*al;
      }
    }
    for (x=1;x!=del+1;x++)
      change[x]=x;
  } /* end del>=1 */
  sumact-=sumn;
  ns='y';

  /********************    initialize the net     ************************/

  /* a C rogton 0-kat tesz a lefoglalt  teruletre ? */

  for (i=1;i!=level+1;i++) {
    for (y=0;y!=al*aw;y+=aw) {
      for (x=0;x!=aw;x++) {
	s[i][y+x]=defs[i];  /* the default values  - it's important */
	o[i][y+x]=defo[i];  /* because of the frame,too */
      }
    }

    if (in[i]==2) {
      for (y=rt*aw;y!=(rt+alin)*aw;y+=aw)
      for (x=rt;x!=rt+awin;x++)
      o[i][y+x]=o[0][y+x];
    }
    else if(in[i]==3) {
      for (y=rt*aw;y!=(rt+alin)*aw;y+=aw)
      for (x=rt;x!=rt+awin;x++)
      o[i][y+x]=-o[0][y+x];
    }
    else if(in[i]==0) {
      for (y=rt*aw;y!=(rt+alin)*aw;y+=aw)
      for (x=rt;x!=rt+awin;x++)
      o[i][y+x]=-1;
    }
    else if(in[i]==1) {
      for (y=rt*aw;y!=(rt+alin)*aw;y+=aw)
      for (x=rt;x!=rt+awin;x++)
      o[i][y+x]=1;
    }
  }
  if (del>=1)
  for (z=1;z!=level+1;z++)
    for (y=0;y!=al*aw;y+=aw)
      for (x=0;x!=aw;x++)
	for (i=1;i!=del+1;i++)
	  delo[i][z][y+x]=o[z][y+x];

} /* net_init */



akosmain(void)
{
  if ((rt==1) && (R==1) && (C==1) && (tst==1) && (cent==0)
    && (sigm==1) && (del==0)) return(0);
  if ((rt==2) && (R==1) && (C==1) && (tst==1) && (cent==0)
    && (sigm==1) && (del==0)) return(1);
  if (del==0) return(2);
  if ((rt==1) && (R==1) && (C==1) && (tst==1) && (cent==0)
    && (sigm==1)) return(3);
  return(4);
}



void run(void)
{
  if(run_mode==0) then f_stepbystep();
  if(run_mode==1) then f_fluently();
}



void f_fluently(void)
{
  int f_f_error,esc;

  f_f_error=0;
  if (is=='n') {
    message(" Input is not set ","iins");
    f_f_error=1;
  }
  if (ts=='n') {
    message(" Template is not set ","tins");
    f_f_error=1;
  }
  if (ns=='n') {
    message(" Net is not initialized ","nini");
    f_f_error=1;
  }
  if (f_f_error==1) then return;

  esc=0;
  while(esc!=2 and !bhit2()) {
    f_stepbystep();
    esc=pause(2,3,78,23);
  }
}



void f_stepbystep(void)
{
  int f_s_error;

  f_s_error=0;
  if (is=='n') {
    message(" Input is not set ","iins");
    f_s_error=1;
  }
  if (ts=='n') {
    message(" Template is not set ","tins");
    f_s_error=1;
  }
  if (ns=='n') {
    message(" Net is not initialized ","nini");
    f_s_error=1;
  }
  if (f_s_error==1) then return;

  step++;
  (*iter[act_iter])();
  miki();
  status();
}


/******************    read the input level from file	 *******************/

char helpfname[]="cnnsym.hlp";
char menufname[]="cnnsym.mnu";

/* лллллллллллллллллллл main function лллллллллллллллллллл */
void main(void)
{
  menuinitalize();
  farfree((float far *)hmain);
}

void before_main(void)
{
  set_default();
  settextcolor(defc,defg);
  if(mouse) then EGER(2,0,0,0);
  clrscr(); BORDER(defg);
  if(mouse) then EGER(1,0,0,0);
  status();
  bput1(256*130); /* alt-- : some kind of magic */
}

void magic(void)
{
  void *ptr;

  if(mouse) then EGER(2,0,0,0);
  ptr=farmalloc(21*9*2);
  if(ptr==NULL) then { message(" No room for magic.","nrfm"); return; }
  gettext(30,8,50,16,ptr);
  frame(30,8,20,8,"",7,0,0,0);
  gotoxy(32,10); settextcolor(4,7); cprintf("%s","  Cellular Neural");
  gotoxy(32,11); settextcolor(4,7); cprintf("%s","        Net      ");
  gotoxy(32,12); settextcolor(4,7); cprintf("%s","     Simulator   ");
  gotoxy(32,13); settextcolor(0,7); cprintf("%s"," Tech. University");
  gotoxy(32,14); settextcolor(0,7); cprintf("%s","    Of Budapest  ");
  gotoxy(32,15); settextcolor(0,7); cprintf("%s","        '90      ");
  getch();
  puttext(30,8,50,16,ptr);
  if(mouse) then EGER(1,0,0,0);
  farfree(ptr);
}

/* ллл		other functions 	ллл */

void inp_set(void)
{
  initinf();
  status();
}

void tem_set(void)
{
  inittf();
  status();
}

void f_chstatus(void)
{
  ns='n';
  status();
}

void f_statusconfig(void)
{
  int c1,l,t;

  l=st.l; t=st.t; c1=257;
  while(c1!=13 and c1!=27) {
    while (!bhit2()) pause(2,2,78,23);
    c1=bget2();
    if (c1==256*KEY_UP) then { st.t--; if (st.t<1) then st.t++; }
    if (c1==256*KEY_DOWN) then { st.t++; if (st.t+st.h>24) then st.t--; }
    if (c1==256*KEY_LEFT) then { st.l--; if (st.l<1) then st.l++; }
    if (c1==256*KEY_RIGHT) then { st.l++; if (st.l+st.w>80) then st.l--; }
    if (c1==256*KEY_HOME) then st.l=0;
    if (c1==256*KEY_PGUP) then st.t=1;
    if (c1==256*KEY_END) then st.l=80-st.w;
    if (c1==256*KEY_PGDN) then st.t=24-st.h;
    status();
  }
  if(c1==27) then { st.l=l; st.t=t; }
  status();
  return;
}

void status(void)
{
  int i,t,h;
  char sf_inp[15], sf_tem[15];

  h=strlen(s_inp); t=0;
  for(i=0;i<h;i++) if(s_inp[i]=='\\') then t=i+1;
  strcpy(sf_inp,&s_inp[t]);
  h=strlen(s_tem); t=0;
  for(i=0;i<h;i++) if(s_tem[i]=='\\') then t=i+1;
  strcpy(sf_tem,&s_tem[t]);

  if(mouse) then EGER(2,0,0,0);
  h=2;
  for(i=0;i<st.db;i++) if(sts[i]==1) then h++;
  st.h=h; t=2; i=0;
  if (st.t+st.h>24) then st.t=24-st.h;
  if(first_status==0) then first_status=1;
  else if(st.ptr!=NULL) then
    puttext( st_l+1, st_t+1, st_l+st_w, st_t+st_h, st.ptr );
  if (st.ptr!=NULL) then
    gettext( st.l+1, st.t+1, st.l+st.w, st.t+st.h, st.ptr );
  st_l=st.l; st_t=st.t; st_w=st.w; st_h=st.h;
  frame(st.l, st.t, st.w, st.h, st.n, st.c_w, st.c_f, st.c_n, st.lt);
  settextcolor(st.c_f, st.c_w);

  if(sts[ 0]==1) {gotoxy(st.l+2, st.t+t++);
    cprintf("%s","    Input: "); if(is=='y') then cprintf("%s",sf_inp);}
  if(sts[ 1]==1) {gotoxy(st.l+2, st.t+t++);
    cprintf("%s"," Template: "); if(ts=='y') then cprintf("%s",sf_tem);}
  if(sts[ 2]==1) {gotoxy(st.l+2, st.t+t++);
    cprintf("%s","      Net: "); if(ns=='y') then cprintf("%s",s_net);}
  if(sts[ 3]==1) {gotoxy(st.l+2, st.t+t++);
    cprintf("%s","  Windows: "); if(ws=='y') then cprintf("%s",s_win);}
  if(sts[ 4]==1) {gotoxy(st.l+2, st.t+t++); cprintf("%s",  " Run mode: ");
    if(run_mode==0) then cprintf("%s","Step by step");
    else cprintf("%s","Continuous");}
  if(sts[ 5]==1) {gotoxy(st.l+2, st.t+t++);
    cprintf("%s%s","   Layers: ",s_no_level);}
  if(sts[ 6]==1) {gotoxy(st.l+2, st.t+t++);
    cprintf("%s","     Size: ");
    if(awin!=0 and alin!=0) then cprintf("%d,%d",awin,alin);}
  if(sts[ 7]==1) {gotoxy(st.l+2, st.t+t++);
    cprintf("%s","   Radius: "); if(rt!=0) then cprintf("%d",rt);}
  if(sts[ 8]==1) {gotoxy(st.l+2, st.t+t++);
    cprintf("%s%d","    Delay: ",del);}
  if(sts[ 9]==1) {gotoxy(st.l+2, st.t+t++);
    cprintf("%s%s","  Current: ",s_curre);}
  if(sts[10]==1) {gotoxy(st.l+2, st.t+t++);
    cprintf("%s%s","   Resist: ",s_resis);}
  if(sts[11]==1) {gotoxy(st.l+2, st.t+t++);
    cprintf("%s%s"," Capacity: ",s_capac);}
  if(sts[12]==1) {gotoxy(st.l+2, st.t+t++);
    cprintf("%s%s"," Time stp: ",s_tst);}
  if(sts[13]==1) {gotoxy(st.l+2, st.t+t++);
    cprintf("%s%d","     Step: ",step);}
  if(sts[14]==1) {gotoxy(st.l+2, st.t+t++);
    cprintf("%s%lu","   Memory: ",farcoreleft());}
  if(mouse) then EGER(1,0,0,0);
}
