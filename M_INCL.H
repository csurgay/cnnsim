#include "ints.h"		/* EGER(1,0,0,0), CUR_SIZE(),... */
#include "keys.h"    /* KEY_F10, KEY_ENTER,... */

/* 	struct BYTEREGS {												*/
/* 		unsigned char al,ah,bl,bh,cl,ch,dl,dh; };       */
/* 	struct WORDREGS {                                  */
/* 		unsigned int ax,bx,cx,dx,si,di,cflag,flags; };  */
/* 	union REGS {                                       */
/* 		struct BYTEREGS h;                              */
/* 		struct WORDREGS x;                              */
/* 	};                                                 */

#include <dos.h>		/* only for this --^  */

#define then
#define or ||
#define and &&


extern void menuinitalize(void);
extern void menureconstruction(void);
extern void message();
extern void frame();
extern void csprintf();
extern char far *load();
extern void error();
extern void settextcolor();
extern void f_NULL(void);
extern int pause();
extern int bhit2(void);
extern int bget2(void);
extern void bput1();


extern int mouse,mx,my;
extern union REGS inregs,outregs;
extern int AX,BX,CX,DX;

