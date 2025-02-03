#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <sstream>

using namespace std;

const char XOR_KEY = 0x5A; // Encryption key

// Player struct to store stats
struct playerData {
    int money;
    int maxStreak;
    int maxScore;
    int maxWinnings;
    int runs;
};

string encryptDecrypt(const string &data) {
    string result = data;
    for (char &c : result) {
        c ^= XOR_KEY; // XOR each character
    }
    return result;
};

// Function to load player data
void loadData(playerData &player) {
    ifstream file("pData.txt");
    
    if (file.is_open()) {
        string encryptedData;
        getline(file, encryptedData);
        file.close();

        string decryptedData = encryptDecrypt(encryptedData);
        istringstream iss(decryptedData);
        iss >> player.money >> player.maxStreak >> player.maxScore >> player.maxWinnings >> player.runs;
        
        if (iss.fail()) { // Reset to default if file is corrupted
            player.money = 100;
            player.maxStreak = 0;
            player.maxScore = 0;
            player.maxWinnings = 0;
            player.runs = 0;
            }
        } else {
        // Create file with default values if not found
        player.money = 100;
        player.maxStreak = 0;
        player.maxScore = 0;
        player.maxWinnings = 0;
        player.runs = 0;
        playerData(player);
        ofstream newFile("pData.txt");
        newFile << player.money << " " 
            << player.maxStreak << " " 
            << player.maxScore << " " 
            << player.maxWinnings << " " 
            << player.runs << endl;
        newFile.close();
    }
};

// Function to save player data
void saveData(const playerData &player) {
    ofstream file("pData.txt");
    if (file.is_open()) {
        ostringstream oss;
        oss << player.money << " " 
            << player.maxStreak << " " 
            << player.maxScore << " " 
            << player.maxWinnings << " " 
            << player.runs << endl;
        string encryptedData = encryptDecrypt(oss.str());
        file << encryptedData;
        file.close();
    } else {
        cout << "Error: Unable to save data!" << endl;
    }
};

void resetPlayerData() {
    ofstream file("pData.txt", ios::trunc); // Clear player data file
    file.close();
    cout << "it's over, try again in your next life.." << endl;
};

// Function to generate random numbers
int generateRandomNumber(int turn) {
    static random_device rd;
    static mt19937 gen(rd());
    
    int base = 10;  
    double upperLimit = base * (1 + turn / 5.0);  // Slower growth

    uniform_int_distribution<int> dist(-upperLimit, upperLimit);
    return dist(gen);
};

int main() {
    playerData player;
    loadData(player);

    int choice, bet, guess, num, lastNum, turn = 1, streak = 0, score = 0, winnings = 0, run = 0;
    bool gameRunning = true;

    cout << "welcome to Gambler1!" << endl;
    cout << "i'm the DEALER. guess whether the next number will be higher or lower!" << endl;
    cout << "simple shit right? so come and see just how far you can get!" << endl;
    
    while (gameRunning) {
        turn = 1;
        streak = 0;
        score = 0;
        winnings = 0;

        cout << endl;
        cout << "\n1 - Play | 2 - View Stats | 3 - Quit\n";
        cin >> choice;

        if (choice == 1) {
            player.runs += 1;
            if (player.money <= 0) {
                cout << endl;
                cout << "you're actually fucking broke! i'm deleting your save file, dumbass\n";
                resetPlayerData();
                return 0;
            }

            cout << endl;
            cout << "how much you willin' to bet?\n Your cash: " << player.money << "\n";
            do {
                cin >> bet;
            } while (bet <= 0 || bet > player.money);

            player.money -= bet;  // Deduct bet
            turn = 1;
            lastNum = generateRandomNumber(turn);

            cout << endl;
            cout << "heres your first number: " << lastNum << endl;

            while (true) {
                num = generateRandomNumber(turn + 1);
                if (turn != 1) {
                    cout << "nice, the number was " << lastNum << endl;
                }
                cout << "so whats the next number gonna be????\n";
                cout << endl;
                cout << "1 (bigger) | 2 (smaller) | e (cash out and quit, pussy)\n";
                cout << endl;
                
                string input;
                cin >> input;

                if (input == "e" || input == "exit" || input == "leave") {
                    cout << endl;
                    cout << "checkin' out a bit soon aren't we?\n fine, here's your cash, pussy\n";
                    player.money += winnings;  // Add winnings to player money
                    saveData(player);

                    player.maxWinnings = max(player.maxWinnings, winnings);
                    player.runs += run;
                    playerData(player);
                    break;
                }

                guess = stoi(input);

                if ((guess == 1 && num >= lastNum) || (guess == 2 && num <= lastNum)) {
                    streak++;
                    turn++;
                    score += 10 + rand() % (5 * streak);  // Randomized exponential increase
                    winnings += bet * (1 + streak * 0.2);  // Winning multiplier
                    cout << endl;
                    cout << "Streak: " << streak 
                        << " | Score: " << score 
                        << " | Money: " << player.money 
                        << " | Winnings: " << winnings << endl;
                } else {
                    cout << endl;
                    cout << "hahahahahahaha dumbass!\n the number was obiously " << num << endl;
                    
                    // Double or Nothing (10% chance)
                    if (rand() % 10 == 0) {
                        cout << "bonus round, motherfucker!! double or nothin'!\n";
                        int bonusNum = generateRandomNumber(turn + 2);
                        cout << "New number: ???\n";
                        cin >> guess;
                        
                        if ((guess == 1 && bonusNum >= num) || (guess == 2 && bonusNum <= num)) {
                            cout << endl;
                            cout << "nice shit, bucko! easy double amirite?\n why not see you can get away with more?\n";
                            player.money += bet * 2;
                        } else {
                            cout << endl;
                            cout << "man, you fuckin' suck!!\n imagine losing when you get an extra chance..\n i think its time you go and rethink your pathetic little insignificant life";
                        }
                    }
                    
                    streak = 0;
                    score = 0;
                    break;
                }

                turn++;
                lastNum = num;
            }

            player.maxStreak = max(streak, player.maxStreak);
            player.maxScore = max(score, player.maxScore);
            player.runs += run;
            saveData(player);

        } else if (choice == 2) {
            cout << endl;
            cout << "Your Stats: High Score: " << player.maxScore 
                << " | Max Streak: " << player.maxStreak 
                << " | Max Winnings: " << player.maxWinnings 
                << " | Runs: " << player.runs << endl;
        } else if (choice == 3) {
            cout << endl;
            cout << "fuckin' pussy\n";
            saveData(player);
            gameRunning = false;
        } else {
            cout << endl;
            cout << "1 for PLAY, 2 for SCORES and 3 to QUIT, dumbass\n";
        }
    }

    return 0; // Returns 0 to the operating system, indicating that the program ended successfully
}

