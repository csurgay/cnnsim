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

struct trimmerst *gettrimmer();  /* M_BUILD2 */
void getldstr();                 /* */
void optionbanat();              /* */
void windowbanat();              /* */
void getmacros(void);            /* */
void getkeybar(void);            /* */
void getkeybarpart();            /* */
int getkey();                    /* */



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

char s_on[]="on";

char s_main[]="main:";
char s_delay[]="delay:";
char s_showfreememory[]="showfreememory:";
char s_showshiftstatus[]="showshiftstatus:";
char s_macros[]="macros:";
char s_keybarsc[]="keybar:";
char s_normal[]="normal:";
char s_shift[]="shift:";
char s_alt[]="alt:";
char s_ctrl[]="ctrl:";
char s_helpwindow[]="helpwindow:";
char s_loadwindow[]="loadwindow:";
char s_keybarwindow[]="keybarwindow:";
char s_stringcolor[]="stringcolor:";

char s_menu[]="menu";  /* 0 */
char s_function[]="function";
char s_trimmer[]="trimmer";
char s_choice[]="choice";
char s_remark[]="remark";
char s_string[]="string";
char s_load[]="load";
char s_quit[]="quit";

char s_name[]="_name";
char s_living[]="_living";
char s_color[]="_color";
char s_hotlight[]="_hotlight";
char s_hotkey[]="_hotkey";
char s_linetype[]="_linetype";
char s_winwidth[]="_winwidth";
char s_winheight[]="_winheight";
char s_winxpos[]="_winxpos";
char s_winypos[]="_winypos";
char s_optwidth[]="_optwidth";
char s_optxpos[]="_optxpos";
char s_optypos[]="_optypos";
char s_stringmaxwidth[]="_stringmaxwidth";
char s_help[]="_help";
char s_keybar[]="_keybar";
char s_delight[]="_delight";
char s_before[]="_before";
char s_after[]="_after";
char s_inmain[]="_inmain";
char s_variable[]="_variable";
char s_mask[]="_mask";
char s_extrapos[]="_extraxpos";

/*---------------------------------------*/
/*    THE MENUBUILD MAIN FUNCTION	 */
/*---------------------------------------*/
void menubuild(void)
{
  int i,j;

/*** BUFFER FOR THE MENU-STRUCTURE ***/
  bufend=20000; buffer=(char *)farcalloc(bufend,sizeof(char)); bufptr=0;
  if (buffer==NULL) then error("Not enough memory for the menu structure");

/*** LOAD THE .mnu FILE (ALLOCATE THE FILE-BUFFER) ***/
  beolvas();

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
/*    GET THE WHOLE MENU-STRUCTURE IN A RECURSIVE WAY	 */
/*-------------------------------------------------------*/
struct menus *getmenu(s,mp,j,b)	/* gets s OPTION's features & sons */

char *s;		   /* option name */
struct menus *mp;	   /* window ptr */
int j;			   /* this is the j. option in 'mp' window */
int b;			   /* this option's beginning pos in the .mnu file */
{
  int i,k;
  int n,x,y,w;
  int txeaux,txaux;
  int txaux2,maxw2;
  struct menus *m1;	/* this option's submenu-window (only w,c) */
  int txemenu;		/* to save pos of this option */

  str_srch(b,s,fech);	/*  srch option+':' to the end (|) */
  if(txb==-1) then goto gmend;	/*  if not exist, go to getmenu-end */
  txemenu=txe;			/*  save pos of option+':' */
  str_next(txe,otype);	/*  type of this option in otype */
  n=char_count(txe,lsch,lech);	/*  number of sons, and set maxw */
  maxw2=maxw;			/*  save maxw */

  optionbanat(mp,j,txemenu);	/*  features of this opt in his daddy-window */

  if(str_comp(otype,s_remark)==0) then {    /* REMARK - no things to do */
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
  if(str_comp(otype,s_function)==0) then {	/* FUNCTION - get fun_addr */
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
  if(str_comp(otype,s_load)==0) then {	    /* LOAD - get ldstr features */
    getldstr(s_load,txe,mp,j);
    goto gmend;
  }
  if(str_comp(otype,s_string)==0) then {    /* STRING - get ldstr features */
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
  if (fp==NULL) then {
    printf("Can't find menu file: %s...",menufname); exit(0);
  }
  txt=(char *)farcalloc(20000,sizeof(char));
  if (txt==NULL) then error("Not enough memory for the menu file");
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



/* M_BUILD2 */
/************/
/************/


char alt_key[] = {	/* array of alt key codes a..z */
30,48,46,32,18,33,34,35,23,36,37,38,50,49,24,25,16,19,31,20,22,47,17,45,21,44
};

/*------------------------------------------------*/
/*    GET FEATURES OF A LOAD OR STRING WINDOW	  */
/*------------------------------------------------*/
void getldstr(s_,b,mp,j)
char *s_;
int b;
struct menus *mp;
int j;
{
  int txeaux;

  txeaux=txe;
#define lL (mp->st+j)->next.ldstr
  str_copy(b+1,'a',word3);
  (mp->st+j)->ty=( s_==s_load ? 'l' : 's' );
  lL=(struct ldstrs *)csmalloc(sizeof(struct ldstrs));
  if ((lL->x=(mp->st+j)->l+1)+40>=79) then lL->x=79-40;
  if ((lL->y=(mp->st+j)->t+1)+2>=24) then lL->y=24-3;
  lL->w=40; lL->sw=37; lL->n="";
  lL->mask=csmalloc(60); strcpy(lL->mask,"*.*");
  lL->str=str_array[atoi(word3)];
  strcpy(word3,s_name); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    lL->n=csmalloc(strlen(word3)+2);
    strcpy(lL->n,word3);
  }
  strcpy(word3,s_mask); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    strcpy(lL->mask,word3);
  }
  strcpy(word3,s_winxpos); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    lL->x=atoi(word3);
  }
  strcpy(word3,s_winypos); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    lL->y=atoi(word3);
  }
  strcpy(word3,s_winwidth); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    lL->w=atoi(word3);
  }
  strcpy(word3,s_stringmaxwidth); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    lL->sw=atoi(word3);
  }
  txe=txeaux;
}

/*------------------------------------*/
/*     GET FEATURES OF A TRIMMER      */
/*------------------------------------*/
struct trimmerst *gettrimmer(b,n)
int b,n;
{
  int txeaux;
  int i;
  struct trimmerst *tr;

  txeaux=txe;
  tr=(struct trimmerst *)cscalloc(1,sizeof(struct trimmerst));
  tr->db=n;
  tr->ml=maxw; *tr->var=0;
  tr->ts=(char *)cscalloc(maxw*n,sizeof(char));
  for(i=0;i<n;i++) {
    b=str_copy(b+1,'a',word3);
    strcat(&tr->ts[i*maxw],word3);
  }
  strcpy(word3,s_variable); str_srch(b,word3,wech);
  if(txb==-1) then error("Need an integer to point to a variable...");
  str_copy(txe+1,'a',word3);
  i=atoi(word3); if (i==0 and word3[0]!='0') then error("Bad integer...");
  tr->var=int_array[i]; tr->act=*int_array[i];
  txe=txeaux;
  return(tr);
}

/*----------------------------------------*/
/*	GET FEATURES OF AN OPTION         */
/*----------------------------------------*/
void optionbanat(mp,j,b)
struct menus *mp;
int j,b;
{
  int txeaux,i;

  txeaux=txe;

/*** GET WIDTH OF THE WINDOW CONTAINS THE OPTION ***/
  strcpy(word3,s_winwidth); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    maxw=atoi(word3);
  }

/*** GET START AND END POS OF HIGHLIGHTING THE OPTION ***/
  strcpy(word3,s_hotlight); str_srch(b,word3,wech);
  if(txb!=-1) then {
    txe=str_copy(txe+1,'a',word3); (mp->st+j)->sb=atoi(word3);
    txe=str_copy(txe+1,'a',word3); (mp->st+j)->se=atoi(word3);
  }

/*** GET THE HOTKEY(S) OF THE OPTION ***/
  strcpy(word3,s_hotkey); str_srch(b,word3,wech);
  if(txb!=-1) then {
    txe=str_copy(txe+1,'a',word3); (mp->st+j)->c1=getkey(word3);
    if(txt[txe]!=';') then {
      txe=str_copy(txe+1,'a',word3); (mp->st+j)->c2=getkey(word3);
    }
  }

/*** GET X POS OF THE OPTION ***/
  strcpy(word3,s_optxpos); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3); i=atoi(word3);
    (mp->st+j)->r=i+(mp->st+j)->r-(mp->st+j)->l;
    (mp->st+j)->l=i;
  }

/*** GET X POS OF EXTRA STRING BEHIND THE OPTION ***/
  strcpy(word3,s_extrapos); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3); i=atoi(word3);
    (mp->st+j)->el=i;
  }

/*** GET Y POS OF THE OPTION ***/
  strcpy(word3,s_optypos); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3); i=atoi(word3);
    (mp->st+j)->t=i;
  }

/*** GET WIDTH OF THE OPTION ***/
  strcpy(word3,s_optwidth); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3); i=atoi(word3);
    (mp->st+j)->r=(mp->st+j)->l+i;
    if(strlen((mp->st+j)->n)>i) then (mp->st+j)->n[i]='\x0';
  }

/*** GET THE HELP STRING BELONGS THE OPTION ***/
  strcpy(word3,s_help); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    (mp->st+j)->helpstr=csmalloc(strlen(word3)+1);
    strcpy((mp->st+j)->helpstr,word3);
  }

/*** GET THE FUNCTION CALLED AFTER EXECUTED THE OPTION ***/
  strcpy(word3,s_after); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    (mp->st+j)->aft=fun_array[atoi(word3)];
  }
/*** NEED DELIGHT THE OPTION WHILE EXECUTING ? ***/
  strcpy(word3,s_delight); str_srch(b,word3,wech);
  if(txb!=-1) then {
    (mp->st+j)->olt='o';
  }

  txe=txeaux;
}

/*----------------------------------------*/
/*	GET FEATURES OF A WINDOW	  */
/*----------------------------------------*/
void windowbanat(b,m)
int b;
struct menus *m;
{
  int txeaux,i;

  txeaux=txe;

/*** GET THE NAME OF WINDOW ***/
  strcpy(word3,s_name); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    m->n=csmalloc(strlen(word3)+1);
    strcpy(m->n,word3);
  }

/*** GET THE KEYBAR PTR OF WINDOW ***/
  strcpy(word3,s_keybar); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    m->kbar=&h[atoi(word3)];
  }

/*** GET THE NUMBER OF LIVING OPTIONS OF THE WINDOW ***/
  strcpy(word3,s_living); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    i=atoi(word3); if(i==0 and word3[0]!='0') then error("Need integer...");
    m->living=i;
  }

/***  GET THE COLORS OF WINDOW ***/
  strcpy(word3,s_color); str_srch(b,word3,wech);
  if(txb!=-1) then {
    txe=str_copy(txe+1,'a',word3); m->c_w=atoi(word3);
    txe=str_copy(txe+1,'a',word3); m->c_f=atoi(word3);
    txe=str_copy(txe+1,'a',word3); m->c_n=atoi(word3);
    txe=str_copy(txe+1,'a',word3); m->c_o=atoi(word3);
    txe=str_copy(txe+1,'a',word3); m->c_s=atoi(word3);
    txe=str_copy(txe+1,'a',word3); m->c_hp=atoi(word3);
    txe=str_copy(txe+1,'a',word3); m->c_hi=atoi(word3);
  }

/*** GET THE FUNCTION CALLED BEFORE OPENING THE WINDOW ***/
  strcpy(word3,s_before); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    i=atoi(word3); if(i==0 and word3[0]!='0') then error("Need integer...");
    m->bef=fun_array[i];
  }

/*** GET THE X POS OF THE WINDOW ***/
  strcpy(word3,s_winxpos); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    m->l=atoi(word3);
  }

/*** GET THE Y POS OF THE WINDOW ***/
  strcpy(word3,s_winypos); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    m->t=atoi(word3);
  }

/*** GET THE HEIGHT OF THE WINDOW ***/
  strcpy(word3,s_winheight); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    m->h=atoi(word3);
  }

/*** GET THE FRAME-LINETYPE OF THE WINDOW ***/
  strcpy(word3,s_linetype); str_srch(b,word3,wech);
  if(txb!=-1) then {
    str_copy(txe+1,'a',word3);
    m->lt=atoi(word3);
  }

  txe=txeaux;
}

/*----------------------------------------------*/
/*		GET KEYBAR TO ARRAY h		*/
/*----------------------------------------------*/
void getkeybar(void)
{
  int i,acthotend,acthotbegin;

  acthotbegin=txe;
  str_copy(acthotbegin,'a',word3); i=atoi(word3);
  str_srch(acthotbegin,s_keybarsc,fech);
  if(txb!=-1) then acthotend=txb; else acthotend=txl;
  h[i].db[0]=0; h[i].db[1]=0; h[i].db[2]=0; h[i].db[3]=0;
  h[i].ht[0]=NULL; h[i].ht[1]=NULL; h[i].ht[2]=NULL; h[i].ht[3]=NULL;
  str_srch(acthotbegin,s_normal,fech);
  if (txb!=-1 and txb<acthotend) then {
    getkeybarpart(txe,0,i,acthotend);
  }
  str_srch(acthotbegin,s_shift,fech);
  if (txb!=-1 and txb<acthotend) then {
    getkeybarpart(txe,1,i,acthotend);
  }
  str_srch(acthotbegin,s_alt,fech);
  if (txb!=-1 and txb<acthotend) then {
    getkeybarpart(txe,2,i,acthotend);
  }
  str_srch(acthotbegin,s_ctrl,fech);
  if (txb!=-1 and txb<acthotend) then {
    getkeybarpart(txe,3,i,acthotend);
  }
}

/*---------------------------------------------------*/
/*  GET THE PART OF KEYBAR  (normal,shift,alt,ctrl)  */
/*---------------------------------------------------*/
void getkeybarpart(beg,what,ind,end)
int beg;  /* BEGIN OF THIS PART IN THE .mnu FILE-BUFFER */
int what; /* WHAT PART (normal,shift,...) */
int ind;  /* INDEX IN ARRAY h */
int end;  /* END OF THIS PART IN THE .mnu FILE-BUFFER (acthotend) */
{
  int i,b,j,k,w,n,x,y;
  struct menust *m1;	/*  FOR THE KEYBAR-ELEMENT STRUCT  */

  txe=beg-1; x=keybarw.l+1; y=keybarw.t;
  if(keybarw.h>1) then y++;
  n=char_count(txe,lsch,lech);	/*  number of sons  */
  if(what>0) then n++;
  m1=(struct menust *)cscalloc(n,sizeof(struct menust));
  h[ind].db[what]=n; h[ind].ht[what]=m1;
  if(what>0) then { n--; (m1+n)->l=x; (m1+n)->t=y; (m1+n)->r=0; }
  if(what==1) then { (m1+n)->n=s_shift; x+=8; }
  if(what==2) then { (m1+n)->n=s_alt; x+=6; }
  if(what==3) then { (m1+n)->n=s_ctrl; x+=7; }

  for (j=0;j<n;j++) {
#define Cc (m1+j)->
    txe=step_blank(txe+1);
    txe=str_copy(txe,'a',word3);
    w=strlen(word3);
    Cc n=(char *)csmalloc(w+2);
    strcpy(Cc n,word3);
    Cc sb=0; Cc se=0; Cc c1=257; Cc c2=257;
    for(k=0;k<w;k++)
      if(Cc n[k]<='Z' and Cc n[k]>='A') then {
	if(Cc n[k]=='F') then {
	  if(Cc n[k+1]>='1' and Cc n[k+1]<='9') then {
	    Cc sb=k+1; Cc se=k+2;
	    Cc c1=256*(KEY_F1+Cc n[k+1]-'1'); Cc c2=257;
	    if(Cc n[k+2]=='0') then { ++Cc se; Cc c1=256*KEY_F10; }
	    if(what==1) then Cc c1+=25*256;
	    if(what==2) then Cc c1+=45*256;
	    if(what==3) then Cc c1+=35*256;
	    break;
	  }
	}
	Cc sb=k+1; Cc se=k+1; Cc c1=Cc n[k]; Cc c2=Cc n[k]+32;
	break;
      }
      Cc l=x; Cc r=x+w-1; Cc t=y;
      x=x+w+2;
      Cc olt=' '; Cc ty=' ';
      Cc aft=NULL; Cc next.f=NULL; Cc helpstr=NULL; Cc kbar=NULL;
  }

  str_srch(beg,s_normal,fech); if(txb!=-1 and txe<end) then end=txb;
  str_srch(beg,s_shift,fech);  if(txb!=-1 and txe<end) then end=txb;
  str_srch(beg,s_alt,fech);    if(txb!=-1 and txe<end) then end=txb;
  str_srch(beg,s_ctrl,fech);   if(txb!=-1 and txe<end) then end=txb;
  for(j=0;j<n;j++) {
    strcpy(word3,(m1+j)->n); toendsem(word3);
    str_srch(beg,word3,fech);
    if(txb!=-1 and txb<end) then {
      b=txe;

      /*** START AND END POS OF HIGHLIGHTING THE KEYBAR-ELEMENT ***/
      strcpy(word3,s_hotlight); str_srch(b,word3,wech);
      if(txb!=-1) then {
	txe=str_copy(txe+1,'a',word3); Cc sb=atoi(word3);
	txe=str_copy(txe+1,'a',word3); Cc se=atoi(word3);
      }

      /*** THE HOTKEY(S) OF THE KEYBAR-ELEMENT ***/
      strcpy(word3,s_hotkey); str_srch(b,word3,wech);
      if(txb!=-1) then {
	txe=str_copy(txe+1,'a',word3); Cc c1=getkey(word3);
	if(txt[txe]!=';') then {
	  txe=str_copy(txe+1,'a',word3); Cc c2=getkey(word3);
	}
      }

      /*** X POS OF THE KEYBAR-ELEMENT ***/
      strcpy(word3,s_optxpos); str_srch(b,word3,wech);
      if(txb!=-1) then {
	str_copy(txe+1,'a',word3); i=atoi(word3);
	Cc r=i+Cc r-Cc l;
	Cc l=i;
      }

      /*** Y POS OF THE KEYBAR-ELEMENT ***/
      strcpy(word3,s_optypos); str_srch(b,word3,wech);
      if(txb!=-1) then {
	str_copy(txe+1,'a',word3); i=atoi(word3);
	Cc t=i;
      }

      /*** WIDTH OF THE KEYBAR-ELEMENT ***/
      strcpy(word3,s_optwidth); str_srch(b,word3,wech);
      if(txb!=-1) then {
	str_copy(txe+1,'a',word3); i=atoi(word3);
	Cc r=Cc l+i;
	if(strlen(Cc n)>i) then Cc n[i]='\x0';
      }
    }
  }
}

/*------------------------------------------*/
/*     GET MACROS TO MACRO-BUFFER (mac)     */
/*------------------------------------------*/
void getmacros(void)
{
  int i,j,n;

  i=0; strcpy(word3,""); txe--;
  while(1) {
    txe=step_blank(txe+1);
    txe=str_copy(txe,'a',word3);
    if(strcmp(word3,"endmacro")==0) then { mac[i]=257; return; }
    else if(word3[0]=='"' and word3[1]!='\x0') then {
      n=strlen(word3)-2;
      for(j=0;j<n;j++) mac[i++]=word3[j+1];
    }
    else if(strcmp(word3,"delay")==0) then mac[i++]=300;
    else if(strcmp(word3,"nodelay")==0) then mac[i++]=400;
    else mac[i++]=getkey(word3);
    if (txt[txe]==';') then mac[i++]=258;
  }
}


/*-------------------------------------------*/
/*     GET A KEY CODE FROM HIS NAME	     */
/*-------------------------------------------*/
int getkey(s)
char *s;
{
  int f;

  if(strcmp(s,"nokey")==0) then return(260);
  if(strcmp(s,"space")==0) then return(32);
  if(strcmp(s,"enter")==0) then return(13);
  if(strcmp(s,"esc")==0) then return(27);
  if(strcmp(s,"bcksp")==0) then return(8);
  if(strcmp(s,"tab")==0) then return(9);
  if(strcmp(s,"s-tab")==0) then return(256*15);
  if(strcmp(s,"home")==0) then return(256*71);
  if(strcmp(s,"pgup")==0) then return(256*73);
  if(strcmp(s,"up")==0) then return(256*72);
  if(strcmp(s,"left")==0) then return(256*75);
  if(strcmp(s,"right")==0) then return(256*77);
  if(strcmp(s,"end")==0) then return(256*79);
  if(strcmp(s,"down")==0) then return(256*80);
  if(strcmp(s,"pgdn")==0) then return(256*81);
  if(strcmp(s,"ins")==0) then return(256*82);
  if(strcmp(s,"del")==0) then return(256*83);
  if(strcmp(s,"c-left")==0) then return(256*115);
  if(strcmp(s,"c-right")==0) then return(256*116);
  if(strcmp(s,"c-end")==0) then return(256*117);
  if(strcmp(s,"c-pgdn")==0) then return(256*118);
  if(strcmp(s,"c-home")==0) then return(256*119);
  if(strcmp(s,"c-pgup")==0) then return(256*132);
  if (s[1]!='-') then {
    if (s[0]=='F') then {
      if (s[1]<='9' and s[1]>'1') then return(256*(59+s[1]-'1'));
      else return(256*68);
    }
    else return(s[0]);
  }
  if (s[0]=='a' and s[3]=='\x0') then return(256*alt_key[s[2]-'a']);
  if (s[2]=='F') then {
    if (s[3]<='9' and s[3]>'1') then f=59+s[3]-'1';
    else f=68;
    if (s[0]=='s') then return(256*(f+25));  /* s-... shft */
    if (s[0]=='a') then return(256*(f+45));  /* a-... alt  */
    if (s[0]=='c') then return(256*(f+35));  /* c-... ctrl */
  }
  error("Not valid key code in macros: ! ");
  return(-1); /* for not to get warning 'funct should ret a val' */
}


