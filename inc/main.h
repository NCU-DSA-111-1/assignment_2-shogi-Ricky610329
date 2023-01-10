/*parameter*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>

/*define constant*/
#define NUMPIECE 40 
#define NUMCOLUMN 9
#define PIECE_SIZE 1
#define X_OFFSET 14
#define Y_OFFSET 1
#define U_X_OFFSET 1
#define U_Y_OFFSET 1


/*string process*/
#define CLEAR() printf("\033[2J")
#define MOVEUP(x) printf("\033[%da",(x))
#define MOVEDOWN(x) printf("\033[%dB",(x))
#define MOVELEFT(x) printf("\033[%dD",(x))
#define MOVERIGHT(x) printf("\033[%dC",(x))
#define MOVETO(x,y) printf("\033[%d;%dH", (x), (y))
#define RESET_CURSOR() printf("\033[H")
#define HIDE_CURSOR() printf("\033[?25l")
#define SHOW_CURSOR() printf("\033[?25h")
#define CLEAR_LINE() printf("\033[K")

#define HIGH_LIGHT() printf("\033[7m")
#define UN_HIGH_LIGHT() printf("\033[27m")

#define SET_FRONT_COLOR(color) printf("\033[%dm",(color))
#define SET_BACKGROUND_COLOR(color) printf("\033[%dm",(color))

#define FRONT_BLACK 30
#define FRONT_RED 31
#define FRONT_GREEN 32
#define FRONT_YELLOW 33
#define FRONT_BLUE 34
#define FRONT_PURPLE 35
#define FRONT_DEEP_GREEN 36
#define FRONT_WHITE 37


#define BACKGROUND_BLACK 40
#define BACKGROUND_RED 41
#define BACKGROUND_GREEN 42
#define BACKGROUND_YELLOW 43
#define BACKGROUND_BLUE 44
#define BACKGROUND_PURPLE 45
#define BACKGROUND_DEEP_GREEN 46
#define BACKGROUND_WHITE 47

/*pieces movement*/
typedef enum special{
	NO,
	FORWARD,
	LINE,
	DIAGONAL
}Special;


/*piece type*/
typedef enum p_type{
	FU,//兵
	KY,//香車
	KE,//桂馬
	GI,//銀將
	KI,//金將
	KA,//角行
	HI,//飛車
	GY//王將
}P_type;


/*track pieces state*/
typedef enum state{
	UP,
	NORMAL
}State;

/*side of the pieces*/
typedef enum s{
	P1,
	P2
}Side;

/*position representation*/
typedef struct position{
	int column;
	int row;
}Position;

/*define a piece*/
typedef struct piece{
	struct piece* under;
	P_type type;
	Side side;
	State state;
	Position pos;
}Piece;

/*define the move of the piece*/
typedef struct defmove{
	Special pattern;
	int size;
	int mov_matrix[5][5];
}DefMove;


typedef enum board_state{
	PIECE_FROM,
	PIECE_TO
}Board_state;

typedef enum call{
	REVIVE,
	DEAD,
	MOVE
}Call;

/*place piece*/
typedef struct pp{
	Position back_to;
	Piece* call_piece;
}Pp;

typedef struct move_node{
	struct move_node* next;
	Call call_move;
	Pp mpiece;
	Pp ipiece;
}Move_node;


typedef struct parameter{
	Position* timer_pos;
	bool* timer_flag;
	int p;
}Parameter;

DefMove mov_FU;//兵
DefMove mov_UPGRADE;//金
DefMove mov_KY;//香車
DefMove mov_KE;//桂馬
DefMove mov_GI;//金將
DefMove mov_HI;//飛車
DefMove mov_KA;//角行
DefMove mov_BLOCK;//王

Piece p_FU;
Piece p_KY;
Piece p_KE;
Piece p_GI;
Piece p_KI;
Piece p_KA;
Piece p_HI;
Piece p_GY;



bool stop_flag = false;
bool t1_flag = true;
bool t2_flag = false;
Position timer1_pos = {26,3};
Position timer2_pos = {26,6};
Parameter time1 = {&timer1_pos,&t1_flag,1};
Parameter time2 = {&timer2_pos,&t2_flag,2};
Move_node* move_stack;
Board_state input_state = PIECE_FROM;
Side next_player = P1;//side of the current player
Position P1_BOARD = {0,11};
Position P2_BOARD = {0,15};
Position pos_null = {0,0};
Position pos_last;
Position pos_select;//position that is been selected on the screen
Position pos_dead = {0,20};
Position pos_form;//position that is selected in the first
Position pos_to;//position that is selected in the second
Piece* pieces;/*store alive piece of the game*/
Piece* board;/*store column index*/
Piece* deadlink1 = NULL;/*store dead pieces*/
Piece* deadlink2 = NULL;
pthread_mutex_t mutex_print = PTHREAD_MUTEX_INITIALIZER;

int** gen2D(int row,int column);
void rule_init(void);
void piece_name_init(void);
void piece_init(void);
void assign(Piece* copy,Piece target,Side side,int row,int column);
Piece* finder(Position target,Piece** Pre);
bool piece_exist(Position pos);
bool board_exist(Position pos);
Piece* move(Position mov_from,Position mov_to);
Piece* dead_find(Side side,Position pos_d,Piece** pre);
void dead_place(Piece* dead);
Position to_from(Position mov_from,Position mov_to);
bool check_matrix(DefMove rule,Position minus);
bool move_rule(Piece* mpiece,Position minus);
bool move_move(Position mov_from,Position mov_to);
char* piecetype(P_type type);
void piece_print(Piece* mpiece,Position target);
void board_print(void);
void game_handler(void);
bool input_handler(char command);
bool check_side(Piece* mpiece);
bool game_quit(void);
void change_side(void);
void change_state(void);
void system_line(void);
void universal_moveto(int x,int y);
Piece* revive_find(Side side,Position pos_d);
void move_print(void);
void push_move(Piece* mpiece,Piece* ipiece,Call calling);
bool pop_move(void);
