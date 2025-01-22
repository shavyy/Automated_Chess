// **** V2
// Make b[] extern, for can acces from Automated Chess
// Call game_mode as external for acces it from micromax

// ************************************  PROTOTYPE
unsigned short myrand(void);
void gameOver();
void bkp();
void serialBoard();
void AI_HvsH();
void AI_HvsC();

// *****************************  EXTERN VARIABLES
extern char mov [];
extern byte sequence;
extern boolean no_valid_move;

// **************************** Make b[] Global V8
/* board is left part, center-pts table is right part, and dummy */
extern char b[];

// **************************** Make game_mode Global V8
extern byte game_mode;
