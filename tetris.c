#include "tetris.h"

static struct sigaction act, oact;
int score_number=0;
Node *rankHead = NULL;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	createRankList();

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_REC_PLAY: recommendedPlay(); break;
		case MENU_EXIT: writeRankFile(); exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++) {
		for(i=0;i<WIDTH;i++) {
			field[j][i]=0;
		}
	}
		
	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
    nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2; //새 블록을 게임판 가운데에서 시작
	score=0;	
	gameOver=0;
	timed_out=0;

	recommendX=-100;
	recommendY=-100;
	recommendR=0;

	RecNode root;
	root.level=0;
	root.accumulatedScore=0;
	root.child=NULL;
	root.parent=NULL;

	for(i=0; i<HEIGHT; i++) {
		for(j=0; j<WIDTH; j++) {
			root.recField[i][j] = field[i][j];
		}
	}

	modified_recommend(&root);

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);

	move(8, WIDTH+10);
	DrawBox(9, WIDTH+10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);  
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;  //아래로 이동
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++; //오른쪽 이동
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--; //왼쪽 이동
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1); //위쪽 테두리 및 왼쪽 테두리를 넘어 표현해야해서
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}

    for( i = 0; i < 4; i++ ){
		move(10+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i, j;
	int drawBlockY, drawBlockX;

	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			if(block[blockID][blockRotate][i][j] == 1) {
				drawBlockY = y + i;
				drawBlockX = x + j;

				if(drawBlockY >= 0 && drawBlockY < HEIGHT && drawBlockX >= 0 && drawBlockX < WIDTH) {
					move(drawBlockY + 1, drawBlockX + 1);
					attron(A_REVERSE);
					printw("%c", tile);
					attroff(A_REVERSE);
				}
			}
		}
	}

	move(HEIGHT, WIDTH + 10); //커서를 게임판 바깥으로 치우기
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();  //사용자가 키를 하나 누를 때까지 기다림

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			if(block[currentBlock][blockRotate][i][j] == 1) {
				int newblockY=blockY + i;
				int newblockX=blockX + j;
				if (newblockY >= HEIGHT || newblockX < 0 || newblockX >= WIDTH) {
    				return 0;
				}

				if (newblockY >= 0 && f[newblockY][newblockX] == 1) {
   					return 0;
				}
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	
	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다. 

}

void BlockDown(int sig){
	// user code
	if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX) ==1 ) {
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}

	else {	
		if(blockY == -1) {
			gameOver = 1;
			return;
		}

		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		score += DeleteLine(field);

		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand() % 7;

		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH / 2 - 2;

		RecNode root;

		root.level = 0;
		root.accumulatedScore = 0;
		root.child = NULL;
		root.parent = NULL;

		for(int i = 0; i < HEIGHT; i++) {
			for(int j = 0; j < WIDTH; j++) {
				root.recField[i][j] = field[i][j];
    		}
		}

		modified_recommend(&root);

		DrawNextBlock(nextBlock);
		DrawField();
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
		PrintScore(score);
	}
	
	timed_out = 0;
	//강의자료 p26-27의 플로우차트를 참고한다.
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code

	int touched=0;

	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			if(block[currentBlock][blockRotate][i][j] == 1) {
				int newblockY=blockY + i;
				int newblockX=blockX + j;

				if(newblockY < 0) {
					continue;
				}

				if(i==3 || block[currentBlock][blockRotate][i+1][j]==0) {
					if(newblockY==HEIGHT-1 || f[newblockY+1][newblockX] == 1) {
						touched++;
					}

				}

			}
		}
	}
	
	 for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(block[currentBlock][blockRotate][i][j] == 1) {
                int newblockY = blockY + i;
                int newblockX = blockX + j;

				if(newblockY>=0) {
					f[newblockY][newblockX] = 1;
				}
            }
        }
    }

	return touched * 10;

	//Block이 추가된 영역의 필드값을 바꾼다.
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int deletedLines = 0;
	for(int i=HEIGHT-1; i>=0; i--) {
		int fullFlag = 1;

		for(int j=0; j<WIDTH; j++) {
			if(f[i][j]==0) {
				fullFlag = 0;
				break;
			}
		}
		if(fullFlag==1) {
			deletedLines++;
			for(int reduceY=i; reduceY>0; reduceY--) {
				for(int reduceX=0; reduceX<WIDTH; reduceX++) {
					f[reduceY][reduceX] = f[reduceY-1][reduceX];
				}
			}
			for (int reduceX=0; reduceX<WIDTH; reduceX++) {
				f[0][reduceX] = 0;
			}
			i++;

		}
	}
	return deletedLines*deletedLines*100;

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	while(CheckToMove(field, blockID, blockRotate, y+1, x) == 1) {
		y++;
	}

	DrawBlock(y, x, blockID, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {

	if(recommendX != -100) {
		DrawRecommend(recommendY, recommendX, blockID, recommendR);
	}

	DrawShadow(y, x, blockID, blockRotate);

	DrawBlock(y, x, blockID, blockRotate, ' ');
	
}	

void createRankList(){
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE *fp;
	int i;
	char str[NAMELEN+1];
	int rankScore;
	Node *rankNew;
	Node *rankLast = NULL;
	
	//1. 파일 열기
	fp = fopen("rank.txt", "r");

	if(fp==NULL) {
		score_number = 0;
    	rankHead = NULL;
		return;
	}

	// 2. 정보읽어오기
	/* int fscanf(FILE* stream, const char* format, ...);
	stream:데이터를 읽어올 스트림의 FILE 객체를 가리키는 파일포인터
	format: 형식지정자 등등
	변수의 주소: 포인터
	return: 성공할 경우, fscanf 함수는 읽어들인 데이터의 수를 리턴, 실패하면 EOF리턴 */
	// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문

	if (fscanf(fp, "%d", &score_number) != EOF) {
		rankHead=NULL;

		for(i=0; i<score_number; i++) {
			fscanf(fp, "%s %d", str, &rankScore);

			rankNew=(Node*)malloc(sizeof(Node));
			strcpy(rankNew->name,str); //배열끼리는 =으로 복사불가
			rankNew->score=rankScore;
			rankNew->next=NULL;

			if(rankHead == NULL) {
				rankHead = rankNew;
				rankLast = rankNew;
			}
		
			else {
				rankLast->next = rankNew;
				rankLast=rankNew;
			}
		
		}


	}
	else {
		score_number = 0;
		rankHead=NULL;

	}
	// 4. 파일닫기
	fclose(fp);
}

void rank(){
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화
	int X=1, Y=score_number, ch;
	clear();

	//2. printw()로 3개의 메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	//3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);

	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (ch == '1') {
		int count = 0;
		Node *current = rankHead;
		
		echo();
		printw("X: ");
		scanw("%d", &X);
		printw("Y: ");
		scanw("%d", &Y);
		noecho();

		if (X > Y || X < 1 || Y > score_number) {
			printw("search failure: no rank in the list\n");
		}
		else {
			printw("%-14s | %10s\n", "name", "score");
			printw("---------------------------\n");
			while( current != NULL ) {
				count++;
				if(count >= X && count <= Y) {	
					printw("%-14s | %10d\n", current->name, current->score);
				}

				else if(count > Y) {
					break;
				}
				current=current->next;

			}
			
		}
	}

	//4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if ( ch == '2') {
		char str[NAMELEN+1];
		int check = 0;
        Node *current = rankHead;
        echo();
        printw("input the name: ");
        scanw("%s", str);
        noecho();
        printw("%-14s | %10s\n", "name", "score");
		printw("---------------------------\n");
        while(current!=NULL) {
            if(strcmp(current->name,str)==0) {
                printw("%-14s | %10d\n", current->name, current->score);
                check++;
            }
            current=current->next;
        }
        if(check==0) {
            printw("\nsearch failure: no name in the list");
        }


	}

	//4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if ( ch == '3') {
		int num, count;
        Node *current=rankHead;
        Node *rankPrev=NULL;
        echo();
        printw("input the rank: ");
        scanw("%d\n", &num);
        noecho();
        if(score_number>=num && num>=1) {
            if(num==1) {
                rankHead=rankHead->next;
                free(current);
            }
            else {
                count=1;
                while(count<num) {
                rankPrev=current;
                current=current->next;
                count++;
                }
                rankPrev->next=current->next;
                free(current);
            }
            score_number--;
            printw("result: the rank deleted\n");
            writeRankFile();

        }
        else {
            printw("search failure: the rank not in the list");
        }
	}
	getch();

}

void writeRankFile(){
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	int sn, i;
	Node *current;
	//1. "rank.txt" 연다
	FILE *fp = fopen("rank.txt", "w");

	if(fp == NULL) {
		return;
	}
 
	//2. 랭킹 정보들의 수를 "rank.txt"에 기록
	fprintf(fp, "%d\n", score_number);
	//3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료
	
	current = rankHead;	
	
	while(current != NULL) {
		fprintf(fp, "%s %d\n", current->name, current->score);
		current = current->next;
	}

	fclose(fp);
}

void newRank(int score){
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char str[NAMELEN+1];
	int i, j;
	Node* rankNew;
	Node* rankPrev = NULL;
	Node* current = rankHead; 

	clear();
	//1. 사용자 이름을 입력받음
	echo();
	printw("your name: ");
	scanw("%s", str);
	noecho();

	//2. 새로운 노드를 생성해 이름과 점수를 저장, score_number가
	rankNew=(Node*)malloc(sizeof(Node));
	strcpy(rankNew->name,str);
	rankNew->score = score;
	rankNew->next = NULL;

	while(current!=NULL && current->score >= score) {
		rankPrev = current;
		current = current->next;
	}
	if(rankPrev==NULL) { //현재 score가 리스트의 1위점수보다 높거나, 리스트가 빈 경우
		rankNew->next = rankHead;
		rankHead=rankNew;
	}
	else {
		rankNew->next = current;
		rankPrev->next = rankNew;
	}
	
	score_number++;
	writeRankFile();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
	if(CheckToMove(field, blockID, blockRotate, y, x) == 1) {
		DrawBlock(y, x, blockID, blockRotate, 'R');
	}
}

int GetRotationCount(int blockID) {

	if(blockID == 4) {
		return 1;
	}
	else if(blockID == 0 || blockID == 5 || blockID == 6) {
		return 2;
	}
	else {
		return 4;
	}
}

void GetFieldInfo(char f[HEIGHT][WIDTH], int *maxHeight, int *holes, int *bumpiness) {

	int height[WIDTH];

	*maxHeight = 0;
	*holes = 0;
	*bumpiness = 0;

	for(int x = 0; x < WIDTH; x++) {
		int firstBlock = -1;

		for(int y = 0; y < HEIGHT; y++) {
			if(f[y][x] == 1) {
				firstBlock = y;
				break;
			}
		}

		if(firstBlock == -1) {
			height[x] = 0;
		}
		else {
			height[x] = HEIGHT - firstBlock;

			for(int y = firstBlock + 1; y < HEIGHT; y++) {
				if(f[y][x] == 0) {
					(*holes)++;
				}
			}
		}

		if(height[x] > *maxHeight) {
			*maxHeight = height[x];
		}
	}

	for(int x = 0; x < WIDTH - 1; x++) {
		int diff = height[x] - height[x + 1];

		if(diff < 0) {
			diff = -diff;
		}

		*bumpiness += diff;
	}
}

int recommend(RecNode *root){
	
    // user code
	int max = -1;
	int blockID;
	int r, x, y;
	int point, resultScore=0;
	RecNode *child;

	if(root->level == 0) {
		recommendX = -100;
		recommendY = -100;
		recommendR = 0;
	}

	root->recBlockX = -1;
	root->recBlockY = -1;
	root->recBlockRotate = -1;

	if(root->level >= BLOCK_NUM) {
		return root->accumulatedScore;
	}

	blockID = nextBlock[root->level];

	for(r = 0; r < 4; r++) {
		for(x = -2; x < WIDTH + 2; x++) {
			y = -1;

			if(CheckToMove(root->recField, blockID, r, y, x) == 0) {
				continue;
			}
			
			while(CheckToMove(root->recField, blockID, r, y + 1, x) == 1) {
				y++;
			}

			child = (RecNode *)malloc(sizeof(RecNode));
			if(child == NULL)
				return max;

			child->level = root->level + 1;
			child->accumulatedScore = root->accumulatedScore;
			child->curBlockID = blockID;

			child->recBlockX = x;
			child->recBlockY = y;
			child->recBlockRotate = r;

			child->parent = root;
			child->child = NULL;

			for(int i = 0; i < HEIGHT; i++) {
				for(int j = 0; j < WIDTH; j++) {
					child->recField[i][j] = root->recField[i][j];
				}
			}

			point = AddBlockToField(child->recField, blockID, r, y, x);
			point += DeleteLine(child->recField);

			child->accumulatedScore += point;

			if(child->level < BLOCK_NUM) {
				resultScore = recommend(child);

				if(resultScore < 0) {
					resultScore = child->accumulatedScore;
				}
			}
			else {
				resultScore = child->accumulatedScore;
			}

			if(resultScore > max) {
				max = resultScore;
				root->recBlockX = x;
				root->recBlockY = y;
				root->recBlockRotate = r;

				if(root->level == 0) {
					recommendX = x;
					recommendY = y;
					recommendR = r;
				}
			}

			free(child);
		}
	}

	return max;
}

int modified_recommend(RecNode *root) {

	int max = -1000000;
	int blockID = nextBlock[0];
	int rotateLimit = GetRotationCount(blockID);

	recommendX = -100;
	recommendY = -100;
	recommendR = 0;

	root->recBlockX = -1;
	root->recBlockY = -1;
	root->recBlockRotate = -1;

	for(int r = 0; r < rotateLimit; r++) {
		for(int x = -2; x < WIDTH + 2; x++) {
			int y = -1;

			if(CheckToMove(root->recField, blockID, r, y, x) == 0) {
				continue;
			}

			while(CheckToMove(root->recField, blockID, r, y + 1, x) == 1) {
				y++;
			}

			char tempField[HEIGHT][WIDTH];

			for(int i = 0; i < HEIGHT; i++) {
				for(int j = 0; j < WIDTH; j++) {
					tempField[i][j] = root->recField[i][j];
				}
			}

			int point = AddBlockToField(tempField, blockID, r, y, x);
			point += DeleteLine(tempField);

			int maxHeight;
			int holes;
			int bumpiness;

			GetFieldInfo(tempField, &maxHeight, &holes, &bumpiness);

			int value = point;
			value -= holes * 50;
			value -= maxHeight * 3;
			value -= bumpiness * 5;

			if(holes >= 6) {
				value -= 500;
			}

			if(maxHeight >= HEIGHT - 3) {
				value -= 700;
			}

			if(value > max) {
				max = value;

				root->recBlockX = x;
				root->recBlockY = y;
				root->recBlockRotate = r;

				recommendX = x;
				recommendY = y;
				recommendR = r;
			}
		}
	}

	return max;
}

long evalSize() {
	return sizeof(char) * HEIGHT * WIDTH + sizeof(int) * WIDTH + sizeof(int)* 3;
}

void recommendedPlay(){
	// user code

	time_t playStart;
	time_t playStop;
	clock_t recommandStart;
	clock_t recommandStop;
	double playDuration=0, recommandDuration=0;

	playStart=time(NULL);
	clear();
	InitTetris();
	nodelay(stdscr, TRUE);
	
	while(!gameOver){
		int command = wgetch(stdscr);

		if(command == 'q' || command == 'Q'){
			nodelay(stdscr, FALSE);
    		DrawBox(HEIGHT/2-1, WIDTH/2-5, 1, 10);
    		move(HEIGHT/2, WIDTH/2-4);
    		printw("Good-bye!!");
    		refresh();
    		getch();

    		return;
		}

		RecNode root;
		root.level = 0;
		root.accumulatedScore = 0;
		root.child=NULL;
		root.parent=NULL;
		for(int i = 0; i < HEIGHT; i++) {
			for(int j = 0; j < WIDTH; j++) {
				root.recField[i][j] = field[i][j];
			}
		}

		recommandStart=clock();
		modified_recommend(&root);
		recommandStop=clock();
		recommandDuration+=(double)(recommandStop-recommandStart)/CLOCKS_PER_SEC;

		blockRotate=0;
		blockY=-1;
		blockX=WIDTH/2-2;

		DrawField();
        DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
        DrawNextBlock(nextBlock);
        PrintScore(score);
        refresh();

		usleep(200000);

		blockRotate = recommendR;
        blockX = recommendX;
        blockY = recommendY;

		if(recommendX == -100 || CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX) == 0) {
            gameOver = 1;
            break;
        }

		DrawField();
        DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
        DrawNextBlock(nextBlock);
        PrintScore(score);
        refresh();

		usleep(200000);

		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
        score += DeleteLine(field);

		nextBlock[0] = nextBlock[1];
        nextBlock[1] = nextBlock[2];
        nextBlock[2] = rand() % 7;

		blockRotate = 0;
        blockY = -1;
        blockX = WIDTH / 2 - 2;

	}

	nodelay(stdscr, FALSE);
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	playStop=time(NULL);
	playDuration=(double)difftime(playStop, playStart);

	clear();
	DrawBox(3, 5, 8, 60);

	move(4, 7);
	printw("The total time of the play: %.6f seconds", playDuration);
	move(5, 7);
	printw("The total score of the play: %d", score);
	move(6, 7);
	printw("The total time of the recommandation: %.6f seconds", recommandDuration);
	move(7, 7);
	printw("The total space of the play: %ld bytes", evalSize());
	move(8, 7);
	printw("The total time efficiency: %.6f", score/recommandDuration);
	move(9, 7);
	printw("The total space efficiency: %.6f", (double)score/evalSize());

	refresh();
	getch();

	newRank(score);	
	
	
}
