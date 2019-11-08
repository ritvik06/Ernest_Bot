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

long long eval(vector<vector<int> > &board)
{
	int n=board.size(),m=board[0].size();
	int fmove,bmove;

	if(solCol==whiteSol)
		fmove=1,bmove=-2;
	else
		fmove=-1,bmove=2;

	int numOfSol=0,posAttk=0,posOppAttk=0,posOppAttkOnTH=0,posAttkOnTH=0,numOfOppSol=0;
	int numOfTH=0,numOfOppTH=0,posCannAttk=0,posOppCannAttk=0;
	int loc=0,locOpp=0;

	for(int i=0;i<n;i++)
	{
		for(int j=0;j<m;j++)
		{
			if(board[i][j]==solCol)
				numOfSol++;
			if(board[i][j]==oppCol)
				numOfOppSol++;
			if(board[i][j]==townCol)
				numOfTH++;
			if(board[i][j]==oppTownCol)
				numOfOppTH++;

			if(retLoc(i,LOC,solCol) && board[i][j]==solCol) loc++;
			if(retLoc(i,LOC-2*fmove,oppCol) && board[i][j]==oppCol) locOpp++;

			if(board[i][j]==townCol)
			{
				if(boardCell(i+fmove,j,n,m) && isColorSoldier(board,i+fmove,j,oppCol,oppCol)) posOppAttkOnTH++;
				if(boardCell(i+fmove,j+1,n,m) && isColorSoldier(board,i+fmove,j+1,oppCol,oppCol)) posOppAttkOnTH++;
				if(boardCell(i+fmove,j-1,n,m) && isColorSoldier(board,i+fmove,j-1,oppCol,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+2*fmove,j,i+3*fmove,j,i+4*fmove,j,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+5*fmove,j,i+3*fmove,j,i+4*fmove,j,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+2*fmove,j-2,i+3*fmove,j-3,i+4*fmove,j-4,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+5*fmove,j-5,i+3*fmove,j-3,i+4*fmove,j-4,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+2*fmove,j+2,i+3*fmove,j+3,i+4*fmove,j+4,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+5*fmove,j+5,i+3*fmove,j+3,i+4*fmove,j+4,oppCol)) posOppAttkOnTH++;
			}

			if(board[i][j]==oppTownCol)
			{
				if(boardCell(i-fmove,j,n,m) && isColorSoldier(board,i-fmove,j,solCol,solCol)) posAttkOnTH++;
				if(boardCell(i-fmove,j+1,n,m) && isColorSoldier(board,i-fmove,j+1,solCol,solCol)) posAttkOnTH++;
				if(boardCell(i-fmove,j-1,n,m) && isColorSoldier(board,i-fmove,j-1,solCol,solCol)) posAttkOnTH++;
				if(isCannon(board,i-2*fmove,j,i-3*fmove,j,i-4*fmove,j,solCol)) posOppAttkOnTH++;
				if(isCannon(board,i-5*fmove,j,i-3*fmove,j,i-4*fmove,j,solCol)) posOppAttkOnTH++;
				if(isCannon(board,i-2*fmove,j-2,i-3*fmove,j-3,i-4*fmove,j-4,solCol)) posOppAttkOnTH++;
				if(isCannon(board,i-5*fmove,j-5,i-3*fmove,j-3,i-4*fmove,j-4,solCol)) posOppAttkOnTH++;
				if(isCannon(board,i-2*fmove,j+2,i-3*fmove,j+3,i-4*fmove,j+4,solCol)) posOppAttkOnTH++;
				if(isCannon(board,i-5*fmove,j+5,i-3*fmove,j+3,i-4*fmove,j+4,solCol)) posOppAttkOnTH++;
			}

			if(board[i][j]==solCol)
			{
				if(boardCell(i+fmove,j,n,m) && isColorSoldier(board,i+fmove,j,oppCol,oppCol)) posAttk++;
				if(boardCell(i+fmove,j+1,n,m) && isColorSoldier(board,i+fmove,j+1,oppCol,oppCol)) posAttk++;
				if(boardCell(i+fmove,j-1,n,m) && isColorSoldier(board,i+fmove,j-1,oppCol,oppCol)) posAttk++;
				if(boardCell(i,j+1,n,m) && isColorSoldier(board,i,j+1,oppCol,oppCol)) posAttk++;
				if(boardCell(i,j-1,n,m) && isColorSoldier(board,i,j-1,oppCol,oppCol)) posAttk++;
				if(isCannon(board,i+2*fmove,j,i+3*fmove,j,i+4*fmove,j,oppCol)) posOppCannAttk++;
				if(isCannon(board,i+5*fmove,j,i+3*fmove,j,i+4*fmove,j,oppCol)) posOppCannAttk++;
				if(isCannon(board,i+2*fmove,j-2,i+3*fmove,j-3,i+4*fmove,j-4,oppCol)) posOppCannAttk++;
				if(isCannon(board,i+5*fmove,j-5,i+3*fmove,j-3,i+4*fmove,j-4,oppCol)) posOppCannAttk++;
				if(isCannon(board,i+5*fmove,j+5,i+3*fmove,j+3,i+4*fmove,j+4,oppCol)) posOppCannAttk++;
				if(isCannon(board,i+2*fmove,j+2,i+3*fmove,j+3,i+4*fmove,j+4,oppCol)) posOppCannAttk++;
			}

			if(board[i][j]==oppCol)
			{
				if(boardCell(i-fmove,j,n,m) && isColorSoldier(board,i-fmove,j,solCol,solCol)) posOppAttk++;
				if(boardCell(i-fmove,j+1,n,m) && isColorSoldier(board,i-fmove,j+1,solCol,solCol)) posOppAttk++;
				if(boardCell(i-fmove,j-1,n,m) && isColorSoldier(board,i-fmove,j-1,solCol,solCol)) posOppAttk++;
				if(boardCell(i,j+1,n,m) && isColorSoldier(board,i,j+1,solCol,solCol)) posOppAttk++;
				if(boardCell(i,j-1,n,m) && isColorSoldier(board,i,j-1,solCol,solCol)) posOppAttk++;
				if(isCannon(board,i-2*fmove,j,i-3*fmove,j,i-4*fmove,j,solCol)) posCannAttk++;
				if(isCannon(board,i-5*fmove,j,i-3*fmove,j,i-4*fmove,j,solCol)) posCannAttk++;
				if(isCannon(board,i-2*fmove,j-2,i-3*fmove,j-3,i-4*fmove,j-4,solCol)) posCannAttk++;
				if(isCannon(board,i-5*fmove,j-5,i-3*fmove,j-3,i-4*fmove,j-4,solCol)) posCannAttk++;
				if(isCannon(board,i-5*fmove,j+5,i-3*fmove,j+3,i-4*fmove,j+4,solCol)) posCannAttk++;
				if(isCannon(board,i-2*fmove,j+2,i-3*fmove,j+3,i-4*fmove,j+4,solCol)) posCannAttk++;
			}
		}
	}
	
	long long func=posAttk - 30000*posOppAttk + 80*posCannAttk - 50*posOppCannAttk + 100*posAttkOnTH - 100*posOppAttkOnTH 
				+ numOfTH + 1000000*(4-numOfOppTH) - 60000*locOpp + 800*(12-numOfOppSol);
	
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

int isSuicide(vector<vector<int> > &board, int i, int j)
{
	int n=board.size(),m=board[0].size();
	int fmove,bmove;

	if(solCol==whiteSol)
		fmove=1,bmove=-2;
	else
		fmove=-1,bmove=2;

	int attk=0;
	if(boardCell(i+fmove,j,n,m) && isColorSoldier(board,i+fmove,j,oppCol,oppCol)) attk++;
	if(boardCell(i+fmove,j+1,n,m) && isColorSoldier(board,i+fmove,j+1,oppCol,oppCol)) attk++;
	if(boardCell(i+fmove,j-1,n,m) && isColorSoldier(board,i+fmove,j-1,oppCol,oppCol)) attk++;
	if(boardCell(i,j+1,n,m) && isColorSoldier(board,i,j+1,oppCol,oppCol)) attk++;
	if(boardCell(i,j-1,n,m) && isColorSoldier(board,i,j-1,oppCol,oppCol)) attk++;
	if(isCannon(board,i-2*fmove,j,i-3*fmove,j,i-4*fmove,j,oppCol)) attk++;
	if(isCannon(board,i-5*fmove,j,i-3*fmove,j,i-4*fmove,j,oppCol)) attk++;
	if(isCannon(board,i-2*fmove,j-2,i-3*fmove,j-3,i-4*fmove,j-4,oppCol)) attk++;
	if(isCannon(board,i-5*fmove,j-5,i-3*fmove,j-3,i-4*fmove,j-4,oppCol)) attk++;
	if(isCannon(board,i-5*fmove,j+5,i-3*fmove,j+3,i-4*fmove,j+4,oppCol)) attk++;
	if(isCannon(board,i-2*fmove,j+2,i-3*fmove,j+3,i-4*fmove,j+4,oppCol)) attk++;

	return attk;
}

void allBranches(vector<node> &child, vector<vector<int> > &board)
{
	int n=board.size(),m=board[0].size();

	int fmove,bmove;

	if(solCol==whiteSol)
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
				bool isPartOfCannon=false;
				bool cannonMoveTown=false, cannonMoveSoldier=false,cannonPart=false;
				bool fwdMoveTown=false, fwdMoveSoldier=false, sideMoveTown=false;

				/*Cannon Moves*/
				/*Orthogonal Cannon*/
				if(isCannon(board,i,j,i+fmove,j,i+2*fmove,j,solCol)|| isCannon(board,i,j,i-fmove,j,i-2*fmove,j,solCol)||
					isCannon(board,i,j,i+fmove,j,i-fmove,j,solCol)|| isCannon(board,i,j,i+fmove,j+1,i+2*fmove,j+2,solCol)||
					isCannon(board,i,j,i-fmove,j-1,i+fmove,j+1,solCol)||isCannon(board,i,j,i-fmove,j-1,i-2*fmove,j-2,solCol)||
					isCannon(board,i,j,i-fmove,j+1,i+fmove,j-1,solCol)||isCannon(board,i,j,i+fmove,j-1,i+2*fmove,j-2,solCol)||
					isCannon(board,i,j,i-fmove,j+1,i-2*fmove,j+2,solCol))
					isPartOfCannon=true;

				/*Cannon moves*/
				if(isCannon(board,i,j,i+fmove,j,i+2*fmove,j,solCol))
				{
					if(boardCell(i+4*fmove,j,n,m) && !isColorSoldier(board,i+4*fmove,j,solCol,townCol) 
						&& !isColorSoldier(board,i+3*fmove,j,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i+4*fmove,j,true);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i+4*fmove,j,oppTownCol,oppTownCol))
							temp.score+=(40000000),cannonMoveTown=true;
						if(isColorSoldier(board,i+4*fmove,j,oppCol,oppCol))
							temp.score+=(9000000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j),mp(i+4*fmove,j));
						child.pb(temp);
						cannonPart=true;
					}
					if(boardCell(i+5*fmove,j,n,m) && !isColorSoldier(board,i+5*fmove,j,solCol,townCol) 
						&& !isColorSoldier(board,i+3*fmove,j,oppCol,solCol) && !isColorSoldier(board,i+4*fmove,j,townCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i+5*fmove,j,true);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i+5*fmove,j,oppTownCol,oppTownCol))
							temp.score+=(40000000),cannonMoveTown=true;
						if(isColorSoldier(board,i+5*fmove,j,oppCol,oppCol))
							temp.score+=(9000000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j),mp(i+5*fmove,j));
						child.pb(temp);
						cannonPart=true;
					}
					if(boardCell(i-2*fmove,j,n,m) && !isColorSoldier(board,i-2*fmove,j,solCol,townCol) 
						&& !isColorSoldier(board,i-fmove,j,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i-2*fmove,j,true);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i-2*fmove,j,oppTownCol,oppTownCol))
							temp.score+=(40000000),cannonMoveTown=true;
						if(isColorSoldier(board,i-2*fmove,j,oppCol,oppCol))
							temp.score+=(9000000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-2*fmove,j));
						child.pb(temp);
						cannonPart=true;
					}
					if(boardCell(i-3*fmove,j,n,m) && !isColorSoldier(board,i-3*fmove,j,solCol,townCol) 
						&& !isColorSoldier(board,i-fmove,j,oppCol,solCol) && !isColorSoldier(board,i-2*fmove,j,townCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i-3*fmove,j,true);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i-3*fmove,j,oppTownCol,oppTownCol))
							temp.score+=(40000000),cannonMoveTown=true;
						if(isColorSoldier(board,i-3*fmove,j,oppCol,oppCol))
							temp.score+=(9000000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-3*fmove,j));
						child.pb(temp);
						cannonPart=true;
					}	
				}

				if(isCannon(board,i,j,i+fmove,j+1,i+2*fmove,j+2,solCol))
				{
					cannonPart=true;
					if(boardCell(i+4*fmove,j+4,n,m) && !isColorSoldier(board,i+4*fmove,j+4,solCol,townCol) 
						&& !isColorSoldier(board,i+3*fmove,j+3,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i+4*fmove,j+4,true);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i+4*fmove,j+4,oppTownCol,oppTownCol))
							temp.score+=(40000000),cannonMoveTown=true;
						if(isColorSoldier(board,i+4*fmove,j+4,oppCol,oppCol))
							temp.score+=(9000000),cannonMoveSoldier=true;
						
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j+2),mp(i+4*fmove,j+4));
						child.pb(temp);
					}
					if(boardCell(i+5*fmove,j+5,n,m) && !isColorSoldier(board,i+5*fmove,j+5,solCol,townCol) 
						&& !isColorSoldier(board,i+3*fmove,j+3,oppCol,solCol) && !isColorSoldier(board,i+4*fmove,j+4,townCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i+5*fmove,j+5,true);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i+5*fmove,j+5,oppTownCol,oppTownCol))
							temp.score+=(40000000),cannonMoveTown=true;
						if(isColorSoldier(board,i+5*fmove,j+5,oppCol,oppCol))
							temp.score+=(9000000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j+2),mp(i+5*fmove,j+5));
						child.pb(temp);
					}
					if(boardCell(i-2*fmove,j-2,n,m) && !isColorSoldier(board,i-2*fmove,j-2,solCol,townCol) 
						&& !isColorSoldier(board,i-fmove,j-1,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i-2*fmove,j-2,true);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i-2*fmove,j-2,oppTownCol,oppTownCol))
							temp.score+=(40000000),cannonMoveTown=true;
						if(isColorSoldier(board,i-2*fmove,j-2,oppCol,oppCol))
							temp.score+=(9000000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-2*fmove,j-2));
						child.pb(temp);
					}
					if(boardCell(i-3*fmove,j-3,n,m) && !isColorSoldier(board,i-3*fmove,j-3,solCol,townCol) 
						&& !isColorSoldier(board,i-fmove,j-1,oppCol,solCol) && !isColorSoldier(board,i-2*fmove,j-2,townCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i-3*fmove,j-3,true);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i-3*fmove,j-3,oppTownCol,oppTownCol))
							temp.score+=(40000000),cannonMoveTown=true;
						if(isColorSoldier(board,i-3*fmove,j-3,oppCol,oppCol))
							temp.score+=(9000000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-3*fmove,j-3));
						child.pb(temp);
					}
				}

				if(isCannon(board,i,j,i+fmove,j-1,i+2*fmove,j-2,solCol))
				{
					cannonPart=true;
					if(boardCell(i+4*fmove,j-4,n,m) && !isColorSoldier(board,i+4*fmove,j-4,solCol,townCol) 
						&& !isColorSoldier(board,i+3*fmove,j-3,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i+4*fmove,j-4,true);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i+4*fmove,j-4,oppTownCol,oppTownCol))
							temp.score+=(40000000),cannonMoveTown=true;
						if(isColorSoldier(board,i+4*fmove,j-4,oppCol,oppCol))
							temp.score+=(9000000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j-2),mp(i+4*fmove,j-4));
						child.pb(temp);
					}
					if(boardCell(i+5*fmove,j-5,n,m) && !isColorSoldier(board,i+5*fmove,j-5,solCol,townCol) 
						&& !isColorSoldier(board,i+3*fmove,j-3,oppCol,solCol) && !isColorSoldier(board,i+4*fmove,j-4,townCol,solCol))
					{ 
						temp.board=changeBoard(board,i,j,i+5*fmove,j-5,true);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i+5*fmove,j-5,oppTownCol,oppTownCol))
							temp.score+=(40000000),cannonMoveTown=true;
						if(isColorSoldier(board,i+5*fmove,j-5,oppCol,oppCol))
							temp.score+=(9000000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j-2),mp(i+5*fmove,j-5));
						child.pb(temp);
					}
					if(boardCell(i-2*fmove,j+2,n,m) && !isColorSoldier(board,i-2*fmove,j+2,solCol,townCol) 
						&& !isColorSoldier(board,i-fmove,j+1,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i-2*fmove,j+2,true);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i-2*fmove,j+2,oppTownCol,oppTownCol))
							temp.score+=(40000000),cannonMoveTown=true;
						if(isColorSoldier(board,i-2*fmove,j+2,oppCol,oppCol))
							temp.score+=(9000000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-2*fmove,j+2));
						child.pb(temp);
					}
					if(boardCell(i-3*fmove,j+3,n,m) && !isColorSoldier(board,i-3*fmove,j+3,solCol,townCol) 
						&& !isColorSoldier(board,i-fmove,j+1,oppCol,solCol) && !isColorSoldier(board,i-2*fmove,j+2,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i-3*fmove,j+3,true);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i-3*fmove,j+3,oppTownCol,oppTownCol))
							temp.score+=(40000000),cannonMoveTown=true;
						if(isColorSoldier(board,i-3*fmove,j+3,oppCol,oppCol))
							temp.score+=(9000000),cannonMoveSoldier=true;
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-3*fmove,j+3));
						child.pb(temp);
					}
				}

				if(isCannon(board,i,j,i+fmove,j,i+2*fmove,j,solCol))
				{
					if(!(cannonMoveSoldier || cannonMoveTown))
					{
						if(boardCell(i-fmove,j,n,m) && isColorSoldier(board,i-fmove,j,empBlock,empBlock))
						{
							temp.board=changeBoard(board,i+2*fmove,j,i-fmove,j,false);
							temp.score=eval(temp.board)+(9000000);
							if(!isSuicide(temp.board,i-fmove,j))
								temp.score+=(9000000);

							temp.isCannonMove=false;
							temp.changeCoordi=mp(mp(i+2*fmove,j),mp(i-fmove,j));
							child.pb(temp);
						}
						if(boardCell(i+3*fmove,j,n,m) && isColorSoldier(board,i+3*fmove,j,empBlock,empBlock))
						{
							temp.board=changeBoard(board,i,j,i+3*fmove,j,false);
							temp.score=eval(temp.board)+(9000000);
							if(!isSuicide(temp.board,i+3*fmove,j))
								temp.score+=(9000000);
							if(!isSuicide(temp.board,i+3*fmove,j) && isColorSoldier(board,i+6*fmove,j,oppTownCol,oppTownCol))
								temp.score+=(10000000);

							temp.isCannonMove=false;
							temp.changeCoordi=mp(mp(i,j),mp(i+3*fmove,j));
							child.pb(temp);
						}
					}
				}

				if(isCannon(board,i,j,i+fmove,j+1,i+2*fmove,j+2,solCol))
				{
					if(!(cannonMoveSoldier || cannonMoveTown))
					{
						if(boardCell(i-fmove,j-1,n,m) && isColorSoldier(board,i-fmove,j-1,empBlock,empBlock))
						{
							temp.board=changeBoard(board,i+2*fmove,j+2,i-fmove,j-1,false);
							temp.score=eval(temp.board)+(9000000);
							if(!isSuicide(temp.board,i-fmove,j-1))
								temp.score+=(9000000);

							temp.isCannonMove=false;
							temp.changeCoordi=mp(mp(i+2*fmove,j+2),mp(i-fmove,j-1));
							child.pb(temp);
						}
						if(boardCell(i+3*fmove,j+3,n,m) && isColorSoldier(board,i+3*fmove,j+3,empBlock,empBlock))
						{
							temp.board=changeBoard(board,i,j,i+3*fmove,j+3,false);
							temp.score=eval(temp.board)+(9000000);
							if(!isSuicide(temp.board,i+3*fmove,j+3))
								temp.score+=(9000000);
							if(!isSuicide(temp.board,i+3*fmove,j+3) && isColorSoldier(board,i+6*fmove,j+6,oppTownCol,oppTownCol))
								temp.score+=(10000000);

							temp.isCannonMove=false;
							temp.changeCoordi=mp(mp(i,j),mp(i+3*fmove,j+3));
							child.pb(temp);
						}
					}
				}

				if(isCannon(board,i,j,i+fmove,j-1,i+2*fmove,j-2,solCol))
				{
					if(!(cannonMoveTown || cannonMoveSoldier))
					{
						if(boardCell(i-fmove,j+1,n,m) && isColorSoldier(board,i-fmove,j+1,empBlock,empBlock))
						{
							temp.board=changeBoard(board,i+2*fmove,j-2,i-fmove,j+1,false);
							temp.score=eval(temp.board)+(9000000);
							if(!isSuicide(temp.board,i-fmove,j+1))
								temp.score+=(9000000);

							temp.isCannonMove=false;
							temp.changeCoordi=mp(mp(i+2*fmove,j-2),mp(i-fmove,j+1));
							child.pb(temp);
						}
						if(boardCell(i+3*fmove,j-3,n,m) && isColorSoldier(board,i+3*fmove,j-3,empBlock,empBlock))
						{
							temp.board=changeBoard(board,i,j,i+3*fmove,j-3,false);
							temp.score=eval(temp.board)+(9000000);
							if(!isSuicide(temp.board,i+3*fmove,j-3))
								temp.score+=(9000000);
							if(!isSuicide(temp.board,i+3*fmove,j-3) && isColorSoldier(board,i+6*fmove,j-6,oppTownCol,oppTownCol))
								temp.score+=(10000000);

							temp.isCannonMove=false;
							temp.changeCoordi=mp(mp(i,j),mp(i+3*fmove,j-3));
							child.pb(temp);
						}
					}
				}

				if(cnt<=1)
					continue;

				if(!cannonMoveTown)
				{
					if(boardCell(i+fmove, j, n, m) && !isColorSoldier(board,i+fmove,j,solCol,solCol))
					{
						bool locTown=false,locSoldier=false;
						temp.board=changeBoard(board,i,j,i+fmove,j,false);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i+fmove,j,oppTownCol,oppTownCol))
							temp.score+=(40000000),locTown=true;
						else
						{
							if(isColorSoldier(board,i+fmove,j,oppCol,oppCol) && !isSuicide(temp.board,i+fmove,j))
								temp.score+=(200000),locSoldier=true;
							if(isSuicide(temp.board,i+fmove,j))
								temp.score-=(2000000);
							/*if(isPartOfCannon)
								temp.score-=(900000);*/
						}

						temp.changeCoordi=mp(mp(i,j),mp(i+fmove,j));
						temp.isCannonMove=false;
						if((locTown || locSoldier) || (!(fwdMoveTown || fwdMoveSoldier)))
						{
							child.pb(temp);
							if(locTown) fwdMoveTown=true;
							if(locSoldier) fwdMoveSoldier=true;
						}
					}
					if(boardCell(i+fmove,j+1,n,m) && !isColorSoldier(board,i+fmove,j+1,solCol,solCol))
					{
						bool locTown=false,locSoldier=false;
						temp.board=changeBoard(board,i,j,i+fmove,j+1,false);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i+fmove,j+1,oppTownCol,oppTownCol))
							temp.score+=(40000000),locTown=true;
						else
						{
							if(isColorSoldier(board,i+fmove,j+1,oppCol,oppCol) && !isSuicide(temp.board,i+fmove,j+1))
								temp.score+=(200000),locSoldier=true;
							if(isSuicide(temp.board,i+fmove,j+1))
								temp.score-=(2000000);
							/*if(isPartOfCannon)
								temp.score-=(900000);*/
						}

						temp.changeCoordi=mp(mp(i,j),mp(i+fmove,j+1));
						temp.isCannonMove=false;
						if((locTown || locSoldier) || (!(fwdMoveTown || fwdMoveSoldier)))
						{
							child.pb(temp);
							if(locTown) fwdMoveTown=true;
							if(locSoldier) fwdMoveSoldier=true;
						}
					}
					if(boardCell(i+fmove,j-1,n,m) && !isColorSoldier(board,i+fmove,j-1,solCol,solCol))
					{
						bool locTown=false,locSoldier=false;
						temp.board=changeBoard(board,i,j,i+fmove,j-1,false);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i+fmove,j-1,oppTownCol,oppTownCol))
							temp.score+=(40000000),locTown=true;
						else
						{
							if(isColorSoldier(board,i+fmove,j-1,oppCol,oppCol) && !isSuicide(temp.board,i+fmove,j-1))
								temp.score+=(200000),locSoldier=true;
							if(isSuicide(temp.board,i+fmove,j-1))
								temp.score-=(2000000);
							/*if(isPartOfCannon)
								temp.score-=(900000);*/
						}

						temp.changeCoordi=mp(mp(i,j),mp(i+fmove,j-1));
						temp.isCannonMove=false;
						if((locTown || locSoldier) || (!(fwdMoveTown || fwdMoveSoldier)))
						{
							child.pb(temp);
							if(locTown) fwdMoveTown=true;
							if(locSoldier) fwdMoveSoldier=true;
						}
					}
				}

				
				if(!(cannonMoveTown || fwdMoveTown))
				{
					/*Sideways Capture Moves*/
					bool isSideMovePossible=false;
					if(isSoldier(i,j+1,n,m,oppCol,oppTownCol,board))
					{
						temp.board=changeBoard(board,i,j,i,j+1,false);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i,j+1,oppTownCol,oppTownCol))
							temp.score+=(40000000), sideMoveTown=true;
						else
						{
							if(isColorSoldier(board,i,j+1,oppCol,oppCol) && !isSuicide(temp.board,i,j+1))
								temp.score+=(20000);
							if(isSuicide(temp.board,i,j+1))
								temp.score-=(2000000);
							/*if(isPartOfCannon)
								temp.score-=(900000);*/
						}

						temp.changeCoordi=mp(mp(i,j),mp(i,j+1));
						temp.isCannonMove=false;
						child.pb(temp);
						isSideMovePossible=true;
					}
					if(isSoldier(i,j-1,n,m,oppCol,oppTownCol,board))
					{
						temp.board=changeBoard(board,i,j,i,j-1,false);
						temp.score=eval(temp.board);
						if(isColorSoldier(board,i,j-1,oppTownCol,oppTownCol))
							temp.score+=(40000000),sideMoveTown=true;
						else
						{
							if(isColorSoldier(board,i,j-1,oppCol,oppCol) && !isSuicide(temp.board,i,j-1))
								temp.score+=(20000);
							if(isSuicide(temp.board,i,j-1))
								temp.score-=(2000000);
							/*if(isPartOfCannon)
								temp.score-=(900000);*/
						}

						temp.changeCoordi=mp(mp(i,j),mp(i,j-1));
						temp.isCannonMove=false;
						child.pb(temp);
						isSideMovePossible=true;
					}

					/*Backward Moves*/
					if(isSideMovePossible && !sideMoveTown)
					{
						if(boardCell(i+bmove,j,n,m) && !isColorSoldier(board,i+bmove,j,solCol,solCol))
						{
							temp.board=changeBoard(board,i,j,i+bmove,j,false);
							temp.score=eval(temp.board);

							if(isSuicide(board,i,j))
								temp.score-=(200000);
							/*if(isPartOfCannon)
								temp.score-=(20000);*/
							temp.changeCoordi=mp(mp(i,j),mp(i+bmove,j));
							temp.isCannonMove=false;
							child.pb(temp);
						}
						if(boardCell(i+bmove,j-2,n,m) && !isColorSoldier(board,i+bmove,j-2,solCol,solCol))
						{
							temp.board=changeBoard(board,i,j,i+bmove,j-2,false);
							temp.score=eval(temp.board);
							if(isSuicide(board,i,j))
								temp.score-=(200000);
							/*if(isPartOfCannon)
								temp.score-=(20000);*/
							temp.changeCoordi=mp(mp(i,j),mp(i+bmove,j-2));
							temp.isCannonMove=false;
							child.pb(temp);
						}
						if(boardCell(i+bmove,j+2,n,m) && !isColorSoldier(board,i+bmove,j+2,solCol,solCol))
						{
							temp.board=changeBoard(board,i,j,i+bmove,j+2,false);
							temp.score=eval(temp.board);
							if(isSuicide(board,i,j))
								temp.score-=(200000);
							/*if(isPartOfCannon)
								temp.score-=(20000);*/
							temp.changeCoordi=mp(mp(i,j),mp(i+bmove,j+2));
							temp.isCannonMove=false;
							child.pb(temp);
						}
					}
				}
			}
		}
	}
}

void makeTree(node &root, int depth)
{
	if(depth>=maxDepthTree)
		return;

	allBranches(root.child, root.board);
	for(int i=0;i<root.child.size();i++)
	{
		makeTree(root.child[i],depth+1);
	}
}

pair<pair<double,bool>,pair<pii,pii> > miniMaxWithAlphaBetaPruning(node &root, int depth,int alpha, int beta, int maxDepth)
{
	if(depth==maxDepth)
		return mp(mp(eval(root.board),false),mp(mp(0,0),mp(0,0)));

	if(depth&1)
	{
		node newBoard;
		for(int i=0;i<root.child.size();i++)
		{
			double temp = miniMaxWithAlphaBetaPruning(root.child[i],depth+1,alpha,beta,maxDepth).fi.fi;

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
			double temp = miniMaxWithAlphaBetaPruning(root.child[i],depth+1,alpha,beta,maxDepth).fi.fi;

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
	
	makeTree(root,0);

	pair<pair<double,bool>,pair<pii,pii> > ans1=miniMaxWithAlphaBetaPruning(root,0,-inf,inf,1);
	pair<pair<double,bool>,pair<pii,pii> > ans2=miniMaxWithAlphaBetaPruning(root,0,-inf,inf,2);
	pair<pair<double,bool>,pair<pii,pii> > ans3=miniMaxWithAlphaBetaPruning(root,0,-inf,inf,3);
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