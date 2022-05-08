#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <winuser.h>
#include <conio.h>
#include <time.h>
#include <set>
#include <fstream>

using namespace std;

const int MAP_ROWS=100;
const int MAP_COLS=100;

const int allWordsSize = 2315 + 10657;
//set<string> allWords;
string allWords[allWordsSize];

int currentGuess = 0;
const int guessesNum = 5;
const int letterCount = 5;
const int letterWidth = 3;
string guesses[guessesNum];
int enteredLetters = 0;
string todaysWord;
//set<string>::iterator it;
int correctCounter = 0;

HANDLE hConsoleOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
COORD screen_buf = {MAP_ROWS, MAP_COLS};
CHAR_INFO blank_screen[MAP_ROWS * MAP_COLS] = {0};

enum COLORS {
    BLACK = 0,
    BLUE = FOREGROUND_BLUE,
    CYAN = FOREGROUND_BLUE | FOREGROUND_GREEN,
    GREEN = FOREGROUND_GREEN,
    RED = FOREGROUND_RED,
    BROWN = FOREGROUND_RED | FOREGROUND_GREEN,
    PURPLE = FOREGROUND_RED | FOREGROUND_BLUE,
    LIGHT_GREY =  FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN,

    GREY = 0 | FOREGROUND_INTENSITY,
    LIGHT_BLUE = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    LIGHT_CYAN = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    LIGHT_GREEN = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    LIGHT_RED = FOREGROUND_RED | FOREGROUND_INTENSITY,
    YELLOW = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    PINK = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    WHITE =  FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,

};


COLORS drawColors[guessesNum][letterCount];
COLORS drawColorsFont[guessesNum][letterCount];

void draw_char(char ch, int y, int x, COLORS foreground_color, COLORS background_color) {
    CHAR_INFO ch_info;
    ch_info.Char.AsciiChar = ch;
    ch_info.Attributes = foreground_color | (background_color << 4);

    COORD buf_size = {1, 1};
    COORD buf_coord = {0, 0};
    SMALL_RECT screen_pos = {x, y, x+1, y+1};
    ::WriteConsoleOutput(hConsoleOutput, &ch_info, buf_size, buf_coord, &screen_pos);

}

void clear_screen() {
    COORD buf_coord = {0, 0};
    SMALL_RECT screen_pos = {0, 0, screen_buf.X, screen_buf.Y};
    ::WriteConsoleOutput(hConsoleOutput, blank_screen, screen_buf, buf_coord, &screen_pos);
}

void set_background(COLORS color) {
    for (int i = 0; i < screen_buf.X; i++) {
        for (int j = 0; j < screen_buf.Y; j++) {
            draw_char(' ', i, j, color, color);
        }
    }
}

void drawGrid() {

    for(int i = 0; i < guessesNum; i++) {

        for(int j = 0; j < letterCount; j++) {

            draw_char(' ',i*2,j*letterWidth+j,drawColorsFont[i][j],drawColors[i][j]);
            draw_char(guesses[i][j],i*2,j*letterWidth+1+j,drawColorsFont[i][j],drawColors[i][j]);
            draw_char(' ',i*2,j*letterWidth+2+j,drawColorsFont[i][j],drawColors[i][j]);
        }


    }

}

int binarySearch(string x, int low, int high) {
  if (high >= low) {
    int mid = low + (high - low) / 2;

    // If found at mid, then return it
    if (allWords[mid] == x)
      return mid;

    // Search the left half
    if (allWords[mid] > x)
      return binarySearch(x, low, mid - 1);

    // Search the right half
    return binarySearch(x, mid + 1, high);
  }

  return -1;
}


bool checkIfWordExists(string word) {
    return binarySearch(word,0,allWordsSize-1) != -1;
}

void checkForMatches(string word) {

    for(int i =0; i<word.length(); i++) {

        for(int j=0; j< todaysWord.length(); j++) {

            if(word[i] == todaysWord[j]) {
                if(i == j) {
                    drawColors[currentGuess][i] = GREEN;
                    drawColorsFont[currentGuess][i] = WHITE;
                    correctCounter++;
                } else {
                    drawColors[currentGuess][i] = BROWN;
                    drawColorsFont[currentGuess][i] = WHITE;

                }
            }

        }

    }

}

void init() {
//    cout << "Loading...." << endl;
    string word;
    int br = 0;
    ifstream allGuesses("allWordsAlphabetically.txt");
    while(!allGuesses.eof()) {
        getline(allGuesses,word);
        allWords[br] = word;
        br++;
    }
    allGuesses.close();
    ifstream possibleAnswers("wordle-answers-alphabetical.txt");
    ///number copied from github
    int i = rand()%2315;
    int k = 0;
    while(!possibleAnswers.eof()) {
        getline(possibleAnswers,word);
        if(i == k){
            todaysWord = word;
            break;
        }
        k++;
    }
    possibleAnswers.close();

//    for (it=allWords.begin(); it!=allWords.end(); ++it)
//    cout << *it;
    ///  cout << todaysWord << endl;
    /// cout << "Loaded!" << endl;
    for(int i =0; i< guessesNum; i++) {
        guesses[i] = "     ";
        for(int j =0; j<letterCount; j++) {
            drawColors[i][j] = LIGHT_GREY;
            drawColorsFont[i][j] = GREY;
        }
    }
}


int main() {
    srand(time(0));

    init();

    drawGrid();

    for(int i =0; i<15; i++) {
        cout << endl;
    }
//    cout << todaysWord << endl;
    char c;
    bool check = false;
    bool wrongText = false;
    string text = "Word not in dictionary!";

    for(int i = 0; i < guessesNum; i++) {
        do {
            c = getch();
            //ohai who checks the source c:
            //  cout << (int)(c) << endl;
            if(c == 8) {
                if(enteredLetters >0) {
                    guesses[currentGuess][enteredLetters-1] = ' ';
                    enteredLetters--;
                }
                if(wrongText) {
                    wrongText = false;
                    for(int i =0; i < text.length(); i++) {
                        draw_char(' ',10,i,RED,BLACK);
                    }
                }
            } else if(c >= 97 && c <=122 && enteredLetters < letterCount) {

                guesses[currentGuess][enteredLetters] = c;
                enteredLetters++;
            } else if( c == 13) {
                if(enteredLetters == letterCount) {
//            cout << currentGuess << endl;
//            cout << guesses[currentGuess] << endl;
                    if(checkIfWordExists(guesses[currentGuess])) {
                        checkForMatches(guesses[currentGuess]);
                        check = true;
                    } else {
                        wrongText = true;
                        for(int i =0; i < text.length(); i++) {
                            draw_char(text[i],10,i,RED,BLACK);
                        }
                    }
                }
            }

            drawGrid();

        } while(!check);
        if(correctCounter == letterCount) {
            break;
        }
        correctCounter = 0;
        check = false;
        currentGuess++;
        enteredLetters = 0;

    }

    if( correctCounter != letterCount) {
        text = "The word was " + todaysWord + ".";
        for(int i =0; i < text.length(); i++) {
            draw_char(text[i],10,i,BROWN,BLACK);
        }
    }

    return 0;
}
