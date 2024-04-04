#include <stdio.h>		/* FILE, fopen(), printf */
#include <alloc.h>		/* farcalloc() */
#include <string.h>		/* strcpy() */
#include <stdlib.h>		/* atoi() */
#include "stru.h"
#include "keys.h"

#define then
#define or ||
#define and &&

#define wech '^'  /* window end character       */
#define fech '|'  /* file end character         */
#define lech ';'  /* list end character         */  /* in the .mnu file */
#define lsch ','  /* list separating character  */
#define lcch ':'  /* label closing character    */

unsigned long alfm;

int strw=7;	/* colors for string window */
int strf=0;	/* window, frame, name, string, highlight_paper, -_ink */
int strn=0;
int stro=0;
int strs=4; /* for nothing */
int strhp=3;
int strhi=15;

int ssx=64, ssy=25;	/* shiftstatus pos's   */
int ssp=3, ssi=0;		/* shiftstatus colors */
int fmx=58, fmy=25;	/* freememory pos's   */
int fmp=2, fmi=0;		/* freememory colors  */

void error();
char *csmalloc();
char *cscalloc();
void menubuild(void);
void beolvas(void);
void str_srch();
int str_copy();
int step_blank();
void str_next();
int blank();
int blank_es();
int str_comp();
int char_count();
void toendsem();
struct menus *getmenu();

extern struct trimmerst *gettrimmer();
extern void getldstr();
extern void optionbanat();
extern void windowbanat();
extern void getmacros(void);
extern void getkeybar(void);
extern void getkeybarpart();
extern int getkey();

extern void (*fun_array[])();
extern int *int_array[];
extern char *str_array[];
extern char menufname[60];
extern unsigned val_delay;

int mac[500];

char search_n[60];
struct menus search = {
	's','9',0,0,0,10,6,60,12,1,search_n,7,0,1,0,4,3,15,"",NULL,NULL,NULL,NULL
};
struct menus helpw = {
	'h','9',0,0,0,14,5,52,17,1,"Help",7,0,1,4,7,4,15,"",NULL,NULL,NULL,NULL
};
struct menus keybarw = {
	'o','9',0,0,0,0,24,79,1,3,"",7,0,0,0,4,0,0,"",NULL,NULL,NULL,NULL
};

struct keybars h[20];
struct menus *m;

int style;   /*  0-semmi  1-TURBO   */
int showfreememory;   /*  0-not show  1-show  */
int showshiftstatus;  /*  0-not show  1-show  */

FILE *fp;
char *txt;
char *buffer;
int bufptr, bufend;
int txb,txe,txl;
int maxw;
char otype[100], word1[100], word2[100], word3[100];
int mm_wi[20];

/*-----------------------------------------------------------*/
/*	STRING CONSTANTS TO SEARCH FOR IN THE .mnu FILE      */
/*-----------------------------------------------------------*/
														/* searched x times */
char s_style[]= 				"style:";               /*1*/
char s_turbo[]= 				"TURBO";                /*1*/
char s_on[]=					"on";

char s_main[]=					"main:";
char s_delay[]= 				"delay:";               /*1*/
char s_showfreememory[]=	"showfreememory:";      /*1*/
char s_showshiftstatus[]=	"showshiftstatus:";     /*1*/
char s_macros[]=				"macros:";              /*1*/
char s_keybarsc[]=			"keybar:";
char s_normal[]=	   		"normal:";
char s_shift[]= 	   		"shift:";
char s_alt[]=					"alt:";
char s_ctrl[]=					"ctrl:";
char s_helpwindow[]=			"helpwindow:";          /*1*/
char s_loadwindow[]=			"loadwindow:";          /*1*/
char s_keybarwindow[]=		"keybarwindow:";     	/*1*/
char s_stringcolor[]=		"stringcolor:";         /*1*/

char s_menu[]=					"menu";                 /*0*/
char s_function[]=			"function";             /*1*/
char s_trimmer[]=				"trimmer";              /*1*/
char s_choice[]=				"choice";               /*1*/
char s_remark[]=				"remark";               /*1*/
char s_string[]=				"string";               /*2*/
char s_load[]=					"load";                 /*3*/
char s_quit[]=					"quit";						/*1*/

char s_name[]=					"_name";                /*2*/
char s_living[]=				"_living";              /*1*/
char s_color[]= 				"_color";               /*1*/
char s_hotlight[]=			"_hotlight";            /*1*/
char s_hotkey[]=				"_hotkey";
char s_linetype[]=			"_linetype";            /*1*/
char s_winwidth[]=			"_winwidth";            /*2*/
char s_winheight[]=			"_winheight";           /*1*/
char s_winxpos[]=				"_winxpos";             /*2*/
char s_winypos[]=				"_winypos";             /*2*/
char s_optwidth[]=			"_optwidth";            /*1*/
char s_optxpos[]=				"_optxpos";             /*1*/
char s_optypos[]=				"_optypos";             /*1*/
char s_stringmaxwidth[]=	"_stringmaxwidth";      /*1*/
char s_help[]=					"_help";                /*1*/
char s_keybar[]=				"_keybar";              /*2*/
char s_delight[]=				"_delight";             /*1*/
char s_before[]=				"_before";              /*1*/
char s_after[]= 				"_after";               /*1*/
char s_inmain[]=				"_inmain";              /*1*/
char s_variable[]=			"_variable";            /*2*/
char s_mask[]=					"_mask";                /*1*/
char s_extrapos[]=			"_extraxpos";           /*1*/

/*---------------------------------------*/
/*		  THE MENUBUILD MAIN FUNCTION	     */
/*---------------------------------------*/
void menubuild(void)
{
	int i,j;

/*** BUFFER FOR THE MENU-STRUCTURE ***/
	bufend=10000; buffer=(char *)farcalloc(bufend,sizeof(char)); bufptr=0;
	alfm=farcoreleft();
	if (buffer==NULL) then error("buffer==NULL");

/*** LOAD THE .mnu FILE (ALLOCATE THE FILE-BUFFER) ***/
	beolvas();

/*** DETECT THE STYLE (IT CAN BE ONLY TURBO NOW) style:TURBO ***/
	str_srch(1,s_style,fech); style=0;
	if(txb!=-1) then {
		str_next(txe,word1); if(str_comp(word1,s_turbo)==0) then style=1;
	}
	if(style==0) then error("A style csak TURBO lehet ( style:TURBO )...");

/*** FEATURES OF KEYBARWINDOW ***/
	str_srch(1,s_keybarwindow,fech);
	if(txb!=-1) then {
		i=txe;
		windowbanat(txe+1,&keybarw);
		strcpy(word3,s_winwidth); str_srch(i,word3,wech);
		if(txb!=-1) then {
			str_copy(txe+1,'a',word3);
			keybarw.w=atoi(word3);
		}
	}

/*** FEATURES OF HELPWINDOW ***/
	str_srch(1,s_helpwindow,fech);
	if(txb!=-1) then {
		i=txe;
		windowbanat(txe+1,&helpw);
		strcpy(word3,s_winwidth); str_srch(i,word3,wech);
		if(txb!=-1) then {
			str_copy(txe+1,'a',word3);
			helpw.w=atoi(word3);
		}
	}

/*** FEATURES OF LOADWINDOW ***/
	str_srch(1,s_loadwindow,fech);
	if(txb!=-1) then {
		i=txe;
		windowbanat(txe+1,&search);
		strcpy(word3,s_winwidth); str_srch(i,word3,wech);
		if(txb!=-1) then {
			str_copy(txe+1,'a',word3);
			search.w=atoi(word3);
		}
	}

/*** COLORS OF STRINGWINDOW ***/
	str_srch(1,s_stringcolor,fech);
	if(txb!=-1) then {
		txe=str_copy(txe,'a',word3); strw=atoi(word3);
		txe=str_copy(txe+1,'a',word3); strf=atoi(word3);
		txe=str_copy(txe+1,'a',word3); strn=atoi(word3);
		txe=str_copy(txe+1,'a',word3); stro=atoi(word3);
		txe=str_copy(txe+1,'a',word3); strs=atoi(word3);
		txe=str_copy(txe+1,'a',word3); strhp=atoi(word3);
		txe=str_copy(txe+1,'a',word3); strhi=atoi(word3);
	}

/*** SHOW THE FREE MEMORY ?  showfreememory:on ***/
	fmy=keybarw.t+keybarw.h;
	str_srch(1,s_showfreememory,fech); showfreememory=0;
	if(txb!=-1) then {
		i=txe;
		str_next(txe,word1); if(str_comp(word1,s_on)==0) then showfreememory=1;
		str_srch(i,s_color,wech);
		if(txb!=-1) then {
			str_next(txe+1,word1); fmp=atoi(word1);
			str_next(txe+1,word1); fmi=atoi(word1);
		}
		str_srch(i,s_optxpos,wech);
		if(txb!=-1) then { str_next(txe+1,word1); fmx=atoi(word1); }
		str_srch(i,s_optypos,wech);
		if(txb!=-1) then { str_next(txe+1,word1); fmy=atoi(word1); }
	}

/*** SHOW THE SHIFT STATUS ?  showshiftstatus:on ***/
	ssy=keybarw.t+keybarw.h;
	str_srch(1,s_showshiftstatus,fech); showshiftstatus=0;
	if(txb!=-1) then {
		i=txe;
		str_next(txe,word1); if(str_comp(word1,s_on)==0) then showshiftstatus=1;
		str_srch(i,s_color,wech);
		if(txb!=-1) then {
			str_next(txe+1,word1); ssp=atoi(word1);
			str_next(txe+1,word1); ssi=atoi(word1);
		}
		str_srch(i,s_optxpos,wech);
		if(txb!=-1) then { str_next(txe+1,word1); ssx=atoi(word1); }
		str_srch(i,s_optypos,wech);
		if(txb!=-1) then { str_next(txe+1,word1); ssy=atoi(word1); }
	}

/*** MEASURE OF DELAY IN MACRO-INSERTING. delay:3 ***/
	val_delay=100;
	str_srch(1,s_delay,fech);
	if(txb!=-1) then {
		str_copy(txe,'a',word3); val_delay=atoi(word3);
	}

/*** GET THE MACRO TO THE MACRO-BUFFER (mac) macros:... ***/
	str_srch(1,s_macros,fech);
	if(txb!=-1) then getmacros(); else mac[0]=257;

/*** GET THE KEYBARS TO h. keybars:0;... ***/
	i=1;
	do {
		str_srch(i,s_keybarsc,fech);
		i=txe; j=txb; if(txb!=-1) then getkeybar();
	} while (j!=-1);

/*** SEARCH THE main: SYMBOL ***/
	str_srch(1,s_main,fech);
	if(txb==-1) then error("No Mainmenu defined...");

/*** ENTER THE RECURSOIN ***/
	m=getmenu(s_main,NULL,0,1);

/*** FREE THE FILE-BUFFER ***/
	free((void *)txt);
}

/*-------------------------------------------------------*/
/*    GET THE WHOLE MENU-STRUCTURE IN A RECURSIVE WAY	   */
/*-------------------------------------------------------*/
struct menus *getmenu(s,mp,j,b) 		/* gets s OPTION's features & sons */

char *s;					/* option name */
struct menus *mp;		/* window ptr */
int j;					/* this is the j. option in 'mp' window */
int b;					/* this option's beginning pos in the .mnu file */
{
	int i,k;
	int n,x,y,w;
	int txeaux,txaux;
	int txaux2,maxw2;
	struct menus *m1;				/* this option's submenu-window (only w,c) */
	int txemenu;					/* to save pos of this option */

	str_srch(b,s,fech);				/*  srch option+':' to the end (|) */
	if(txb==-1) then goto gmend;	/*  if not exist, go to getmenu-end */
	txemenu=txe;						/*  save pos of option+':' */
	str_next(txe,otype);				/*  type of this option in otype */
	n=char_count(txe,lsch,lech);	/*  number of sons, and set maxw */
	maxw2=maxw;							/*  save maxw */

	optionbanat(mp,j,txemenu);		/*  features of this opt in his daddy-window */

	if(str_comp(otype,s_remark)==0) then {		/* REMARK - no things to do */
		goto gmend;
	}
	if(str_comp(otype,s_quit)==0) then {		/* QUIT - sign it */
		(mp->st+j)->ty='q';
		goto gmend;
	}
	if(str_comp(otype,s_trimmer)==0) then { 	/* TRIMMER - get features */
		(mp->st+j)->ty='t'; (mp->st+j)->next.tr=gettrimmer(txe,n);
		goto gmend;
	}
	if(str_comp(otype,s_function)==0) then {		/* FUNCTION - get fun_addr */
		txeaux=txe; str_copy(txe+1,'a',word3); txe=txeaux;
		(mp->st+j)->ty='f'; (mp->st+j)->next.f=fun_array[atoi(word3)];
		txeaux=txe; strcpy(word3,s_inmain); str_srch(txe,word3,wech); txe=txeaux;
		if(txb!=-1) then (mp->st+j)->ty='i';            /* when _inmain fun. */
		strcpy(word3,s_keybar); str_srch(txe,word3,wech);
		if(txb!=-1) then {
			str_copy(txe+1,'a',word3);
			(mp->st+j)->kbar=&h[atoi(word3)];
		}
		goto gmend;
	}
	if(str_comp(otype,s_load)==0) then {		/* LOAD - get ldstr features */
		getldstr(s_load,txe,mp,j);
		goto gmend;
	}
	if(str_comp(otype,s_string)==0) then {		/* STRING - get ldstr features */
		getldstr(s_string,txe,mp,j);
		goto gmend;
	}
	/* IF NOT OTHER, IT MUST BE THE TYPE OF SUBMENU */
	m1=(struct menus *)cscalloc(1,sizeof(struct menus));
	if(s==s_main) then {
		m1->le='0'; m1->ty='m';
		m1->l=0; m1->t=0; m1->w=80; m1->h=1; m1->lt=3;
		txeaux=txe;
		strcpy(word3,s_winwidth); str_srch(txe,word3,wech);
		if(txb!=-1) then {
			str_copy(txe+1,'a',word3);
			m1->w=atoi(word3);
		}
		txe=txeaux;
	}
	else {
		m1->le=mp->le+1; m1->ty='w';
		m1->w=maxw+3; m1->h=n+2;
		if((m1->l=(mp->st+j)->l+(m1->le=='1' ? -1 : 1))+maxw>=77) then
			 m1->l=76-maxw;
		if((m1->t=(mp->st+j)->t+1)+n+2>=24) then
			 m1->t=24-n-2;
		m1->lt=0; (mp->st+j)->ty='m';
		(mp->st+j)->next.m=m1;
	}
	m1->db=n; m1->living=n; m1->act=0; m1->n=NULL; m1->bef=NULL;
	m1->c_w=7; m1->c_f=1; m1->c_n=5; m1->c_o=0; m1->c_s=4;
	m1->c_hp=4; m1->c_hi=15; m1->kbar=h;
	windowbanat(txemenu,m1);
	if(str_comp(otype,s_choice)==0) then {
		txeaux=txe;
		strcpy(word3,s_variable); str_srch(txemenu,word3,wech);
		if(txb==-1) then error("Need an integer to point to a variable...");
		str_copy(txe+1,'a',word3); txe=txeaux;
		i=atoi(word3); if (i==0 and word3[0]!='0') then error("Bad integer...");
		m1->var=int_array[i]; m1->act=*int_array[i];
		m1->ty='c'; (mp->st+j)->ty='c';
	}
	m1->st=(struct menust *)cscalloc(n,sizeof(struct menust));

	if(m1->ty=='m') then {
		x=m1->l+3; y=m1->t; if(m1->h>1) then y++;
	}
	else {x=m1->l+1; y=m1->t+1;}
	txaux=txe;
	for(i=0;i<n;i++) {
#define Ss (m1->st+i)->
		w=(m1->ty=='m' ? mm_wi[i] : maxw);
		Ss n=(char *)csmalloc(w);
		txe=step_blank(txe+1);
		txe=str_copy(txe,'a',Ss n);
		for(k=strlen(Ss n);k<w-1;k++) Ss n[k]=' '; Ss n[w-1]='\x0';
		Ss sb=0; Ss se=0; Ss c1=257; Ss c2=257;
		if(i<m1->living) then
			for(k=0;k<w-1;k++)
				if(Ss n[k]<='Z' and Ss n[k]>='A') then {
					if(Ss n[k]=='F') then {
						if(Ss n[k+1]>='1' and Ss n[k+1]<='9') then {
							Ss sb=k+1; Ss se=k+2; Ss c1=256*(KEY_F1+Ss n[k+1]-'1'); Ss c2=257;
							if(Ss n[k+2]=='0') then { ++Ss se; Ss c1=256*KEY_F10; }
							break;
						}
					}
					Ss sb=k+1; Ss se=k+1; Ss c1=Ss n[k]; Ss c2=Ss n[k]+32;
					break;
				}
		Ss l=x; Ss r=x+w; Ss t=y; Ss el=x+maxw2+3;
		if(m1->ty=='m') then x=x+mm_wi[i]+2; else y++;
		Ss olt=' '; Ss helpstr=NULL; Ss aft=NULL;
	}

	txe=txaux;
	for(i=0;i<n;i++) {
		txe=str_copy(txe+1,'a',word2);
		toendsem(word2);
		txaux2=txe;
		getmenu(word2,m1,i,txe);       /* THIS IS THE RECURSIVE CALL */
		txe=txaux2;
	}
gmend:
	return(m1);
}

/*--------------------------------------------*/
/*   LOAD THE .mnu FILE TO THE FILE-BUFFER    */
/*--------------------------------------------*/
void beolvas(void)
{
	int i;

	fp=fopen(menufname,"rt");
	if (fp==NULL) then {printf("Can't find menu file: %s...",menufname); exit(0); }
	txt=(char *)farcalloc(10000,sizeof(char));
	alfm=farcoreleft();
	if (txt==NULL) then error("txt==NULL");
	for(i=0; (txt[i]=getc(fp))!=fech; i++) ;
	txl=i-1;
	fclose(fp);
}


void str_srch(b,s,e)
int b;
char *s;
char e;
{
	int i,succ;

	for(i=b;txt[i]!=e;i++)
		if(blank(txt[i-1])) then {
			succ=str_comp(&txt[i],s);
			if(succ==0) then {txb=i; txe=i+strlen(s); return;}
		}
	txb=-1; return;
}


void str_next(b,s)
int b;
char *s;
{
	int i;

	i=step_blank(b);
	str_copy(i,'a',s);
	txb=b; txe=b+strlen(s);
}


int step_blank(b)
int b;
{
	int i;
	for(i=b;i<txl;i++)
		if(blank(txt[i])==0) then return(i);
	return(-1);
}


int str_copy(e,c,s)
int e;
char c, *s;
{
	int i,j;

	j=0;
	while(txt[e]=='\n' or txt[e]=='\r' or txt[e]=='\t')
		e++;
	for(i=e;i<txl;i++) {
		switch(c) {
			case 'b': if(blank(txt[i])) then { s[j]='\x0'; goto scend; }
						break;
			case 'a': if(blank_es(txt[i]) or txt[i]==lsch or txt[i]==lech or
						txt[i]==lcch) then { s[j]='\x0'; goto scend; }
						break;
			default: if(txt[i]==c) then { s[j]='\x0'; goto scend; }
		}
		s[j++]=txt[i];
	}
scend:
	return(i);
}


int blank(c)
char c;
{
	if(c==' ' or c=='\r' or c=='\n' or c=='\t') then return(1);
	else return(0);
}

int blank_es(c)
char c;
{
	if(c=='\n' or c=='\t') then return(1);
	else return(0);
}


int str_comp(s,d)
char *s, *d;
{
	int i;

	for(i=0;d[i]!='\x0';i++) {
		if(d[i]!=s[i]) then return(1);
	}
	return(0);
}


void error(s)
char *s;
{
	printf("%s\n",s);
	exit(0);
}


int char_count(b,c,e)
int b;
char c,e;
{
	int i,n,w;

	n=0; maxw=0; w=0;
	for(i=b;i<txl;i++,w++) {
		if(w==1) then i=step_blank(i);
		if(txt[i]==c or txt[i]==e) then {
			mm_wi[n++]=w; if(w>maxw) then maxw=w; w=0;
		}
		if(txt[i]==e) then return(n);
	}
	return(-1);
}


char *csmalloc(n)
int n;
{
	char *ptr;

	ptr=&buffer[bufptr]; bufptr+=n;
	if (bufptr>=bufend) then error("Mallokacios hiba...");
	return(ptr);
}
char *cscalloc(n,i)
int n,i;
{
	char *ptr;

	ptr=&buffer[bufptr]; bufptr+=n*i;
	if (bufptr>=bufend) then error("Callokacios hiba...");
	return(ptr);
}


void toendsem(s)
char *s;
{
	int i;
	i=0;
	while(s[i++]!='\x0') ;
	s[i]='\x0'; s[i-1]=lcch;
}

