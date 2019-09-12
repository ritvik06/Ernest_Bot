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

struct node
{
	vector<vector<int> > board;
	double score;
	vector<node> child;
	pair<pii,pii> changeCoordi;
	bool isCannonMove;
};

bool boardCell(int i, int j, int n, int m)
{
	if(i>=0 && j>=0 && i<n && j<m) return true;
	return false;
}

bool isSoldier(int i, int j, int n, int m, int oppCol, int oppTownCol, vector<vector<int> > &board)
{
	if(i>=0 && i<n && j>=0 && j<m)
	{
		if(board[i][j]==oppCol || board[i][j]==oppTownCol)
			return true;
	}
	return true;
}

bool isOpponent(vector<vector<int> > &board,int i,int j,int oppCol)
{
	return (board[i][j]==oppCol);
}

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
			}
			
			if(board[i][j]==oppTownCol)
			{
				if(boardCell(i-fmove,j,n,m) && isOpponent(board,i-fmove,j,oppCol)) posAttkOnTH++;
				if(boardCell(i-fmove,j+1,n,m) && isOpponent(board,i-fmove,j+1,oppCol)) posAttkOnTH++;
				if(boardCell(i-fmove,j-1,n,m) && isOpponent(board,i-fmove,j-1,oppCol)) posAttkOnTH++;
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

	double func=(numOfSol)-posOppAttk-posOppAttkOnTH+posAttkOnTH;

	return func;
}

bool isCannon(vector<vector<int> > &board,int a1,int b1,int a2,int b2,int a3,int b3,int n, int m)
{
	if(!(boardCell(a1,b1,n,m) && boardCell(a2,b2,n,m) && boardCell(a3,b3,n,m)))
		return false;

	if(!(board[a1][b1]==solCol && board[a2][b2]==solCol && board[a3][b3]))
		return false;

	return true;
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
				if(isCannon(board,i,j,i+fmove,j,i+2*fmove,j,n,m))
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
				if(isCannon(board,i,j,i+fmove,j+1,i+2*fmove,j+2,n,m))
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
				if(isCannon(board,i,j,i+fmove,j-1,i+2*fmove,j-2,n,m))
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
