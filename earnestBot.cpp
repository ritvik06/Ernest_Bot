#include <bits/stdc++.h>
using namespace std;
#define pii pair<int,int>
#define mp make_pair
#define inf 1000000000
#define fi first
#define sec second

/*
0--empty block
1--white soldier
2--black soldier
3--white town
4--black town
*/
/*
move -- S a b M c d || S a b B c d
*/
int empBlock=0, whiteSol=1, blackSol=2, whiteTown=3, blackTown=4;
int solCol,oppCol,oppTownCol,townCol;
int maxDepth=6;/*MiniMax made for maxDepth Even*/
int timelimit;	
int numofCannons

struct node
{
	vector<vector<int> > board;
	double score;
	vector<node> child;
	pair<pii,pii> changeCoordi;
	bool isCannonMove;
};


// if the cell belongs to the board
bool boardCell(int i, int j, int n, int m)
{
	if(i>=0 && j>=0 && i<n && j<m) return true;
	return false;
}

//if there exists a soldier at (i,j)
bool isSoldier(int i, int j, int n, int m, int oppCol, int oppTownCol, vector<vector<int> > &board)
{
	if(i>=0 && i<n && j>=0 && j<m)
	{
		if(board[i][j]==oppCol || board[i][j]==oppTownCol)
			return true;
	}
	return true;
}

//if at (i,j) there is an opponent
bool isOpponent(vector<vector<int> > &board,int i,int j,int oppCol)
{
	return (board[i][j]==oppCol);
}

vector<vector<int> > initializeBoard(int n,int m)
{
	vector<vector<int> > Board(n, vector<int>(m,0));

	for(int i=0;i<4;i++){
		Board[0][2*i] = 3;
		Board[n-1][2*i + 1] = 4;
		for(int j=0;j<3:j++){
			Board[j][2*i + 1] = 1;
			Board[n-1-j][2*i] = 2;
		}

	}

	return Board;

}

//represents the next state of the board after a move
vector<vector<int> > changeBoard(vector<vector<int> > board, int a, int b, int c, int d,int cannonMove)
{
	if(cannonMove)
	{
		board[c][d]=empBlock;
		return board;
	}
	board[c][d]=board[a][b];
	board[a][b]=empBlock;
	return board;
}

//checks basic requirements of being a cannon, not the actual constraint of adjacency
bool isCannon(vector<vector<int> > &board,int a1,int b1,int a2,int b2,int a3,int b3,int n, int m,int col)
{
	if(!(boardCell(a1,b1,n,m) && boardCell(a2,b2,n,m) && boardCell(a3,b3,n,m)))
		return false;

	if(!(board[a1][b1]==col && board[a2][b2]==col && board[a3][b3]==col))
		return false;

	return true;
}

pair<int,int> count_towns(vector<vector<int> > &board, int townCol,int n,int m)
{
	int my_count=0,opp_count=0;

		for(int i=0;i<4;i++){
			if(townCol==3){
				if(board[0][2*i]==3) count++;
				if(board[n-1][2*i+1]==4) opp_count++;

			}
			else if(townCol==4){
				if(board[n-1][2*i+1]==4) count++;	
				if(board[0][2*i]==3) opp_count++;
			}
		}

	return mp(count,opp_count);	
}

bool isGameOver(vector<vector<int> > &board,int townCol, int n, int m)
{
	pair<int, int> counts = count_towns(board,townCol,n,m);
	int my_count = counts.fi;
	int opp_count = counts.sec;

	if(my_count<=2 || opp_count<=2){
		return true;
	}

	return false;
}

bool isWinner(vector<vector<int> > &board,int townCol, int n, int m)
{
	pair<int, int> counts = count_towns(board,townCol,n,m);
	int my_count = counts.fi;
	int opp_count = counts.sec;

	if(my_count==2) return false;
	else return true;

	return true;
}

float town_hall_score(int myHalls,int oppHalls)
{
	if(myHalls==4){
		if(oppHalls==2) return 10;
		else if(oppHalls==3) return 7;
		else if (oppHalls==4) return 5;
	}
	else if(myHalls==3){
		if(oppHalls==2) return 8;
		else if(oppHalls==3) return 5;
		else if (oppHalls==4) return 3;		
	}

	else if(myHalls==2){
		else if(oppHalls==3) return 2;
		else if (oppHalls==4) return 0;
	}

	return 0;

}

pair<float,float> finalScore(vector<vector<int> > &board, int solCol)
{
	int mySol=0,oppSol=0;

	for(int i=0;i<n;i++){
		for(int j=0;j<m;j++){
			if(isSoldier(i,j,board.size(),board[0].size(),solCol,solCol+2,board)) mySol++;
			else if (isSoldier(i,j,board.size(),board[0].size(),solCol,solCol+2,board)) oppSol++;
		}
	}

	float mySolCount = mySol/100;
	float oppSolCount = oppSol/100;

	pair<int, int> counts = count_towns(board,solCol+2,board.size(),board[0].size())
	int my_count = counts.fi;
	int opp_count = counts.sec;

	myScore = (town_hall_score(my_count,opp_count)) + mySolCount;
	OppScore = (town_hall_score(opp_count,my_count)) + oppSolCount;


	return (mp(myScore,OppScore));
}

/*evaluation function consists of next move attacks :-
1. No of my team soldiers
2. No of my team Cannon's that can attack
3. No of attacks on my team by opponent
4. No of townhalls my team can attack
5. No of my townhalls under attack
*/

double eval(vector<vector<int> > &board, int solCol, int townCol, int oppCol, int oppTownCol)
{
	int n=board.size(),m=board[0].size();
	int fmove,bmove;

	if(solCol==white)
		fmove=1,bmove=-2;
	else
		fmove=-1,bmove=2;

	int numOfSol=0,posCannonAttk=0,posOppAttk=0,posOppAttkOnTH=0,posAttkOnTH;

	for(int i=0;i<n;i++)
	{
		for(int j=0;j<m;j++)
		{
			if(board[i][j]==solCol)
				numOfSol++;

			if(board[i][j]==townCol)
			{
				if(boardCell(i+fmove,j,n,m) && isOpponent(board,i+fmove,j,oppCol)) posOppAttkOnTH++;
				if(boardCell(i+fmove,j+1,n,m) && isOpponent(board,i+fmove,j+1,oppCol)) posOppAttkOnTH++;
				if(boardCell(i+fmove,j-1,n,m) && isOpponent(board,i+fmove,j-1,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+2*fmove,j,i+3*fmove,j,i+4*fmove,j,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+2*fmove,j-2,i+3*fmove,j-3,i+4*fmove,j-4,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+2*fmove,j+2,i+3*fmove,j+3,i+4*fmove,j+4,oppCol)) posOppAttkOnTH++;
			}

			if(board[i][j]==oppTownCol)
			{
				if(boardCell(i-fmove,j,n,m) && isOpponent(board,i-fmove,j,solCol)) posAttkOnTH++;
				if(boardCell(i-fmove,j+1,n,m) && isOpponent(board,i-fmove,j+1,solCol)) posAttkOnTH++;
				if(boardCell(i-fmove,j-1,n,m) && isOpponent(board,i-fmove,j-1,solCol)) posAttkOnTH++;
				if(isCannon(board,i-2*fmove,j,i-3*fmove,j,i-4*fmove,j,solCol)) posOppAttkOnTH++;
				if(isCannon(board,i-2*fmove,j-2,i-3*fmove,j-3,i-4*fmove,j-4,solCol)) posOppAttkOnTH++;
				if(isCannon(board,i-2*fmove,j+2,i-3*fmove,j+3,i-4*fmove,j+4,solCol)) posOppAttkOnTH++;
			}

			if(board[i][j]==solCol)
			{
				if(boardCell(i+fmove,j,n,m) && isOpponent(board,i+fmove,j,oppCol)) posOppAttk++;
				if(boardCell(i+fmove,j+1,n,m) && isOpponent(board,i+fmove,j+1,oppCol)) posOppAttk++;
				if(boardCell(i+fmove,j-1,n,m) && isOpponent(board,i+fmove,j-1,oppCol)) posOppAttk++;
				if(boardCell(i,j+1,n,m) && isOpponent(board,i,j+1,oppCol)) posOppAttk++;
				if(boardCell(i,j-1,n,m) && isOpponent(board,i,j-1,oppCol)) posOppAttk++;
			}
		}
	}

	// Assign weights later
	double func=(numOfSol)-posOppAttk-posOppAttkOnTH+posAttkOnTH;



	return func;
}

void allBranches(vector<node> &child, vector<vector<int> > &board, int solCol, int townCol, int oppCol, int oppTownCol)
{
	int n=board.size(),m=board[0].size();

	int fmove,bmove;

	if(solCol==white)
		fmove=1,bmove=-2;
	else
		fmove=-1,bmove=2;

	for(int i=0;i<n;i++)
	{
		for(int j=0;j<m;j++)
		{
			if(board[i][j]==solCol)
			{
				node temp;
				/*Forward Moves*/
				if(boardCell(i+fmove, j, n, m))
				{
					temp.board=changeBoard(board,i,j,i+fmove,j,false);
					temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
					temp.changeCoordi=mp(mp(i,j),mp(i+fmove,j));
					temp.isCannonMove=false;
					child.pb(temp);
				}
				if(boardCell(i+fmove,j+1,n,m))
				{
					temp.board=changeBoard(board,i,j,i+fmove,j+1,false);
					temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
					temp.changeCoordi=mp(mp(i,j),mp(i+fmove,j+1));
					temp.isCannonMove=false;
					child.pb(temp);
				}
				if(boardCell(i+fmove,j-1,n,m))
				{
					temp.board=changeBoard(board,i,j,i+fmove,j-1,false);
					temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
					temp.changeCoordi=mp(mp(i,j),mp(i+fmove,j-1));
					temp.isCannonMove=false;
					child.pb(temp);
				}

				/*Sideways Capture Moves*/
				bool isSideMovePossible=false;
				if(isSoldier(i,j+1,n,m,oppCol,oppTownCol,board))
				{
					temp.board=changeBoard(board,i,j,i,j+1,false);
					temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
					temp.changeCoordi=mp(mp(i,j),mp(i,j+1));
					temp.isCannonMove=false;
					child.pb(temp);
					isSideMovePossible=true;
				}
				if(isSoldier(i,j-1,n,m,oppCol,oppTownCol,board))
				{
					temp.board=changeBoard(board,i,j,i,j-1,false);
					temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
					temp.changeCoordi=mp(mp(i,j),mp(i,j-1));
					temp.isCannonMove=false;
					child.pb(temp);
					isSideMovePossible=true;
				}

				/*Backward Moves*/
				if(isSideMovePossible)
				{
					if(boardCell(i+bmove,j,n,m))
					{
						temp.board=changeBoard(board,i,j,i+bmove,j,false);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.changeCoordi=mp(mp(i,j),mp(i+bmove,j));
						temp.isCannonMove=false;
						child.pb(temp);
					}
					if(boardCell(i+bmove,j-2,n,m))
					{
						temp.board=changeBoard(board,i,j,i+bmove,j-2,false);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.changeCoordi=mp(mp(i,j),mp(i+bmove,j-2));
						temp.isCannonMove=false;
						child.pb(temp);
					}
					if(boardCell(i+bmove,j+2,n,m))
					{
						temp.board=changeBoard(board,i,j,i+bmove,j+2,false);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.changeCoordi=mp(mp(i,j),mp(i+bmove,j+2));
						temp.isCannonMove=false;
						child.pb(temp);
					}
				}

				/*Cannon Moves*/
				/*Orthogonal Cannon*/
				if(isCannon(board,i,j,i+fmove,j,i+2*fmove,j,n,m,solCol))
				{
					if(boardCell(i+4*fmove,j,n,m))
					{
						temp.board=changeBoard(board,i,j,i+4*fmove,j,true);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j),mp(i+4*fmove,j));
						child.pb(temp);
					}
					if(boardCell(i+5*fmove,j,n,m))
					{
						temp.board=changeBoard(board,i,j,i+5*fmove,j,true);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j),mp(i+5*fmove,j));
						child.pb(temp);
					}
					if(boardCell(i-2*fmove,j,n,m))
					{
						temp.board=changeBoard(board,i,j,i-2*fmove,j,true);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-2*fmove,j));
						child.pb(temp);
					}
					if(boardCell(i-3*fmove,j,n,m))
					{
						temp.board=changeBoard(board,i,j,i-3*fmove,j,true);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-3*fmove,j));
						child.pb(temp);
					}
					if(boardCell(i-fmove,j,n,m))
					{
						temp.board=changeBoard(board,i+2*fmove,j,i-fmove,j,false);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=false;
						temp.changeCoordi=mp(mp(i+2*fmove,j),mp(i-fmove,j));
						child.pb(temp);
					}
					if(boardCell(i+3*fmove,j,n,m))
					{
						temp.board=changeBoard(board,i,j,i+3*fmove,j,false);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=false;
						temp.changeCoordi=mp(mp(i,j),mp(i+3*fmove,j));
						child.pb(temp);
					}
				}

				/*Right Diagonal Cannon*/
				if(isCannon(board,i,j,i+fmove,j+1,i+2*fmove,j+2,n,m,solCol))
				{
					if(boardCell(i+4*fmove,j+4,n,m))
					{
						temp.board=changeBoard(board,i,j,i+4*fmove,j+4,true);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j+2),mp(i+4*fmove,j+4));
						child.pb(temp);
					}
					if(boardCell(i+5*fmove,j+5,n,m))
					{
						temp.board=changeBoard(board,i,j,i+5*fmove,j+5,true);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j+2),mp(i+5*fmove,j+5));
						child.pb(temp);
					}
					if(boardCell(i-2*fmove,j-2,n,m))
					{
						temp.board=changeBoard(board,i,j,i-2*fmove,j-2,true);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-2*fmove,j-2));
						child.pb(temp);
					}
					if(boardCell(i-3*fmove,j-3,n,m))
					{
						temp.board=changeBoard(board,i,j,i-3*fmove,j-3,true);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-3*fmove,j-3));
						child.pb(temp);
					}
					if(boardCell(i-fmove,j-1,n,m))
					{
						temp.board=changeBoard(board,i+2*fmove,j+2,i-fmove,j-1,false);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=false;
						temp.changeCoordi=mp(mp(i+2*fmove,j+2),mp(i-fmove,j-1));
						child.pb(temp);
					}
					if(boardCell(i+3*fmove,j+3,n,m))
					{
						temp.board=changeBoard(board,i,j,i+3*fmove,j+3,false);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=false;
						temp.changeCoordi=mp(mp(i,j),mp(i+3*fmove,j+3));
						child.pb(temp);
					}
				}

				/*Left Diagonal Cannon*/
				if(isCannon(board,i,j,i+fmove,j-1,i+2*fmove,j-2,n,m,solCol))
				{
					if(boardCell(i+4*fmove,j-4,n,m))
					{
						temp.board=changeBoard(board,i,j,i+4*fmove,j-4,true);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j-2),mp(i+4*fmove,j-4));
						child.pb(temp);
					}
					if(boardCell(i+5*fmove,j-5,n,m))
					{
						temp.board=changeBoard(board,i,j,i+5*fmove,j-5,true);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j-2),mp(i+5*fmove,j-5));
						child.pb(temp);
					}
					if(boardCell(i-2*fmove,j+2,n,m))
					{
						temp.board=changeBoard(board,i,j,i-2*fmove,j+2,true);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-2*fmove,j+2));
						child.pb(temp);
					}
					if(boardCell(i-3*fmove,j+3,n,m))
					{
						temp.board=changeBoard(board,i,j,i-3*fmove,j+3,true);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-3*fmove,j+3));
						child.pb(temp);
					}
					if(boardCell(i-fmove,j+1,n,m))
					{
						temp.board=changeBoard(board,i+2*fmove,j-2,i-fmove,j+1,false);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=false;
						temp.changeCoordi=mp(mp(i+2*fmove,j-2),mp(i-fmove,j+1));
						child.pb(temp);
					}
					if(boardCell(i+3*fmove,j-3,n,m))
					{
						temp.board=changeBoard(board,i,j,i+3*fmove,j-3,false);
						temp.score=eval(temp.board,solCol,townCol,oppCol,oppTownCol);
						temp.isCannonMove=false;
						temp.changeCoordi=mp(mp(i,j),mp(i+3*fmove,j-3));
						child.pb(temp);
					}
				}
			}
		}
	}
}

void makeTree(node &root, int depth, int solCol, int townCol, int oppCol, int oppTownCol)
{
	if(depth>=maxDepth)
		return;

	root.child=allBranches(child, root.board,solCol,townCol,oppCol,oppTownCol);

	for(int i=0;i<root.child.size();i++)
	{
		makeTree(root.child[i],depth+1,solCol,townCol,oppCol,oppTownCol);
	}
}

pair<double,pair<pii,pii> > miniMaxWithAlphaBetaPruning(node &root, int depth,int alpha, int beta)
{
	if(depth==maxDepth)
		return root.eval;

	if(depth&1)
	{
		node newBoard;
		for(int i=0;i<root.child.size();i++)
		{
			double temp = miniMaxWithAlphaBetaPruning(root.child[i],depth+1,alpha,beta).fi;

			if(temp<=alpha)
				return mp(temp,root.child[i].changeCoordi);
			if(temp<beta)
			{
				beta=temp;
				newBoard=root.child[i];
			}
		}

		return mp(beta,newBoard.changeCoordi);
	}
	else
	{
		node newBoard;
		for(int i=0;i<root.child.size();i++)
		{
			double temp = miniMaxWithAlphaBetaPruning(root.child[i],depth+1,alpha,beta).fi;

			if(temp>=beta)
				return mp(temp,root.child[i].changeCoordi);
			if(temp>alpha)
			{
				alpha=temp;
				newBoard=root.child[i];
			}
		}

		return mp(beta,newBoard.changeCoordi);
	}
}

string ErnestMove(vector<vector<int> > &board, int solCol, int townCol, int oppCol, int oppTownCol)
{
	node root;
	root.board=board;
	
	makeTree(root,0,solCol,townCol,oppCol,oppTownCol);

	pair<pii,pii> move=miniMaxWithAlphaBetaPruning(root,0,-inf,inf);


	return ("S "+to_string(move.fi.fi)+ " "+ to_string(move.fi.sec)+" M "+to_string(move.sec.fi)+" "+to_string(move.sec.sec));
}

int main(){
	//start client
	int n,m; 
	vector<vector<int> > Board;

	cin >> player_id >> n >> m >> timelimit;

	if(player_id==1){
		solCol = 1;
		townCol = 3;
		oppCol =2;
		oppTownCol =4;
	}
	else{
		solCol = 2;
		townCol = 4;
		oppCol =1;
		oppTownCol =3;			
	}

	Board = initializeBoard(n,m);

	if(player_id==0)
		string str = ErnestMove(Board,solCol,townCol,oppCol,oppTownCol) << endl;
		cerr << str;
		Board = changeBoard(Board,stoi(str[2]),stoi(str[4]),stoi(str[8]),stoi(str[10]),0)

	while(true)
	{
		getline(cin,str)

		Board = changeBoard(Board,stoi(str[2]),stoi(str[4]),stoi(str[8]),stoi(str[10]),0)

		if(isGameOver(Board,int townCol, int n, int m))	{
			pair<float,float> scores = finalScore(board,solCol);
			cout << "My score " << scores.fi << endl;
			cout << "opponents score " << scores.sec << endl;
			break;
		}

		str = ErnestMove(Board,solCol,townCol,oppCol,oppTownCol) << endl;
		cerr << str;
		Board = changeBoard(Board,stoi(str[2]),stoi(str[4]),stoi(str[8]),stoi(str[10]),0)		

		if(isGameOver(Board,int townCol, int n, int m))	{
			pair<float,float> sccores = finalScore(board,solCol);
			cout << "My score " << scores.fi << endl;
			cout << "opponents score " << scores.sec << endl;
			break;
		}

	}

	return 0;
}