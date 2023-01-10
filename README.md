# **SHOGI**
## **介紹**
這個系統提供了可執行的將棋遊戲，以Linklist實現，具有倒帶、計時功能。
## **重點說明**
### **移動系統說明**
透過這個函數搭配我們可以處理input並做出對應的決策。
```C
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
```
### **棋子移動規則制定**
透過下面函數，我們可以利用矩陣的方式來設計棋子移動，這樣可以使我們能客製化各種各類別的棋子。
```C
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

				.	
                .
                .
}
```
### **復活系統**
復活系統在設計時遇到一些困難，因為復活想使用選單的方式去呈現，但主要是在靠```small_handler```來完成。
```C
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
```
### **印棋盤系統**
由多個函數組成，以物件的方式呈現於終端上，棋盤可以根據使用者需求做改動，如果想改成2*2的棋子也是相當容易。

### **總結**
由於時間關係不能很好的呈現報告，程式碼也沒有做很好的整理。
此系統所提供的架構，在設計時想的是易於維護與易於改動，所以此系統只要願意，是可以輕易改成任何棋類遊戲的。
