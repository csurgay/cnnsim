#include <stdlib.h>		/* NULL */

extern int akosmain();
extern void setdefault(void);
void base_iter(void);
void iter2(void);
void iterall(void);
void del_iter(void);
void del_iterall(void);

extern int rt;

/* ллл				external variables				ллл */

/********************	      CONSTANTS        **************************/

#define MAXLEVEL 8 /* max. no. of level */
#define MAXDELAY 5 /* max. delay in time
	 for example :
	 delay=3 --> you can use the state and the output
	 array you've define 3 'minutes' before */

/**********************     GLOBAL VARIABLES	 *************************/


extern int
  level, /* the no. of levels */
  al, /* actual length of the input and the other levels */
  aw,  /* actual width of the input and the other levels */
  rt1,	 /* rt1=2*rt+1 ,because this value is used so much */
  change[MAXDELAY+1],	/* this arrays help us to remember which delo
			array contains the values of any delay-time
			for example change[3]==2 means :delo[2]contains
			the values iterated 3 time_step earlier */
  del; /* actual delay --> if del=3 then you can
	use the previous values of the s,o arrays
	that was defined (1,2,3)*time step earlier */

extern float
  mps1,mps2, /* mps1,mps2 are the number we must multiplicate the previous
			states and the other parameters with */
  huge * t,  /* template - max. radius: 2 */
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


  /*********************      iterate	      ***********************/
  /*********  rt==1 R==1 C==1 tst==1 cent==0 sigm==1 del==0 *********/

void base_iter(void)
{
  int	x,y,l,k;
  float  ab,ac,ad;

  {
    for (k=1;k!=level+1;k++) {
      for (y=rt*aw;y!=(al-rt)*aw;y+=aw)
	for (x=rt;x!=aw-rt;x++) s[k][y+x]=II;
      for (l=0;l!=level+1;l++) {
	if (it[0][k][l]!=NULL) {
	  t=it[0][k][l];
	  for (y=rt*aw;y!=(al-rt)*aw;y+=aw) {
	    for (x=rt;x!=aw-rt;x++) {
	      ab=t[0]*o[l][y+aw+x-1]+t[1]*o[l][y+aw+x]+t[2]*o[l][y+aw+x+1];
	      ac=t[3]*o[l][y+x-1]+t[4]*o[l][y+x]+t[5]*o[l][y+x+1];
	      ad=t[6]*o[l][y-aw+x-1]+t[7]*o[l][y-aw+x]+t[8]*o[l][y-aw+x+1];
	      s[k][y+x]+=(ab+ac+ad);
	    }
	  }
	}  /* end if */
      } /* end of l-for */
    }  /* end of k-for */

    for (k=1;k!=level+1;k++) {
      for (y=rt*aw;y!=(al-rt)*aw;y+=aw)
	for (x=rt;x!=aw-rt;x++) {
	  o[k][x+y]=0.5*(abs(s[k][x+y]+1)-abs(s[k][x+y]-1));
	}
    }  /* end of k-for 2. */

  } /* end of the block iterate in base_iter */
}


/******************		  iterate	    ********************/
/******** rt==2 R==1 C==1 tst==1 cent==0 sigm==1 del==0 ************/

void iter2(void)
{
  int	x,y,l,k;
  float  ab,ac,ad,ae,af;

  for (k=1;k!=level+1;k++) {
    for (y=rt*aw;y!=(al-rt)*aw;y+=aw)
      for (x=rt;x!=aw-rt;x++) s[k][y+x]+=II;
    for (l=0;l!=level+1;l++) {
      if (it[0][k][l]!=NULL) {
	t=it[0][k][l];
	for (y=rt*aw;y!=(al-rt)*aw;y+=aw) {
	  for (x=rt;x!=aw-rt;x++) {
	    ab=t[0]*o[l][y+aw+aw+x-2]+t[1]*o[l][y+aw+aw+x-1]+
	      t[2]*o[l][y+aw+aw+x]+t[3]*o[l][y+aw+aw+x+1]+
	      t[4]*o[l][y+aw+aw+x+2];
	    ac=t[5]*o[l][y+aw+x-2]+t[6]*o[l][y+aw+x-1]+
	      t[7]*o[l][y+aw+x]+t[8]*o[l][y+aw+x+1]+
	      t[9]*o[l][y+aw+x+2];
	    ad=t[10]*o[l][y+x-2]+t[11]*o[l][y+x-1]+
	      t[12]*o[l][y+x]+t[13]*o[l][y+x+1]+
	      t[14]*o[l][y+x+2];
	    ae=t[15]*o[l][y-aw+x-2]+t[16]*o[l][y-aw+x-1]+
	      t[17]*o[l][y-aw+x]+t[18]*o[l][y-aw+x+1]+
	      t[19]*o[l][y-aw+x+2];
	    af=t[20]*o[l][y-aw-aw+x-2]+t[21]*o[l][y-aw-aw+x-1]+
	      t[22]*o[l][y-aw-aw+x]+t[23]*o[l][y-aw-aw+x+1]+
	      t[24]*o[l][y-aw-aw+x+2];
	    s[k][y+x]+=(ab+ac+ad+ae+af);
	  }
	}
      }  /* end if */
    }
  }  /* end of k-for */

  for (k=1;k!=level+1;k++) {
    for (y=rt*aw;y!=(al-rt)*aw;y+=aw)
      for (x=rt;x!=aw-rt;x++) {
	o[k][y+x]=0.5*(abs(s[k][y+x]+1)-abs(s[k][y+x]-1));
      }
  }  /* end of k-for 2. */

}  /* end of the block iterate in iter2 */


/******************		   iterate	      ********************/
/**************************	del==0	   ***************************/

void iterall(void)
{
  int	x,y,l,k,ind;

  for (k=1;k!=level+1;k++) {
    for (y=rt*aw;y!=(al-rt)*aw;y+=aw)
      for (x=rt;x!=aw-rt;x++) {
	s[k][y+x]*=mps1;
	s[k][y+x]+=(mps2*II);
      }

    for (l=0;l!=level+1;l++) {
      if (it[0][k][l]!=NULL) {
	t=it[0][k][l];
	for (y=rt*aw;y!=(al-rt)*aw;y+=aw) {
	  for (x=rt;x!=aw-rt;x++) {
	    for (ind=0;ind!=rt1*rt1;ind++) {
	      s[k][y+x]+=
		(mps2*t[ind]*o[l][y-(ind/rt1-rt)*aw+x+(ind%rt1-rt)]);
	    }
	  }
	}
      }  /* end if */
    }
  }  /* end of k-for */

  for (k=1;k!=level+1;k++) {
    for (y=rt*aw;y!=(al-rt)*aw;y+=aw)
      for (x=rt;x!=aw-rt;x++) {
	o[k][y+x]=cent+0.5*(abs(s[k][y+x]+sigm)-abs(s[k][y+x]-sigm));
      }
  }  /* end of k-for 2. */

}  /* end of the block iterate in iterall */


/*********************	    iterate	    ***********************/
/*********  rt==1 R==1 C==1 tst==1 cent==0 sigm==1 del<>0 *********/

void del_iter(void)
{
  int	x,y,l,k,d;
  float  ab,ac,ad;

  {
    for (k=1;k!=level+1;k++) {
      for (y=rt*aw;y!=(al-rt)*aw;y+=aw)
	for (x=rt;x!=aw-rt;x++) s[k][y+x]=+II;

      for (d=0;d!=del+1;d++) {
	for (l=0;l!=level+1;l++) {
	  if (it[d][k][l]!=NULL) {
	    t=it[d][k][l];
	    for (y=rt*aw;y!=(al-rt)*aw;y+=aw) {
	      for (x=rt;x!=aw-rt;x++) {
		ab=t[0]*delo[d][l][y+aw+x-1]+t[1]*delo[d][l][y+aw+x]+
		  t[2]*delo[d][l][y+aw+x+1];
		ac=t[3]*delo[d][l][y+x-1]+t[4]*delo[d][l][y+x]+
		  t[5]*delo[d][l][y+x+1];
		ad=t[6]*delo[d][l][y-aw+x-1]+t[7]*delo[d][l][y-aw+x]+
		  t[8]*delo[d][l][y-aw+x+1];
		s[k][y+x]+=(ab+ac+ad);
	      }
	    }
	  }  /* end if */
	} /* end of l-for */
      }  /* end of d-for */
    }  /* end of k-for */

    for (k=1;k!=level+1;k++) {
      for (y=rt*aw;y!=(al-rt)*aw;y+=aw)
	for (x=rt;x!=aw-rt;x++) {
	  o[k][y+x]=0.5*(abs(s[k][y+x]+1)-abs(s[k][y+x]-1));
	}
    }  /* end of k-for 2. */
  } /* end of the block iterate in del_iter */

  y=change[del];      /* y only saves the value of change[3] */
  for (x=del;x!=1;x--)
    change[x]=change[x-1];
  change[1]=y;	      /* y means the previous value of change[del] ! */
  for (k=1;k!=level+1;k++)
    for (y=aw;y!=al*aw;y+=aw)	  /* here rt==1 ! */
      for (x=1;x!=aw-1;x++)
	delo[change[1]][k][y+x]=o[k][y+x]; /* we copy array o to
					    array delo[change[1]] */
}


/******************		   iterate	      ********************/
/**************************	del<>0	   ***************************/

void del_iterall(void)
{
  int	x,y,l,k,ind,d;

  for (k=1;k!=level+1;k++) {
    for (y=rt*aw;y!=(al-rt)*aw;y+=aw)
      for (x=rt;x!=aw-rt;x++) {
	s[k][y+x]*=mps1;
	s[k][y+x]+=(mps2*II);
      }

    for (d=0;d!=del+1;d++) {
      for (l=0;l!=level+1;l++) {
	if (it[d][k][l]!=NULL) {
	  t=it[d][k][l];
	  for (y=rt*aw;y!=(al-rt)*aw;y+=aw) {
	    for (x=rt;x!=aw-rt;x++) {
	      for (ind=0;ind!=rt1*rt1;ind++) {
		s[k][y+x]+=
		  (mps2*t[ind]*delo[d][l][y-(ind/rt1-rt)*aw+x+(ind%rt1-rt)]);
	      } 	      /* ez itt ^ nem volt jo */
	    }
	  }
	}  /* end if */
      }
    } /* end of d-for */
  }  /* end of k-for */

  for (k=1;k!=level+1;k++) {
    for (y=rt*aw;y!=(al-rt)*aw;y+=aw)
      for (x=rt;x!=aw-rt;x++) {
	o[k][y+x]=cent+0.5*(abs(s[k][y+x]+sigm)-abs(s[k][y+x]-sigm));
      }
  }  /* end of k-for 2. */

  y=change[del];      /* y only saves the value of change[3] */
  for (x=del;x!=1;x--)
    change[x]=change[x-1];
  change[1]=y;	      /* y means the previous value of change[del] ! */
  for (k=1;k!=level+1;k++)
    for (y=rt*aw;y!=(al-rt)*aw;y+=aw)
      for (x=rt;x!=aw-rt;x++)
	delo[change[1]][k][y+x]=o[k][y+x]; /* we copy array o to
																							array delo[change[1]] */
}  /* end of the block iterate in del_iterall */
