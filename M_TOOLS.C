#include <stdarg.h>	/* va_list, va_start */
#include <string.h>	/* strcpy(), strcat() */
#include <stdio.h>	/* vsprintf(), printf() */
#include <alloc.h>	/* farmaloc() */
#include <conio.h>	/* gotoxy(), textcolor() */
#include <dos.h>		/* delay() */
#include "stru.h"
#include "keys.h"
#include "ints.h"

#define FALSE 0
#define TRUE 1

extern int mouse,menulevel,gorg;
extern int mx, my;
extern char helpstr[20];
extern union REGS inregs,outregs;
extern int AX,BX,CX,DX;
extern int mac[];

extern void f_help();

void light();
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

char lty[][7] = {
	'Ú','Ä','¿','³','À','Ä','Ù',
	'Õ','Í','¸','³','À','Ä','Ù',
	'É','Í','»','º','È','Í','¼',
	' ',' ',' ',' ',' ',' ',' '
};

#define BUFSIZE 500
int b1[BUFSIZE], b2[BUFSIZE];
int b1b,b1e,b2b,b2e;
int key_delay;
unsigned val_delay;


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
				else
					while(mac[i++]!=258);
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


