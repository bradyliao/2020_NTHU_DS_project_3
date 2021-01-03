#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "../include/algorithm.h"
#include "../include/rules.h"


using namespace std;

#define ROW 5
#define COL 6
#define INF 10000
#define DEPTH 4

/******************************************************
 * In your algorithm, you can just use the the funcitons
 * listed by TA to get the board information.(functions 
 * 1. ~ 4. are listed in next block)
 * 
 * The STL library functions is not allowed to use.
******************************************************/

/*************************************************************************
 * 1. int board.get_orbs_num(int row_index, int col_index)
 * 2. int board.get_capacity(int row_index, int col_index)
 * 3. char board.get_cell_color(int row_index, int col_index)
 * 4. void board.print_current_board(int row_index, int col_index, int round)
 * 
 * 1. The function that return the number of orbs in cell(row, col)
 * 2. The function that return the orb capacity of the cell(row, col)
 * 3. The function that return the color fo the cell(row, col)
 * 4. The function that print out the current board statement
*************************************************************************/


struct Move
{
    int score ;
    int r ;
    int c ;
} ;







class Process_Board{
    public:
    Cell cells[ROW][COL];                       // The 5*6 board whose index (0,0) is start from the upper left corner
    void cell_reaction_marker();                // After the explosion, mark all the cell that  will explode in next iteration
    bool cell_is_full(Cell* cell);              // Check wether the cell is full of orbs and set the explosion variable to be true
    void add_orb(int i, int j, char color);     // Add orb into the cell (i, j)
    void cell_reset(int i, int j);              // Reset the cell to the initial state (color = 'w', orb_num = 0)
    void cell_explode(int i, int j);            // The explosion of cell (i, j). It will add the orb into neighbor cells
    void cell_chain_reaction(Player player);    // If there is aa explosion, check wether the chain reaction is active

    public:
    //Process_Board();
    Process_Board(Board board) ;
        
    // The basic functions of the Board
    int get_orbs_num(int i, int j);
    int get_capacity(int i, int j);
    char get_cell_color(int i, int j);
    bool place_orb(int i, int j, Player* player);      // Use this function to place a orb into a cell
    bool win_the_game(Player player);                  // The function that is used to check wether the player wins the game after his/her placemnet operation
    
public:     // my own function
    void my_place_orb(int i, int j, char currentPlayerColor) ;
    void my_cell_chain_reaction(char currentPlayerColor) ;
    bool my_win_the_game(char currentPlayerColor) ;
    int my_total_orbs(char currentPlayerColor) ;
    void my_print_current_board() ;
};

/*
Process_Board::Process_Board(){

    ////// Initialize the borad with correct capacity //////
    // The corners of the board
    cells[0][0].set_capacity(3), cells[0][5].set_capacity(3),
    cells[4][0].set_capacity(3), cells[4][5].set_capacity(3);

    // The edges of the board
    cells[0][1].set_capacity(5), cells[0][2].set_capacity(5), cells[0][3].set_capacity(5), cells[0][4].set_capacity(5),
    cells[1][0].set_capacity(5), cells[2][0].set_capacity(5), cells[3][0].set_capacity(5),
    cells[1][5].set_capacity(5), cells[2][5].set_capacity(5), cells[3][5].set_capacity(5),
    cells[4][1].set_capacity(5), cells[4][2].set_capacity(5), cells[4][3].set_capacity(5), cells[4][4].set_capacity(5);

}
*/

Process_Board::Process_Board(Board board)
{
    for(int r = 0; r < ROW; r++)
    {
        for (int c = 0; c < COL; c++)
        {
            cells[r][c].set_color(board.get_cell_color(r,c)) ;
            cells[r][c].set_capacity(board.get_capacity(r,c));
            cells[r][c].set_orbs_num(board.get_orbs_num(r,c));
        }
    }
}


bool Process_Board::place_orb(int i, int j, Player * player){
    
    if(!index_range_illegal(i, j) && !placement_illegal(*player, cells[i][j])){
        int temp = cells[i][j].get_orbs_num();
        temp += 1;
        cells[i][j].set_orbs_num(temp);
        cells[i][j].set_color(player->get_color());
    }
    else{
        player->set_illegal();
        return false;
    }

    if(cell_is_full(&cells[i][j])){
        cell_explode(i, j);
        cell_reaction_marker();
        cell_chain_reaction(*player);
    }

    return true;
}


bool Process_Board::cell_is_full(Cell* cell){
    if(cell->get_orbs_num() >= cell->get_capacity()){
        cell->set_explode(true);
        return true;
    }
    else return false;
}


void Process_Board::add_orb(int i, int j, char color){
    int orb_num = cells[i][j].get_orbs_num();
    orb_num ++;
    cells[i][j].set_orbs_num(orb_num);
    cells[i][j].set_color(color);
}


void Process_Board::cell_reset(int i, int j){
    cells[i][j].set_orbs_num(0);
    cells[i][j].set_explode(false);
    cells[i][j].set_color('w');
}


void Process_Board::cell_explode(int i, int j){

    int orb_num;
    char color = cells[i][j].get_color();

    cell_reset(i, j);

    if( i + 1 < ROW){
        add_orb(i+1, j, color);
    }

    if( j + 1 < COL){
        add_orb(i, j+1, color);
    }

    if( i - 1 >= 0){
        add_orb(i-1, j, color);
    }

    if( j - 1 >= 0){
        add_orb(i, j-1, color);
    }
    if( i + 1 < ROW && j - 1 >= 0){
        add_orb(i+1, j-1, color);
    }
    if( i - 1 >= 0 && j - 1 >= 0){
        add_orb(i-1, j-1, color);
    }
    if( i + 1 < ROW && j + 1 < COL){
        add_orb(i+1, j+1, color);
    }
    if( i - 1 >= 0 && j + 1 < COL){
        add_orb(i-1, j+1, color);
    }
}


void Process_Board::cell_reaction_marker(){

    // Mark the next cell whose number of orbs is equal to the capacity
    for(int i = 0; i < ROW; i++){
            for(int j = 0; j < COL; j++){
                cell_is_full(&cells[i][j]);
            }
        }
}


void Process_Board::cell_chain_reaction(Player player){
    
    bool chain_reac = true;

    while(chain_reac){

        chain_reac = false;

        for(int i = 0; i < ROW; i++){
            for(int j = 0; j < COL; j++){
                if(cells[i][j].get_explode()){
                    cell_explode(i ,j);
                    chain_reac = true;
                }
            }
        }

        if(win_the_game(player)){
            return;
        }

        cell_reaction_marker();
    }
}


bool Process_Board::win_the_game(Player player){

    char player_color = player.get_color();
    bool win = true;

    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COL; j++){
            if(cells[i][j].get_color() == player_color || cells[i][j].get_color() == 'w') continue;
            else{
                win = false;
                break;
            }
        }
        if(!win) break;
    }
    return win;
}


int Process_Board::get_orbs_num(int i, int j){
    return cells[i][j].get_orbs_num();
}


int Process_Board::get_capacity(int i, int j){
    return cells[i][j].get_capacity();
}


char Process_Board::get_cell_color(int i, int j){
    return cells[i][j].get_color();
}


    
    





// my own function


void Process_Board::my_place_orb(int i, int j, char currentPlayerColor){
    cells[i][j].set_orbs_num( cells[i][j].get_orbs_num()+1 ) ;

    if(cell_is_full(&cells[i][j])){
        cell_explode(i, j);
        cell_reaction_marker();
        cell_chain_reaction(currentPlayerColor);
    }
}


void Process_Board::my_cell_chain_reaction(char currentPlayerColor){
    
    bool chain_reac = true;

    while(chain_reac){

        chain_reac = false;

        for(int i = 0; i < ROW; i++){
            for(int j = 0; j < COL; j++){
                if(cells[i][j].get_explode()){
                    cell_explode(i ,j);
                    chain_reac = true;
                }
            }
        }

        if(win_the_game(currentPlayerColor)){
            return;
        }

        cell_reaction_marker();
    }
}


bool Process_Board::my_win_the_game(char currentPlayerColor){

    
    bool win = true;

    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COL; j++){
            if(cells[i][j].get_color() == currentPlayerColor || cells[i][j].get_color() == 'w') continue;
            else{
                win = false;
                break;
            }
        }
        if(!win) break;
    }
    return win;
}


int Process_Board::my_total_orbs(char currentPlayerColor)
{
    int count = 0 ;
    
    for(int i = 0; i < ROW; i++)
        for(int j = 0; j < COL; j++)
            if(cells[i][j].get_color() == currentPlayerColor)
                count += cells[i][j].get_orbs_num() ;

    return count ;
}


void Process_Board::my_print_current_board(){

    int orb_num;
    char symbol;



    cout << "=============================================================" << endl;
    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COL; j++){

            symbol = cells[i][j].get_color();
            switch(symbol){
                case 'r':
                    symbol = 'O';
                    break;
                case 'b':
                    symbol = 'X';
                    break;
                default:
                    break;
            }

            orb_num = cells[i][j].get_orbs_num();
            switch(orb_num){
                case 0:
                    cout << "|       | ";
                    break;
                case 1:
                    cout << "|" << symbol << "      | ";
                    break;
                case 2:
                    cout << "|" << symbol << symbol << "     | ";
                    break;
                case 3:
                    cout << "|" << symbol << symbol << symbol << "    | ";
                    break;
                case 4:
                    cout << "|" << symbol << symbol << symbol << symbol << "   | ";
                    break;
                case 5:
                    cout << "|" << symbol << symbol << symbol << symbol << symbol << "  | ";
                    break;
                case 6:
                    cout << "|" << symbol << symbol << symbol << symbol << symbol << symbol << " | ";
                    break;
                default:
                    cout << "|" << symbol << symbol << symbol << symbol << symbol << symbol << symbol << "| ";
                    break;

            }
        }
        cout << endl;
    }
    cout << "=============================================================" << endl << endl;
}
































 Move miniMax(Process_Board currentBoard, int depth, Player player, char currentPlayerColor)
{
    //currentBoard.my_print_current_board() ;
    
    
    char nextPlayerColor = (currentPlayerColor == 'r') ? 'b' : 'r';
    Move bm ;
    bm.r = -1 ;
    bm.c = -1 ;
    
    
    if (depth == 0)
    {
        //cout << 3 ;
        bm.score = currentBoard.my_total_orbs(player.get_color()) ;
        return bm ;
    }
    
    
     
    
    
    
    if (player.get_color() == currentPlayerColor)
    {
        //cout << 1 ;
        if (currentBoard.my_win_the_game(currentPlayerColor) && currentBoard.my_total_orbs(currentPlayerColor) > 1)
        {
            bm.score = INF ;
            return bm ;
        }
        
        bm.score = -INF ;
        for (int r = 0; r < ROW; r++)
        {
            for (int c = 0; c < COL; c++)
            {
                if (currentBoard.get_cell_color(r, c) != nextPlayerColor)
                {
                    Move cm ;
                    cm.score = -INF ; //cm.r = r ; cm.c = c ;
                    
                    Process_Board nextBoard(currentBoard) ;
                    nextBoard.my_place_orb(r /*cm.r*/, c /*cm.c*/, currentPlayerColor) ;
                    
                    cm = miniMax(nextBoard, depth - 1, player, nextPlayerColor) ;
                    
                    if (cm.score > bm.score)
                    {
                        bm.score = cm.score ;
                        bm.r = r ; /*cm.r*/
                        bm.c = c ; /*cm.c*/
                    }
                }
            }
        }
        return bm ;
    }
    
    else
    {
        //cout << 2 ;
        if (currentBoard.my_win_the_game(currentPlayerColor) && currentBoard.my_total_orbs(currentPlayerColor) > 1)
        {
            bm.score = -INF ;
            return bm ;
        }
        
        bm.score = INF ;
        for (int r = 0; r < ROW; r++)
        {
            for (int c = 0; c < COL; c++)
            {
                if (currentBoard.get_cell_color(r, c) != nextPlayerColor)
                {
                    Move cm ;
                    cm.score = INF ;// cm.r = r ; cm.c = c ;
                    
                    Process_Board nextBoard(currentBoard) ;
                    nextBoard.my_place_orb(r /*cm.r*/, c /*cm.c*/, currentPlayerColor) ;
                    
                    cm = miniMax(nextBoard, depth - 1, player, nextPlayerColor) ;
                    
                    if (cm.score < bm.score)
                    {
                        bm.score = cm.score ;
                        bm.r = r ; /*cm.r*/
                        bm.c = c ; /*cm.c*/
                    }
                }
            }
        }
        return bm ;
        
        
        
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
}

 
















































void algorithm_A(Board board, Player player, int index[]){

    //your algorithm design//
    Process_Board processBoard(board) ;
    Move bestMove = miniMax(processBoard, DEPTH, player, player.get_color()) ;
    index[0] = bestMove.r ;
    index[1] = bestMove.c ;
    
    
    
    
}




