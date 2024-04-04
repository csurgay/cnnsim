
struct keybars { int db[4]; struct menust *ht[4] ;};

struct menus { /* WINDOW-attributes */
  char ty;	      /* type: m'ain, w'indow (normal submenu), c'hoice, */
  char le;	      /* level in the menu structure */
  char db;	      /* number of options */
  char living;	      /* number of 'living' options */
  char act;	      /* actual highlighted option */
  char l,t,w,h;	      /* left, top, width, height of window */
  char lt;	      /* line type of the frame */
  char *n;	      /* name in the middle of top side */
  char c_w,c_f,c_n;   /* colors: window, frame, name, 	*/
  char c_o,c_s;	      /* 	option, short key       */
  char c_hp,c_hi;     /*	highlight_paper, _ink 	*/
  char *ptr;	      /* saved area under the window */
  int *var; 	      /* an integer (only in choice) */
  void (*bef)();      /* a function called before opening the window */
  struct menust *st;	  /* the array of OPTION-attributes */
  struct keybars *kbar;   /* keybar structure */
};

struct menust { /* OPTION-attributes */
  char *n;	  /* name of option */
  char sb,se;	  /* short key highlight begin, end */
  int c1, c2;	  /* short key codes */
  char l,r,t,el;  /* l, r, top, extra string left (trimmer, load, string) */
  char ty;	  /* type: sub'm'enu, t'rimmer, c'hoice, l'oad, s'tring, */
		  /*		f'unction, func.i'nmain */
  char olt;	  /* delight, when enter the option */
  void (*aft)();  /* a function called after leaveing the option */
  union {
    void (*f)();		/* function */
    struct menus *m;		/* submenu (normal, choice) */
    struct trimmerst *tr;	/* trimmer */
    struct ldstrs *ldstr;	/* extra string */
  } next;
  char *helpstr;		/* help-topic */
  struct keybars *kbar;		/* keybar structure (only in functions) */
};

struct ldstrs {	/* LOAD and STRING */
  char x,y,w,sw;	/* x, y, w, string w of string window */
  char *n;		/* name of string window */
  char *str;		/* extra string beside option */
  char *mask;		/* mask (only in load) */
};

struct trimmerst { /* TRIMMER */
  char db;	/* number of rolling trimmer-strings */
  char act;	/* actual displayed trimmer-string */
  char ml;	/* biggest lenght of trimer-strings */
  char *ts;     /* string containing all trimmer-strings */
  int *var;	/* an integer */
};

struct help_topic {
  char topic[20];
  char l,r,t,b;
};
