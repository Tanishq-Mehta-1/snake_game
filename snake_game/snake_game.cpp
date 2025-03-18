#include <iostream>
#include <conio.h>
#include <thread>
#include <chrono>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

bool gameOver;
bool Exit;
const int width = 20;
const int height = 20;
int score{ 0 };

int t{ 150 };

fstream file{"High_Score.txt"};
int h_score{};

enum Direction {STOP = 0, UP,DOWN, LEFT, RIGHT};
Direction dir;

class vec2 {
	public :
	
	int x;
	int y;

	vec2() : x(), y() {};
	vec2(int X, int Y): x(X), y(Y) {};
	

};
vec2 p;
vec2 f;

vector <vec2> tail;
int ind = 0;

static void clearScreen() {
	std::cout << "\033[H\033[?25l";
	/* 
	\033[H - moves cursor to beginning
	\033[? 25l - hides the cursor 
	*/
}

static void Setup() {

	gameOver = false;
	dir = STOP;

	//check Score file
	if (!file) {
		fstream file{ "High_Score.txt", ios::out};
		file.close();

		cerr << "File could not be opened!\ncreated the file!";
		exit(0);
	}
	
	//set player's initial coord
	p.x = width / 2;
	p.y = height / 2;

	//set fruit's initial coord
	srand(time(nullptr));
	f.x = (rand() % (width - 1)) + 1; // from 1 to ( width - 1 )
	f.y = (rand() % (height - 1)) + 1; // from 1 to ( height - 1 )

}

static void Input() {

	//if a key is hit, get the character
	if (_kbhit()) {
		switch (_getch())
		{
		case 'a':
		case 'A':
			dir = LEFT;
			break;
		case 's':
		case 'S':
			dir = DOWN;
			break;
		case 'w':
		case 'W':
			dir = UP;
			break;
		case 'd':
		case 'D':
			dir = RIGHT;
			break;
		case 'x':
		case 'X':
			gameOver = true;
			break;
		default:
			break;
		}
	}
}

static void Logic() {

	//hitting the wall
	if (p.x == 0 || p.x == width || p.y == 0 || p.y == height)
		gameOver = true;

	//if a fruit is eaten
	if (p.x == f.x && p.y == f.y)
	{
		score += 10;
		if (t > 0)
			t -= 10;

		//increasing the tail
		if (!tail.empty()) {
			tail.push_back(tail.back());
		} else 
			tail.push_back(vec2( p.x - 1, p.y - 1));

		//generating the next fruit location
		srand(time(nullptr));
		f.x = (rand() % (width - 1)) + 1; // from 1 to ( width - 1 )
		f.y = (rand() % (height - 1)) + 1; // from 1 to ( height - 1 )
	}

	//updating tail coords
	vec2 prev = p;
	for (auto &it : tail)
	{
		//checking if head hits the tail
		if (p.x == it.x && p.y == it.y)
			gameOver = true;

		// [x1y1, x2y2, x3y3] ==> [p.x p.y, x1y1, x2y2]
		vec2 temp = it;
		it.x = prev.x;
		it.y = prev.y;
		prev = temp;
	}
 
	//updating direction based on the key-press
	switch (dir)
	{
	case UP: 
		p.y--;
		break;
	case DOWN:
		p.y++;
		break;
	case RIGHT:
		p.x++;
		break;
	case LEFT:
		p.x--;
		break;
	}
}

static void End_Screen() {

	system("cls");

	//game over screen
	if (gameOver) {
		cout << "GAME OVER\n";
		cout << "Final Score: " << score << endl << endl;

		int count{1};
		string str{};

		if (file.peek() != EOF) {
			
			//prints score table and counts the number of scores
			cout << left << "SCORE TABLE" << endl;
			while (getline(file, str)) {
				count++;
				cout << setw(16) << str << endl;
			}
		}
		else
			count = 1;

		char ans{'n'};
		cout << endl << "Do you want to add you score? Y/N" << endl;
		cin >> ans;
		
		if (ans == 'y' || ans == 'Y') {
			file.clear();  //resets cursor to beginning
			file.seekp(0, std::ios::end);  //moves cursor to end
			file << '#' << count << ' ' << score << endl;
			cout << "Score Recorded!" << endl;
		}
		
		file.close();
	}
}

static void Draw() {
	
	//normal screen
	for (int j = 0; j <= height; j++) {
		for (int i = 0; i <= width; i++) {

			bool print{ false }; //if tail is printed, dont print a whitespace
			for (auto it : tail) {
				if (i == it.x && j == it.y){
					print = true;
					cout << 'o';
				}
			}

			if (i == 0 || i == width || j == 0 || j == height) //drawing borders
				cout << '#';
			else if (i == f.x && j == f.y) //draw fruit
				cout << 'F';
			else if (i == p.x && j == p.y) //draw head
				cout << 'O';
			else if (!print)
				cout << ' ';
		}
		cout << endl;
	}
	cout << "Score: " << score << endl;
}

int main()
{
	Setup();
	while (!gameOver) {
		clearScreen();

		Input();
		Logic();
		Draw();
		
		this_thread::sleep_for(chrono::milliseconds(t));
	}

	End_Screen();
	
	return 0;
}