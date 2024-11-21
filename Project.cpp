#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>

using namespace std;

void Clean() {
    for (int i = 0; i < 3; i++) {
        cout << '.';
        Sleep(1000);
    }
    system("cls");
}

class Score {
public:
    static int player1Score;
    static int player2Score;

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
            cout << "Введені значення не є корректним\n";
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

    Field& operator+=(Ship& ship) {
        int** coordinates = ship.getCoordinates();
        int x{ 0 }, y{ 0 };
        for (int i = 0; i < ship.getSize(); i++) {
            x = coordinates[i][0];
            y = coordinates[i][1];
            field[x][y] = 1;
        }
        return *this;
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

    friend ostream& operator<<(ostream& output, const Field& f) {
        output << "  ";
        for (int i = 1; i <= 10; i++) {
            output << i << ' ';
        }
        output << endl;
        for (int y = 0; y < 10; y++) {
            output << char('A' + y) << ' ';
            for (int x = 0; x < 10; x++) {
                if (f.field[x][y] == 2) output << "X ";
                else if (f.field[x][y] == 3) output << "- ";
                else output << "? ";
            }
            output << endl;
        }
        return output;
    }

    int** getField() { return field; }

    ~Field() {
        for (int i = 0; i < 10; i++) {
            delete[] field[i];
        }
        delete[] field;
    }
};

class Player {
protected:
    Ship ship6[1];
    Ship ship4[2];
    Ship ship3[3];
    Ship ship2[4];
    Field field;
    string name;
    int shipsNum = 0;

    virtual void place(int n, int size, Ship* ship) {
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
                    cout << "Корабель неможливо розташувати в цій точці!\n";
                    valid = false;
                }

            } while (!valid);
            shipsNum += size;
            field += ship[i];
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {
                    if (field.getField()[i][j] == 1) field.getField()[i][j] = 2;
                }
            }
            cout << field;
        }
    }

public:
    Player(string name_i) : name{ name_i } {}

    virtual void placeShips() {
        cout << "ГРА МОРСЬКИЙ БІЙ" << endl;
        cout << field;
        cout << name << ", розташуйте кораблі на полі" << endl;
        place(1, 6, ship6);
        place(2, 4, ship4);
        place(3, 3, ship3);
        place(4, 2, ship2);
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (field.getField()[i][j] == 2) field.getField()[i][j] = 1;
            }
        }
        Clean();
    }

    virtual bool shootOpponent(Field& opponent) {
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
        } while (!valid);

        int** opField = opponent.getField();
        if (opField[x][y] == 1) {
            opField[x][y] = 2;
            cout << "Влучив!\n";
            Clean();
            return true;
        }
        else if (opField[x][y] == 2) {
            cout << "Ви вже стріляли в це місце!\n";
            Clean();
            return false;
        }
        else {
            opField[x][y] = 3;
            cout << "Промахнувся!\n";
            Clean();
            return false;
        }
    }

    friend ostream& operator<<(ostream& output, const Player& p) {
        output << "Поле гравця " << p.name << endl;
        output << p.field;
        return output;
    }

    Field& getField() { return field; }
    string getName() { return name; }
    int getShipsNum() { return shipsNum; }
};

class Bot : public Player {
    void place(int n, int size, Ship* ship) override {
        int x, y, d;
        bool valid = true;
        for (int i = 0; i < n; i++) {
            do {
                valid = true;
                x = rand() % 10;
                y = rand() % 10;
                d = rand() % 2;
                ship[i] = Ship(size, x, y, d);
                if (!getField().isPlaceable(ship[i])) {
                    valid = false;
                }
            } while (!valid);
            getField() += ship[i];
        }
    }
public:
    Bot() : Player("Бот") {}

    void placeShips() override {
        cout << "ГРА МОРСЬКИЙ БІЙ" << endl;
        cout << field;
        cout << getName() << " розташовує кораблі";
        place(1, 6, ship6);
        place(2, 4, ship4);
        place(3, 3, ship3);
        place(4, 2, ship2);
        Clean();
    }

    bool shootOpponent(Field& opponent) override {
        int x, y;
        int** opField = opponent.getField();
        do {
            x = rand() % 10;
            y = rand() % 10;
        } while (opField[x][y] == 2 || opField[x][y] == 3);
        cout << getName() << " стріляє по координатах " << x + 1 << " " << char('A' + y) << "\n";
        if (opField[x][y] == 1) {
            opField[x][y] = 2;
            cout << "Влучив!\n";
            Clean();
            return true;
        }
        else {
            opField[x][y] = 3;
            cout << "Промахнувся!\n";
            Clean();
            return false;
        }
    }
};


class Game {
    Player* player1;
    Player* player2;
    Score score;

public:
    Game(Player* player1, Player* player2) : player1(player1), player2(player2) {}

    void start() {
        player1->placeShips();
        player2->placeShips();

        bool play = true;
        while (play) {
            cout << *player1;
            if (player2->shootOpponent(player1->getField())) {
                score.player2Won();
            }
            cout << *player2;
            if (player1->shootOpponent(player2->getField())) {
                score.player1Won();
            }
            if (score.player1Score >= player1->getShipsNum() || score.player2Score >= player1->getShipsNum()) {
                play = false;
            }
        }
    }
};

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    srand((time(0)));

    while (true) {
        cout << "Меню:\n1. Грати\n2. Переглянути результати останньої гри\n3. Вийти\nВаш вибір: ";
        int choiceMenu;
        cin >> choiceMenu;
        cin.ignore();

        if (choiceMenu == 1) {
            string playerName1, playerName2;
            cout << "Введіть ім'я гравця 1: ";
            getline(cin, playerName1);
            Player player1(playerName1);
            char choice;
            Player* player2;
            while (true) {
                cout << "Хочете грати з ботом? (y/n) ";
                cin >> choice;
                if (tolower(choice) == 'y') {
                    playerName2 = "Бот";
                    player2 = new Bot();
                    break;
                }
                else if (tolower(choice == 'n')) {
                    cout << "Введіть ім'я гравця 2: ";
                    cin.ignore();
                    getline(cin, playerName2);
                    player2 = new Player(playerName2);
                    break;
                }
                else cout << "Неправильно введені дані\n";
            }
            Clean();
            Game game(&player1, player2);
            game.start();
            delete player2;

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
        }
        else if (choiceMenu == 2) {
            ifstream file("results.txt");
            if (file.is_open()) {
                string line;
                while (getline(file, line)) {
                    cout << line << endl;
                }
                file.close();
            }
            else {
                cerr << "Не вдалося відкрити файл \"results.txt\".\n";
            }
        }
        else if (choiceMenu == 3) {
            cout << "Дякую за гру!";
            break;
        }
        else cout << "Некоректне введення\n";
        Clean();
    }
    return 0;
}