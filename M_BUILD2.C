
#include <string.h>		/* srtcpy() */
#include <stdlib.h>		/* atoi() */
#include "stru.h"
#include "keys.h"

#define then
#define or ||
#define and &&

#define wech '^'  /* window end character               */
#define fech '|'        /* file end character                   */
#define lech ';'        /* list end character                   */  /* in the .mnu file */
#define lsch ','  /* list separating character  */
#define lcch ':'        /* label closing character      */

char alt_key[] = {	/* array of alt key codes a..z */
30,48,46,32,18,33,34,35,23,36,37,38,50,49,24,25,16,19,31,20,22,47,17,45,21,44
};

/*------------------------------------------------------------*/
/*			STRING CONSTANTS TO SEARCH FOR IN THE .mnu FILE      */
/*------------------------------------------------------------*/
														/* searched x times */
extern char s_style[];
extern char s_turbo[];
extern char s_on[];

extern char s_main[];
extern char s_delay[];
extern char s_showfreememory[];
extern char s_showshiftstatus[];
extern char s_macros[];
extern char s_keybarsc[];
extern char s_normal[];
extern char s_shift[];
extern char s_alt[];
extern char s_ctrl[];
extern char s_helpwindow[];
extern char s_loadwindow[];
extern char s_keybarwindow[];
extern char s_stringcolor[];

extern char s_menu[];
extern char s_function[];
extern char s_trimmer[];
extern char s_choice[];
extern char s_remark[];
extern char s_string[];
extern char s_load[];

extern char s_name[];
extern char s_living[];
extern char s_color[];
extern char s_hotlight[];
extern char s_hotkey[];
extern char s_linetype[];
extern char s_winwidth[];
extern char s_winheight[];
extern char s_winxpos[];
extern char s_winypos[];
extern char s_optwidth[];
extern char s_optxpos[];
extern char s_optypos[];
extern char s_stringmaxwidth[];
extern char s_help[];
extern char s_keybar[];
extern char s_delight[];
extern char s_before[];
extern char s_after[];
extern char s_inmain[];
extern char s_variable[];
extern char s_mask[];
extern char s_extrapos[];


extern void error();
extern char *csmalloc();
extern char *cscalloc();
extern void str_srch();
extern int str_copy();
extern int step_blank();
extern char_count();
extern void toendsem();

struct trimmerst *gettrimmer();
void getldstr();
void optionbanat();
void windowbanat();
void getmacros(void);
void getkeybar(void);
void getkeybarpart();
int getkey();

extern void (*fun_array[])();
extern int *int_array[];
extern char *str_array[];

extern int mac[];
extern struct keybars h[];
extern struct menus keybarw;

extern char *txt;
extern int txb,txe,txl;
extern int maxw;

extern char word3[];

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
	lL->w=40; lL->sw=37; lL->n=""; lL->mask="*.*";
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
		lL->mask=csmalloc(60);
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
/*			GET FEATURES OF AN OPTION	  		*/
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
/*			GET FEATURES OF A WINDOW	  		*/
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
/*	  				GET KEYBAR TO ARRAY h 	      	*/
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
	};
	str_srch(acthotbegin,s_shift,fech);
	if (txb!=-1 and txb<acthotend) then {
		getkeybarpart(txe,1,i,acthotend);
	};
	str_srch(acthotbegin,s_alt,fech);
	if (txb!=-1 and txb<acthotend) then {
		getkeybarpart(txe,2,i,acthotend);
	};
	str_srch(acthotbegin,s_ctrl,fech);
	if (txb!=-1 and txb<acthotend) then {
		getkeybarpart(txe,3,i,acthotend);
	};
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
/*	 		GET A KEY CODE FROM HIS NAME	      */
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

