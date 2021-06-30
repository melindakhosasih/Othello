#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <bits/stdc++.h>
#define MAX INT_MAX
#define MIN INT_MIN

struct Point {
    int x, y;
	Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

int player;
int DEPTH;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> weight = {{
    {   10,  -8,  3,  3,  3,  3,  -8,  10},
    {  -8, -10, -1, -1, -1, -1, -10, -8},
    {   3,  -1,  2,  0,  0,  2,  -1,  3},
    {   3,  -1,  0,  1,  1,  0,  -1,  3},
    {   3,  -1,  0,  1,  1,  0,  -1,  3},
    {   3,  -1,  2,  0,  0,  2,  -1,  3},
    {  -8, -10, -1, -1, -1, -1, -10, -8},
    {   10,  -8,  3,  3,  3,  3,  -8,  10}
}};
std::array<std::array<int, SIZE>, SIZE> weight_copy;
std::vector<Point> next_valid_spots;
std::vector<Point> next_recommended_spots;

class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    const std::array<Point, 4> corners {{
        Point(0, 0), Point(0, SIZE-1),
        Point(SIZE-1, 0), Point(SIZE-1, SIZE-1)
    }};
    const std::array<Point, 8> wall {{
        Point(1, 0), Point(0, 1),
        Point(1, 0), Point(0, -1),
        Point(-1, 0), Point(0, 1),
        Point(-1, 0), Point(0, -1)
    }};
    const std::array<Point, 4> xSquares {{
	    Point(1, 1), Point(1, SIZE-2),
	    Point(SIZE-2, 1), Point(SIZE-2, SIZE-2)
    }};
    const std::array<Point, 8> cSquares {{
	    Point(0, 1), Point(1, 0),
        Point(0, SIZE-2), Point(1, SIZE-2),
        Point(SIZE-2, 0), Point(SIZE-1, 1),
	    Point(SIZE-2, SIZE-1), Point(SIZE-1, SIZE-2)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    int heuristic;
    Point recommended_spot;
private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player))) //check if the opponent's piece between line exist or not
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) { //check if the line can be made
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    OthelloBoard() {}
    OthelloBoard operator=(const OthelloBoard& rhs) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = rhs.board[i][j];
            }
        }
        next_valid_spots = rhs.next_valid_spots;
        recommended_spot = rhs.recommended_spot;
        heuristic = rhs.heuristic;
        return *this;
	}
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        set_heuristic();
        return true;
    }
    void set_heuristic() {
        weight_copy = weight;
        for(int i = 0; i < 4; i++) {
            Point loc = corners[i];
            if(get_disc(loc) == player) {
                bool next_corner;
                Point p = xSquares[i];
                weight_copy[p.x][p.y] = 6;
                p = cSquares[i*2];
                weight_copy[p.x][p.y] = 10;
                p = cSquares[i*2+1];
                weight_copy[p.x][p.y] = 10;
            }
        }

        int total_weight = 0;
        int opponent = get_next_player(player);
        int i = 0;
        while(i < 64) {
            if(board[i/8][i%8] == player)
                total_weight += weight_copy[i/8][i%8];
            else if(board[i/8][i%8] == opponent)
                total_weight -= weight_copy[i/8][i%8];
            i++;
        }
        // for(int i = 0; i < SIZE; i++) {
        //     for(int j = 0; j < SIZE; j++) {
        //         if(board[i][j] == player)
        //             total_weight += weight_copy[i][j];
        //         else if(board[i][j] == opponent)
        //             total_weight -= weight_copy[i][j];
        //     }
        // }

        int stable = 0;
        if(disc_count[EMPTY] < 44) {
            //player
            std::array<std::array<int, SIZE>, SIZE> been;
            for(int i = 0; i < 4; i++) {
                int idx = 0;
                Point loc = corners[i];
                while(idx < 8) { //vertical
                    if(get_disc(loc) != player || been[loc.x][loc.y] == player)
                        break;
                    been[loc.x][loc.y] = player;
                    loc = loc + wall[i*2];
                    stable++;
                    idx++;
                }
                idx = 0;
                loc = corners[i];
                while(idx < 8) { //horizontal
                    if(get_disc(loc) != player || been[loc.x][loc.y] == player)
                        break;
                    been[loc.x][loc.y] = player;
                    loc = loc + wall[i*2+1];
                    stable++;
                    idx++;
                }
            }
            //oppponent
            for(int i = 0; i < 4; i++) {
                int idx = 0;
                Point loc = corners[i];
                while(idx < 8) { //vertical
                    if(get_disc(loc) != opponent || been[loc.y][loc.y] == opponent)
                        break;
                    been[loc.x][loc.y] = opponent;
                    loc = loc + wall[i*2];
                    stable--;
                    idx++;
                }
                idx = 0;
                loc = corners[i];
                while(idx < 8) { //horizontal
                    if(get_disc(loc) != opponent || been[loc.y][loc.y] == opponent)
                        break;
                    been[loc.x][loc.y] = opponent;
                    loc = loc + wall[i*2+1];
                    stable--;
                    idx++;
                }
            }
        }
        
        int mobi = next_valid_spots.size();
        if(cur_player == opponent)
            mobi *= -1;

        int flip = disc_count[player] - disc_count[opponent];
        if(disc_count[EMPTY] > 44)
            heuristic = total_weight*5 + mobi*6 - flip;
        else if(disc_count[EMPTY] > 20)
            heuristic = total_weight*5 + mobi*6 + stable*5;
        else
            heuristic = total_weight*3 + mobi*6 + stable*5 + flip;
        //std::cout << "HEURISTIC " << heuristic << std::endl;
    }
};

OthelloBoard main_board;

void read_board(std::ifstream& fin) {
    fin >> player;
    if (player == 1) DEPTH = 5;
    else if(player == 2) DEPTH = 5;
    main_board.cur_player = player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> main_board.board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
    main_board.next_valid_spots = next_valid_spots;
    main_board.set_heuristic();
}

int minimax (OthelloBoard &curr_board, int depth, int alpha, int beta, bool isMaximizingPlayer) {
    int value;
    if(curr_board.next_valid_spots.size() == 0 || depth == 0) {
        return curr_board.heuristic;
        //std::cout << "VALUEEEEE " << curr_board.heuristic << std::endl;
    }

    if(isMaximizingPlayer) {
        value = MIN;
        //std::cout << "SPOTTTT " << curr_board.next_valid_spots.size() << std::endl;
        if(curr_board.next_valid_spots.size() == 0)
            return curr_board.heuristic;
        for(auto spot : curr_board.next_valid_spots) {
            OthelloBoard next_board = curr_board;
            next_board.put_disc(spot);
            int child = minimax(next_board, depth-1, alpha, beta, false);
            if(child > value) {
                curr_board.recommended_spot = spot;
            }
            value = std::max(value, child);
            alpha = std::max(alpha, value);
            if(alpha >= beta)
                break;
        }
    }
    else {
        value = MAX;
        if(curr_board.next_valid_spots.size() == 0)
            return curr_board.heuristic;
        for(auto spot : curr_board.next_valid_spots) {
            OthelloBoard next_board = curr_board;
            next_board.put_disc(spot);
            int child = minimax(next_board, depth-1, alpha, beta, true);
            value = std::min(value, child);
            beta = std::min(beta, value);
            if(beta <= alpha)
                break;
        }
    }
    //std::cout << "HEURISTIC DECIDED " << value << std::endl;
    return value;
}

void write_valid_spot(std::ofstream& fout) {
    srand(time(NULL));
    int value;
    Point p;
    if(next_valid_spots.size() == 1) {
        p = next_valid_spots[0];
    }
    else {
        int alpha = MIN;
        int beta = MAX;
        minimax(main_board, DEPTH, alpha, beta, true);
        p = main_board.recommended_spot;
    }
    // Choose random spot. (Not random uniform here)
    // Remember to flush the output to ensure the last action is written to file.
    std::cout << "REAL OUPUT\n";
    std::cout << p.x << " " << p.y << std::endl;
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}