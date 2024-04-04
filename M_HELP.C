#include "stru.h"
#include "ints.h"
#include "keys.h"
#include <string.h>
#include <conio.h>		/* clrscr(), gotoxy() */
#include <stdio.h>		/* FILE, fopen() */
#include <dos.h>

extern char *wopen();
extern void error();
extern void bput2();
extern void bunget2();
extern void frame();
extern void settextcolor();
extern void exit();
extern int bhit2();
extern int pause();
extern int bget2();
extern void wclose();
extern void inc();
extern void dec();

extern int mouse, mx, my;
extern union REGS inregs,outregs;
extern int AX,BX,CX,DX;
extern struct menus helpw;
extern char helpstr[];
extern char helpfilename[];

int helpis=0, helpwindowis=0, lasthelpis=0, lasthelp=0;
int nohelpinfo=1;
char lasthelpstr[32][20];
char helpstr_save[20];
struct help_topic h_top[40];

void f_lasthelp(void);
void f_help(void);
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
		while(strcmp(s,helpstr)!=0 and s[0]!='Û')
			fgets(s,80,helpf);
		if (s[0]=='Û') then goto hlp4;
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
