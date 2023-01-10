/*shogi fix*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>
#include "../inc/main.h"





/*getch implementation*/
static struct termios old, current;

void change_t_flag(void){
	if(t1_flag == true){
		t1_flag = false;
		t2_flag = true;
	}
	else if(t1_flag == false){
		t1_flag = true;
		t2_flag = false;
	}
}


void timer_core(int second, int minute){
	printf("%2d:%2d",minute,second);
}


void* timer_system(void * data){
	Parameter * syset = (Parameter*)data;
	int m = 0;
	int s = 0;
	while(!stop_flag){
		usleep(50001);
		if(*(syset->timer_flag)){
			sleep(1);
			s += 1;
			if(s > 60){
				s = 0;
				m += 1;
			}
		}
		//pthread_mutex_lock(&mutex_print);
		SET_FRONT_COLOR(FRONT_WHITE);
		universal_moveto(syset->timer_pos->column,syset->timer_pos->row);
		printf("玩家%d時間",syset->p);
		timer_core(s,m);
		//pthread_mutex_unlock(&mutex_print);
	}
	
	pthread_exit(NULL);
}



/* Initialize new terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  current = old; /* make new settings same as old settings */
  current.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo) {
      current.c_lflag |= ECHO; /* set echo mode */
  } else {
      current.c_lflag &= ~ECHO; /* set no echo mode */
  }
  tcsetattr(0, TCSANOW, &current); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
	return getch_(0);
}

/* Read 1 character with echo */
char getche(void) 
{
  return getch_(1);
}

/*end of the code*/

void copyfor2d(int ptr0[5][5],int ptr1[5][5],int size){
	for(int i = 0;i<size;i++){
		for(int j = 0;j<size;j++){
			ptr0[i][j] = ptr1[i][j];
		}
	}

}



/*define how piece should move*/

void rule_init(void){

	/*define how 兵 move*/
	mov_FU.pattern = NO;
	mov_FU.size = 2;
	int temp0[5][5] ={
			{0,0,0,0,0},
			{0,0,1,0,0},
			{0,0,0,0,0},
			{0,0,0,0,0},
			{0,0,0,0,0}
			}; 
	copyfor2d(mov_FU.mov_matrix,temp0,5);

					

	/*define how 升級和其他棋子 move*/
	mov_UPGRADE.pattern = NO;
	mov_UPGRADE.size = 2;
	int temp1[5][5] = {
				{0,0,0,0,0},
				{0,1,1,1,0},
				{0,1,0,1,0},
				{0,0,1,0,0},
				{0,0,0,0,0}
				};
	copyfor2d(mov_UPGRADE.mov_matrix,temp1,5);
					
	/*define how 香車 move*/
	mov_KY.pattern = FORWARD;
	mov_KY.size = 0;

	/*define how 桂馬 move*/
	mov_KE.pattern = NO;
	mov_KE.size = 2;
	int temp2[5][5] = {
				{0,1,0,1,0},
				{0,0,0,0,0},
				{0,0,0,0,0},
				{0,0,0,0,0},
				{0,0,0,0,0}
				};
	copyfor2d(mov_KE.mov_matrix,temp2,5);


	/*define how 銀將 move*/
	mov_GI.pattern = NO;
	mov_GI.size = 2;
	int temp3[5][5] = {
				{0,0,0,0,0},
				{0,1,1,1,0},
				{0,0,0,0,0},
				{0,1,0,1,0},
				{0,0,0,0,0}
				};
	copyfor2d(mov_GI.mov_matrix,temp3,5);

	/*define how 飛車 move*/
	mov_HI.pattern = LINE;
	mov_HI.size = 0;

	/*define how 角行 move*/
	mov_KA.pattern = DIAGONAL;
	mov_KA.size = 0;


	/*A block movement for 王 upgrade 角行,飛車*/
	mov_BLOCK.pattern = NO;
	mov_BLOCK.size = 2;
	int temp4[5][5] = {
				{0,0,0,0,0},
				{0,1,1,1,0},
				{0,1,0,1,0},
				{0,1,1,1,0},
				{0,0,0,0,0}
				};
	copyfor2d(mov_BLOCK.mov_matrix ,temp4,5);
}


/*assign piece for it's identity*/
void assign(Piece* copy,Piece target,Side side,int column,int row){
	copy->state = target.state;
	copy->type = target.type;
	copy->side = side;
	copy->pos.row = row;
	copy->pos.column = column; 
}


/*initial identy piece*/
void piece_name_init(void){
	p_FU.type = FU;
	p_FU.state = NORMAL;
	p_KY.type = KY;
	p_KY.state = NORMAL;
	p_KE.type = KE;
	p_KE.state = NORMAL;
	p_GI.type = GI;
	p_GI.state = NORMAL;
	p_KI.type = KI;
	p_KI.state = NORMAL;
	p_KA.type = KA;
	p_KA.state = NORMAL;
	p_HI.type = HI;
	p_HI.state = NORMAL;
	p_GY.type = GY;
	p_GY.state = NORMAL;
}


/*initial board*/
void piece_init(void){
	rule_init();
	piece_name_init();
	board = (Piece*)malloc(NUMCOLUMN * sizeof(Piece));
	pieces = (Piece*)malloc(NUMPIECE * sizeof(Piece));
	int column;
	int row;
	int used = 0;
	int next;
	for(next = 0; next < 39; next++){
		pieces[next].under = &pieces[next+1];
	}
	pieces[39].under = &pieces[0];
	for(column = 0;column<9;column++){
		switch(column){
			case 0:case 8:
				board[column].under = &pieces[used];
				assign(&pieces[used],p_KY,P2,column,0);
				used+=1;
				assign(&pieces[used],p_FU,P2,column,2);
				used+=1;
				assign(&pieces[used],p_FU,P1,column,6);
				used+=1;
				assign(&pieces[used],p_KY,P1,column,8);
				pieces[used].under = NULL;
				used+=1;
				break;
			case 1:
				board[column].under = &pieces[used];
				assign(&pieces[used],p_KE,P2,column,0);
				used+=1;
				assign(&pieces[used],p_KA,P2,column,1);
				used+=1;
				assign(&pieces[used],p_FU,P2,column,2);
				used+=1;
				assign(&pieces[used],p_FU,P1,column,6);
				used+=1;
				assign(&pieces[used],p_HI,P1,column,7);
				used+=1;
				assign(&pieces[used],p_KE,P1,column,8);
				pieces[used].under = NULL;
				used+=1;
				break;
			case 7:
				board[column].under = &pieces[used];
				assign(&pieces[used],p_KE,P2,column,0);
				used+=1;
				assign(&pieces[used],p_HI,P2,column,1);
				used+=1;
				assign(&pieces[used],p_FU,P2,column,2);
				used+=1;
				assign(&pieces[used],p_FU,P1,column,6);
				used+=1;
				assign(&pieces[used],p_KA,P1,column,7);
				used+=1;
				assign(&pieces[used],p_KE,P1,column,8);
				pieces[used].under = NULL;
				used+=1;
				break;
			case 6:case 2:
				board[column].under = &pieces[used];
				assign(&pieces[used],p_GI,P2,column,0);
				used+=1;
				assign(&pieces[used],p_FU,P2,column,2);
				used+=1;
				assign(&pieces[used],p_FU,P1,column,6);
				used+=1;
				assign(&pieces[used],p_GI,P1,column,8);
				pieces[used].under = NULL;
				used+=1;
				break;
			case 5:case 3:
				board[column].under = &pieces[used];
				assign(&pieces[used],p_KI,P2,column,0);
				used+=1;
				assign(&pieces[used],p_FU,P2,column,2);
				used+=1;
				assign(&pieces[used],p_FU,P1,column,6);
				used+=1;
				assign(&pieces[used],p_KI,P1,column,8);
				pieces[used].under = NULL;
				used+=1;
				break;
			case 4:
				board[column].under = &pieces[used];
				assign(&pieces[used],p_GY,P2,column,0);
				used+=1;
				assign(&pieces[used],p_FU,P2,column,2);
				used+=1;
				assign(&pieces[used],p_FU,P1,column,6);
				used+=1;
				assign(&pieces[used],p_GY,P1,column,8);
				pieces[used].under = NULL;
				used+=1;
				break;
		}
	}
}


/*a useful tool for finding piece in paticular position, Pre will return the piece that is point to it*/
Piece* finder(Position target,Piece** Pre){
	Piece* current;
	if(board[target.column].under == NULL){
		return NULL;
	}
	current = board[target.column].under;
	if(Pre != NULL){
		*Pre = &board[target.column];
	}
	do{
		if(current->pos.row == target.row){
			return current;
		}
		else if(current->pos.row > target.row){
			return NULL;
		}
		if(Pre != NULL){
			*Pre = current;
		}
		current = current->under;
	}while(current != NULL);
	return NULL;
}

/*check if piece is exist in certern position*/
bool piece_exist(Position pos){
	if(finder(pos,NULL) != NULL)
		return true;
	else
		return false;
}


/*check is user assign invalid board coordinate*/
bool board_exist(Position pos){
	if(pos.column < 0 || pos.column > 8)
		return false;
	else if(pos.row < 0 || pos.row > 8)
		return false;
	else
		return true;
}

/*move piece return dead piece*/
Piece* move(Position mov_from,Position mov_to){
	Piece* temp=NULL;
	Piece* mpiece = NULL;
	Piece* dead = NULL;
	if(abs(mov_from.row)>10){
		mpiece = revive_find(next_player,pos_dead);
		finder(mov_to,&temp);
		mpiece->pos.row = mov_to.row;
		mpiece->pos.column = mov_to.column;
		mpiece->under = temp->under;
		temp->under = mpiece;
		push_move(mpiece,NULL,REVIVE);
		return mpiece;
	}
	else{
		mpiece = finder(mov_from,&temp);
		temp->under = mpiece->under;//link the old spot
		dead = finder(mov_to,&temp);
		if(dead == NULL){
			mpiece->under = temp->under;
			temp->under = mpiece;
			mpiece->pos.row = mov_to.row;
			mpiece->pos.column = mov_to.column;
			return NULL;
		}
		else{
			mpiece->under = dead->under;
			temp->under = mpiece;
			dead->under = NULL;
			mpiece->pos.row = mov_to.row;
			mpiece->pos.column = mov_to.column;
			return dead;
		}	
	}
}



/*find dead piece of the one side*/
Piece* dead_find(Side side,Position pos_d,Piece** pre){
	Piece* target;
	if(side == P1){
		target = deadlink1;
	}
	else if(side == P2){
		target = deadlink2;
	}
	if(pre != NULL){
		*pre = NULL;
	}
	//universal_moveto(0,25);
	//printf("%d,%d",pos_d.column,pos_d.row);
	while(target != NULL){
		if(target->pos.column == pos_d.column){
			return target;
		}
		else if(target->pos.column < pos_d.column){
			return NULL;
		}
		if(pre != NULL){
			*pre = target;
		}
		target = target->under;
	}
	return NULL;
}


void numberfix(Side side,Position pos_d){
	Piece* pre;
	dead_find(side,pos_d,&pre);
	if(pre != NULL){
		pos_d.column+=1;
		numberfix(side,pos_d);
		pre->pos.column-=1;
	}
}
/*revive dead piece*/
Piece* revive_find(Side side,Position pos_dd){
	Piece* pre;
	Piece* dead;
	Position pos_d;
	pos_d.row = pos_dd.row;
	pos_d.column = pos_dd.column;
	dead = dead_find(side,pos_d,&pre);
	if(dead == NULL){
		return NULL;
	}
	else if(pre != NULL){
		if(dead->under == NULL ){
			pre->under == NULL;
		}
		else{
			pre->under = dead->under;
		}
	}
	else if(pre == NULL){
		if(side == P1){
			if(dead->under == NULL){
				deadlink1 = NULL;
			}
			else{
				deadlink1 = dead->under;
			}
		}
		else if(side == P2){
			if(dead->under == NULL){
				deadlink2 = NULL;
			}
			else{
				deadlink2 = dead->under;
			}
		}
	
	}
	numberfix(side,pos_d);
	dead->under = NULL;
	return dead;
}



void dead_number(Piece* dead){
	Piece* temp = dead->under;
	if(dead->under == NULL){
		dead->pos.column = 0;
	}
	else{
		dead->pos.column = temp->pos.column+1;
	}
}


void check_winner(Piece* piece_type){
	if(piece_type->type == GY){
		if(piece_type->side == P1){
			system_line();
			printf("玩家 2贏了");
		}
		else if(piece_type->side == P2){
			system_line();
			printf("玩家 1贏了");		}
	}

}

/*place dead piece to link that store dead pieces*/
void dead_place(Piece* dead){
	dead->under = NULL;
	dead->state = NORMAL;
	//dead->pos.column = 20;
	check_winner(dead);
	if(dead->side == P1){
		dead->side = P2;
		dead->pos.row = P2_BOARD.row+3;
		dead->under = deadlink2;
		deadlink2 = dead;
		dead_number(deadlink2);
	}
	else if(dead->side == P2){
		dead->side = P1;
		dead->pos.row = P1_BOARD.row+3;
		dead->under = deadlink1;
		deadlink1 = dead;
		dead_number(deadlink1);
	}
}

/*revieve dead piece*/
/*bool revieve_place(Piece* mpiece,Position pos){
	Piece* temp;
	Piece* check;
	check = finder(pos,&temp);
	if(check !=NULL)
		return false;
	mpiece->under = temp->under;
	temp->under = mpiece;
}*/




/*calculate position vector*/
Position to_from(Position mov_from,Position mov_to){
	Position minus;
	minus.row = mov_to.row - mov_from.row;
	minus.column = mov_to.column - mov_from.column;
	return minus;
}


/*check if the piece obeys the rule*/
bool check_matrix(DefMove rule,Position minus){
	if(rule.pattern == NO){
		//printf("%d,%d",minus.column+rule.size,minus.row+rule.size);
		if(abs(minus.row) > rule.size){
			//printf("0");
			return false;
		}
		else if(abs(minus.column) > rule.size){
			//printf("1");
			return false;
		}
		else{
			//printf("%d",rule.mov_matrix[minus.row+rule.size][minus.column+rule.size]);
			if(rule.mov_matrix[minus.row+rule.size][minus.column+rule.size] == 1){
				//printf("hehe");
				return true;
			}
			else{
				return false;
			}
		}
	}
	else if(rule.pattern == FORWARD){
		if(minus.column == 0 && minus.row <0)
			return true;
	}
	else if(rule.pattern == DIAGONAL){
		if(abs(minus.row) == abs(minus.column))
			return true;
	}
	else if(rule.pattern ==LINE){
		if(minus.row != 0 && minus.column !=0)
			return false;
		else if(minus.row == 0 && minus.column ==0)
			return false;
		else
			return true;
	}
}

/*take piece and check identity and check movement*/
bool move_rule(Piece* mpiece,Position minus){
	if(mpiece->side == P2){
		minus.row = -minus.row;
		minus.column = -minus.column;
	}
	if(mpiece->state == NORMAL){
		if(mpiece->type == FU)
			return check_matrix(mov_FU,minus);
		else if(mpiece->type == KY)
			return check_matrix(mov_KY,minus);
		else if(mpiece->type == KE)
			return check_matrix(mov_KE,minus);
		else if(mpiece->type == GI)
			return check_matrix(mov_GI,minus);
		else if(mpiece->type == KI)
			return check_matrix(mov_UPGRADE,minus);
		else if(mpiece->type == KA)
			return check_matrix(mov_KA,minus);
		else if(mpiece->type == HI)
			return check_matrix(mov_HI,minus);
		else if(mpiece->type == GY)
			return check_matrix(mov_BLOCK,minus);
	}
	else if(mpiece->state == UP){
		if(mpiece->type == FU)
			return check_matrix(mov_UPGRADE,minus);
		else if(mpiece->type == KY)
			return check_matrix(mov_UPGRADE,minus);
		else if(mpiece->type == KE)
			return check_matrix(mov_UPGRADE,minus);
		else if(mpiece->type == GI)
			return check_matrix(mov_UPGRADE,minus);
		else if(mpiece->type == KI)
			return check_matrix(mov_UPGRADE,minus);
		else if(mpiece->type == KA)
			return (check_matrix(mov_KA,minus)||check_matrix(mov_BLOCK,minus));
		else if(mpiece->type == HI)
			return (check_matrix(mov_HI,minus)||check_matrix(mov_BLOCK,minus));
		else if(mpiece->type == GY)
			return check_matrix(mov_BLOCK,minus);
	}
}


/*rap all previous function into one move piece function*/
bool move_move(Position mov_from,Position mov_to){
	if(!piece_exist(mov_from))
		return false;
	if(!board_exist(mov_to))
		return false;
	Piece* mpiece;
	Piece* dead;
	Position minus;
	minus = to_from(mov_from,mov_to);
	dead = finder(mov_to,NULL);
	mpiece = finder(mov_from,NULL);
	if(dead != NULL){
		if(mpiece->side == dead->side){
			return false;
		}
	}
	if(!move_rule(mpiece,minus)){
		return false;
	}
	if(dead == NULL){
		push_move(mpiece,dead,MOVE);
	}
	else if(dead !=NULL){
		push_move(mpiece,dead,DEAD);
	}
	dead = move(mov_from,mov_to);
	if(dead == NULL){
		return true;
	}
	dead_place(dead);
	return true;
}


/*print out piece*/
char* piecetype(P_type type){
	switch(type){
	case FU:
		return "兵";
	case KY:
		return "香";
	case KE:
		return "桂";
	case GI:
		return "銀";
	case KI:
		return "金";
	case KA:
		return "角";
	case HI:
		return "飛";
	case GY:
		return "王";
	}
}


/*piece print*/
void piece_moveto(int x,int y){
	pthread_mutex_lock(&mutex_print);
	MOVETO(PIECE_SIZE*(y+1)+Y_OFFSET,2*PIECE_SIZE*(8-x)+1+X_OFFSET);
	pthread_mutex_unlock(&mutex_print);
}


/*object print*/
void universal_moveto(int x,int y){
	pthread_mutex_lock(&mutex_print);
	MOVETO(PIECE_SIZE * y + U_Y_OFFSET, 2* PIECE_SIZE * x + U_X_OFFSET);
	pthread_mutex_unlock(&mutex_print);
}

/*print out piece*/
void piece_print(Piece* pie,Position target){
	if(pie == NULL){
		piece_moveto(target.column,target.row);
		printf("口");
	}
	else{
		piece_moveto(pie->pos.column,pie->pos.row);
		if(pie->side == P1)
			SET_FRONT_COLOR(FRONT_BLUE);
		if(pie->side == P2)
			SET_FRONT_COLOR(FRONT_RED);
		printf("%s",piecetype(pie->type));
		SET_FRONT_COLOR(FRONT_WHITE);
	}
}

void ddprint(Piece* pie){
	if(pie == NULL){
		system_line();
		UN_HIGH_LIGHT();
		universal_moveto(pie->pos.column,pie->pos.row);
		printf("錯誤！！！");
	}
	else{
		if(pie->side == P1){
			universal_moveto(pie->pos.column+P1_BOARD.column,pie->pos.row);
			SET_FRONT_COLOR(FRONT_BLUE);
		}
		if(pie->side == P2){
			universal_moveto(pie->pos.column+P2_BOARD.column,pie->pos.row);
			SET_FRONT_COLOR(FRONT_RED);
		}
		printf("%s",piecetype(pie->type));
		SET_FRONT_COLOR(FRONT_WHITE);
	}

}


void dprint(Side side){
	Piece* target;
	if(side == P1){
		target = deadlink1;
		universal_moveto(P1_BOARD.column,P1_BOARD.row+2);
		SET_FRONT_COLOR(FRONT_BLUE);
		printf("可復活的棋子:");
		universal_moveto(P1_BOARD.column,P1_BOARD.row+3);
	}
	else if(side == P2){
		target = deadlink2;
		universal_moveto(P2_BOARD.column,P2_BOARD.row+2);
		SET_FRONT_COLOR(FRONT_RED);
		printf("可復活的棋子:");
		universal_moveto(P2_BOARD.column,P2_BOARD.row+3);
	}
	if(target == NULL){
		printf("             ");
	}
	else{
		while(target!=NULL){
			ddprint(target);
			target = target->under;
		}
	}
}

/*print dead piece*/
void dead_print(void){
	dprint(P1);
	dprint(P2);
}


/*print out board*/
void board_print(void){
	int num_p;
	CLEAR();
	RESET_CURSOR();
	int row,column;
	for(row = 0; row < 9; row++){
		for(column = 0; column < 9; column++){
			piece_moveto(column,row);
			printf("口");
		}
	}
	for(num_p = 0;num_p < NUMPIECE; num_p++){
		piece_print(&pieces[num_p],pos_null);
	}
}


bool check_side(Piece* mpiece){
	if(mpiece->side == next_player){
		if(next_player == P1)
			next_player = P2;
		else if(next_player == P2)
			next_player =P1;
		return true;
	}
	return false;
}
void run_revive(void){
	Piece* mpiece;
	if(!piece_exist(pos_select)){
		mpiece = move(pos_dead,pos_select);
		change_side();
		move_print();
		dead_print();
	}
}

bool small_handler(char command){
	if(command == 'q'){
		system_line();
		printf("取消復活棋子");
		return false;
	}
	else if(command == 'a'){
		pos_dead.column-=1;
		if(pos_dead.column<0){
			pos_dead.column = 0;
		}
		return true;
	}
	else if(command == 'd'){
		int last;
		last = pos_dead.column;
		pos_dead.column+=1;
		if(dead_find(next_player,pos_dead,NULL) == NULL){
			pos_dead.column = last;
		}
		return true;
	}
	else if(command == '\n'){
		char input;
		while(input = getch()){
			if(input != 'q' && input != 'r' && input != '\n'){
				input_handler(input);
				move_print();
			}
			else if(input == '\n'){
				if(!piece_exist(pos_select)){
					run_revive();
					change_t_flag();
					break;
				}
				else{
					UN_HIGH_LIGHT();
					system_line();
					printf("已經有棋子了！");
				}
				
			}
			else if(input == 'q'){
				UN_HIGH_LIGHT();
				system_line();
				printf("取消復活棋子");
				break;
			}
		}
		return false;
	}
}


void revive_task(void){
	Piece* target;
	Piece* last;
	system_line();
	printf("選擇復活棋子");
	last = dead_find(next_player,pos_dead,NULL);
	if(last == NULL){
		system_line();
		printf("沒有可復活的棋子");
	}
	else{
		while(small_handler(getch())){
			UN_HIGH_LIGHT();
			ddprint(last);
			target = dead_find(next_player,pos_dead,NULL);
			HIGH_LIGHT();
			ddprint(target);
			last = target;
			UN_HIGH_LIGHT();
		}
		dead_print();
	}
}

void push_move(Piece* mpiece,Piece* ipiece,Call calling){
	Move_node* new_node;
	new_node = (Move_node*) malloc(sizeof(Move_node));
	new_node->next = move_stack;
	move_stack = new_node;
	
	new_node->call_move = calling;
	
	new_node->mpiece.call_piece = mpiece;
	new_node->mpiece.back_to.row = mpiece->pos.row;
	new_node->mpiece.back_to.column = mpiece->pos.column;
	
	if(ipiece != NULL){
		new_node->ipiece.call_piece = ipiece;
		new_node->ipiece.back_to.row = ipiece->pos.row;
		new_node->ipiece.back_to.column = ipiece->pos.column;
	}
}

bool pop_move(void){
	Move_node* temp;
	temp = move_stack;
	if(temp == NULL){
		system_line();
		printf("無法再後退了");
		return false;
	}
	
	move_stack = move_stack->next;
	if(temp->call_move == MOVE){
		piece_print(NULL,temp->mpiece.call_piece->pos);
		move(temp->mpiece.call_piece->pos,temp->mpiece.back_to);
		piece_print(temp->mpiece.call_piece,temp->mpiece.call_piece->pos);
		
	}
	else if(temp->call_move == DEAD){
		Piece* tp;
		move(temp->mpiece.call_piece->pos,temp->mpiece.back_to);
		system_line();
		revive_find(temp->ipiece.call_piece->side,temp->ipiece.call_piece->pos);
		finder(temp->ipiece.back_to,&tp);
		temp->ipiece.call_piece->pos.row = temp->ipiece.back_to.row;
		temp->ipiece.call_piece->pos.column = temp->ipiece.back_to.column;
		temp->ipiece.call_piece->under = tp->under;
		tp->under = temp->ipiece.call_piece;
		if(temp->ipiece.call_piece->side == P1){
			temp->ipiece.call_piece->side = P2;
		}
		else if(temp->ipiece.call_piece->side == P2){
			temp->ipiece.call_piece->side = P1;
		}
		piece_print(temp->mpiece.call_piece,temp->mpiece.call_piece->pos);
		piece_print(temp->ipiece.call_piece,temp->ipiece.call_piece->pos);
		dead_print();
	}
	else if(temp->call_move == REVIVE){
		Piece* tp;
		if(temp->mpiece.call_piece->side == P1){
			temp->mpiece.call_piece->side = P2;
		}
		else if(temp->mpiece.call_piece->side == P2){
			temp->mpiece.call_piece->side = P1;
		}
		piece_print(NULL,temp->mpiece.call_piece->pos);
		finder(temp->mpiece.call_piece->pos,&tp);
		tp->under = temp->mpiece.call_piece->under;
		dead_place(temp->mpiece.call_piece);
		dead_print();
	}
	free(temp);
	return true;
}


/*handle keyboard input*/
bool input_handler(char command){
	if(command == 'q'){//esc
		if(game_quit()){
			stop_flag = true;
			return false;
		}
		else{
			return true;
		}
	}
	if(command == 'w'){//up
		pos_last = pos_select;
		pos_select.row -= 1;
		if(!board_exist(pos_select))
			pos_select.row += 1;
		return true;
	}
	else if(command == 's'){//down
		pos_last = pos_select;
		pos_select.row += 1;
		if(!board_exist(pos_select))
			pos_select.row -= 1;
		return true;
	}
	else if(command == 'a'){//left
		pos_last = pos_select;
		pos_select.column += 1;
		if(!board_exist(pos_select))
			pos_select.column -= 1;
		return true;
	}
	else if(command == 'd'){//right
		pos_last = pos_select;
		pos_select.column -= 1;
		if(!board_exist(pos_select))
			pos_select.column += 1;
		return true;
	}
	else if(command == '\n'){//enter
		change_state();
		return true;
	}
	else if(command == 'r'){
		pos_dead.column = 0;
		revive_task();
		return true;
	}
	else if(command == 'b'){
		if(pop_move()){
			change_side();
		}
		return true;
	
	}
	
	/*else{
		printf("%c",command);
		return true;
	}*/
}
void system_line(void){
	universal_moveto(U_X_OFFSET,U_Y_OFFSET+20);
	printf("                                        ");
	universal_moveto(U_X_OFFSET,U_Y_OFFSET+20);
	SET_FRONT_COLOR(FRONT_WHITE);
	printf("系統提示:");
}


bool game_quit(void){
	system_line();
	UN_HIGH_LIGHT();
	printf("Quit?[y\\n]");
	char input = 'n';
	while(input!='y'&&input!='Y'){
		input = getch();
		if(input == 'n'||input == 'N'){
			system_line();
			return false;
		}
	}
	return true;
}

void move_print(void){
	UN_HIGH_LIGHT();
	piece_print(finder(pos_last,NULL),pos_last);
	HIGH_LIGHT();
	piece_print(finder(pos_select,NULL),pos_select);
	UN_HIGH_LIGHT();
}


void change_input_state(){
	if(input_state == PIECE_FROM)
		input_state = PIECE_TO;
	else if(input_state == PIECE_TO)
		input_state = PIECE_FROM;

}

void change_state(void){
	Piece* temp;
	if(input_state == PIECE_FROM){
		temp = finder(pos_select,NULL);
		if(temp == NULL){
			system_line();
			printf("無效選擇");
		}
		if(piece_exist(pos_select)){
			if(temp->side != next_player){
				system_line();
				printf("請選擇玩家%2d的棋子",next_player+1);
			}
			else{
				pos_form = pos_select;
				change_input_state();
			}
		}
	}
	
	else if(input_state == PIECE_TO){
			temp = finder(pos_select,NULL);
			pos_to = pos_select;
			if(move_move(pos_form,pos_to)){
				piece_print(finder(pos_form,NULL),pos_form);
				change_input_state();
				change_side();
				change_t_flag();
				/*if(piece_exist(pos_to)){
					push_move(finder(pos_form,NULL),finder(pos_to,NULL),DEAD);
				}
				else{
					push_move(finder(pos_form,NULL),NULL,MOVE);
				}*/
			}
			else{
				change_input_state();
			}
		}
}

void line_print(int length){
	SET_FRONT_COLOR(FRONT_WHITE);
	int i;
	for(i = 0;i<length;i++){
		printf("=");
	}
}
void straight_print(int length,int row,int column){
	SET_FRONT_COLOR(FRONT_WHITE);
	int i;
	for(i = 0;i<length;i++){
		printf("\033[%d;%dH",i+column,row);
		printf("|");
	}
}

void pattern(void){
	srand(42);//magic number
	int i,j;
	char grap[4][4] = {"●","○","■","□"};
	for(i = 0;i < 20;i++){
		universal_moveto(36,i);
		for(j = 0;j < 4;j++){
			printf("%s ",grap[rand()%4]);
		}	
	}
	/*universal_moveto(36,0);
	printf("● ○ □ ■ ");
	universal_moveto(36,1);*/
} 

void instruction(void){
	universal_moveto(22,12);
	printf("說明：");
	universal_moveto(22,13);
	printf("輸入q來退出");
	universal_moveto(22,14);
	printf("輸入b來回到上一步");
	universal_moveto(22,15);
	printf("輸入r來復活棋子");
	universal_moveto(22,16);
	printf("以w,a,s,d選則棋子");
	universal_moveto(22,17);
	HIGH_LIGHT();
	printf("<><>  by Ricky  <><>");
	UN_HIGH_LIGHT();
}

void draw_UI(void){
	SET_FRONT_COLOR(FRONT_BLUE);
	universal_moveto(P1_BOARD.column,P1_BOARD.row);
	printf("玩家 1：");
	SET_FRONT_COLOR(FRONT_RED);
	universal_moveto(P2_BOARD.column,P2_BOARD.row);
	printf("玩家 2：");
	universal_moveto(0,10);
	line_print(72);
	straight_print(11,47,0);
	straight_print(11,72,0);
	universal_moveto(0,19);
	line_print(72);
	straight_print(8,72,12);
	straight_print(8,36,12);
	instruction();
	pattern();
}

void system_and_print(Position target){
	if(input_state == PIECE_FROM){
		universal_moveto(target.column,target.row+1);
		printf("                 ");
		universal_moveto(target.column,target.row+1);
		SET_FRONT_COLOR(FRONT_WHITE);
		printf("(%2d,%2d)->",pos_select.column+1,pos_select.row+1);
	}
	else if(input_state == PIECE_TO){
		universal_moveto(target.column+5,target.row+1);
		SET_FRONT_COLOR(FRONT_WHITE);
		printf("(%2d,%2d)",pos_to.column+1,pos_to.row+1);
	}
}


void system_system(void){
	UN_HIGH_LIGHT();
	if(next_player == P1){
		SET_FRONT_COLOR(FRONT_BLUE);
		universal_moveto(P1_BOARD.column,P1_BOARD.row);
		printf("玩家 1：");
		universal_moveto(P1_BOARD.column,P1_BOARD.row+1);
		system_and_print(P1_BOARD);	
	}
	else if(next_player == P2){
		SET_FRONT_COLOR(FRONT_RED);
		universal_moveto(P2_BOARD.column,P2_BOARD.row);
		printf("玩家 2：");
		universal_moveto(P2_BOARD.column,P2_BOARD.row+1);
		system_and_print(P2_BOARD);
	}
}

void change_side(void){
	if(next_player == P1)
		next_player = P2;
	else if(next_player == P2)
		next_player = P1;
}

/*game init*/
void game_handler(void){
	HIDE_CURSOR();
	pos_select.row = 0;
	pos_select.column = 0;
	Piece* mpiece = NULL;
	while(input_handler(getch())){
		system_system();
		move_print();
		dead_print();
	}
}

int main(void){
	pthread_t timer1;
	pthread_t timer2;
	pthread_create(&timer1,NULL,timer_system,&time1);
	pthread_create(&timer2,NULL,timer_system,&time2);
	
	piece_init();
	board_print();
	draw_UI();
	game_handler();
	pthread_join(timer1,NULL);
	pthread_join(timer2,NULL);
	universal_moveto(0,50);
	return 0; 
}
