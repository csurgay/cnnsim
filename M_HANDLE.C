

#include <string.h>	/* strcat(), strcpy() */
#include <alloc.h>	/* farmalloc() */
#include <conio.h>	/* wherex(), gotoxy() */
#include <dir.h>		/* getdisk(), getcurdir() */
#include <dos.h>		/* REGS */
#include "stru.h"
#include "keys.h"
#include "ints.h"

extern void menubuild(void);
extern char *load();
extern int string();
extern void f_help(void);
extern void f_lasthelp(void);
extern void magic(void);

extern void exit();
extern void error();
extern void settextcolor();
extern void frame();
extern void inc();
extern void dec();
extern void light();
extern void delight();
extern void mopen();
extern void mclose();
extern int bhit2(void);
extern void bput1();
extern void bunget2();
extern int bget2(void);
extern void bclr1(void);
extern void bclr2(void);

void menuinitalize(void);
void menureconstruction(void);
void menu();
int pause();
void shiftstatus();
void f_NULL(void) {;};
void drawkeybar();

extern char helpfname[];
extern int mac[];
extern struct menus *m;
extern struct menus keybarw;
extern struct keybars h[];
extern int showfreememory;
extern int showshiftstatus;
extern int ssp,ssi,ssx,ssy,fmp,fmi,fmx,fmy;
extern int key_delay;

int defcursor_x, defcursor_y, defcursor_top, defcursor_bottom;
extern int rctop, rcbot;
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
	while(!to_mainmenu)
	{
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

		for (i=0;i<m->living;++i)
			if ((mt+i)->c1==b or (mt+i)->c2==b) then {
				delight(m,mt,mmp);
				mmp=i;
				light(m,mt,mmp);
				goto cim2;
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
		};

		if (m->le=='0' and quit_menu==0) then to_mainmenu=0;
	};

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
			for (i=0; i<m[0].living; i++)
				if (mx>=m->st[i].l and mx<=m->st[i].r and
				my==m->st[i].t) then {
					bput1(256*KEY_F10); bput1(m->st[i].c1); goto cim5;
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


