// 20 Juni 2022
// F1D021044 - I Dewa Agung Adhinata Budhiastawa
// PROJECT Praktikum Pemrograman Dasar, Semester 2
// (only for windows because of dependency "conio.h")
//TODO: convert all comments to english
#include <Windows.h>
#include <conio.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// global variabel;
const int SIZE_ARR_BOAT = 10;
const int MAX_X_BOARD = 10;
const int MAX_Y_BOARD = 10;
string BOARD_COOR_TOP = "\n    0 1 2 3 4 5 6 7 8 9\n";
string BOARD_BORDER = "|";
string BOARD_CURSOR = "&|";
string BOARD_CROSS = "*.";
string BOARD_SHIP_HIT = "#.";
string BOARD_SHIP = "O|";
string BOARD_SHIP_ADD = "+|";
string BOARD_BLANKSPACE = " .";
int GLOBAL_CURSOR_POS[2] = {0, 0};
// membersihkan layar, dikarenakan masalah bug ketika mengugnakan getch() dan system('cls') secara bersamaan
void clearScreen() {
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
}

void gamePlay();
// Battleship Blueprint, template kapal perang masing - masing pemain
struct Boat {
    string name;
    int length;
    int arrH[10][SIZE_ARR_BOAT];
    int arrV[10][SIZE_ARR_BOAT];
    bool isVertical = false;
    char body = 'o';
    int health;

    // inisialisasi variabel struct
    void init() {
        for (int i = 0; i < length; i++) {
            arrH[i][0] = 0, arrH[i][1] = i;
        }
        for (int i = 0; i < length; i++) {
            arrV[i][1] = arrH[i][0], arrV[i][0] = arrH[i][1];
        }
        health = length;
    }
    // menambah offset kepada array saat ini
    void addArrOffset(int offset_y, int offset_x) {
        for (int i = 0; i < length; i++) {
            arrH[i][0] += offset_y, arrH[i][1] += offset_x;
            arrV[i][0] += offset_y, arrV[i][1] += offset_x;
        }
    }
    // me-register serangan dari musuh, dengan mengecek apakah array vertikal / horizontal merupakan bagian dari koordinat pada parameter
    void receiveDMG(int y, int x) {
        for (int i = 0; i < length; i++) {
            if ((arrV[i][0] == y && arrV[i][1] == x) || (arrH[i][0] == y && arrH[i][1] == x)) {
                health--;
            }
        }
    }
    // mengecek apakah array vertikal / horizontal merupakan bagian dari koordinat pada parameter, memberi return boolean
    bool checkYX(int y, int x) {
        for (int i = 0; i < length; i++) {
            if (isVertical) {
                if ((arrV[i][0] == y && arrV[i][1] == x)) return true;
            } else {
                if ((arrH[i][0] == y && arrH[i][1] == x)) return true;
            }
        }
        return false;
    }
};
vector<Boat> ship_list = {
    {"carrier", 5},
    {"battleship", 4},
    {"destroyer", 3},
    {"submarine", 3},
    {"boat", 2},
};

struct Coordinate {
    int pos[2];
    Coordinate *next = NULL;
};

// linked list untuk koordinat serangan yang dilancarkan pemain
struct CoordinateHandler {
    Coordinate *head = NULL, *tail = NULL, *temp = NULL, *print = NULL;
    // menambahkan coordinate struct
    void addCrossCoordinate(int y, int x) {
        Coordinate *p = new Coordinate;
        p->pos[0] = y;
        p->pos[1] = x;

        if (head == NULL) {
            head = p;
            tail = head;
        } else {
            p->next = tail;
            tail = p;
        }
    }
    // print stored coordinate
    void printItem() {
        print = tail;
        while (print != NULL) {
            cout << "[" << print->pos[0] << "," << print->pos[1] << "]";
            print = print->next;
            if (print == NULL) break;
        }
        cout << endl;
    }
    // cek jika koordinat terinput sama dengan koordinat pada linked list
    bool checkYX(int y, int x) {
        print = tail;
        while (print != NULL) {
            if (y == print->pos[0] && x == print->pos[1]) return true;
            print = print->next;
        }
        return false;
    }
};

struct Player {
    string name;
    int cursor[2] = {0, 0};  // y, x
    int health = 5;
    vector<Boat> player_ships;
    CoordinateHandler coor;
    // cek jika koordinat saat ini mengandung salah satu dari array kapal
    bool checkContainPlacedShips(int y, int x) {
        for (int i = 0; i < player_ships.size(); i++) {
            if (player_ships[i].checkYX(y, x)) return true;
        }
        return false;
    }

    // me-register serangan dari pemain lain dengan memeriksa koordinat parameter dengan koordinat pada array kapal pemain
    void receiveDMG(int y, int x) {
        for (int i = 0; i < player_ships.size(); i++) {
            if (player_ships[i].checkYX(y, x)) {
                player_ships[i].receiveDMG(y, x);
            }
        }
    }

    // memeriksa apakah array kapal pemain memiliki hit poin sebanyak 0
    void checkShip() {
        for (int i = 0; i < player_ships.size(); i++) {
            if (player_ships[i].health == 0) {
                health--;
                player_ships[i].health--;  // -1
            }
        }
    }

} player1, player2;

// BOARD FUNCTIONS

// menampilkan papan 10x10, serta instruksi penempatan kapal
void displayPlacementBoard(Player &pl, Boat &curr_ship) {
    clearScreen();
    cout << "Pemain saat ini : " << pl.name << "\n";
    for (int y = 0; y < MAX_Y_BOARD; y++) {
        if (y == 0) cout << BOARD_COOR_TOP;
        cout << y;
        cout << "  |";
        for (int x = 0; x < MAX_X_BOARD; x++) {
            if (curr_ship.checkYX(y, x)) {
                cout << BOARD_SHIP_ADD;
            } else if (pl.checkContainPlacedShips(y, x)) {
                cout << BOARD_SHIP;
            } else {
                cout << BOARD_BLANKSPACE;
            }
        }
        cout << BOARD_BORDER << endl;
    }
    cout << " [v] ubah orientasi kapal; [e] letakkan kapal; gunakan tombol panah untuk bergerak\n";
}

// menampilkan papan 10x10, tanpa cursor, menampilkan kapal pemain yang telah ditempatkan, serta indikator lokasi tembahkan musuh
void displayGameBoard(Player &pl, Player &opponent) {
    clearScreen();
    cout << "----------------------------------------\n";
    cout << "Pemain saat ini : " << pl.name << " - Sisa kapal: " << pl.health << "\n Kapal pemain saat ini:\n";
    for (int y = 0; y < MAX_Y_BOARD; y++) {
        if (y == 0) cout << BOARD_COOR_TOP;
        cout << y;
        cout << "  |";
        for (int x = 0; x < MAX_X_BOARD; x++) {
            if (pl.checkContainPlacedShips(y, x) && opponent.coor.checkYX(y, x)) {
                cout << BOARD_SHIP_HIT;
            } else if (pl.checkContainPlacedShips(y, x)) {
                cout << BOARD_SHIP;
            } else if (opponent.coor.checkYX(y, x)) {
                cout << BOARD_CROSS;
            } else {
                cout << BOARD_BLANKSPACE;
            }
        }
        cout << BOARD_BORDER << endl;
    }
    cout << "----------------------------------------\n";
}
// menampilkan papan 10x10,hanya menampilkan kursor, serta posisi terakhir kali menembak
void displayCoordinateBoard(Player &pl, Player &opponent, bool clearScr = false) {
    if (clearScr) clearScreen();

    cout << "Musuh : " << opponent.name << " Sisa kapal musuh: " << opponent.health << "\n";
    for (int y = 0; y < MAX_Y_BOARD; y++) {
        if (y == 0) cout << BOARD_COOR_TOP;
        cout << y;
        cout << "  |";
        for (int x = 0; x < MAX_X_BOARD; x++) {
            if (y == pl.cursor[0] && x == pl.cursor[1]) {
                cout << BOARD_CURSOR;
            } else if (pl.coor.checkYX(y, x) && opponent.checkContainPlacedShips(y, x)) {
                cout << BOARD_SHIP_HIT;
            } else if (pl.coor.checkYX(y, x)) {
                cout << BOARD_CROSS;
            } else {
                cout << BOARD_BLANKSPACE;
            }
        }
        cout << BOARD_BORDER << endl;
    }
    cout << "[e] tandai posisi; [q] abort game; gunakan tombol panah untuk menggerakkan kursor\n";
}

// mengontrol pergerakan kursor, serta input lainnya. bhanay bekerja jika disandingkan dengan displayPlacementBoard()
bool cursorPlacementMovement(char input, Player &pl, Boat &ship) {
    // H = up, P = down, M= right, K = left
    if (input == 'H') {
        if (GLOBAL_CURSOR_POS[0] > 0) {
            GLOBAL_CURSOR_POS[0]--;
            ship.addArrOffset(-1, 0);
        }
    } else if (input == 'K') {
        if (GLOBAL_CURSOR_POS[1] > 0) {
            GLOBAL_CURSOR_POS[1]--;
            ship.addArrOffset(0, -1);
        }
    }
    if (ship.isVertical) {
        if (input == 'P') {
            if (GLOBAL_CURSOR_POS[0] < MAX_Y_BOARD - ship.length) {
                GLOBAL_CURSOR_POS[0]++;
                ship.addArrOffset(1, 0);
            }
        } else if (input == 'M') {
            if (GLOBAL_CURSOR_POS[1] < MAX_X_BOARD - 1) {
                GLOBAL_CURSOR_POS[1]++;
                ship.addArrOffset(0, 1);
            }
        }
    } else {
        if (input == 'P') {
            if (GLOBAL_CURSOR_POS[0] < MAX_Y_BOARD - 1) {
                GLOBAL_CURSOR_POS[0]++;
                ship.addArrOffset(1, 0);
            }
        } else if (input == 'M') {
            if (GLOBAL_CURSOR_POS[1] < MAX_X_BOARD - ship.length) {
                GLOBAL_CURSOR_POS[1]++;
                ship.addArrOffset(0, 1);
            }
        }
    }
    // keybind v untuk melakukan toggle orientasi kapal
    if (input == 'v' || input == 'V') {
        ship.isVertical = !ship.isVertical;
    }
    // keybind e untuk mengkonfirmasi penempatan kapal
    if (input == 'e' || input == 'E') {
        return true;
    }
    return false;
}

// mengontrol pergerakan kursor, berlaku ketika fungsi displayCoordinateBoard() ikut berjalan untuk menampilkan kursornya
bool cursorMovement(char input, Player &pl) {
    // H = up, P = down, M= right, K = left
    if (input == 'H') {
        if (pl.cursor[0] > 0) pl.cursor[0]--;
    } else if (input == 'P') {
        if (pl.cursor[0] < MAX_Y_BOARD - 1) pl.cursor[0]++;
    } else if (input == 'M') {
        if (pl.cursor[1] < MAX_X_BOARD - 1) pl.cursor[1]++;
    } else if (input == 'K') {
        if (pl.cursor[1] > 0) pl.cursor[1]--;
    }
    if (input == 'e' || input == 'E') {
        return true;
    }
    return false;
}
// fungsi untuk melakukan penempatan kapal kedua pemain
void gameSetup() {
    char pilihan;
    bool isInputDone = false;
    cout << "masukkan nama pemain 1: ";
    getline(cin >> ws, player1.name);
    cout << "masukkan nama pemain 2: ";
    getline(cin >> ws, player2.name);

    cout << "Pemain 1 mendapat giliran untuk meletakkan kapalnya\n";
    getch();
    for (int i = 0; i < ship_list.size(); i++) {
        isInputDone = false;
        GLOBAL_CURSOR_POS[0] = 0, GLOBAL_CURSOR_POS[1] = 0;
        Boat newship = ship_list[i];
        newship.init();
        while (true) {
            displayPlacementBoard(player1, newship);
            cout << "\nKapal saat ini: " << newship.name << "\nSisa kapal yang belum disiapkan: " << ship_list.size() - i << "\n";
            pilihan = getch();
            isInputDone = cursorPlacementMovement(pilihan, player1, newship);
            if (isInputDone) {
                break;
            }
        }
        player1.player_ships.push_back(newship);
    }
    clearScreen();
    cout << "Pemain 2 mendapat giliran untuk meletakkan kapalnya\n";
    getch();
    for (int i = 0; i < ship_list.size(); i++) {
        isInputDone = false;
        GLOBAL_CURSOR_POS[0] = 0, GLOBAL_CURSOR_POS[1] = 0;
        Boat newship = ship_list[i];
        newship.init();
        while (true) {
            displayPlacementBoard(player2, newship);
            cout << "\nKapal saat ini: " << newship.name << "\nSisa kapal yang belum disiapkan: " << ship_list.size() - i << "\n";
            pilihan = getch();
            isInputDone = cursorPlacementMovement(pilihan, player2, newship);
            if (isInputDone) {
                break;
            }
        }
        player2.player_ships.push_back(newship);
    }
    clearScreen();
    cout << "tekan tombol apa pun untuk meulai permainan\n";
    getch();
    return gamePlay();
}

// main loop games
void gamePlay() {
    char pilihan;
    int turn = 2;
    Player *pl_curr = NULL, *pl_opponent = NULL;
    bool isInputDone = false, isNotOverlapping = false;
    while (true) {
        // change player and opponent position, by check if turn is even or odd
        if (turn % 2 == 0) {
            pl_curr = &player1, pl_opponent = &player2;
        } else {
            pl_curr = &player2, pl_opponent = &player1;
        }
        clearScreen();
        cout << "Giliran " << pl_curr->name << ", turn ke - " << int(turn / 2) << "\n";
        getch();
        // current player turn gameplay
        while (true) {
            isInputDone = false;
            pl_curr->checkShip();
            pl_opponent->checkShip();
            if (pl_opponent->health == 0) {
                clearScreen();
                cout << "Pemain " << pl_opponent->name << " Telah kalah!!\n selamat kepada Pemain " << pl_curr->name << " atas kemenangannya!!!\n";
                Sleep(2000);
                getch();
                return;
            }

            displayGameBoard(*pl_curr, *pl_opponent);
            displayCoordinateBoard(*pl_curr, *pl_opponent);
            cout << "koordinat kursor yang telah ditekan : ";
            pl_curr->coor.printItem();
            pilihan = getch();
            if (pilihan == 'q') return;
            isInputDone = cursorMovement(pilihan, *pl_curr);
            if (isInputDone) {
                if (pl_curr->coor.checkYX(pl_curr->cursor[0], pl_curr->cursor[1])) {
                    // jika kursor berada di posisi yang sama dengan posisi kursor sebelumnya

                    isInputDone = false;
                } else if (pl_opponent->checkContainPlacedShips(pl_curr->cursor[0], pl_curr->cursor[1])) {
                    // jika posisi kursor mengandung koordinat milik salah satu kapal musuh

                    pl_curr->coor.addCrossCoordinate(pl_curr->cursor[0], pl_curr->cursor[1]);
                    pl_opponent->receiveDMG(pl_curr->cursor[0], pl_curr->cursor[1]);
                    cout << "anda mengenai kapal musuh!\n";
                    Sleep(1000);
                } else {
                    // jika posisi kursor adalah kosong

                    pl_curr->coor.addCrossCoordinate(pl_curr->cursor[0], pl_curr->cursor[1]);
                    cout << "missile launched\n";
                    Sleep(1000);
                    break;
                }
            }
        }
        turn++;
    }
}

void mainMenu() {
    int pilihan;
    while (true) {
        cout << "====================================\n";
        cout << "    --- BATTLESHIP BOARD GAME ---\n";
        cout << "[1] Mulai Permainan\n[2] Keluar\n>> ";
        cin >> pilihan;
        if (pilihan == 1) return gameSetup();
        if (pilihan == 2) return;
    }
}

int main() {
    mainMenu();
    return 0;
}