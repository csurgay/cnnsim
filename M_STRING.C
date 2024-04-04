
#include "ints.h"
#include "keys.h"
#include <stdio.h>		/* NULL */
#include <string.h>
#include <dos.h>			/* REGS */
#include <conio.h>		/* gotoxy */

int string();

extern char *wopen();
extern void frame();
extern void settextcolor();
extern void bput2();
extern int bhit2();
extern int pause();
extern int bget2();
extern void bunget2();
extern void wclose();

extern int mouse;
extern union REGS inregs,outregs;
extern int AX,BX,CX,DX;
extern int i,j,mx,my,b;

extern int strw;
extern int strf;
extern int strn;
extern int stro;
extern int strs;
extern int strhp;
extern int strhi;

extern int defcursor_top, defcursor_bottom;

int rctop, rcbot;


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
	while(!bhit2() and ch!=2)
		ch=pause(l,t,w,3);
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
