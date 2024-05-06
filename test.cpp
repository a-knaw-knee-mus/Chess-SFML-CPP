#include <cstdio>
#include <cstring> // Include the header for strcmp
#include <string>
#include <vector>
#include <iostream>
using namespace std;

int main() {
    FILE* stockfishProcess = popen("stockfish.exe", "w");
    if (!stockfishProcess) {
        // Handle error
        return 1;
    }

    std::vector<std::string> movesPlayed; // Store the moves played

    // Function to send a move to Stockfish and update the position
    auto sendMoveAndUpdatePosition = [&](const std::string& move) {
        movesPlayed.push_back(move); // Add the move to the list of moves played

        std::string positionCommand = "position startpos moves";
        for (const std::string& move : movesPlayed) {
            positionCommand += " " + move;
        }

        fprintf(stockfishProcess, "%s\n", positionCommand.c_str()); // Update the position
        fprintf(stockfishProcess, "go\n"); // Request Stockfish to analyze the position
        fflush(stockfishProcess); // Flush the buffer
    };

    // Initial move
    sendMoveAndUpdatePosition("e2e4");

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stockfishProcess)) {
        // Read Stockfish's output line by line
        printf("Stockfish response: %s", buffer);

        // Parse Stockfish's response to get the best move
        // (You'll need to implement this parsing logic)

        // Example: Assuming Stockfish outputs the best move as "bestmove d2d4"
        if (strncmp(buffer, "bestmove", 8) == 0) {
            // Extract the best move from Stockfish's output
            std::string bestMove = buffer + 9; // Skip "bestmove "
            // Use the best move to update the position
            sendMoveAndUpdatePosition(bestMove);
            cout << "here" << endl;
        }
    }

    pclose(stockfishProcess); // Close the Stockfish process
    return 0;
}