#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <bits/stdc++.h>
#define DEPTH 3
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
	Point operator=(const Point& rhs) {
        x = rhs.x;
        y = rhs.y;
        return *this;
	}
};

int player;
const int SIZE = 8;
//std::array<std::array<int, SIZE>, SIZE> main_board;
std::array<std::array<int, SIZE>, SIZE> weight = {{
    {   4,  -3,  2,  2,  2,  2,  -3,  4},
    {  -3,  -4, -1, -1, -1, -1,  -4, -3},
    {   2,  -1,  1,  0,  0,  1,  -1,  2},
    {   2,  -1,  0,  1,  1,  0,  -1,  2},
    {   2,  -1,  9,  1,  1,  0,  -1,  2},
    {   2,  -1,  1,  0,  0,  2,  -1,  2},
    {  -3,  -4, -1, -1, -1, -1,  -4, -3},
    {   4,  -3,  2,  2,  2,  2,  -3,  4}
}};
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
    const std::array<Point, 4> xSquares {{
	    Point(1, 1), Point(1, SIZE-2),
	    Point(SIZE-2, 1), Point(SIZE-2, SIZE-2)
    }};
    const std::array<Point, 8> cSquares {{
	    Point(0, 1), Point(0, SIZE-2),
        Point(1, 0), Point(1, SIZE-2),
        Point(SIZE-2, 0), Point(SIZE-2, SIZE-1),
	    Point(SIZE-1, 1), Point(SIZE-1, SIZE-2)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    //bool done;
    //int winner;
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
    OthelloBoard() {
        //reset();
    }
    OthelloBoard operator=(const OthelloBoard& rhs) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = rhs.board[i][j];
            }
        }
        next_valid_spots = rhs.next_valid_spots;
        //done = rhs.done;
        //winner = rhs.winner;
        recommended_spot = rhs.recommended_spot;
        heuristic = rhs.heuristic;
        return *this;
	}
    void reset() {
//        for (int i = 0; i < SIZE; i++) {
//            for (int j = 0; j < SIZE; j++) {
//                board[i][j] = EMPTY;
//            }
//        }
//        board[3][4] = board[4][3] = BLACK;
//        board[3][3] = board[4][4] = WHITE;
//        cur_player = BLACK;
//        disc_count[EMPTY] = 8*8-4;
//        disc_count[BLACK] = 2;
//        disc_count[WHITE] = 2;
        //next_valid_spots = get_valid_spots();
        //done = false;
        //winner = -1;
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
            //winner = get_next_player(cur_player);
            //done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        // if (next_valid_spots.size() == 0) {
        //     cur_player = get_next_player(cur_player);
        //     next_valid_spots = get_valid_spots();
        //     if (next_valid_spots.size() == 0) {
        //         // Game ends
        //         done = true;
        //         int white_discs = disc_count[WHITE];
        //         int black_discs = disc_count[BLACK];
        //         if (white_discs == black_discs) winner = EMPTY;
        //         else if (black_discs > white_discs) winner = BLACK;
        //         else winner = WHITE;
        //     }
        // }
        set_heuristic();
        return true;
    }
    void set_heuristic() {
        // for(int i = 0; i < 4; i++) {
        //     Point c = corners[i];
        //     if(get_disc(c) == player) {
        //         Point p = xSquares[i];
        //         //weight[p.x][p.y] = abs(weight[p.x][p.y] * 2);
        //         p = cSquares[i*2+1];
        //     }
        // }

        int h = 0;
        int opponent = get_next_player(player);
        for(int i = 0; i < SIZE; i++) {
            for(int j = 0; j < SIZE; j++) {
                if(board[i][j] == player)
                    h += weight[i][j];
                else if(board[i][j] == opponent)
                    h -= weight[i][j];
            }
        }
        int mobi = next_valid_spots.size();
        if(cur_player == opponent)
            mobi *= -1;

        int j = 0;
        int stable = 0;
        if(disc_count[EMPTY] < 40) {
            for(int i = 0; i < 4; i++) {
                if(j == 6) {
                    j = 0;
                    continue;
                }
                Point cur = corners[i];
                for(j = 0; j < 6; j++) {
                    int cur_disc = get_disc(cur);
                    if(cur_disc != player)
                        break;
                    stable++;
                    //cur += dangerSpots[i];
                }
            }
            j = 0;
            for(int i = 0; i < 4; i++) {
                if(j == 6) {
                    j = 0;
                    continue;
                }
                Point cur = corners[i];
                for(j = 0; j < 6; j++) {
                    int cur_disc = get_disc(cur);
                    if(cur_disc != player)
                        break;
                    stable++;
                    //cur += dangerSpots[i];
                }
            }
            j = 0;
            for(int i = 0; i < 4; i++) {
                if(j == 6) {
                    j = 0;
                    continue;
                }
                Point cur = corners[i];
                for(j = 0; j < 6; j++) {
                    int cur_disc = get_disc(cur);
                    if(cur_disc != opponent)
                        break;
                    stable--;
                    //cur += dangerSpots[i];
                }
            }
            j = 0;
            for(int i = 0; i < 4; i++) {
                if(j == 6) {
                    j = 0;
                    continue;
                }
                Point cur = corners[i];
                for(j = 0; j < 6; j++) {
                    int cur_disc = get_disc(cur);
                    if(cur_disc != opponent)
                        break;
                    stable--;
                    //cur += dangerSpots[i];
                }
            }

            int flip = disc_count[player] - disc_count[opponent];
            if(disc_count[EMPTY] > 40)
                heuristic = h*1 + mobi*10 + stable*10 + flip*-1;
            else if(disc_count[EMPTY] > 20)
                heuristic = h*1 + mobi*10 + stable*10 + flip*0;
            else
                heuristic = h*1 + mobi*10 + stable*10 + flip*2;

            // for(int i = 0; i < 4; i++) {
            //     Point c = corners[i];
            //     if(get_disc(c) == player) {
            //         Point p = xSquares[i];
            //         //weight[p.x][p.y] = -abs(weight[p.x][p.y] / 2);
            //         //p = cSquares[i*2+1];
            //     }
            // }
        }
        //std::cout << "HEURISTIC " << heuristic << std::endl;
    }
};

OthelloBoard main_board;

void read_board(std::ifstream& fin) {
    fin >> player;
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
    std::cout << "HEURISTIC INITIAL " << main_board.heuristic << std::endl;
}

int minimax (OthelloBoard &curr_board, int depth, int alpha, int beta, bool isMaximizingPlayer) {
    int value;
    if(next_valid_spots.size() == 0 || depth == 0) {
        value = curr_board.heuristic;
        //std::cout << "VALUEEEEE " << value << std::endl;
        return value;
    }

    if(isMaximizingPlayer) {
        value = MIN;
        for(auto spot : next_valid_spots) {
            OthelloBoard next_board = curr_board;
            next_board.put_disc(spot);
            int child = minimax(next_board, depth-1, alpha, beta, false);
            if(child > value) {
                value = child;
                curr_board.recommended_spot = spot;
                alpha = std::max(alpha, value);
                if(alpha >= beta)
                    break;
                 if(depth == DEPTH) {
                     //main_board.heuristic = value;
                     std::cout << "THE PATH OUT MAXI \n";
                     std::cout << spot.x << " " << spot.y << std::endl;
                     std::cout << "HEURISTIC " << value << std:: endl;
                     // fout << spot.x << " " << spot.y << std::endl;
                     // fout.flush();
                 }
            }
        }
    }
    else {
        value = MAX;
        for(auto spot : next_valid_spots) {
            OthelloBoard next_board = curr_board;
            next_board.put_disc(spot);
            int child = minimax(next_board, depth-1, alpha, beta, true);
            if(child < value) {
                value = child;
                beta = std::min(beta, value);
                if(beta <= alpha)
                    break;
                //curr_board.recommended_spot = spot;
                //std::cout << "HEURISTIC DECIDED " << main_board.heuristic << std::endl;
                // if(depth == DEPTH) {
                //     //main_board.heuristic = value;
                //     // std::cout << "THE PATH OUT MINIMUM\n";
                //     // std::cout << spot.x << " " << spot.y << std::endl;
                //     // fout << spot.x << " " << spot.y << std::endl;
                //     // fout.flush();
                // }
            }
        }
    }
    //std::cout << "HEURISTIC DECIDED " << value << std::endl;
    return value;
}

void write_valid_spot(std::ofstream& fout) {
/*
    int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    // Choose random spot. (Not random uniform here)
    int index = (rand() % n_valid_spots);
    Point p = next_valid_spots[index];
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
*/

    srand(time(NULL));
    //int child = MIN;
    int value;
    Point p;
    if(next_valid_spots.size() == 1) {
        p = next_valid_spots[0];
        // std::cout << "THE PATH\n";
        // std::cout << p.x << " " << p.y << std::endl;
    }
    else {
        int alpha = MIN;
        int beta = MAX;
        minimax(main_board, DEPTH, alpha, beta, true);
        std::cout << "HEURISTIC LAST " << main_board.heuristic << std::endl;
        p = main_board.recommended_spot;
        // for(auto spot : next_valid_spots) {
        //     std::cout << "MINIMAX\n";
        //     std::cout << spot.x << " " << spot.y << std::endl;
        //     child = minimax(spot, DEPTH, true);
        //     std::cout << child << std::endl;
        //     if(child > value) {
        //         value = child;
        //         p = spot;
        //         std::cout << "THE PATH\n";
        //         std::cout << p.x << " " << p.y << std::endl;
        //         fout << p.x << " " << p.y << std::endl;
        //         fout.flush();
        //     }
        // }
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
