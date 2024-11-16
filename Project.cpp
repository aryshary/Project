#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>

using namespace std;

void Clean() {
    for (int i = 0; i < 3; i++) {
        cout << ".";
        Sleep(1000);
    }
    system("cls");
}

class Score {
public:
    static int player1Score;
    static int player2Score;

    static void printScore(string player1Name, string player2Name) {
        cout << "Рахунок:\nГравець " << player1Name << " - " << player1Score << " очок\nГравець " << player2Name << " - " << player2Score << " очок\n";
        player1Score > player2Score ? cout << "Переможець - " << player1Name << "\n" : player2Score > player1Score ? cout << "Переможець - " << player2Name << "\n" : cout << "Нічия\n";
    }

    static void player1Won() { player1Score++; }
    static void player2Won() { player2Score++; }
};

int Score::player1Score = 0;
int Score::player2Score = 0;

class Ship {
    int size;
    int beginX;
    int beginY;
    int direction;
    int** coordinates;
public:
    Ship(int size_i, int beginX_i, int beginY_i, int direction_i)
        : size{ size_i }, beginX{ beginX_i }, beginY{ beginY_i }, direction{ direction_i }
    {
        coordinates = new int* [size];
        for (int i = 0; i < size; i++) {
            coordinates[i] = new int[2];
        }
        int index = 0;
        if (direction == 0) {
            for (int i = beginY; i < beginY + size; i++) {
                coordinates[index][0] = beginX;
                coordinates[index][1] = i;
                index++;
            }
        }
        else if (direction == 1) {
            for (int i = beginX; i < beginX + size; i++) {
                coordinates[index][0] = i;
                coordinates[index][1] = beginY;
                index++;
            }
        }
        else {
            cout << "Введені значення не є корректним" << endl;
        }
    }

    Ship() : Ship(0, 0, 0, 0) {}

    Ship(const Ship& ship)
        : size{ ship.size }, beginX{ ship.beginX }, beginY{ ship.beginY }, direction{ ship.direction }
    {
        coordinates = new int* [size];
        for (int i = 0; i < size; i++) {
            coordinates[i] = new int[2];
            coordinates[i][0] = ship.coordinates[i][0];
            coordinates[i][1] = ship.coordinates[i][1];
        }
    }

    Ship& operator=(Ship&& ship) {
        if (this != &ship) {
            for (int i = 0; i < size; i++) {
                delete[] coordinates[i];
            }
            delete[] coordinates;

            size = ship.size;
            beginX = ship.beginX;
            beginY = ship.beginY;
            direction = ship.direction;
            coordinates = ship.coordinates;

            ship.size = 0;
            ship.beginX = 0;
            ship.beginY = 0;
            ship.direction = 0;
            ship.coordinates = nullptr;

        }
        return *this;
    }

    int getSize() { return size; }
    int** getCoordinates() { return coordinates; }

    ~Ship() {
        for (int i = 0; i < size; i++) {
            delete[] coordinates[i];
        }
        delete[] coordinates;
    }
};

class Field {
    int** field;
public:
    Field() {
        field = new int* [10];
        for (int i = 0; i < 10; i++) {
            field[i] = new int[10];
            for (int j = 0; j < 10; j++) {
                field[i][j] = 0;
            }
        }
    }

    void addShip(Ship& ship) {
        int** coordinates = ship.getCoordinates();
        int x{ 0 }, y{ 0 };
        for (int i = 0; i < ship.getSize(); i++) {
            x = coordinates[i][0];
            y = coordinates[i][1];
            field[x][y] = 1;
        }
    }

    bool isPlaceable(Ship& ship) {
        int** coordinates = ship.getCoordinates();
        int x{ 0 }, y{ 0 };
        for (int i = 0; i < ship.getSize(); i++) {
            x = coordinates[i][0];
            y = coordinates[i][1];
            if (x > 9 || y > 9 || x < 0 || y < 0) return false;
            if ((x > 0 && field[x - 1][y] != 0) ||
                (x < 9 && field[x + 1][y] != 0) ||
                (y > 0 && field[x][y - 1] != 0) ||
                (y < 9 && field[x][y + 1] != 0) ||
                (x > 0 && y > 0 && field[x - 1][y - 1] != 0) ||
                (x > 0 && y < 9 && field[x - 1][y + 1] != 0) ||
                (x < 9 && y > 0 && field[x + 1][y - 1] != 0) ||
                (x < 9 && y < 9 && field[x + 1][y + 1] != 0)) {
                return false;
            }
        }
        return true;
    }

    void display(bool showShips = false) {
        cout << "  ";
        for (int i = 1; i <= 10; i++) {
            cout << i << ' ';
        }
        cout << endl;
        for (int y = 0; y < 10; y++) {
            cout << char('A' + y) << ' ';
            for (int x = 0; x < 10; x++) {
                if (showShips) {
                    if (field[x][y] == 1 || field[x][y] == 2) cout << "X ";
                    else cout << "- ";
                }
                else {
                    if (field[x][y] == 2) cout << "X ";
                    else if (field[x][y] == 3) cout << "- ";
                    else cout << "? ";
                }
            }
            cout << endl;
        }
    }

    int** getField() { return field; }
};

class Player {
    Ship ship6[1];
    Ship ship4[2];
    Ship ship3[3];
    Ship ship2[4];
    Field field;
    string name;
    int shipsNum = 0;

    void place(int n, int size, Ship* ship) {
        int x, y, d;
        char charY;
        bool valid = true;
        for (int i = 0; i < n; i++) {
            do {
                valid = true;
                cout << "Початкові координати корабля на " << size << " клітинок у форматі 1 A та розташування(вертикальне - 0, горизонтальне - 1): ";
                cin >> x >> charY >> d;
                x--;
                y = toupper(charY) - 'A';
                ship[i] = Ship(size, x, y, d);
                if (!field.isPlaceable(ship[i])) {
                    cout<< "Координати виходять за межі поля!\n";
                    valid = false;
                }
            } while (!valid);
            shipsNum += size;
            field.addShip(ship[i]);
            field.display(true);
        }
    }

public:
    Player(string name_i) : name{ name_i } { }

    void placeShips() {
        cout << "ГРА МОРСЬКИЙ БІЙ" << endl;
        Field field0;
        field0.display();
        cout << name << ", розташуйте кораблі на полі" << endl;
        place(1, 6, ship6);
        place(2, 4, ship4);
        place(3, 3, ship3);
        place(4, 2, ship2);
        Clean();
    }

    bool shootOpponent(Field& opponent) {
        int x, y;
        char charY;
        bool valid = true;
        do {
            valid = true;
            cout << name << ", введіть координати для пострілу у форматі 1 A: ";
            cin >> x >> charY;
            x--;
            y = toupper(charY) - 'A';

            if (x < 0 || x > 9 || y < 0 || y > 9) {
                cout << "Координати виходять за межі поля!\n";
                valid = false;
            }
            else {
                if (opponent.getField()[x][y] == 0) {
                    opponent.getField()[x][y] = 3;
                    cout << "Промахнувся" << endl;
                    Clean();
                    return false;

                }
                else if (opponent.getField()[x][y]==1) {
                    opponent.getField()[x][y] = 2;
                    cout << "Влучив!" << endl;
                    Clean();
                    return true;
                }
            }
        } while (!valid);
    }

    void showPlayer(bool showShips = false) {
        cout << "Поле гравця " << name << endl;
        field.display(showShips);
    }

    Field& getField() { return field; }
    string getName() { return name; }
    int getShipsNum() { return shipsNum; }
};

class Game {
    Player player1;
    Player player2;
    Score score;

public:
    Game(string name1, string name2) : player1(name1), player2(name2) {}

    void start() {
        player1.placeShips();
        player2.placeShips();

        bool play = true;
        while (play) {
            player1.showPlayer(false);
            if (player2.shootOpponent(player1.getField())) {
                score.player2Won();
            }
            player2.showPlayer(false);
            if (player1.shootOpponent(player2.getField())) {
                score.player1Won();
            }
            if (score.player1Score >= player1.getShipsNum() || score.player2Score >= player1.getShipsNum()) {
                score.printScore(player1.getName(), player2.getName());
                play = false;
            }
        }
    }
};

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    srand((time(0)));

    string playerName1, playerName2;
    cout << "Введіть ім'я гравця 1: ";
    getline(cin, playerName1);
    cout << "Введіть ім'я гравця 2: ";
    getline(cin, playerName2);
    Clean();

    Game game(playerName1, playerName2);
    game.start();

    ofstream file("results.txt");
    if (file.is_open()) {
        file << "Рахунок:\nГравець " << playerName1 << " - " << Score::player1Score << " очок\nГравець " << playerName2 << " - " << Score::player2Score << " очок\n";
        Score::player1Score > Score::player2Score ? file << "Переможець - " << playerName1 << "\n" : Score::player2Score > Score::player1Score ? file << "Переможець - " << playerName2 << "\n" : file << "Нічия\n";
        file.close();
        cout << "Результати збережено у файл \"results.txt\"\n";
    }
    else {
        cerr << "Не вдалося відкрити файл \"results.txt\".\n";
    }
    return 0;
}