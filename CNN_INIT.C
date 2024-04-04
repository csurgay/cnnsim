#include <stdio.h>		/* FILE, fopen, fclose, fgetc, fscanf, NULL */
#include <stdlib.h>		/* gcvt() */
#include <string.h>		/* strcpy() */

void initinf(void);
void inittf(void);

extern void message();
extern void error();

int succ;

extern first_mikiconfig;
extern int rt;
extern char s_inp[], s_tem[], s_net[];
extern char s_no_level[], s_resis[], s_capac[], s_tst[], s_curre[];

/********************	      CONSTANTS        **************************/

#define MAXLEVEL 8 /* max. no. of level */
#define MAXDELAY 5 /* max. delay in time
		       for example :
			 delay=3 --> you can use the state and the output
			 array you've define 3 'minutes' before */

/**********************     GLOBAL VARIABLES	 *************************/

extern char
  fname[],      /*?*/
  ns,    /* if the net is initialized then ns='y' else ns='n' */
  is,    /* if the input level is set then is='y' else is='n' */
  ts,    /* if any template is set then ts='y' else ts='n' */
  ws;

extern unsigned long
  sumn,  /* the actual size of the net */
  sumi,  /* the actual size of the input */
  sumt,  /* the actual size of the templates (all) */
  sumact,/* this is my free memory
	       at the beginning it means the whole
	       memory I've allocated */
  sum;   /* the memory for me */

extern int
  level, /* the no. of levels */
  in[MAXLEVEL+1], /* we need to load the input level to the levels
			 where in[i]=1 */
  al, /* actual length of the input and the other levels */
  alin ,     /* the length from the input file */
  aw,  /* actual width of the input and the other levels */
  awin,     /* the width from the input file */
  rt1,   /* rt1=2*rt+1 ,because this value is used so much */
  del; /* actual delay --> if del=3 then you can
		use the previous values of the s,o arrays
		that was defined (1,2,3)*time step earlier */

extern float
  defo[MAXLEVEL+1],/* the values we must initialize the arrays with */
  huge *thloc, /* the pointer --> the last free byte+1*/
  huge *hmain, /* the pointer --> the start of my place in memory */
  huge * t,  /* template - max. radius: 2 */
  huge * o[MAXLEVEL+1], /* levelpointers of the output array
			   and of the input array */
  huge * it[MAXDELAY+1][MAXLEVEL+1][MAXLEVEL+1],
		 /* it[0][i][j] is a pointer,it[0][i][j]<>NULL
		   means that the template between level
		   i and j is set,and this pointer shows
		   the first element of this template */
  II;  /* current */


void initinf(void)
{
  FILE	*fileptr;
  int x,y,ac1,in1;
  float hel;
  int ch;

  if(is=='y')  {
    sumact-=sumi;
    sumi=0;
  }
  is='n';
  ws='n';

  if (ns=='y') {
    sumact+=sumn;
    ns='n';
  }

  strcpy(fname,s_inp);
  if (fname==NULL || fname[0]=='\x0')  goto err2;
  fileptr=fopen(fname,"r");
  if (fileptr==NULL) {
    message(" Can't read the Input file","crif");
    goto err2;
  }

  ch=fgetc(fileptr);
  if (ch!='I') goto oldi;
  do ch=fgetc(fileptr); while ((ch!=':')&&(ch!=EOF));
  if (fscanf(fileptr,"%d",&alin)==0)  goto err;
  do ch=fgetc(fileptr); while ((ch!=':')&&(ch!=EOF));
  if (fscanf(fileptr,"%d",&awin)==0)  goto err;
samei:
  al=alin+2*rt;
  aw=awin+2*rt;
  sumi=al*aw;   /*!?*/
  if (sumact<sumi) {
    fclose(fileptr);
    message(" No memory for the Input ","nmfi");
    goto err2;
  }

  for (y=0;y!=al*aw;y+=aw)
    for (x=0;x!=aw;x++)
      o[0][y+x]=defo[0];  /* because of the frame */

  ac1=rt*aw;
  in1=0;
  do
  {
    ac1+=rt;
    for (x=0;x!=awin;x++) {
      succ=fscanf(fileptr,"%g",&hel);
      if(succ==EOF) goto err;
      if(succ==0) goto err;

      o[0][ac1]=hel;
      /* printf("%g\t",o[0][ac1]);*/ /* garbage */
      ac1++;in1++;
    }
    ac1+=rt;
    fscanf(fileptr,"\n");
  } while (in1<awin*alin);
  sumact-=sumi;
  is='y';
  first_mikiconfig=1;
  fclose(fileptr);
  return;

oldi:
  fclose(fileptr);
  fileptr=fopen(fname,"r");
  if (fileptr==NULL) {
    message(" Can't read the Input file","crif");
    goto err2;
  }
  if (fscanf(fileptr,"%d",&alin)==0)  goto err;
  if (fscanf(fileptr,"%d",&awin)==0)  goto err;
  goto samei;
err:
  fclose(fileptr);
  message(" Not correct Input format ","ncif");
err2:
  is='n'; awin=0; alin=0;
  return;
}		   /* result :  the input array is full */

/********************	 read the template from file	*******************/

void inittf(void)
{
  FILE	*fileptr;
  int i,j,k,so,ta,rtl,act_del;
  float flo;
  int ch;
  unsigned long sum1;

  if (ts=='y') sumact+=sumt;
  thloc=hmain+sum;
  sumt=0;
  ts='n';

  if (ns=='y') {
    sumact+=sumn;
    ns='n';
  }

  strcpy(fname,s_tem);
  if (fname==NULL || fname[0]=='\x0')  goto err2;
  fileptr=fopen(fname,"r");
  if (fileptr==NULL) {
    message(" Can't read the Template file","crtf");
    goto err2;
  }

  for (k=0;k!=MAXDELAY+1;k++)
    for (i=0;i!=MAXLEVEL+1;i++)
      for (j=0;j!=MAXLEVEL+1;j++)
	it[k][i][j]=NULL;
  act_del=0;
  rtl=rt;
  ch=fgetc(fileptr);
  if (ch!='T') goto oldt;
  do ch=fgetc(fileptr) ; while ((ch!=':')&&(ch!=EOF));
  if (ch==EOF) goto err;
  if (fscanf(fileptr,"%d",&rt)==0)  goto err;
  rt1=rt+rt+1;
  if ((is=='y') && (rt!=rtl)) {
    is='n';
    message(" Radius changed, reload the Input !","rcri");
    sumact+=sumi;
    sumi=0;
  }
  do ch=fgetc(fileptr) ; while ((ch!=':')&&(ch!=EOF));
  if (ch==EOF) goto err;
  if (fscanf(fileptr,"%g",&II)==0) {
    goto err2;
  }
  gcvt(II,5,s_curre);
  do ch=fgetc(fileptr) ; while ((ch!=':')&&(ch!=EOF));
  if (ch==EOF) goto err;
  if (fscanf(fileptr,"%lu",&level)==0)  goto err;
  itoa(level,s_no_level,10);
  do ch=fgetc(fileptr) ; while ((ch!=':')&&(ch!=EOF));
  if (ch==EOF) goto err;
  if (fscanf(fileptr,"%d",&del)==0)  goto err;
  do ch=fgetc(fileptr) ; while ((ch!='A')&&(ch!='D')&&(ch!=EOF));
  if (ch==EOF) goto err;
  if (ch=='D')
    if (fscanf(fileptr,"%d",&act_del)==0)  goto err;

  sum1=rt1*rt1;

  while((succ=fscanf(fileptr,"%d%d\n",&so,&ta))!=EOF) {
    if (succ==0)  goto err;
    if (sumact<sum1) {
      fclose(fileptr);
      message(" No memory for the Template ","nmft");
      goto err2;
    }
    thloc-=sum1;
    sumt+=sum1;
    it[act_del][ta][so]=thloc;
    for (i=0;i!=rt1*rt1;i++) {
      if (i % (2*rt+1)==0) fscanf(fileptr,"\n");
      if((succ=fscanf(fileptr,"%g",&flo))!=EOF) {
	if (succ==0)  goto err;
	it[act_del][ta][so][i]=flo;
      }
      else goto err;
    }
    do ch=fgetc(fileptr) ; while ((ch!='A')&&(ch!='D')&&(ch!=EOF));
    if (ch=='D')
      if (fscanf(fileptr,"%d",&act_del)==0)  goto err;

  }
  ts='y';
  sumact-=sumt;
  fclose(fileptr);
  return;

oldt:

  do ch=fgetc(fileptr) ; while ((ch!=':')&&(ch!=EOF));
  if (ch==EOF) goto err;
  if (fscanf(fileptr,"%d",&rt)==0)  goto err;
  rt1=rt+rt+1;
  if ((is=='y') && (rt!=rtl)) {
    is='n';
    message(" Radius changed, reaload the Input !","rcri");
    sumact+=sumi;
    sumi=0;
  }
  sum1=rt1*rt1;
  level=1; strcpy(s_no_level,"1");
  II=0; strcpy(s_curre,"0.000");
  in[0]=1;
  in[1]=2;
  while (ch!=EOF) {
    do ch=fgetc(fileptr) ;
    while ((ch!='f')&&(ch!='F')&&(ch!='c')&&(ch!='C')&&
	   (ch!='d')&&(ch!='D')&&(ch!=EOF));
    switch (ch)
    {
      case 'c':
      case 'C':
	ch=fgetc(fileptr);
	if ((ch=='u')||(ch=='U')) {
	  do ch=fgetc(fileptr) ; while ((ch!=':')&&(ch!=EOF));
	  if (ch==EOF) goto err;
	  if (fscanf(fileptr,"%g",&II)==0) goto err2;
	  gcvt(II,5,s_curre);
	}
	else {
	  if (sumact<sum1) {
	    fclose(fileptr);
	    message(" No memory for the Template ","nmft");
	    goto err2;
	  }
	  while ((ch!=':')&&(ch!=EOF)) ch=fgetc(fileptr);
	  if (ch==EOF) goto err;
	  thloc-=sum1;
	  sumt+=sum1;
	  it[act_del][1][0]=thloc;
	  for (i=0;i!=rt1*rt1;i++) {
	    if (i % (2*rt+1)==0) fscanf(fileptr,"\n");
	    if(fscanf(fileptr,"%g",&flo)!=EOF) it[act_del][1][0][i]=flo;
	    else goto err;
	  }
	}
	break;

	case 'f':
	case 'F':
	  do ch=fgetc(fileptr) ; while ((ch!=':')&&(ch!=EOF));
	  if (ch==EOF) goto err;
	  if (sumact<sum1) {
	    fclose(fileptr);
	    message(" No memory for the Template ","nmft");
	    goto err2;
	  }
	  thloc-=sum1;
	  sumt+=sum1;
	  it[act_del][1][1]=thloc;
	  for (i=0;i!=rt1*rt1;i++) {
	    if (i % (2*rt+1)==0) fscanf(fileptr,"\n");
	    if(fscanf(fileptr,"%g",&flo)!=EOF) it[act_del][1][1][i]=flo;
	    else goto err;
	  }
	  break;

	  case 'd':
	  case 'D':
	  for (i=0;i!=5;i++) ch=fgetc(fileptr);
	  if ((ch!='e')&&(ch!='E')) {
	    while ((ch!=':')&&(ch!=EOF)) ch=fgetc(fileptr);
	    if (ch==EOF) goto err;
	    fscanf(fileptr,"%d",&del);
	    act_del=del;
	  }
	  else {
	    while ((ch!='_')&&(ch!=EOF)) ch=fgetc(fileptr);
	    if (ch==EOF) goto err;
	  }
	  break;
    }
  }
  ts='y';
  sumact-=sumt;
  fclose (fileptr);
  return;

err:
  fclose(fileptr);
  message(" Not correct Template format ","nctf");
err2:
  ts='n';
  strcpy(s_no_level,"1"); strcpy(s_curre,"0.000"); rt=1; del=0;
  II=0; level=1; strcpy(s_curre,"0.000");
  return;

}
