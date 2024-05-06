#include <iostream>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "chess.hpp"
#include <cstdio>
#include <cstring>
using namespace std;
using namespace sf;
using namespace chess;

Sprite f[32];
Texture t[32];
int squareSize = 56;

vector<vector<vector<int>>> positions;

vector<vector<int>> board = {
    {-1,-2,-3,-4,-5,-3,-2,-1},
    {-6,-6,-6,-6,-6,-6,-6,-6},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {6, 6, 6, 6, 6, 6, 6, 6},
    {1, 2, 3, 4, 5, 3, 2, 1},
};

unordered_map<int, string> piece_image_map {
    {-1, "r-b"},
    {-2, "n-b"},
    {-3, "b-b"},
    {-4, "q-b"},
    {-5, "k-b"},
    {-6, "p-b"},
    {1, "r-w"},
    {2, "n-w"},
    {3, "b-w"},
    {4, "q-w"},
    {5, "k-w"},
    {6, "p-w"},
};

unordered_map<int, vector<string>> piece_map {
    {-1, {"rook", "black"}},
    {-2, {"knight", "black"}},
    {-3, {"bishop", "black"}},
    {-4, {"queen", "black"}},
    {-5, {"king", "black"}},
    {-6, {"pawn", "black"}},
    {1, {"rook", "white"}},
    {2, {"knight", "white"}},
    {3, {"bishop", "white"}},
    {4, {"queen", "white"}},
    {5, {"king", "white"}},
    {6, {"pawn", "white"}},
    {0, {"blank", "blank"}},
};

void loadPosition() {
    int k=0;
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            int n = positions.back()[i][j];
            if (n == 0) continue;
            string piece_name = piece_image_map[n];
            string name = string("images/") + piece_name + ".png";
            t[k].loadFromFile(name);
            f[k].setTexture(t[k]);
            f[k].setPosition(squareSize*j + 7, squareSize*i + 7);
            k++;
        }
    }
}

int getIndexFromCoord(float mouseCoord) {
    return mouseCoord/squareSize;
}

vector<string> getPieceFromSquareIndex(int row, int col) {
    int pieceId = board[row][col];
    return piece_map[pieceId];
}

void savePosition(int startRowIdx, int startColIdx, int endRowIdx, int endColIdx) {
    const int pieceId = board[startRowIdx][startColIdx];
    board[startRowIdx][startColIdx] = 0;
    board[endRowIdx][endColIdx] = pieceId;
    positions.push_back(board);
}

string squareToChessNote(int rowIdx, int colIdx) {
    string s;
    s += char(colIdx+97);   // letter
    s += char(7-rowIdx+49); // numer
    return s;
}

void printBoard(vector<vector<int>> board) {
    for (auto & row : board) {
        for (int cell : row) {
            cout << cell << " ";
        }
        cout << endl;
    }
}

bool isValidMove(vector<string> piece, int startRowIdx, int starColIdx, int endRowIdx, int endColIdx) {
    string pieceName = piece[0], pieceColor = piece[1];
    return true;
}

int main() {
    bool playComputer = false;
    int stockfishDepth = -1;
    try {
        cout << "player vs player or player vs stockfish (1 or 2)?: ";
        int choice;
        cin >> choice;
        cout << endl;
        if (choice == 1) {
            playComputer = false;
        } else if (choice == 2) {
            playComputer = true;
            cout << "Enter stockfish depth (1-10): ";
            cin >> stockfishDepth;
            if (stockfishDepth < 1 || stockfishDepth > 10) {
                throw range_error("Invalid depth");
            }
        } else {
            throw range_error("Invalid option");
        }
    } catch (range_error& e) {
        cerr << e.what() << endl;
        return 1;
    }

    RenderWindow window(VideoMode(453, 453), "Chess", Style::Titlebar | Style::Close);
    Vector2u windowSize = window.getSize();
    const auto screenWidth = static_cast<float>(windowSize.x);
    const auto screenHeight = static_cast<float>(windowSize.y);

    FILE* stockfishProcess;
    if (playComputer) {
        stockfishProcess = popen("stockfish.exe", "w");
        if (!stockfishProcess) return 1;
    }

    positions.push_back(board);
    loadPosition();

    Texture tBoard;
    tBoard.loadFromFile("images/board0.png");
    Sprite sBoard(tBoard);

    bool isMove=false;
    bool whiteMove=true;
    float dx=0, dy=0;
    int currPieceIndex=0;
    int startRowIdx=0, startColIdx=0;
    Vector2f startPos, endPos;
    vector<string> movesPlayed;

    while (window.isOpen()) {
        Vector2i mousePos = Mouse::getPosition(window);

        Event event{};
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
               window.close();
            }

            if (event.type == Event::MouseButtonPressed) {
                if (event.mouseButton.button == Mouse::Left) {
                    for (int i=0; i<32; i++) {
                        if (f[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            startRowIdx = getIndexFromCoord(mousePos.y);
                            startColIdx = getIndexFromCoord(mousePos.x);
                            vector<string> piece = getPieceFromSquareIndex(startRowIdx, startColIdx);
                            if (whiteMove && piece[1] != "white") continue;
                            if (!whiteMove && piece[1] != "black") continue;
                            // cout << startRowIdx << startColIdx << " ";
                            // currPiece = getPieceFromSquareIndex(startRowIdx, startColIdx);
                            // cout << currPiece[0] << " " << currPiece[1] << " ";

                            isMove = true;
                            currPieceIndex = i;

                            // center piece on mouse cursor
                            f[currPieceIndex].setPosition(mousePos.x - (squareSize/2 - 7), mousePos.y - (squareSize/2));

                            dx = mousePos.x - f[currPieceIndex].getPosition().x;
                            dy = mousePos.y - f[currPieceIndex].getPosition().y;
                        }
                    }
                }
            }

            if (event.type == Event::MouseButtonReleased) {
                if (event.mouseButton.button == Mouse::Left && isMove) {
                    isMove = false;

                    // center piece on square after moving
                    Vector2f p = f[currPieceIndex].getPosition() + Vector2f(squareSize/2, squareSize/2);
                    Vector2f newPos = Vector2f(squareSize*int(p.x/squareSize)+7, squareSize*int(p.y/squareSize)+7);

                    int endRowIdx = getIndexFromCoord(newPos.y);
                    int endColIdx = getIndexFromCoord(newPos.x);
                    if (mousePos.x < 0 || mousePos.x > screenWidth || mousePos.y < 0 || mousePos.y > screenHeight
                        || endRowIdx > 7 || endColIdx > 7 || (startRowIdx==endRowIdx && startColIdx==endColIdx))
                    { // invalid end point
                        f[currPieceIndex].setPosition(squareSize*startColIdx+7, squareSize*startRowIdx+7);
                    } else {
                        savePosition(startRowIdx, startColIdx, endRowIdx, endColIdx);
                        loadPosition();

                        if (!playComputer) { // not playing stockfish; flip current color and continue
                            whiteMove = !whiteMove;
                            continue;
                        }

                        string myMove = squareToChessNote(startRowIdx, startColIdx) + squareToChessNote(endRowIdx, endColIdx);
                        movesPlayed.push_back(myMove);
                        string positionCommand = "position startpos moves";
                        for (const string& move: movesPlayed) {
                            positionCommand += " " + move;
                        }
                        int startRow, startCol, endRow, endCol;
                        cout << positionCommand << endl;
                        fprintf(stockfishProcess, "%s\n", positionCommand.c_str());
                        fprintf(stockfishProcess, "go depth %d\n", stockfishDepth);
                        fflush(stockfishProcess);

                        char buffer[1024];
                        while (fgets(buffer, sizeof(buffer), stockfishProcess)!=nullptr) {
                            // Read Stockfish's output line by line
                            printf("Stockfish response: %s", buffer);
                            if (strncmp(buffer, "bestmove", 8) == 0) {
                                // Extract the best move from Stockfish's output
                                cout << "here" << endl;
                                string bestMove = buffer + 9; // Skip "bestmove "
                                // Use the best move to update the position
                                movesPlayed.push_back(bestMove);
                            }
                        }
                        string move = movesPlayed.back();
                        startCol = move[0] - 'a';
                        startRow = 8-(move[1]-'0');
                        endCol = move[2] - 'a';
                        endRow = 8-(move[3]-'0');
                        cout << "hi" << startRow << " " << startCol << " " << endRow << " " << endCol << endl;
                        savePosition(startRow, startCol, endRow, endCol);
                        loadPosition();
                    }
                }
            }

            if (event.type == Event::KeyReleased) {
                if (event.key.code == Keyboard::Backspace) {
                    if (positions.size() <= 1) continue; // can't undo
                    positions.pop_back();
                    board = positions.back();
                    loadPosition();
                    whiteMove = !whiteMove;
                }
            }
        }

       if (isMove) f[currPieceIndex].setPosition(mousePos.x-dx,mousePos.y-dy);

       window.clear();
       window.draw(sBoard);
       for (int i=0; i<32; i++) window.draw(f[i]);
       window.display();
    }

    pclose(stockfishProcess);
    return 0;
}
