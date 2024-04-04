
#include "ints.h"
#include "stru.h"
#include "keys.h"
#include <stdlib.h>		/* random, NULL */
#include <dos.h>			/* REGS */
#include <conio.h>		/* gotoxy, window */
#include <dir.h>			/* ffblk, findfirst */
#include <string.h>		/* strlen, strcpy */
#include <ctype.h>		/* toupper() */

char far *load();
void csqsort();		/* A QUICSORT FOR FILE NAMES */
int fcmp();				/* COMPARE TWO FILE NAMES    */

extern char *wopen();
extern void wclose();
extern void frame();
extern int string();
extern void settextcolor();
extern int bhit2();
extern int pause();
extern int bget2();
extern void message();
extern void bunget2();

extern int mouse;
extern union REGS inregs,outregs;
extern int AX,BX,CX,DX;
extern int i,j,mx,my,b;

extern char *nevek[];
extern struct menus search;

char curdir[60];		/* CURRENT DIR */
char path[60];			/* CURRENT DIR + DIR OF MASKSTR */
char dir[4]="*.*";
char fulldirs[60];   /* PATH + *.*			*/
char mask[60];       /* MASK OF MASKSTR 	*/
char fullmask[60];   /* PATH + MASK 		*/
char str[60];
char chosen[60];		/* CHOSEN PATH + FILE NAME */
char probadir[60];
int succs;
int n_nevek,elso,ahany,act;
struct ffblk fblock[1];


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
			if (b==ESC)         then { b=KEY_ENTER; escape=1; }
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
