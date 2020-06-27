#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

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
const int SIZE = 8;
const int DEPTH = 3;
const int MIN = -9999;
const int MAX = 9999;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;
std::array<Point, 8> directions{{
    Point(-1, -1), Point(-1, 0), Point(-1, 1),
    Point(0, -1), /*{0, 0},  */Point(0, 1),
    Point(1, -1), Point(1, 0), Point(1, 1)
}};

int score (std::array<std::array<int, SIZE>, SIZE> brd, int validSpots, int plyer) {
    int val = validSpots * 1;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if ((i == 0 || i == SIZE - 1) && (j == 0 || j == SIZE - 1)) {
                if (brd[i][j] == plyer) val += 50;
                else if (brd[i][j] == 3 - plyer) val -= 40;
            } else if ((i + j == 1) || (i + j == 13) || abs(i - j) == 6 || ((i == 1 || i == 6) && (j == 1 || j == 6))) {
                if (brd[i][j] == plyer) val -= 40;
                else if (brd[i][j] == 3 - plyer) val += 30;
            } else if (((i == 0 || i == 1 || i == 2 || i == 5 || i == 6 || i == 7) && (j == 2 || j == 5)) ||
                       ((i == 2 || i == 5) && (j == 0 || j == 1 || j == 2 || j == 5 || j == 6 || j == 7))) {
                if (brd[i][j] == plyer) val = val + 20 + abs(i - j) * 2;
            } else if (i == 0 || i == SIZE - 1 || j == 0 || j == SIZE - 1) {
                if (brd[i][j] == plyer) val += 20;
                else if (brd[i][j] == 3 - plyer) val -= 10;
            }

            if (brd[i][j] == plyer) val += 1;
            else if (brd[i][j] == 3 - plyer) val -= 1;
        }
    }
    if ((brd[0][0] == plyer && (brd[0][2] == plyer || brd[1][1] == plyer || brd[2][0] == plyer)) || 
        (brd[0][7] == plyer && (brd[0][6] == plyer || brd[1][6] == plyer || brd[1][7] == plyer)) ||
        (brd[7][0] == plyer && (brd[6][0] == plyer || brd[6][1] == plyer || brd[7][1] == plyer)) ||
        (brd[7][7] == plyer && (brd[6][7] == plyer || brd[6][6] == plyer || brd[7][6] == plyer)))
        val += 50;

    return val;
}

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back(Point(x, y));
    }
}
bool is_spot_on_board(Point p) {
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
}
bool is_disc_at(Point p, int disc) {
    if (!is_spot_on_board(p))
        return false;
    if (board[p.x][p.y] != disc)
        return false;
    return true;
}
bool is_spot_valid(Point center, int plyr) {
    if (board[center.x][center.y] != 0)
        return false;
    for (Point dir: directions) {
        // Move along the direction while testing.
        Point p = center + dir;
        if (!is_disc_at(p, 3 - plyr))
            continue;
        p = p + dir;
        while (is_spot_on_board(p) && board[p.x][p.y] != 0) {
            if (is_disc_at(p, plyr))
                return true;
            p = p + dir;
        }
    }
    return false;
}
std::vector<Point> get_valid_spots(int plyr) {
    std::vector<Point> valid_spots;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            Point p = Point(i, j);
            if (board[i][j] != 0)
                continue;
            if (is_spot_valid(p, plyr))
                valid_spots.push_back(p);
        }
    }
    return valid_spots;
}
std::array<std::array<int, SIZE>, SIZE> newBoard(std::array<std::array<int, SIZE>, SIZE> prvbrd, Point center, int plyer) {
    std::array<std::array<int, SIZE>, SIZE> nwbrd;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            nwbrd[i][j] = prvbrd[i][j];
    
    nwbrd[center.x][center.y] = plyer;
    // flip
    for (Point dir: directions) {
        // Move along the direction while testing.
        Point p = center + dir;
        if (!is_disc_at(p, 3 - plyer))
            continue;
        std::vector<Point> discs({p});
        p = p + dir;
        while (is_spot_on_board(p) && nwbrd[p.x][p.y] != 0) {
            if (is_disc_at(p, plyer)) {
                for (Point s: discs) {
                    nwbrd[s.x][s.y] = plyer;
                }
                break;
            }
            discs.push_back(p);
            p = p + dir;
        }
    }
    
    return nwbrd;
}

int minimax(std::array<std::array<int, SIZE>, SIZE> brd, std::vector<Point> next_Spots, int depth, int plyer, int alpha, int beta) {
    if (depth == 0) return score(brd, next_valid_spots.size(), plyer);
    if (plyer == player) {
        int bestval = MIN;
        for (auto i: next_Spots) {
            int val = minimax(newBoard(brd, i, plyer), get_valid_spots(3 - plyer), depth - 1, 3 - plyer, alpha, beta);
            bestval = std::max(bestval, val);
            alpha = std::max(alpha, bestval);
            if (beta <= alpha) break;
        }
        return bestval;
    } else {
        int bestval = MAX;
        for (auto i: next_Spots) {
            int val = minimax(newBoard(brd, i, plyer), get_valid_spots(3 - plyer), depth - 1, 3 - plyer, alpha, beta);
            bestval = std::min(bestval, val);
            beta = std:: min(beta, bestval);
            if (beta <= alpha) break;
        }
        return bestval;
    }
}

void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    int bestID = 0;
    int maxVal = MIN;
    for (int i = 0; i < n_valid_spots; i++) {
        Point cur = next_valid_spots[i];
        if ((cur.x == 0 || cur.x == SIZE - 1) && (cur.y == SIZE - 1 || cur.y == 0)) {
            bestID = i;
            break;
        }
        int val = minimax(newBoard(board, cur, player), get_valid_spots(3 - player), 3, 3 - player, MIN, MAX);
        bestID = val > maxVal ? i : bestID;
        maxVal = std::max(maxVal, val);
    }
    Point p = next_valid_spots[bestID];
    // Remember to flush the output to ensure the last action is written to file.
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
