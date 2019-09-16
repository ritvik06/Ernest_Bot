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
	return false;
}

//if at (i,j) there is an opponent
bool isOpponent(vector<vector<int> > &board,int i,int j,int col1,int col2)
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

double eval(vector<vector<int> > &board)
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
				if(boardCell(i+fmove,j,n,m) && isOpponent(board,i+fmove,j,oppCol,oppCol)) posOppAttkOnTH++;
				if(boardCell(i+fmove,j+1,n,m) && isOpponent(board,i+fmove,j+1,oppCol,oppCol)) posOppAttkOnTH++;
				if(boardCell(i+fmove,j-1,n,m) && isOpponent(board,i+fmove,j-1,oppCol,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+2*fmove,j,i+3*fmove,j,i+4*fmove,j,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+5*fmove,j,i+3*fmove,j,i+4*fmove,j,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+2*fmove,j-2,i+3*fmove,j-3,i+4*fmove,j-4,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+5*fmove,j-5,i+3*fmove,j-3,i+4*fmove,j-4,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+2*fmove,j+2,i+3*fmove,j+3,i+4*fmove,j+4,oppCol)) posOppAttkOnTH++;
				if(isCannon(board,i+5*fmove,j+5,i+3*fmove,j+3,i+4*fmove,j+4,oppCol)) posOppAttkOnTH++;
			}

			if(board[i][j]==oppTownCol)
			{
				if(boardCell(i-fmove,j,n,m) && isOpponent(board,i-fmove,j,solCol,solCol)) posAttkOnTH++;
				if(boardCell(i-fmove,j+1,n,m) && isOpponent(board,i-fmove,j+1,solCol,solCol)) posAttkOnTH++;
				if(boardCell(i-fmove,j-1,n,m) && isOpponent(board,i-fmove,j-1,solCol,solCol)) posAttkOnTH++;
				if(isCannon(board,i-2*fmove,j,i-3*fmove,j,i-4*fmove,j,solCol)) posOppAttkOnTH++;
				if(isCannon(board,i-5*fmove,j,i-3*fmove,j,i-4*fmove,j,solCol)) posOppAttkOnTH++;
				if(isCannon(board,i-2*fmove,j-2,i-3*fmove,j-3,i-4*fmove,j-4,solCol)) posOppAttkOnTH++;
				if(isCannon(board,i-5*fmove,j-5,i-3*fmove,j-3,i-4*fmove,j-4,solCol)) posOppAttkOnTH++;
				if(isCannon(board,i-2*fmove,j+2,i-3*fmove,j+3,i-4*fmove,j+4,solCol)) posOppAttkOnTH++;
				if(isCannon(board,i-5*fmove,j+5,i-3*fmove,j+3,i-4*fmove,j+4,solCol)) posOppAttkOnTH++;
			}

			if(board[i][j]==solCol)
			{
				if(boardCell(i+fmove,j,n,m) && isOpponent(board,i+fmove,j,oppCol,oppCol)) posAttk++;
				if(boardCell(i+fmove,j+1,n,m) && isOpponent(board,i+fmove,j+1,oppCol,oppCol)) posAttk++;
				if(boardCell(i+fmove,j-1,n,m) && isOpponent(board,i+fmove,j-1,oppCol,oppCol)) posAttk++;
				if(boardCell(i,j+1,n,m) && isOpponent(board,i,j+1,oppCol,oppCol)) posAttk++;
				if(boardCell(i,j-1,n,m) && isOpponent(board,i,j-1,oppCol,oppCol)) posAttk++;
				if(isCannon(board,i+2*fmove,j,i+3*fmove,j,i+4*fmove,j,oppCol)) posOppCannAttk++;/*Straight 2*/
				if(isCannon(board,i+5*fmove,j,i+3*fmove,j,i+4*fmove,j,oppCol)) posOppCannAttk++;
				if(isCannon(board,i+2*fmove,j-2,i+3*fmove,j-3,i+4*fmove,j-4,oppCol)) posOppCannAttk++;
				if(isCannon(board,i+5*fmove,j-5,i+3*fmove,j-3,i+4*fmove,j-4,oppCol)) posOppCannAttk++;
				if(isCannon(board,i+5*fmove,j+5,i+3*fmove,j+3,i+4*fmove,j+4,oppCol)) posOppCannAttk++;
				if(isCannon(board,i+2*fmove,j+2,i+3*fmove,j+3,i+4*fmove,j+4,oppCol)) posOppCannAttk++;
			}

			if(board[i][j]==oppCol)
			{
				if(boardCell(i-fmove,j,n,m) && isOpponent(board,i-fmove,j,solCol,solCol)) posOppAttk++;
				if(boardCell(i-fmove,j+1,n,m) && isOpponent(board,i-fmove,j+1,solCol,solCol)) posOppAttk++;
				if(boardCell(i-fmove,j-1,n,m) && isOpponent(board,i-fmove,j-1,solCol,solCol)) posOppAttk++;
				if(boardCell(i,j+1,n,m) && isOpponent(board,i,j+1,solCol,solCol)) posOppAttk++;
				if(boardCell(i,j-1,n,m) && isOpponent(board,i,j-1,solCol,solCol)) posOppAttk++;
				if(isCannon(board,i-2*fmove,j,i-3*fmove,j,i-4*fmove,j,solCol)) posCannAttk++;/*Straight 2*/
				if(isCannon(board,i-5*fmove,j,i-3*fmove,j,i-4*fmove,j,solCol)) posCannAttk++;
				if(isCannon(board,i-2*fmove,j-2,i-3*fmove,j-3,i-4*fmove,j-4,solCol)) posCannAttk++;
				if(isCannon(board,i-5*fmove,j-5,i-3*fmove,j-3,i-4*fmove,j-4,solCol)) posCannAttk++;
				if(isCannon(board,i-5*fmove,j+5,i-3*fmove,j+3,i-4*fmove,j+4,solCol)) posCannAttk++;
				if(isCannon(board,i-2*fmove,j+2,i-3*fmove,j+3,i-4*fmove,j+4,solCol)) posCannAttk++;
			}
		}
	}
	
	double func=posAttk - 30000*posOppAttk + 80*posCannAttk - 50*posOppCannAttk + 100*posAttkOnTH - 100*posOppAttkOnTH 
				+ numOfTH + 100000*(4-numOfOppTH) /*+ 30*loc */- 60000*locOpp + 800*(12-numOfOppSol);
	// double func=30*(numOfSol)-30*(posOppAttk)+100*posAttkOnTH+30*(8-numOfOppSol)+100*(4-numOfOppTH)+(20*numOfTH)+30*loc-30*locOpp;

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

void allBranches(vector<node> &child, vector<vector<int> > &board)
{
	int n=board.size(),m=board[0].size();

	int fmove,bmove;

	if(solCol==whiteSol)
		fmove=1,bmove=-2;
	else
		fmove=-1,bmove=2;

	/*for(int i=0;i<n;i++)
	{
		for(int j=0;j<m;j++)
			cerr<<board[i][j]<<" ";
		cerr<<endl;
	}*/

	for(int i=0;i<n;i++)
	{
		for(int j=0;j<m;j++)
		{
			if(board[i][j]==solCol)
			{
				node temp;
				/*Forward Moves*/
				if(boardCell(i+fmove, j, n, m) && !isOpponent(board,i+fmove,j,solCol,solCol))
				{
					temp.board=changeBoard(board,i,j,i+fmove,j,false);
					temp.score=eval(temp.board);
					temp.changeCoordi=mp(mp(i,j),mp(i+fmove,j));
					temp.isCannonMove=false;
					child.pb(temp);
				}
				if(boardCell(i+fmove,j+1,n,m) && !isOpponent(board,i+fmove,j+1,solCol,solCol))
				{
					temp.board=changeBoard(board,i,j,i+fmove,j+1,false);
					temp.score=eval(temp.board);
					temp.changeCoordi=mp(mp(i,j),mp(i+fmove,j+1));
					temp.isCannonMove=false;
					child.pb(temp);
				}
				if(boardCell(i+fmove,j-1,n,m) && !isOpponent(board,i+fmove,j-1,solCol,solCol))
				{
					temp.board=changeBoard(board,i,j,i+fmove,j-1,false);
					temp.score=eval(temp.board);
					temp.changeCoordi=mp(mp(i,j),mp(i+fmove,j-1));
					temp.isCannonMove=false;
					child.pb(temp);
				}
				

				/*Sideways Capture Moves*/
				bool isSideMovePossible=false;
				if(isSoldier(i,j+1,n,m,oppCol,oppTownCol,board))
				{
					// cerr<<"started -- "<<i<<" "<<j<<endl;
					temp.board=changeBoard(board,i,j,i,j+1,false);
					temp.score=eval(temp.board);
					temp.changeCoordi=mp(mp(i,j),mp(i,j+1));
					temp.isCannonMove=false;
					child.pb(temp);
					isSideMovePossible=true;
					// cerr<<"ended -- "<<i<<" "<<j<<endl;
				}
				if(isSoldier(i,j-1,n,m,oppCol,oppTownCol,board))
				{
					// cerr<<"started -- "<<i<<" "<<j<<endl;
					temp.board=changeBoard(board,i,j,i,j-1,false);
					temp.score=eval(temp.board);
					temp.changeCoordi=mp(mp(i,j),mp(i,j-1));
					temp.isCannonMove=false;
					child.pb(temp);
					isSideMovePossible=true;
					// cerr<<"ended -- "<<i<<" "<<j<<endl;
				}

				/*Backward Moves*/
				if(isSideMovePossible)
				{
					if(boardCell(i+bmove,j,n,m) && !isOpponent(board,i+bmove,j,solCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i+bmove,j,false);
						temp.score=eval(temp.board);
						temp.changeCoordi=mp(mp(i,j),mp(i+bmove,j));
						temp.isCannonMove=false;
						child.pb(temp);
					}
					if(boardCell(i+bmove,j-2,n,m) && !isOpponent(board,i+bmove,j-2,solCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i+bmove,j-2,false);
						temp.score=eval(temp.board);
						temp.changeCoordi=mp(mp(i,j),mp(i+bmove,j-2));
						temp.isCannonMove=false;
						child.pb(temp);
					}
					if(boardCell(i+bmove,j+2,n,m) && !isOpponent(board,i+bmove,j+2,solCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i+bmove,j+2,false);
						temp.score=eval(temp.board);
						temp.changeCoordi=mp(mp(i,j),mp(i+bmove,j+2));
						temp.isCannonMove=false;
						child.pb(temp);
					}
				}

				/*Cannon Moves*/
				/*Orthogonal Cannon*/
				if(isCannon(board,i,j,i+fmove,j,i+2*fmove,j,solCol))
				{
					if(boardCell(i+4*fmove,j,n,m) && !isOpponent(board,i+4*fmove,j,solCol,townCol) && !isOpponent(board,i+3*fmove,j,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i+4*fmove,j,true);
						temp.score=eval(temp.board);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j),mp(i+4*fmove,j));
						child.pb(temp);
					}
					if(boardCell(i+5*fmove,j,n,m) && !isOpponent(board,i+5*fmove,j,solCol,townCol) && !isOpponent(board,i+3*fmove,j,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i+5*fmove,j,true);
						temp.score=eval(temp.board);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j),mp(i+5*fmove,j));
						child.pb(temp);
					}
					if(boardCell(i-2*fmove,j,n,m) && !isOpponent(board,i-2*fmove,j,solCol,townCol) && !isOpponent(board,i-fmove,j,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i-2*fmove,j,true);
						temp.score=eval(temp.board);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-2*fmove,j));
						child.pb(temp);
					}
					if(boardCell(i-3*fmove,j,n,m) && !isOpponent(board,i-3*fmove,j,solCol,townCol) && !isOpponent(board,i-fmove,j,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i-3*fmove,j,true);
						temp.score=eval(temp.board);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-3*fmove,j));
						child.pb(temp);
					}
					if(boardCell(i-fmove,j,n,m) && isOpponent(board,i-fmove,j,empBlock,empBlock))
					{
						temp.board=changeBoard(board,i+2*fmove,j,i-fmove,j,false);
						temp.score=eval(temp.board);
						temp.isCannonMove=false;
						temp.changeCoordi=mp(mp(i+2*fmove,j),mp(i-fmove,j));
						child.pb(temp);
					}
					if(boardCell(i+3*fmove,j,n,m) && isOpponent(board,i+3*fmove,j,empBlock,empBlock))
					{
						temp.board=changeBoard(board,i,j,i+3*fmove,j,false);
						temp.score=eval(temp.board);
						temp.isCannonMove=false;
						temp.changeCoordi=mp(mp(i,j),mp(i+3*fmove,j));
						child.pb(temp);
					}
				}

				/*Right Diagonal Cannon*/
				if(isCannon(board,i,j,i+fmove,j+1,i+2*fmove,j+2,solCol))
				{
					if(boardCell(i+4*fmove,j+4,n,m) && !isOpponent(board,i+4*fmove,j+4,solCol,townCol) && !isOpponent(board,i+3*fmove,j+3,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i+4*fmove,j+4,true);
						temp.score=eval(temp.board);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j+2),mp(i+4*fmove,j+4));
						child.pb(temp);
					}
					if(boardCell(i+5*fmove,j+5,n,m) && !isOpponent(board,i+5*fmove,j+5,solCol,townCol) && !isOpponent(board,i+3*fmove,j+3,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i+5*fmove,j+5,true);
						temp.score=eval(temp.board);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j+2),mp(i+5*fmove,j+5));
						child.pb(temp);
					}
					if(boardCell(i-2*fmove,j-2,n,m) && !isOpponent(board,i-2*fmove,j-2,solCol,townCol) && !isOpponent(board,i-fmove,j-1,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i-2*fmove,j-2,true);
						temp.score=eval(temp.board);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-2*fmove,j-2));
						child.pb(temp);
					}
					if(boardCell(i-3*fmove,j-3,n,m) && !isOpponent(board,i-3*fmove,j-3,solCol,townCol) && !isOpponent(board,i-fmove,j-1,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i-3*fmove,j-3,true);
						temp.score=eval(temp.board);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-3*fmove,j-3));
						child.pb(temp);
					}
					if(boardCell(i-fmove,j-1,n,m) && isOpponent(board,i-fmove,j-1,empBlock,empBlock))
					{
						temp.board=changeBoard(board,i+2*fmove,j+2,i-fmove,j-1,false);
						temp.score=eval(temp.board);
						temp.isCannonMove=false;
						temp.changeCoordi=mp(mp(i+2*fmove,j+2),mp(i-fmove,j-1));
						child.pb(temp);
					}
					if(boardCell(i+3*fmove,j+3,n,m) && isOpponent(board,i+3*fmove,j+3,empBlock,empBlock))
					{
						temp.board=changeBoard(board,i,j,i+3*fmove,j+3,false);
						temp.score=eval(temp.board);
						temp.isCannonMove=false;
						temp.changeCoordi=mp(mp(i,j),mp(i+3*fmove,j+3));
						child.pb(temp);
					}
				}

				/*Left Diagonal Cannon*/
				if(isCannon(board,i,j,i+fmove,j-1,i+2*fmove,j-2,solCol))
				{
					if(boardCell(i+4*fmove,j-4,n,m) && !isOpponent(board,i+4*fmove,j-4,solCol,townCol) && !isOpponent(board,i+3*fmove,j-3,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i+4*fmove,j-4,true);
						temp.score=eval(temp.board);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j-2),mp(i+4*fmove,j-4));
						child.pb(temp);
					}
					if(boardCell(i+5*fmove,j-5,n,m) && !isOpponent(board,i+5*fmove,j-5,solCol,townCol) && !isOpponent(board,i+3	*fmove,j-3,oppCol,solCol))
					{ 
						temp.board=changeBoard(board,i,j,i+5*fmove,j-5,true);
						temp.score=eval(temp.board);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i+2*fmove,j-2),mp(i+5*fmove,j-5));
						child.pb(temp);
					}
					if(boardCell(i-2*fmove,j+2,n,m) && !isOpponent(board,i-2*fmove,j+2,solCol,townCol) && !isOpponent(board,i-fmove,j+1,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i-2*fmove,j+2,true);
						temp.score=eval(temp.board);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-2*fmove,j+2));
						child.pb(temp);
					}
					if(boardCell(i-3*fmove,j+3,n,m) && !isOpponent(board,i-3*fmove,j+3,solCol,townCol) && !isOpponent(board,i-fmove,j+1,oppCol,solCol))
					{
						temp.board=changeBoard(board,i,j,i-3*fmove,j+3,true);
						temp.score=eval(temp.board);
						temp.isCannonMove=true;
						temp.changeCoordi=mp(mp(i,j),mp(i-3*fmove,j+3));
						child.pb(temp);
					}
					if(boardCell(i-fmove,j+1,n,m) && isOpponent(board,i-fmove,j+1,empBlock,empBlock))
					{
						temp.board=changeBoard(board,i+2*fmove,j-2,i-fmove,j+1,false);
						temp.score=eval(temp.board);
						temp.isCannonMove=false;
						temp.changeCoordi=mp(mp(i+2*fmove,j-2),mp(i-fmove,j+1));
						child.pb(temp);
					}
					if(boardCell(i+3*fmove,j-3,n,m) && isOpponent(board,i+3*fmove,j-3,empBlock,empBlock))
					{
						temp.board=changeBoard(board,i,j,i+3*fmove,j-3,false);
						temp.score=eval(temp.board);
						temp.isCannonMove=false;
						temp.changeCoordi=mp(mp(i,j),mp(i+3*fmove,j-3));
						child.pb(temp);
					}
				}
			
			}
			// cerr<<board[i][j]<<" "<<i<<" "<<j<<endl;
		}
		// cerr<<endl;
	}

	// cerr<<"branches made"<<endl;
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
		cerr<<str<<endl;
		
		if(str[6]=='M')
			Board = changeBoard(Board,str[4]-'0',str[2]-'0',str[10]-'0',str[8]-'0',0);	
		else
			Board = changeBoard(Board,str[4]-'0',str[2]-'0',str[10]-'0',str[8]-'0',1);	
	}

	while(true)
	{
		string str;
		char t[6];
		cin>>t[0]>>t[1]>>t[2]>>t[3]>>t[4]>>t[5];
		
		if(t[3]=='M')
			Board = changeBoard(Board,(t[2]-'0'),(t[1]-'0'),(t[5]-'0'),(t[4]-'0'),0);
		else
			Board = changeBoard(Board,(t[2]-'0'),(t[1]-'0'),(t[5]-'0'),(t[4]-'0'),1);
		cerr<<"START\n";
		printBoard(Board);
		str = ErnestMove(Board) ;
		cout << str<<endl;
		cerr<<"Move "<<str<<endl;
		printBoard(Board);
		
		if(str[6]=='M')
			Board = changeBoard(Board,str[4]-'0',str[2]-'0',str[10]-'0',str[8]-'0',0);	
		else
			Board = changeBoard(Board,str[4]-'0',str[2]-'0',str[10]-'0',str[8]-'0',1);	
	}

	return 0;
}