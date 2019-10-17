#include<fstream>
#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include<algorithm>
using std::vector;
using std::string;
using std::sort;

const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

enum class State {KEmpty, KObstacle, KClosed, KPath, kStart, kFinish};

std::vector<State> ParseLine(std::string line){
    std::istringstream sline(line);
    int n;
    char c;
    std::vector<State> row;
    while (sline>>n>>c && c ==','){
        if (n == 0 ) {
            row.push_back(State::KEmpty);
        }else {
            row.push_back(State::KObstacle);
        }
    }
    return row;
}

std::vector<std::vector<State>> ReadBoardFile(string path){
    std::ifstream myfile (path);
    vector<vector<State>> board{};
    if (myfile){
        string line;
        while (getline(myfile, line)){
            vector<State> row = ParseLine(line);
            board.push_back(row);
        }
    }    
    return board;
}

string CellString(State Cell){
    switch(Cell){
        case State:: KObstacle: return "⛰️   ";
        case State:: KPath: return     "🚗   ";
        case State:: kStart: return    "🚦   ";
        case State:: kFinish: return   "🏁   ";
        default: return                "  0    "   ;
    }
}

void printBoard(const vector<vector<State>> board){
    for (int i = 0; i < board.size(); i++){
        for (int j = 0; j < board[i].size(); j++){
            std::cout << CellString(board[i][j]);
        }
        std::cout << "\n";
    }
}

int Heuristic(int x1, int y1 , int x2, int y2){ // Calculate the manhattan distance 
    return abs(x2-x1) + abs(y2-y1);
}

void AddToOpen(int x, int y, int g, int h,  vector<vector<int>> &openlist, vector<vector<State>> &grid){
    openlist.push_back(vector<int>{x,y,g,h});
    grid[x][y]= State::KClosed;

}
bool Compare(const vector<int> node1, const vector<int> node2){
    return (node1[2]+node1[3]) > (node2[2]+node2[3]);
}

void Cellsort (vector<vector<int>> *v){
    sort (v->begin(),v->end(),Compare);
}

bool CheckValidCell(int x, int y, vector<vector<State>> &grid){
    bool on_grid_x = (x >= 0 && x < grid.size());
    bool on_grid_y = (y >= 0 && y < grid[0].size());
    if (on_grid_x && on_grid_y){
        return grid[x][y] == State::KEmpty;
    }
    return false;
    
}
void ExpandNeighbors(const vector<int> &current, int goal[2], vector<vector<int>> &openlist, vector<vector<State>> &grid){

    //get current node's data
    int x = current[0];
    int y = current[1];
    int g = current[2];

    //Loop through current node's potential neighbors 
    for (int i = 0; i < 4 ; i++){
        int x2 = x + delta[i][0];
        int y2 = y + delta[i][1];
    

    //Check the potential neighbors x2 and y2 values are on the grid and not closed 
        if (CheckValidCell(x2,y2,grid)){
            int g2 = g + 1;
            int h2 = Heuristic(x2,y2,goal[0], goal[1]);
            AddToOpen(x2,y2,g2,h2,openlist,grid);
        }
    }
}

//Implementation of A* search algorithm
vector<vector<State>> Search(vector<vector<State>> grid, int init[2], int goal[2]){
    vector<vector<int>> open {};
    
    int x = init[0];
    int y = init[1];
    int g = 0;
    int h = Heuristic(x,y,goal[0],goal[1]);
    AddToOpen(x,y,g,h,open,grid);
    while(open.size()>0){
        //get the next node 
        Cellsort(&open);
        auto current = open.back();
        open.pop_back();
        x = current[0];
        y = current[1];
        grid[x][y]= State::KPath;

        if (x == goal[0] && y == goal[1]) {
            grid[init[0]][init[1]] = State::kStart;
            grid[goal[0]][goal[1]] = State::kFinish;
            return grid;
        }
        //if we are not done, expand search to current node's neighbors
        ExpandNeighbors(current,goal,open,grid);
    }

    //we have run out of new nodes to explore and havent found a path.
    std::cout << "No path Found! \n";
    return std::vector<vector<State>> {}; 

}





int main () {
    int init[2]{0, 0};
    int goal[2]{4, 5};
    auto board = ReadBoardFile("1.board");
    auto solution = Search(board,init, goal);
    printBoard(solution);
}