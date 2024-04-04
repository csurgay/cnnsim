#define then
#define and &&
#define or ||

#define EGER(kk,ll,mm,nn)\
	inregs.x.ax=kk;\
	inregs.x.bx=ll;\
	inregs.x.cx=mm;\
	inregs.x.dx=nn;\
	int86(0x33,&inregs,&outregs);\
	AX=outregs.x.ax;\
	BX=outregs.x.bx;\
	CX=outregs.x.cx;\
	DX=outregs.x.dx;

#define CUR_SIZE(beg,end)\
	inregs.h.ch=beg;\
	inregs.h.cl=end;\
	inregs.h.ah=1;\
	int86(0x10,&inregs,&outregs);

#define GET_CUR_SIZE(beg,end)\
	inregs.h.bh=0;\
	inregs.h.ah=03;\
	int86(0x10,&inregs,&outregs);\
	beg=outregs.h.ch;\
	end=outregs.h.cl;

#define CHARS(ch,col,num)\
	inregs.h.ah=9;\
	inregs.h.al=ch;\
	inregs.h.bh=0;\
	inregs.h.bl=col;\
	inregs.x.cx=num;\
	int86(0x10,&inregs,&outregs);

#define CHIN(ch,attr)\
	inregs.h.ah=8;\
	int86(0x10,&inregs,&outregs);\
	ch=outregs.h.al;\
	attr=outregs.h.ah;

#define GETSHIFT(sh)\
	inregs.h.ah=2;\
	int86(0x16,&inregs,&outregs);\
	sh=outregs.h.al;\

#define BORDER(col)\
	inregs.h.ah=11;\
	inregs.h.bh=0;\
	inregs.h.bl=col;\
	int86(0x10,&inregs,&outregs);
