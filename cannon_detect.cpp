#include<bits/stdc++.h>

using namespace std;

struct cannon{
	vector<int> x_coor(3); 
	vector<int> y_coor(3);
};

vector<cannon> find_cannon(vector<vector<int> > board,int color){
	int m = board.size(), n= board[0].size();
	vector<cannon> cannon_list;
	cannon Cannon;

	for(int i=0;i<m;i++){
		for(int j=0;j<n;j++){
			if(board[i][j]==color){
				if(color==white){
						if(board[i+1][j-1]==white){
							if(board[i+2][j-2]==white){
								Cannon.x_coor = {j,j-1,j-2}
								Cannon.y_coor = {i,i+1,i+2}
								cannon_list.push_back(Cannon)
							}
						}
						else if(board[i+1][j]==white){
							if(board[i+2][j]==white){
								Cannon.x_coor = {j,j,j}
								Cannon.y_coor = {i,i+1,i+2}
								cannon_list.push_back(Cannon)								
							}
						}
						else if(board[i+1][j+1]==white){
							if(board[i+2][j+2]==white){
								Cannon.x_coor = {j,j+1,j+2}
								Cannon.y_coor = {i,i+1,i+2}
								cannon_list.push_back(Cannon)								
							}
						}
				}
				else if(color==black){
						if(board[i-1][j-1]==black){
							if(board[i-2][j-2]==black){
								Cannon.x_coor = {j,j-1,j-2}
								Cannon.y_coor = {i,i-1,i-2}
								cannon_list.push_back(Cannon)
							}
						}
						else if(board[i-1][j]==white){
							if(board[i-2][j]==white){
								Cannon.x_coor = {j,j,j}
								Cannon.y_coor = {i,i-1,i-2}
								cannon_list.push_back(Cannon)								
							}
						}
						else if(board[i-1][j+1]==white){
							if(board[i-2][j+2]==white){
								Cannon.x_coor = {j,j+1,j+2}
								Cannon.y_coor = {i,i-1,i-2}
								cannon_list.push_back(Cannon)								
							}
						}
				}				
			}
		}
	}

	return cannon_list;

}




