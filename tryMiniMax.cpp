#include <bits/stdc++.h>
using namespace std;
#define pii pair<int,int>
#define mp make_pair
#define inf 1000000000
#define fi first
#define sec second
#define pb push_back
#define LOC 4

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
int maxDepthTree=3;/*MiniMax made for maxDepth*/
int timelimit;	
int numofCannons;
int cnt=0;

struct node
{
	vector<vector<int> > board;
	long long score;
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
	return false;
}

//if at (i,j) there is an opponent
bool isColorSoldier(vector<vector<int> > &board,int i,int j,int col1,int col2)
{
	return (board[i][j]==col1 || board[i][j]==col2);
}

vector<vector<int> > initializeBoard(int n,int m)
{
	vector<vector<int> > Board(n, vector<int>(m,0));

	for(int i=0;i<4;i++){
		Board[0][2*i] = 3;
		Board[n-1][2*i + 1] = 4;
		for(int j=0;j<3;j++){
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
bool isCannon(vector<vector<int> > &board,int a1,int b1,int a2,int b2,int a3,int b3,int col)
{
	int n=board.size(),m=board[0].size();
	if(!(boardCell(a1,b1,n,m) && boardCell(a2,b2,n,m) && boardCell(a3,b3,n,m)))
		return false;

	if(!(board[a1][b1]==col && board[a2][b2]==col && board[a3][b3]==col))
		return false;

	return true;
}

bool retLoc(int i,int row, int col)
{
	if(col==whiteSol && i>=row)
	{
		return true;
	}
	else if(col==blackSol && i<=row)
	{
		return true;
	}

	return false;
}

/*evaluation function consists of next move attacks :-
1. No of my team soldiers
2. No of my team Cannon's that can attack
3. No of attacks on my team by opponent
4. No of townhalls my team can attack
5. No of my townhalls under attack
*/

int isSuicide(vector<vector<int> > &board, int i, int j, int col)
{
	int n=board.size(),m=board[0].size();
	int fmove,bmove;

	if(col==whiteSol)
		fmove=1,bmove=-2;
	else
		fmove=-1,bmove=2;

	int attk=0;
	if(boardCell(i+fmove,j,n,m) && isColorSoldier(board,i+fmove,j,3-col,3-col)) attk++;
	if(boardCell(i+fmove,j+1,n,m) && isColorSoldier(board,i+fmove,j+1,3-col,3-col)) attk++;
	if(boardCell(i+fmove,j-1,n,m) && isColorSoldier(board,i+fmove,j-1,3-col,3-col)) attk++;
	if(boardCell(i,j+1,n,m) && isColorSoldier(board,i,j+1,3-col,3-col)) attk++;
	if(boardCell(i,j-1,n,m) && isColorSoldier(board,i,j-1,3-col,3-col)) attk++;
	if(isCannon(board,i-2*fmove,j,i-3*fmove,j,i-4*fmove,j,3-col)) attk++;
	if(isCannon(board,i-5*fmove,j,i-3*fmove,j,i-4*fmove,j,3-col)) attk++;
	if(isCannon(board,i-2*fmove,j-2,i-3*fmove,j-3,i-4*fmove,j-4,3-col)) attk++;
	if(isCannon(board,i-5*fmove,j-5,i-3*fmove,j-3,i-4*fmove,j-4,3-col)) attk++;
	if(isCannon(board,i-5*fmove,j+5,i-3*fmove,j+3,i-4*fmove,j+4,3-col)) attk++;
	if(isCannon(board,i-2*fmove,j+2,i-3*fmove,j+3,i-4*fmove,j+4,3-col)) attk++;
	if(isCannon(board,i,j-2,i,j-3,i,j-4,3-col)) attk++;
	if(isCannon(board,i,j-3,i,j-4,i,j-5,3-col)) attk++;
	if(isCannon(board,i,j+2,i,j+3,i,j+4,3-col)) attk++;
	if(isCannon(board,i,j+3,i,j+4,i,j+5,3-col)) attk++;

	return attk;
}

long long eval(vector<vector<int> > &board,int col)
{
	int n=board.size(),m=board[0].size();
	int fmove,bmove,tcol;

	col=solCol;

	if(col==whiteSol)
		fmove=1,bmove=-2,tcol=3;
	else
		fmove=-1,bmove=2,tcol=4;

	int numOfSol=0,posAttk=0,posOppAttk=0,posOppAttkOnTH=0,posAttkOnTH=0,numOfOppSol=0;
	int numOfTH=0,numOfOppTH=0,posCannAttk=0,posOppCannAttk=0,suicide=0,
		posCannAttkTH=0,posOppCannAttkTH=0,killSolWithHC=0;
	int loc=0,locOpp=0;
	// cerr<<"eval calculating..."<<endl;
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<m;j++)
		{
			if(board[i][j]==col)
				numOfSol++;
			if(board[i][j]==3-col)
				numOfOppSol++;
			if(board[i][j]==tcol)
				numOfTH++;
			if(board[i][j]==7-tcol)
				numOfOppTH++;

			if(retLoc(i,LOC,col) && board[i][j]==col) loc++;
			if(retLoc(i,LOC-2*fmove,3-col) && board[i][j]==3-col) locOpp++;

			if(board[i][j]==tcol)
			{
				if(boardCell(i+fmove,j,n,m) && isColorSoldier(board,i+fmove,j,3-col,3-col)) posOppAttkOnTH++;
				if(boardCell(i+fmove,j+1,n,m) && isColorSoldier(board,i+fmove,j+1,3-col,3-col)) posOppAttkOnTH++;
				if(boardCell(i+fmove,j-1,n,m) && isColorSoldier(board,i+fmove,j-1,3-col,3-col)) posOppAttkOnTH++;
				if(isCannon(board,i+2*fmove,j,i+3*fmove,j,i+4*fmove,j,3-col)) posOppCannAttkTH++;
				if(isCannon(board,i+5*fmove,j,i+3*fmove,j,i+4*fmove,j,3-col)) posOppCannAttkTH++;
				if(isCannon(board,i+2*fmove,j-2,i+3*fmove,j-3,i+4*fmove,j-4,3-col)) posOppCannAttkTH++;
				if(isCannon(board,i+5*fmove,j-5,i+3*fmove,j-3,i+4*fmove,j-4,3-col)) posOppCannAttkTH++;
				if(isCannon(board,i+2*fmove,j+2,i+3*fmove,j+3,i+4*fmove,j+4,3-col)) posOppCannAttkTH++;
				if(isCannon(board,i+5*fmove,j+5,i+3*fmove,j+3,i+4*fmove,j+4,3-col)) posOppCannAttkTH++;
			}

			if(board[i][j]==7-tcol)
			{
				if(boardCell(i-fmove,j,n,m) && isColorSoldier(board,i-fmove,j,col,col)) posAttkOnTH++;
				if(boardCell(i-fmove,j+1,n,m) && isColorSoldier(board,i-fmove,j+1,col,col)) posAttkOnTH++;
				if(boardCell(i-fmove,j-1,n,m) && isColorSoldier(board,i-fmove,j-1,col,col)) posAttkOnTH++;
				if(isCannon(board,i-2*fmove,j,i-3*fmove,j,i-4*fmove,j,col)) posCannAttkTH++;
				if(isCannon(board,i-5*fmove,j,i-3*fmove,j,i-4*fmove,j,col)) posCannAttkTH++;
				if(isCannon(board,i-2*fmove,j-2,i-3*fmove,j-3,i-4*fmove,j-4,col)) posCannAttkTH++;
				if(isCannon(board,i-5*fmove,j-5,i-3*fmove,j-3,i-4*fmove,j-4,col)) posCannAttkTH++;
				if(isCannon(board,i-2*fmove,j+2,i-3*fmove,j+3,i-4*fmove,j+4,col)) posCannAttkTH++;
				if(isCannon(board,i-5*fmove,j+5,i-3*fmove,j+3,i-4*fmove,j+4,col)) posCannAttkTH++;
			}

			if(board[i][j]==col)
			{
				if(boardCell(i+fmove,j,n,m) && isColorSoldier(board,i+fmove,j,3-col,3-col)) posAttk++;
				if(boardCell(i+fmove,j+1,n,m) && isColorSoldier(board,i+fmove,j+1,3-col,3-col)) posAttk++;
				if(boardCell(i+fmove,j-1,n,m) && isColorSoldier(board,i+fmove,j-1,3-col,3-col)) posAttk++;
				if(boardCell(i,j+1,n,m) && isColorSoldier(board,i,j+1,3-col,3-col)) posAttk++;
				if(boardCell(i,j-1,n,m) && isColorSoldier(board,i,j-1,3-col,3-col)) posAttk++;
				if(isCannon(board,i+2*fmove,j,i+3*fmove,j,i+4*fmove,j,3-col)) posOppCannAttk++;
				if(isCannon(board,i+5*fmove,j,i+3*fmove,j,i+4*fmove,j,3-col)) posOppCannAttk++;
				if(isCannon(board,i+2*fmove,j-2,i+3*fmove,j-3,i+4*fmove,j-4,3-col)) posOppCannAttk++;
				if(isCannon(board,i+5*fmove,j-5,i+3*fmove,j-3,i+4*fmove,j-4,3-col)) posOppCannAttk++;
				if(isCannon(board,i+5*fmove,j+5,i+3*fmove,j+3,i+4*fmove,j+4,3-col)) posOppCannAttk++;
				if(isCannon(board,i+2*fmove,j+2,i+3*fmove,j+3,i+4*fmove,j+4,3-col)) posOppCannAttk++;
				if(isCannon(board,i,j+2,i,j+3,i,j+4,3-col)) posOppCannAttk++;
				if(isCannon(board,i,j-2,i,j-3,i,j-4,3-col)) posOppCannAttk++;
				if(isCannon(board,i,j+3,i,j+4,i,j+5,3-col)) posOppCannAttk++;
				if(isCannon(board,i,j-3,i,j-4,i,j-5,3-col)) posOppCannAttk++;
			}

			if(board[i][j]==3-col)
			{
				if(boardCell(i-fmove,j,n,m) && isColorSoldier(board,i-fmove,j,col,col)) posOppAttk++;
				if(boardCell(i-fmove,j+1,n,m) && isColorSoldier(board,i-fmove,j+1,col,col)) posOppAttk++;
				if(boardCell(i-fmove,j-1,n,m) && isColorSoldier(board,i-fmove,j-1,col,col)) posOppAttk++;
				if(boardCell(i,j+1,n,m) && isColorSoldier(board,i,j+1,col,col)) posOppAttk++;
				if(boardCell(i,j-1,n,m) && isColorSoldier(board,i,j-1,col,col)) posOppAttk++;
				if(isCannon(board,i-2*fmove,j,i-3*fmove,j,i-4*fmove,j,col)) posCannAttk++;
				if(isCannon(board,i-5*fmove,j,i-3*fmove,j,i-4*fmove,j,col)) posCannAttk++;
				if(isCannon(board,i-2*fmove,j-2,i-3*fmove,j-3,i-4*fmove,j-4,col)) posCannAttk++;
				if(isCannon(board,i-5*fmove,j-5,i-3*fmove,j-3,i-4*fmove,j-4,col)) posCannAttk++;
				if(isCannon(board,i-5*fmove,j+5,i-3*fmove,j+3,i-4*fmove,j+4,col)) posCannAttk++;
				if(isCannon(board,i-2*fmove,j+2,i-3*fmove,j+3,i-4*fmove,j+4,col)) posCannAttk++;
				if(isCannon(board,i,j+2,i,j+3,i,j+4,col)) posCannAttk++;
				if(isCannon(board,i,j-2,i,j-3,i,j-4,col)) posCannAttk++;
				if(isCannon(board,i,j+3,i,j+4,i,j+5,col)) posCannAttk++;
				if(isCannon(board,i,j-3,i,j-4,i,j-5,col)) posCannAttk++;
			}

			if(board[i][j]==3-col)
			{
				if(isCannon(board,i,j+2,i,j+3,i,j+4,col)) killSolWithHC++;
				if(isCannon(board,i,j-2,i,j-3,i,j-4,col)) killSolWithHC++;
				if(isCannon(board,i,j+3,i,j+4,i,j+5,col)) killSolWithHC++;
				if(isCannon(board,i,j-3,i,j-4,i,j-5,col)) killSolWithHC++;
			}

			if(board[i][j]==col)
			{
				suicide+=isSuicide(board,i,j,col);
			}
		}
	}
	
	long long func= posAttk - 100*posOppAttk + 80*posCannAttk - 50*posOppCannAttk + 1000*posCannAttkTH
				+ 100*posAttkOnTH - 100*posOppAttkOnTH - 1000*posOppCannAttkTH + 100*killSolWithHC
				+ 100*numOfTH - 10000*numOfOppTH /*- 60000*locOpp */- 800*(numOfOppSol)-1000*suicide;
	
	// cerr<<"eval calculated!"<<endl;
	return func;
}

void printBoard(vector<vector<int> > &board)
{
	for(int i=0;i<board.size();i++)
	{
		for(int j=0;j<board[0].size();j++)
			cerr<<board[i][j]<<" ";
		cerr<<endl;
	}
}

void allBranches(vector<node> &child, vector<vector<int> > &board, int col)
{
	int n=board.size(),m=board[0].size();

	int fmove,bmove,tcol;

	if(col==whiteSol)
		fmove=1,bmove=-2,tcol=3;
	else
		fmove=-1,bmove=2,tcol=4;
	// cerr<<"start making branches..."<<endl;
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<m;j++)
		{
			if(board[i][j]==col)
			{
				node temp;
				bool isPartOfCannon=false;
				bool cannonMoveTown=false, cannonMoveSoldier=false,cannonPart=false;
				bool fwdMoveTown=false, fwdMoveSoldier=false, sideMoveTown=false;

				// cerr<<"Cannon started...";
				/*Cannon Moves*/
				/*Orthogonal Cannon*/
				if(isCannon(board,i,j,i+fmove,j,i+2*fmove,j,col)|| isCannon(board,i,j,i-fmove,j,i-2*fmove,j,col)||
					isCannon(board,i,j,i+fmove,j,i-fmove,j,col)|| isCannon(board,i,j,i+fmove,j+1,i+2*fmove,j+2,col)||
					isCannon(board,i,j,i-fmove,j-1,i+fmove,j+1,col)||isCannon(board,i,j,i-fmove,j-1,i-2*fmove,j-2,col)||
					isCannon(board,i,j,i-fmove,j+1,i+fmove,j-1,col)||isCannon(board,i,j,i+fmove,j-1,i+2*fmove,j-2,col)||
					isCannon(board,i,j,i-fmove,j+1,i-2*fmove,j+2,col)||isCannon(board,i,j,i,j-1,i,j-2,col)||
					isCannon(board,i,j,i,j-1,i,j+1,col)||isCannon(board,i,j,i,j+1,i,j+2,col))
					isPartOfCannon=true;

				/*Cannon moves*/
				if(isCannon(board,i,j,i+fmove,j,i+2*fmove,j,col))
				{
					if(boardCell(i+4*fmove,j,n,m) && !isColorSoldier(board,i+4*fmove,j,col,tcol) 
						&& !isColorSoldier(board,i+3*fmove,j,3-col,col))
					{
						temp.board=changeBoard(board,i,j,i+4*fmove,j,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i+4*fmove,j,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i+4*fmove,j,3-col,3-col))
						// 	temp.score+=(100),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j),mp(i+4*fmove,j));
						child.pb(temp);
						cannonPart=true;
					}
					if(boardCell(i+5*fmove,j,n,m) && !isColorSoldier(board,i+5*fmove,j,col,tcol) 
						&& !isColorSoldier(board,i+3*fmove,j,3-col,col) && !isColorSoldier(board,i+4*fmove,j,tcol,col))
					{
						temp.board=changeBoard(board,i,j,i+5*fmove,j,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i+5*fmove,j,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i+5*fmove,j,3-col,3-col))
						// 	temp.score+=(100),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j),mp(i+5*fmove,j));
						child.pb(temp);
						cannonPart=true;
					}
					if(boardCell(i-2*fmove,j,n,m) && !isColorSoldier(board,i-2*fmove,j,col,tcol) 
						&& !isColorSoldier(board,i-fmove,j,3-col,col))
					{
						temp.board=changeBoard(board,i,j,i-2*fmove,j,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i-2*fmove,j,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i-2*fmove,j,3-col,3-col))
						// 	temp.score+=(100),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-2*fmove,j));
						child.pb(temp);
						cannonPart=true;
					}
					if(boardCell(i-3*fmove,j,n,m) && !isColorSoldier(board,i-3*fmove,j,col,tcol) 
						&& !isColorSoldier(board,i-fmove,j,3-col,col) && !isColorSoldier(board,i-2*fmove,j,tcol,col))
					{
						temp.board=changeBoard(board,i,j,i-3*fmove,j,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i-3*fmove,j,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i-3*fmove,j,3-col,3-col))
						// 	temp.score+=(100),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-3*fmove,j));
						child.pb(temp);
						cannonPart=true;
					}	
				}
				// cerr<<"c1";
				if(isCannon(board,i,j,i+fmove,j+1,i+2*fmove,j+2,col))
				{
					cannonPart=true;
					if(boardCell(i+4*fmove,j+4,n,m) && !isColorSoldier(board,i+4*fmove,j+4,col,tcol) 
						&& !isColorSoldier(board,i+3*fmove,j+3,3-col,col))
					{
						temp.board=changeBoard(board,i,j,i+4*fmove,j+4,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i+4*fmove,j+4,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i+4*fmove,j+4,3-col,3-col))
						// 	temp.score+=(100),cannonMoveSoldier=true;
						
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j+2),mp(i+4*fmove,j+4));
						child.pb(temp);
					}
					if(boardCell(i+5*fmove,j+5,n,m) && !isColorSoldier(board,i+5*fmove,j+5,col,tcol) 
						&& !isColorSoldier(board,i+3*fmove,j+3,3-col,col) && !isColorSoldier(board,i+4*fmove,j+4,tcol,col))
					{
						temp.board=changeBoard(board,i,j,i+5*fmove,j+5,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i+5*fmove,j+5,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i+5*fmove,j+5,3-col,3-col))
						// 	temp.score+=(100),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j+2),mp(i+5*fmove,j+5));
						child.pb(temp);
					}
					if(boardCell(i-2*fmove,j-2,n,m) && !isColorSoldier(board,i-2*fmove,j-2,col,tcol) 
						&& !isColorSoldier(board,i-fmove,j-1,3-col,col))
					{
						temp.board=changeBoard(board,i,j,i-2*fmove,j-2,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i-2*fmove,j-2,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i-2*fmove,j-2,3-col,3-col))
						// 	temp.score+=(100),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-2*fmove,j-2));
						child.pb(temp);
					}
					if(boardCell(i-3*fmove,j-3,n,m) && !isColorSoldier(board,i-3*fmove,j-3,col,tcol) 
						&& !isColorSoldier(board,i-fmove,j-1,3-col,col) && !isColorSoldier(board,i-2*fmove,j-2,tcol,col))
					{
						temp.board=changeBoard(board,i,j,i-3*fmove,j-3,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i-3*fmove,j-3,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i-3*fmove,j-3,3-col,3-col))
						// 	temp.score+=(100),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-3*fmove,j-3));
						child.pb(temp);
					}
				}
				// cerr<<"c2";
				if(isCannon(board,i,j,i+fmove,j-1,i+2*fmove,j-2,col))
				{
					cannonPart=true;
					if(boardCell(i+4*fmove,j-4,n,m) && !isColorSoldier(board,i+4*fmove,j-4,col,tcol) 
						&& !isColorSoldier(board,i+3*fmove,j-3,3-col,col))
					{
						temp.board=changeBoard(board,i,j,i+4*fmove,j-4,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i+4*fmove,j-4,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i+4*fmove,j-4,3-col,3-col))
						// 	temp.score+=(100),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j-2),mp(i+4*fmove,j-4));
						child.pb(temp);
					}
					if(boardCell(i+5*fmove,j-5,n,m) && !isColorSoldier(board,i+5*fmove,j-5,col,tcol) 
						&& !isColorSoldier(board,i+3*fmove,j-3,3-col,col) && !isColorSoldier(board,i+4*fmove,j-4,tcol,col))
					{ 
						temp.board=changeBoard(board,i,j,i+5*fmove,j-5,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i+5*fmove,j-5,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i+5*fmove,j-5,3-col,3-col))
						// 	temp.score+=(100),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j-2),mp(i+5*fmove,j-5));
						child.pb(temp);
					}
					if(boardCell(i-2*fmove,j+2,n,m) && !isColorSoldier(board,i-2*fmove,j+2,col,tcol) 
						&& !isColorSoldier(board,i-fmove,j+1,3-col,col))
					{
						temp.board=changeBoard(board,i,j,i-2*fmove,j+2,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i-2*fmove,j+2,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i-2*fmove,j+2,3-col,3-col))
						// 	temp.score+=(100),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-2*fmove,j+2));
						child.pb(temp);
					}
					if(boardCell(i-3*fmove,j+3,n,m) && !isColorSoldier(board,i-3*fmove,j+3,col,tcol) 
						&& !isColorSoldier(board,i-fmove,j+1,3-col,col) && !isColorSoldier(board,i-2*fmove,j+2,3-col,col))
					{
						temp.board=changeBoard(board,i,j,i-3*fmove,j+3,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i-3*fmove,j+3,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i-3*fmove,j+3,3-col,3-col))
						// 	temp.score+=(100),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-3*fmove,j+3));
						child.pb(temp);
					}
				}
				
				if(isCannon(board,i,j,i,j-1,i,j-2,col))
				{
					if(boardCell(i,j+2,n,m) && !isColorSoldier(board,i,j+2,col,tcol) 
							&& !isColorSoldier(board,i,j+1,3-col,col))
					{
						temp.board=changeBoard(board,i,j,i,j+2,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i,j+2,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i,j+2,3-col,3-col))
						// 	temp.score+=(900000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i,j+2));
						child.pb(temp);
						cannonPart=true;
					}
					if(boardCell(i,j+3,n,m) && !isColorSoldier(board,i,j+3,col,tcol) 
						&& !isColorSoldier(board,i,j+1,3-col,col) && !isColorSoldier(board,i,j+2,tcol,col))
					{
						temp.board=changeBoard(board,i,j,i,j+3,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i,j+3,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i,j+3,3-col,3-col))
						// 	temp.score+=(900000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i,j+3));
						child.pb(temp);
						cannonPart=true;
					}
					if(boardCell(i,j-4,n,m) && !isColorSoldier(board,i,j-4,col,tcol) 
						&& !isColorSoldier(board,i,j-3,3-col,col))
					{
						temp.board=changeBoard(board,i,j,i,j-4,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i,j-4,7-tcol,7-tcol))
						 cannonMoveTown=true;
						// if(isColorSoldier(board,i,j-4,3-col,3-col))
						// 	temp.score+=(900000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i,j-4));
						child.pb(temp);
						cannonPart=true;
					}
					if(boardCell(i,j-5,n,m) && !isColorSoldier(board,i,j-5,col,tcol) 
						&& !isColorSoldier(board,i,j-3,3-col,col) && !isColorSoldier(board,i,j-4,tcol,col))
					{
						temp.board=changeBoard(board,i,j,i,j-5,true);
						temp.score=eval(temp.board,col);
						if(isColorSoldier(board,i,j-5,7-tcol,7-tcol))
							cannonMoveTown=true;
						// if(isColorSoldier(board,i,j-5,3-col,3-col))
						// 	temp.score+=(900000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i,j-5));
						child.pb(temp);
						cannonPart=true;
					}
				}				


				if(isCannon(board,i,j,i+fmove,j,i+2*fmove,j,col))
				{
					if(!(cannonMoveSoldier || cannonMoveTown))
					{
						if(boardCell(i-fmove,j,n,m) && isColorSoldier(board,i-fmove,j,empBlock,empBlock))
						{
							temp.board=changeBoard(board,i+2*fmove,j,i-fmove,j,false);
							temp.score=eval(temp.board,col)+(100);
							// if(!isSuicide(temp.board,i-fmove,j,col))
							// 	temp.score+=(100);

							temp.isCannonMove=false;
							temp.changeCoordi=mp(mp(i+2*fmove,j),mp(i-fmove,j));
							child.pb(temp);
						}
						if(boardCell(i+3*fmove,j,n,m) && isColorSoldier(board,i+3*fmove,j,empBlock,empBlock))
						{
							temp.board=changeBoard(board,i,j,i+3*fmove,j,false);
							temp.score=eval(temp.board,col)+(100);
							// if(!isSuicide(temp.board,i+3*fmove,j,col))
							// 	temp.score+=(100);
							// if(!isSuicide(temp.board,i+3*fmove,j,col) && boardCell(i+6*fmove,j,n,m) && isColorSoldier(board,i+6*fmove,j,7-tcol,7-tcol))
							// 	temp.score+=(500);

							temp.isCannonMove=false;
							temp.changeCoordi=mp(mp(i,j),mp(i+3*fmove,j));
							child.pb(temp);
						}
					}
				}
				
				if(isCannon(board,i,j,i+fmove,j+1,i+2*fmove,j+2,col))
				{
					if(!(cannonMoveSoldier || cannonMoveTown))
					{
						if(boardCell(i-fmove,j-1,n,m) && isColorSoldier(board,i-fmove,j-1,empBlock,empBlock))
						{
							temp.board=changeBoard(board,i+2*fmove,j+2,i-fmove,j-1,false);
							temp.score=eval(temp.board,col)+(100);
							// if(!isSuicide(temp.board,i-fmove,j-1,col))
							// 	temp.score+=(100);

							temp.isCannonMove=false;
							temp.changeCoordi=mp(mp(i+2*fmove,j+2),mp(i-fmove,j-1));
							child.pb(temp);
						}
						if(boardCell(i+3*fmove,j+3,n,m) && isColorSoldier(board,i+3*fmove,j+3,empBlock,empBlock))
						{
							temp.board=changeBoard(board,i,j,i+3*fmove,j+3,false);
							temp.score=eval(temp.board,col)+(100);
							// if(!isSuicide(temp.board,i+3*fmove,j+3,col))
							// 	temp.score+=(100);
							// if(!isSuicide(temp.board,i+3*fmove,j+3,col) && boardCell(i+6*fmove,j+6,n,m) && isColorSoldier(board,i+6*fmove,j+6,7-tcol,7-tcol))
							// 	temp.score+=(500);

							temp.isCannonMove=false;
							temp.changeCoordi=mp(mp(i,j),mp(i+3*fmove,j+3));
							child.pb(temp);
						}
					}
				}
				
				if(isCannon(board,i,j,i+fmove,j-1,i+2*fmove,j-2,col))
				{
					if(!(cannonMoveTown || cannonMoveSoldier))
					{
						if(boardCell(i-fmove,j+1,n,m) && isColorSoldier(board,i-fmove,j+1,empBlock,empBlock))
						{
							temp.board=changeBoard(board,i+2*fmove,j-2,i-fmove,j+1,false);
							temp.score=eval(temp.board,col)+(100);
							// if(!isSuicide(temp.board,i-fmove,j+1,col))
							// 	temp.score+=(100);

							temp.isCannonMove=false;
							temp.changeCoordi=mp(mp(i+2*fmove,j-2),mp(i-fmove,j+1));
							child.pb(temp);
						}
						if(boardCell(i+3*fmove,j-3,n,m) && isColorSoldier(board,i+3*fmove,j-3,empBlock,empBlock))
						{
							temp.board=changeBoard(board,i,j,i+3*fmove,j-3,false);
							temp.score=eval(temp.board,col)+(100);
							// if(!isSuicide(temp.board,i+3*fmove,j-3,col))
							// 	temp.score+=(100);
							// if(!isSuicide(temp.board,i+3*fmove,j-3,col) && boardCell(i+6*fmove,j-6,n,m) && isColorSoldier(board,i+6*fmove,j-6,7-tcol,7-tcol))
							// 	temp.score+=(500);

							temp.isCannonMove=false;
							temp.changeCoordi=mp(mp(i,j),mp(i+3*fmove,j-3));
							child.pb(temp);
						}
					}
				}
				
				if(isCannon(board,i,j,i,j-1,i,j-2,col))
				{
					if(!(cannonMoveTown || cannonMoveSoldier))
					{
						if(boardCell(i,j+1,n,m) && isColorSoldier(board,i,j+1,empBlock,empBlock))
						{
							temp.board=changeBoard(board,i,j-2,i,j+1,false);
							temp.score=eval(temp.board,col)+(100);
							// if(!isSuicide(temp.board,i,j+1,col))
							// 	temp.score+=(100);

							temp.isCannonMove=false;
							temp.changeCoordi=mp(mp(i,j-2),mp(i,j+1));
							child.pb(temp);
						}
						if(boardCell(i,j-3,n,m) && isColorSoldier(board,i,j-3,empBlock,empBlock))
						{
							temp.board=changeBoard(board,i,j,i,j-3,false);
							temp.score=eval(temp.board,col)+(100);
							// if(!isSuicide(temp.board,i,j-3,col))
							// 	temp.score+=(100);
							// if(!isSuicide(temp.board,i,j-3,col) && boardCell(i,j-6,n,m) && isColorSoldier(board,i,j-6,7-tcol,7-tcol))
							// 	temp.score+=(500);

							temp.isCannonMove=false;
							temp.changeCoordi=mp(mp(i,j),mp(i,j-3));
							child.pb(temp);
						}
					}
				}

				if(cnt<=1)
					continue;
				
				if(!cannonMoveTown)
				{
					if(boardCell(i+fmove, j, n, m) && !isColorSoldier(board,i+fmove,j,col,col))
					{
						bool locTown=false,locSoldier=false;
						temp.board=changeBoard(board,i,j,i+fmove,j,false);
						temp.score=eval(temp.board,col);
						// if(isSuicide(temp.board,i+fmove,j,col))
						// 	temp.score-=(200);
						/*if(isColorSoldier(board,i+fmove,j,7-tcol,7-tcol))
							temp.score+=(100),locTown=true;
						else
						{
							if(isColorSoldier(board,i+fmove,j,3-col,3-col) && !isSuicide(temp.board,i+fmove,j))
								temp.score+=(200),locSoldier=true;
							if(isSuicide(temp.board,i+fmove,j,col))
								temp.score-=(2000);
							if(isPartOfCannon)
								temp.score-=(100);
						}*/

						temp.changeCoordi=mp(mp(i,j),mp(i+fmove,j));
						temp.isCannonMove=false;
						// if(!isSuicide(temp.board,i+fmove,j,col))
						child.pb(temp);
						/*if((locTown || locSoldier) || (!(fwdMoveTown || fwdMoveSoldier)))
						{
							child.pb(temp);
							if(locTown) fwdMoveTown=true;
							if(locSoldier) fwdMoveSoldier=true;
						}*/
					}
					if(boardCell(i+fmove,j+1,n,m) && !isColorSoldier(board,i+fmove,j+1,col,col))
					{
						bool locTown=false,locSoldier=false;
						temp.board=changeBoard(board,i,j,i+fmove,j+1,false);
						temp.score=eval(temp.board,col);
						// if(isSuicide(temp.board,i+fmove,j+1,col))
						// 	temp.score-=(200);
						/*if(isColorSoldier(board,i+fmove,j+1,7-tcol,7-tcol))
							temp.score+=(100),locTown=true;
						else
						{
							if(isColorSoldier(board,i+fmove,j+1,3-col,3-col) && !isSuicide(temp.board,i+fmove,j+1))
								temp.score+=(200),locSoldier=true;
							if(isSuicide(temp.board,i+fmove,j+1))
								temp.score-=(2000);
							if(isPartOfCannon)
								temp.score-=(900000);
						}*/

						temp.changeCoordi=mp(mp(i,j),mp(i+fmove,j+1));
						temp.isCannonMove=false;
						// if(!isSuicide(temp.board,i+fmove,j,col))
						child.pb(temp);
						/*if((locTown || locSoldier) || (!(fwdMoveTown || fwdMoveSoldier)))
						{
							if(locTown) fwdMoveTown=true;
							if(locSoldier) fwdMoveSoldier=true;
						}*/
					}
					if(boardCell(i+fmove,j-1,n,m) && !isColorSoldier(board,i+fmove,j-1,col,col))
					{
						bool locTown=false,locSoldier=false;
						temp.board=changeBoard(board,i,j,i+fmove,j-1,false);
						temp.score=eval(temp.board,col);
						// if(isSuicide(temp.board,i+fmove,j-1,col))
						// 	temp.score-=(200);
						/*if(isColorSoldier(board,i+fmove,j-1,7-tcol,7-tcol))
							temp.score+=(100),locTown=true;
						else
						{
							if(isColorSoldier(board,i+fmove,j-1,3-col,3-col) && !isSuicide(temp.board,i+fmove,j-1))
								temp.score+=(200),locSoldier=true;
							if(isPartOfCannon)
								temp.score-=(900000);
						}*/

						temp.changeCoordi=mp(mp(i,j),mp(i+fmove,j-1));
						temp.isCannonMove=false;
						// if(!isSuicide(temp.board,i+fmove,j,col))
						child.pb(temp);
						/*if((locTown || locSoldier) || (!(fwdMoveTown || fwdMoveSoldier)))
						{
							if(locTown) fwdMoveTown=true;
							if(locSoldier) fwdMoveSoldier=true;
						}*/
					}
				}

				if(!(cannonMoveTown || fwdMoveTown))
				{
					/*Sideways Capture Moves*/
					bool isSideMovePossible=false;
					if(isSoldier(i,j+1,n,m,3-col,7-tcol,board))
					{
						temp.board=changeBoard(board,i,j,i,j+1,false);
						temp.score=eval(temp.board,col);
						// if(isSuicide(temp.board,i,j+1,col))
						// 	temp.score-=(200);
						/*if(isColorSoldier(board,i,j+1,7-tcol,7-tcol))
							temp.score+=(100), sideMoveTown=true;
						else
						{
							if(isColorSoldier(board,i,j+1,3-col,3-col) && !isSuicide(temp.board,i,j+1))
								temp.score+=(20000);
							if(isPartOfCannon)
								temp.score-=(900000);
						}*/

						temp.changeCoordi=mp(mp(i,j),mp(i,j+1));
						temp.isCannonMove=false;
						child.pb(temp);
						isSideMovePossible=true;
					}
					if(isSoldier(i,j-1,n,m,3-col,7-tcol,board))
					{
						temp.board=changeBoard(board,i,j,i,j-1,false);
						temp.score=eval(temp.board,col);
						// if(isSuicide(temp.board,i,j-1,col))
						// 	temp.score-=(200);
						/*if(isColorSoldier(board,i,j-1,7-tcol,7-tcol))
							temp.score+=(100),sideMoveTown=true;
						else
						{
							if(isColorSoldier(board,i,j-1,3-col,3-col) && !isSuicide(temp.board,i,j-1))
								temp.score+=(20000);
							if(isPartOfCannon)
								temp.score-=(900000);
						}*/

						temp.changeCoordi=mp(mp(i,j),mp(i,j-1));
						temp.isCannonMove=false;
						child.pb(temp);
						isSideMovePossible=true;
					}

					/*Backward Moves*/
					if(isSideMovePossible && !sideMoveTown)
					{
						if(boardCell(i+bmove,j,n,m) && !isColorSoldier(board,i+bmove,j,col,col))
						{
							temp.board=changeBoard(board,i,j,i+bmove,j,false);
							temp.score=eval(temp.board,col);

							// if(isSuicide(board,i,j,col))
							// 	temp.score-=(200);
							// if(isPartOfCannon)
							// 	temp.score-=(50);
							temp.changeCoordi=mp(mp(i,j),mp(i+bmove,j));
							temp.isCannonMove=false;
							child.pb(temp);
						}
						if(boardCell(i+bmove,j-2,n,m) && !isColorSoldier(board,i+bmove,j-2,col,col))
						{
							temp.board=changeBoard(board,i,j,i+bmove,j-2,false);
							temp.score=eval(temp.board,col);
							// if(isSuicide(board,i,j,col))
							// 	temp.score-=(200);
							// if(isPartOfCannon)
							// 	temp.score-=(50);
							temp.changeCoordi=mp(mp(i,j),mp(i+bmove,j-2));
							temp.isCannonMove=false;
							child.pb(temp);
						}
						if(boardCell(i+bmove,j+2,n,m) && !isColorSoldier(board,i+bmove,j+2,col,col))
						{
							temp.board=changeBoard(board,i,j,i+bmove,j+2,false);
							temp.score=eval(temp.board,col);
							// if(isSuicide(board,i,j,col))
							// 	temp.score-=(200);
							// if(isPartOfCannon)
							// 	temp.score-=(50);
							temp.changeCoordi=mp(mp(i,j),mp(i+bmove,j+2));
							temp.isCannonMove=false;
							child.pb(temp);
						}
					}
				}
			}
		}
	}

	// cerr<<"branches made."<<endl;
}

void makeTree(node &root, int depth, int col)
{
	// cerr<<depth<<endl;
	if(depth>=maxDepthTree)
		return;

	allBranches(root.child, root.board,col);
	for(int i=0;i<root.child.size();i++)
	{
		makeTree(root.child[i],depth+1,(3-col));
	}
}

pair<pair<double,bool>,pair<pii,pii> > miniMaxWithAlphaBetaPruning(node &root, int depth,int alpha, int beta, int maxDepth, int col)
{
	if(depth==maxDepth)
		return mp(mp(eval(root.board,col),false),mp(mp(0,0),mp(0,0)));

	if(depth&1)
	{
		node newBoard;
		for(int i=0;i<root.child.size();i++)
		{
			double temp = miniMaxWithAlphaBetaPruning(root.child[i],depth+1,alpha,beta,maxDepth,3-col).fi.fi;

			if(temp<=alpha)
				return mp(mp(temp,root.child[i].isCannonMove),root.child[i].changeCoordi);
			if(temp<beta)
			{
				beta=temp;
				newBoard=root.child[i];
			}
		}

		return mp(mp(beta,newBoard.isCannonMove),newBoard.changeCoordi);
	}
	else
	{
		node newBoard;
		for(int i=0;i<root.child.size();i++)
		{
			double temp = miniMaxWithAlphaBetaPruning(root.child[i],depth+1,alpha,beta,maxDepth,3-col).fi.fi;

			if(temp>=beta)
				return mp(mp(temp,root.child[i].isCannonMove),root.child[i].changeCoordi);
			if(temp>alpha)
			{
				alpha=temp;
				newBoard=root.child[i];
			}
		}

		return mp(mp(alpha,newBoard.isCannonMove),newBoard.changeCoordi);
	}
}

string ErnestMove(vector<vector<int> > &board)
{
	node root;
	root.board=board;
	
	makeTree(root,0,solCol);

	pair<pair<double,bool>,pair<pii,pii> > ans1=miniMaxWithAlphaBetaPruning(root,0,-inf,inf,1,solCol);
	// cerr<<"MiniMax1 Done!"<<endl;
	pair<pair<double,bool>,pair<pii,pii> > ans2=miniMaxWithAlphaBetaPruning(root,0,-inf,inf,2,solCol);
	// cerr<<"MiniMax2 Done!"<<endl;
	pair<pair<double,bool>,pair<pii,pii> > ans3=miniMaxWithAlphaBetaPruning(root,0,-inf,inf,3,solCol);
	// cerr<<"MiniMax3 Done!"<<endl;
	pair<pair<double,bool>,pair<pii,pii> > ans;

	if(ans1.fi.fi>ans2.fi.fi) ans=ans1;
	else ans=ans2;

	if(ans.fi.fi<ans3.fi.fi) ans=ans3;

	pair<pii,pii> move=ans.sec;

	if(!ans.fi.sec)
		return ("S "+ to_string(move.fi.sec)+" "+to_string(move.fi.fi)+" M "+to_string(move.sec.sec)+" "+to_string(move.sec.fi));
	else
		return ("S "+to_string(move.fi.sec)+ " "+ to_string(move.fi.fi)+" B "+to_string(move.sec.sec)+" "+to_string(move.sec.fi));
}

int main(){
	//start client
	int n,m,player_id; 
	vector<vector<int> > Board;

	cin >> player_id >> n >> m >> timelimit;

	if(player_id==2){
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

	if(player_id==1)
	{
		string str = ErnestMove(Board) ;
		cout << str<<endl;
		
		if(str[6]=='M')
			Board = changeBoard(Board,str[4]-'0',str[2]-'0',str[10]-'0',str[8]-'0',0);	
		else
			Board = changeBoard(Board,str[4]-'0',str[2]-'0',str[10]-'0',str[8]-'0',1);	
	}

	while(true)
	{
		cnt++;
		string str;
		char t[6];
		cin>>t[0]>>t[1]>>t[2]>>t[3]>>t[4]>>t[5];
		
		if(t[3]=='M')
			Board = changeBoard(Board,(t[2]-'0'),(t[1]-'0'),(t[5]-'0'),(t[4]-'0'),0);
		else
			Board = changeBoard(Board,(t[2]-'0'),(t[1]-'0'),(t[5]-'0'),(t[4]-'0'),1);
		
		str = ErnestMove(Board) ;
		cout << str<<endl;
		
		if(str[6]=='M')
			Board = changeBoard(Board,str[4]-'0',str[2]-'0',str[10]-'0',str[8]-'0',0);	
		else
			Board = changeBoard(Board,str[4]-'0',str[2]-'0',str[10]-'0',str[8]-'0',1);	
	}

	return 0;
}