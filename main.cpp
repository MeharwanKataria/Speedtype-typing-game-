#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <ctime>
#include <chrono>

using namespace std;

// Add SecurityInfo structure after the includes and before the User class
struct SecurityInfo {
    string age;
    string schoolName;
};

// Forward declarations
bool resetPassword(const string& username, const string& newPassword);
string hashString(const string& input);

// Simple string hashing function using djb2 algorithm for simple understanding
string hashString(const string& input) {
    unsigned int hash = 5381;
    for (char c : input) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    stringstream ss;
    ss << hex << hash;
    return ss.str();
}

// Password reset function implementation
bool resetPassword(const string& username, const string& newPassword) {
    cout << "Attempting to reset password for user: " << username << endl;
    
    ifstream file("users.txt");
    if (!file.is_open()) {
        cout << "Error: Could not open users.txt" << endl;
        return false;
    }

    vector<string> lines;
    string line;
    bool userFound = false;

    // Read all lines and find the user
    while (getline(file, line)) {
        istringstream iss(line);
        string storedUsername;
        iss >> storedUsername;
        
        if (storedUsername == username) {
            userFound = true;
            // Add the new password hash
            lines.push_back(username + " " + hashString(newPassword));
        } else {
            lines.push_back(line);
        }
    }

    if (!userFound) {
        cout << "Error: User not found" << endl;
        return false;
    }

    // Write back all lines
    ofstream outFile("users.txt");
    if (outFile.is_open()) {
        for (const auto& l : lines) {
            outFile << l << endl;
        }
        cout << "Password reset successful" << endl;
        return true;
    }

    cout << "Error: Could not write to users.txt" << endl;
    return false;
}

// User class for authentication
class User {
private:
    string username;
    string hashedPassword;
    SecurityInfo securityInfo;

    string hashPassword(const string& password) {
        return hashString(password);
    }

public:
    User() : username("") {}
    
    bool registerUser(const string& username, const string& password, const SecurityInfo& security) {
        cout << "Attempting to register user: " << username << endl;
        
        // Check if username is empty
        if (username.empty()) {
            cout << "Error: Username cannot be empty" << endl;
            return false;
        }
        
        // Check if password is empty
        if (password.empty()) {
            cout << "Error: Password cannot be empty" << endl;
            return false;
        }

        ifstream file("users.txt");
        string line;
        
        // Check if file exists and can be opened
        if (!file.is_open()) {
            cout << "Creating new users.txt file" << endl;
        }
        
        // Check if username already exists
        while (getline(file, line)) {
            istringstream iss(line);
            string storedUsername;
            iss >> storedUsername;
            if (storedUsername == username) {
                cout << "Error: Username already exists" << endl;
                return false;
            }
        }
        
        // Register new user
        ofstream outFile("users.txt", ios::app);
        if (outFile.is_open()) {
            string hashedPass = hashPassword(password);
            outFile << username << " " << hashedPass << " " 
                   << security.age << " " << security.schoolName << endl;
            this->username = username;
            this->hashedPassword = hashedPass;
            this->securityInfo = security;
            cout << "Successfully registered user: " << username << endl;
            return true;
        } else {
            cout << "Error: Could not open users.txt for writing" << endl;
            return false;
        }
    }

    bool verifySecurityInfo(const string& username, const SecurityInfo& security) {
        ifstream file("users.txt");
        string line;
        
        while (getline(file, line)) {
            istringstream iss(line);
            string storedUsername, storedHash, storedAge, storedSchool;
            iss >> storedUsername >> storedHash >> storedAge >> storedSchool;
            
            if (storedUsername == username && 
                storedAge == security.age && 
                storedSchool == security.schoolName) {
                return true;
            }
        }
        return false;
    }
    
    bool loginUser(const string& username, const string& password) {
        cout << "Attempting to login user: " << username << endl;
        
        // Check if username is empty
        if (username.empty()) {
            cout << "Error: Username cannot be empty" << endl;
            return false;
        }
        
        // Check if password is empty
        if (password.empty()) {
            cout << "Error: Password cannot be empty" << endl;
            return false;
        }

        ifstream file("users.txt");
        if (!file.is_open()) {
            cout << "Error: Could not open users.txt" << endl;
            return false;
        }

        string line;
        string hashedPass = hashPassword(password);
        
        while (getline(file, line)) {
            istringstream iss(line);
            string storedUsername, storedHash, storedAge, storedSchool;
            iss >> storedUsername >> storedHash >> storedAge >> storedSchool;
            
            cout << "Checking against stored user: " << storedUsername << endl;
            
            if (storedUsername == username && storedHash == hashedPass) {
                cout << "Login successful for user: " << username << endl;
                this->username = username;
                this->hashedPassword = storedHash;
                this->securityInfo = {storedAge, storedSchool};
                return true;
            }
        }
        
        cout << "Login failed: Invalid username or password" << endl;
        return false;
    }
    
    string getUsername() const {
        return username;
    }
    
    bool isLoggedIn() const {
        return !username.empty();
    }
};

// FileHandler class for file operations
class FileHandler {
public:
//saving the information of user into user.txt
    static bool saveUser(const string& username, const string& hashedPassword) {
        try {
            ofstream file("users.txt", ios::app);
            if (!file.is_open()) {
                throw runtime_error("Could not open users.txt for writing");
            }
            file << username << " " << hashedPassword << endl;
            return true;
        } catch (const exception& e) {
            cerr << "Error saving user: " << e.what() << endl;
            return false;
        }
    }
// checking the information of user from user.txt is present or not
    static bool checkUser(const string& username, const string& hashedPassword) {
        try {
            ifstream file("users.txt");
            if (!file.is_open()) {
                throw runtime_error("Could not open users.txt for reading");
            }
            string line;
            while (getline(file, line)) {
                istringstream iss(line);
                string storedUsername, storedHash;
                iss >> storedUsername >> storedHash;
                if (storedUsername == username && storedHash == hashedPassword) {
                    return true;
                }
            }
            return false;
        } catch (const exception& e) {
            cerr << "Error checking user: " << e.what() << endl;
            return false;
        }
    }
// loading the paragraphs from the file according to the difficulty level
    static vector<string> loadParagraphs(const string& difficulty) {
        vector<string> paragraphs;
        string filename = "paragraphs_" + difficulty + ".txt";
        
        try {
            ifstream file(filename);
            if (!file.is_open()) {
                throw runtime_error("Could not open " + filename);
            }
        
            string paragraph;
            string line;
            bool inParagraph = false;
            
            while (getline(file, line)) {
                // Trim whitespace from the line
                line.erase(0, line.find_first_not_of(" \t\n\r"));
                line.erase(line.find_last_not_of(" \t\n\r") + 1);
                
                if (line.empty()) {
                    // If we were in a paragraph and hit a blank line, save the paragraph
                    if (inParagraph && !paragraph.empty()) {
                        paragraphs.push_back(paragraph);
                        paragraph.clear();
                        inParagraph = false;
                    }
                } else {
                    // If this is a new paragraph, start fresh
                    if (!inParagraph) {
                        paragraph = line;
                        inParagraph = true;
                    } else {
                        // If we're continuing a paragraph, add a space and the line
                        paragraph += " " + line;
                    }
                }
            }
            
            // Don't forget to add the last paragraph if it exists
            if (!paragraph.empty()) {
                paragraphs.push_back(paragraph);
            }

            if (paragraphs.empty()) {
                throw runtime_error("No paragraphs found in " + filename);
            }

            return paragraphs;
        } catch (const exception& e) {
            throw runtime_error("Error loading paragraphs: " + string(e.what()));
        }
    }
// saving the record of user into records.txt
    static void saveRecord(const string& username, float wpm, float accuracy, const string& difficulty) {
        try {
            ofstream file("records.txt", ios::app);
            if (!file.is_open()) {
                throw runtime_error("Could not open records.txt for writing");
            }
            time_t now = time(0);
            char* dt = ctime(&now);
            string timestamp(dt);
            timestamp = timestamp.substr(0, timestamp.length() - 1);
            
            file << username << "," << wpm << "," << accuracy << "," 
                 << difficulty << "," << timestamp << endl;
        } catch (const exception& e) {
            cerr << "Error saving record: " << e.what() << endl;
        }
    }
// loading the records from the file to view the leaderboard
    static vector<string> loadRecords() {
        vector<string> records;
        try {
            ifstream file("records.txt");
            if (!file.is_open()) {
                throw runtime_error("Could not open records.txt for reading");
            }
            string line;
            while (getline(file, line)) {
                if (!line.empty()) {
                    records.push_back(line);
                }
            }
            return records;
        } catch (const exception& e) {
            cerr << "Error loading records: " << e.what() << endl;
            return records;
        }
    }
// checking the required files for the game of difficulty levels
    static bool checkRequiredFiles() {
        vector<string> requiredFiles = {
            "paragraphs_easy.txt",
            "paragraphs_medium.txt",
            "paragraphs_hard.txt"
        };
        
        for (const auto& filename : requiredFiles) {
            ifstream file(filename);
            if (!file.is_open()) {
                cerr << "Error: Required file " << filename << " not found!" << endl;
                return false;
            }
        }
        return true;
    }
};

// ParagraphManager class for managing typing paragraphs
class ParagraphManager {
private:
    vector<string> paragraphs;
    mt19937 rng; //using built in function for random number generation,
                // which generates random paragraph from file

public:
    ParagraphManager(const string& difficulty) {
        paragraphs = FileHandler::loadParagraphs(difficulty);
        random_device rd;
        rng = mt19937(rd());
    }

    string getRandomParagraph() {
        if (paragraphs.empty()) {
            return "No paragraphs available.";
        }
        
        uniform_int_distribution<size_t> dist(0, paragraphs.size() - 1);
        return paragraphs[dist(rng)];
    }

    size_t getParagraphCount() const {
        return paragraphs.size();
    }
};

// Record structure for storing game results
struct Record {
    string username;
    float wpm;
    float accuracy;
    string difficulty;
    string timestamp;

    // Add default constructor
    Record() : wpm(0), accuracy(0) {}

    Record(const string& line) {
        istringstream iss(line);
        string token;
        
        getline(iss, username, ',');
        getline(iss, token, ',');
        wpm = stof(token);
        getline(iss, token, ',');
        accuracy = stof(token);
        getline(iss, difficulty, ',');
        getline(iss, timestamp, ',');
    }
};

// Leaderboard class for managing scores
class Leaderboard {
private:
    vector<Record> records;

public:
    Leaderboard() {
        loadRecords();
    }

    void loadRecords() {
        records.clear();
        vector<string> lines = FileHandler::loadRecords();
        for (const auto& line : lines) {
            records.emplace_back(line);
        }
    }

    vector<Record> getAllScores() const {
        return records;
    }

    float getBestSpeed() const {
        if (records.empty()) return 0.0f;
        return max_element(records.begin(), records.end(),
            [](const Record& a, const Record& b) { return a.wpm < b.wpm; })->wpm;
    }

    vector<Record> getTop5() const {
        vector<Record> sorted = records;
        sort(sorted.begin(), sorted.end(),
            [](const Record& a, const Record& b) { return a.wpm > b.wpm; });
        
        if (sorted.size() > 5) {
            sorted.resize(5);
        }
        return sorted;
    }

    vector<Record> getUserRecords(const string& username) const {
        vector<Record> userRecords;
        for (const auto& record : records) {
            if (record.username == username) {
                userRecords.push_back(record);
            }
        }
        return userRecords;
    }

    float getUserAverageSpeed(const string& username) const {
        auto userRecords = getUserRecords(username);
        if (userRecords.empty()) return 0.0f;
        
        float total = 0.0f;
        for (const auto& record : userRecords) {
            total += record.wpm;
        }
        return total / userRecords.size();
    }

    float getUserBestSpeed(const string& username) const {
        auto userRecords = getUserRecords(username);
        if (userRecords.empty()) return 0.0f;
        
        return max_element(userRecords.begin(), userRecords.end(),
            [](const Record& a, const Record& b) { return a.wpm < b.wpm; })->wpm;
    }
};

// Constants for window and UI
namespace GameConstants { // namespace is like a container or class which contain constants data memebers or function and variable
    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 600;
    constexpr float BASE_WIDTH = 800.0f;
    constexpr float BASE_HEIGHT = 600.0f;
    constexpr float MIN_SCALE = 0.8f;
    constexpr float MAX_SCALE = 2.0f;
    constexpr size_t MAX_LINE_LENGTH = 70;
    constexpr int LINE_HEIGHT = 30;
    constexpr int FONT_SIZE = 22;
    constexpr int MARGIN_X = 50;
    constexpr int MARGIN_Y = 40;
    constexpr int MAX_VISIBLE_LINES = 12;
    constexpr int COMPLETION_FONT_SIZE = 32;
    constexpr float TYPING_SPEED_THRESHOLD = 0.1f;  // Minimum time between keystrokes
    
    // Update timer durations for different difficulties (in seconds)
    constexpr float EASY_TIMER_DURATION = 60.0f;    // 1 minute
    constexpr float MEDIUM_TIMER_DURATION = 120.0f;  // 2 minutes
    constexpr float HARD_TIMER_DURATION = 180.0f;    // 3 minutes
}

// Helper function for clamping values 
// it help the number to be in the range of min and max
float clamp(float value, float min, float max) {
    return value < min ? min : (value > max ? max : value);
}

// Color constants
const sf::Color CORRECT_COLOR(34, 139, 34);     // Green
const sf::Color INCORRECT_COLOR(220, 20, 60);   // Red
const sf::Color REMAINING_COLOR(128, 128, 128); // Gray
const sf::Color CURRENT_COLOR(70, 130, 180);    // Blue

// Game class for managing the typing game
class Game {
private:
    User& user;
    string paragraph;
    string userInput;
    vector<string> wrappedLines;
    vector<vector<bool>> charCorrect;
    size_t currentLine;
    size_t currentChar;
    int mistakes;
    sf::Clock clock;
    bool finished;
    float wpm;
    float accuracy;
    string difficulty;
    bool timerStarted;
    bool showFullParagraph;
    bool statsUpdated;
    bool gameStarted;
    bool showingResults;
    sf::Vector2u windowSize;
    bool isMinimized;
    sf::Time lastKeyPressTime;
    sf::Clock keyPressClock;
    float timerDuration;  // Add this member variable
    sf::Time pausedTime;

    // Helper function to get timer duration based on difficulty
    float getTimerDuration(const string& diff) {
        if (diff == "easy") return GameConstants::EASY_TIMER_DURATION;
        if (diff == "medium") return GameConstants::MEDIUM_TIMER_DURATION;
        if (diff == "hard") return GameConstants::HARD_TIMER_DURATION;
        return GameConstants::MEDIUM_TIMER_DURATION; // default
    }

    // Helper function to calculate proper scaling
    float calculateScale(const sf::Vector2u& size) {
        float scaleX = size.x / GameConstants::BASE_WIDTH;
        float scaleY = size.y / GameConstants::BASE_HEIGHT;
        float scale = std::min(scaleX, scaleY);
        return clamp(scale, GameConstants::MIN_SCALE, GameConstants::MAX_SCALE);
    }

    // Helper function to center text horizontally
    void centerText(sf::Text& text, float x, float y, float scale = 1.0f) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setPosition(
            x - (bounds.width * scale) / 2.0f,
            y
        );
    }

    void wrapParagraphPixelWidth(const sf::Font& font, float fontSize, float maxWidth) {
        wrappedLines.clear();
        charCorrect.clear();
        string cleanParagraph = paragraph;
        size_t pos;
        while ((pos = cleanParagraph.find("  ")) != string::npos) {
            cleanParagraph.replace(pos, 2, " ");
        }
        cleanParagraph.erase(0, cleanParagraph.find_first_not_of(" \t\n\r"));
        cleanParagraph.erase(cleanParagraph.find_last_not_of(" \t\n\r") + 1);
        istringstream iss(cleanParagraph);
        string word, lineBuffer;
        vector<bool> currentLineCorrect;
        while (iss >> word) {
            sf::Text testText(lineBuffer.empty() ? word : lineBuffer + " " + word, font, (unsigned int)fontSize);
            float testWidth = testText.getLocalBounds().width;
            if (!lineBuffer.empty() && testWidth > maxWidth) {
                wrappedLines.push_back(lineBuffer);
                charCorrect.push_back(currentLineCorrect);
                lineBuffer = word;
                currentLineCorrect = vector<bool>(word.length(), true);
            } else {
                if (!lineBuffer.empty()) {
                    lineBuffer += " ";
                    currentLineCorrect.push_back(true);
                }
                lineBuffer += word;
                currentLineCorrect.insert(currentLineCorrect.end(), word.length(), true);
            }
        }
        if (!lineBuffer.empty()) {
            wrappedLines.push_back(lineBuffer);
            charCorrect.push_back(currentLineCorrect);
        }
        currentLine = 0;
        currentChar = 0;
        mistakes = 0;
        userInput.clear();
        timerStarted = false;
        lastKeyPressTime = sf::Time::Zero;
    }

    void calculateStats() {
        if (!timerStarted) return;
        
        // Calculate elapsed time in minutes, accounting for paused time
        float timeInMinutes = (clock.getElapsedTime().asSeconds() - pausedTime.asSeconds()) / 60.0f;
        if (timeInMinutes <= 0) return;

        // Count correct characters from what has been typed so far
        int correctChars = 0;
        int totalTyped = 0;
        
        // Count characters up to current position
        for (size_t line = 0; line <= currentLine; ++line) {
            size_t lineLength = (line == currentLine) ? currentChar : wrappedLines[line].length();
            for (size_t col = 0; col < lineLength; ++col) {
                if (line < charCorrect.size() && col < charCorrect[line].size()) {
                    totalTyped++;
                    // Only count as correct if it matches the original text exactly
                    if (charCorrect[line][col] && 
                        line < wrappedLines.size() && 
                        col < wrappedLines[line].length() && 
                        userInput[totalTyped - 1] == wrappedLines[line][col]) {
                        correctChars++;
                    }
                }
            }
        }

        // Calculate WPM (Words Per Minute)
        // Standard definition: 1 word = 5 characters
        // WPM = (correct characters / 5) / minutes
        if (timeInMinutes > 0 && totalTyped > 0) {
            // Convert correct characters to words (divide by 5)
            float correctWords = correctChars / 5.0f;
            // Calculate WPM
            wpm = correctWords / timeInMinutes;
            
            // Ensure WPM is not negative and is reasonable
            if (wpm < 0) wpm = 0;
            if (wpm > 200) wpm = 200; // Cap at 200 WPM for sanity
        } else {
            wpm = 0;
        }

        // Calculate accuracy percentage
        if (totalTyped > 0) {
            accuracy = (static_cast<float>(correctChars) / totalTyped) * 100.0f;
            // Ensure accuracy is between 0 and 100
            if (accuracy < 0) accuracy = 0;
            if (accuracy > 100) accuracy = 100;
        } else {
            accuracy = 0;
        }
    }

    bool isTimeUp() const {
        return timerStarted && clock.getElapsedTime().asSeconds() >= timerDuration;
    }

    void updateLeaderboard() {
        if (!statsUpdated) {
            FileHandler::saveRecord(user.getUsername(), wpm, accuracy, difficulty);
            statsUpdated = true;
        }
    }

public:
    Game(User& user, const string& difficulty) 
        : user(user), mistakes(0), finished(false), wpm(0), accuracy(0), 
          difficulty(difficulty), currentLine(0), currentChar(0),
          timerStarted(false), showFullParagraph(false), statsUpdated(false), 
          gameStarted(true), showingResults(false), isMinimized(false) {
        try {
            // Set timer duration based on difficulty
            timerDuration = getTimerDuration(difficulty);
            
            // Load paragraphs for the selected difficulty
            vector<string> paragraphs = FileHandler::loadParagraphs(difficulty);
            if (paragraphs.empty()) {
                throw runtime_error("No paragraphs available for " + difficulty + " difficulty");
            }
            
            // Use a better random number generation method
            // Get a random seed from the system clock and random device
            auto seed = chrono::high_resolution_clock::now().time_since_epoch().count() ^ 
                       random_device{}();
            mt19937 gen(static_cast<unsigned int>(seed));
            
            // Create uniform distribution for random selection
            uniform_int_distribution<size_t> dist(0, paragraphs.size() - 1);
            
            // Select a random paragraph
            paragraph = paragraphs[dist(gen)];
            
            // Load font locally for pixel-based wrapping
            sf::Font localFont;
            if (!localFont.loadFromFile("arial.ttf")) {
                throw runtime_error("Failed to load font for wrapping");
            }
            float marginX = GameConstants::WINDOW_WIDTH * 0.07f;
            float maxWidth = GameConstants::WINDOW_WIDTH - 2 * marginX;
            wrapParagraphPixelWidth(localFont, GameConstants::FONT_SIZE, maxWidth);
        } catch (const exception& e) {
            throw runtime_error("Failed to initialize game: " + string(e.what()));
        }
    }

    void processInput(sf::Event& event) {
        if (finished && showingResults) {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q) {
                showingResults = false;
            }
            return;
        }

        if (event.type == sf::Event::TextEntered) {
            sf::Time currentTime = keyPressClock.getElapsedTime();
            
            if (!timerStarted) {
                timerStarted = true;
                clock.restart();
                keyPressClock.restart();
            }

            if (event.text.unicode == '\b') {
                if (currentChar > 0) {
                    currentChar--;
                    userInput.pop_back();
                    // Update accuracy calculation
                    if (!charCorrect[currentLine][currentChar]) {
                        mistakes--;
                    }
                } else if (currentLine > 0) {
                    currentLine--;
                    currentChar = wrappedLines[currentLine].length();
                    userInput = userInput.substr(0, userInput.length() - wrappedLines[currentLine].length());
                }
            }
            else if (event.text.unicode < 128 && event.text.unicode != '\r' && event.text.unicode != '\n') {
                char c = static_cast<char>(event.text.unicode);
                
                if (currentLine < wrappedLines.size()) {
                    if (currentChar < wrappedLines[currentLine].length()) {
                        bool isCorrect = (c == wrappedLines[currentLine][currentChar]);
                        charCorrect[currentLine][currentChar] = isCorrect;
                        if (!isCorrect) mistakes++;
                        
                        userInput += c;
                        currentChar++;
                        
                        if (currentChar == wrappedLines[currentLine].length()) {
                            currentLine++;
                            currentChar = 0;
                            if (currentLine >= wrappedLines.size()) {
                                finish();
                            }
                        }
                        
                        calculateStats();
                    }
                }
            }
        }
    }

    void render(sf::RenderWindow& window) {
        if (isMinimized) return;
        window.clear(sf::Color(240, 240, 240));
        windowSize = window.getSize();
        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            return;
        }
        float scale = calculateScale(windowSize);
        float fontSize = GameConstants::FONT_SIZE * scale;
        float marginX = windowSize.x * 0.07f;
        float maxWidth = windowSize.x - 2 * marginX;
        if (wrappedLines.empty() || windowSize != window.getSize()) {
            wrapParagraphPixelWidth(font, fontSize, maxWidth);
        }
        float centerX = windowSize.x / 2.0f;
        float centerY = windowSize.y / 2.0f;

        // Draw timer as countdown with difficulty-specific duration
        if (!finished && timerStarted) {
            float remaining = timerDuration - clock.getElapsedTime().asSeconds();
            if (remaining < 0) remaining = 0;

            sf::Text timerText(std::to_string(static_cast<int>(remaining)), font, static_cast<int>(GameConstants::COMPLETION_FONT_SIZE * scale));
            timerText.setFillColor(sf::Color::Black);
            timerText.setPosition(50, GameConstants::MARGIN_Y * scale);
            window.draw(timerText);

            if (remaining <= 0 && !finished) {
                finish();
            }
        }

        if (finished && showingResults) {
            window.clear(sf::Color(0, 80, 160));

            // Background box centered at 800x400 (for 1600x800 window)
            sf::RectangleShape resultsBg(sf::Vector2f(800.0f * scale, 500.0f * scale));  // Made wider and taller to fit text
            resultsBg.setPosition(800 - 400.0f * scale, 400 - 250.0f * scale);  // Centered
            resultsBg.setFillColor(sf::Color(100, 200, 255));
            resultsBg.setOutlineThickness(2.0f * scale);
            resultsBg.setOutlineColor(sf::Color(200, 200, 200));
            window.draw(resultsBg);

            sf::Text completionText("Typing Complete! :)", font, static_cast<int>(GameConstants::COMPLETION_FONT_SIZE * scale));
            centerText(completionText, 800, 400 - 180.0f * scale);
            completionText.setFillColor(sf::Color::White);
            window.draw(completionText);

            sf::Text wpmText("Speed: " + to_string(static_cast<int>(wpm)) + " WPM", font, static_cast<int>(32 * scale));
            centerText(wpmText, 800, 400 - 90.0f * scale);
            wpmText.setFillColor(sf::Color::White);
            window.draw(wpmText);

            sf::Text accuracyText("Accuracy: " + to_string(static_cast<int>(accuracy)) + "%", font, static_cast<int>(32 * scale));
            centerText(accuracyText, 800, 400 - 30.0f * scale);
            accuracyText.setFillColor(sf::Color::White);
            window.draw(accuracyText);

            sf::Text mistakesText("Mistakes: " + to_string(mistakes), font, static_cast<int>(32 * scale));
            centerText(mistakesText, 800, 400 + 30.0f * scale);
            mistakesText.setFillColor(sf::Color::White);
            window.draw(mistakesText);

            sf::Text instructionText("Press Q to return to main menu", font, static_cast<int>(24 * scale));
            centerText(instructionText, 800, 400 + 120.0f * scale);
            instructionText.setFillColor(sf::Color::White);
            window.draw(instructionText);

            window.display();
            return;
        }

        // Draw typing interface with dynamic margin
        float y = GameConstants::MARGIN_Y * 2 * scale;
        float visibleHeight = windowSize.y - (3 * GameConstants::MARGIN_Y * scale);
        float maxVisibleLines = visibleHeight / (GameConstants::LINE_HEIGHT * scale);
        size_t startLine = 0;
        if (currentLine >= maxVisibleLines) {
            startLine = currentLine - maxVisibleLines + 1;
        }
        for (size_t i = startLine; i < wrappedLines.size() && i < startLine + GameConstants::MAX_VISIBLE_LINES; ++i) {
            const string& line = wrappedLines[i];
            float x = marginX;
            if (i < currentLine) {
                for (size_t j = 0; j < line.length(); ++j) {
                    sf::Text charText(string(1, line[j]), font, static_cast<int>(fontSize));
                    charText.setPosition(x, y);
                    charText.setFillColor(charCorrect[i][j] ? CORRECT_COLOR : INCORRECT_COLOR);
                    window.draw(charText);
                    x += charText.getLocalBounds().width;
                }
            } else if (i == currentLine) {
                for (size_t j = 0; j < line.length(); ++j) {
                    if (j < currentChar) {
                        sf::Text charText(string(1, line[j]), font, static_cast<int>(fontSize));
                        charText.setPosition(x, y);
                        charText.setFillColor(charCorrect[i][j] ? CORRECT_COLOR : INCORRECT_COLOR);
                        window.draw(charText);
                        x += charText.getLocalBounds().width;
                    } else if (j == currentChar) {
                        sf::Text charText(string(1, line[j]), font, static_cast<int>(fontSize));
                        charText.setPosition(x, y);
                        charText.setFillColor(CURRENT_COLOR);
                        window.draw(charText);
                        if (static_cast<int>(clock.getElapsedTime().asSeconds() * 2) % 2 == 0) {
                            sf::RectangleShape cursor(sf::Vector2f(2.0f * scale, fontSize));
                            cursor.setPosition(x, y);
                            cursor.setFillColor(CURRENT_COLOR);
                            window.draw(cursor);
                        }
                        x += charText.getLocalBounds().width;
                    } else {
                        sf::Text charText(string(1, line[j]), font, static_cast<int>(fontSize));
                        charText.setPosition(x, y);
                        charText.setFillColor(REMAINING_COLOR);
                        window.draw(charText);
                        x += charText.getLocalBounds().width;
                    }
                }
            } else {
                sf::Text lineText(line, font, static_cast<int>(fontSize));
                lineText.setPosition(x, y);
                lineText.setFillColor(REMAINING_COLOR);
                window.draw(lineText);
            }
            y += GameConstants::LINE_HEIGHT * scale;
        }
        window.display();
    }

    bool isFinished() const {
        return finished && !showingResults;
    }

    void finish() {
        if (!finished) {
            finished = true;
            calculateStats();
            updateLeaderboard();
            showingResults = true;
        }
    }

    void handleResize(const sf::Vector2u& newSize) {
        windowSize = newSize;
        if (windowSize.x < GameConstants::WINDOW_WIDTH) windowSize.x = GameConstants::WINDOW_WIDTH;
        if (windowSize.y < GameConstants::WINDOW_HEIGHT) windowSize.y = GameConstants::WINDOW_HEIGHT;
    }

    void setMinimized(bool minimized) {
        isMinimized = minimized;
    }

    float getWPM() const { return wpm; }
    float getAccuracy() const { return accuracy; }
};

// Button class for GUI elements
class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    bool isHovered;

public:
    Button(const sf::Vector2f& size, const sf::Vector2f& position, 
           const string& label, const sf::Font& font) {
        shape.setSize(size);
        shape.setPosition(position);
        shape.setFillColor(sf::Color(0, 122, 204));
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color(0, 95, 153));

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
        
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition(
            position.x + (size.x - textBounds.width) / 2,
            position.y + (size.y - textBounds.height) / 2
        );
        
        isHovered = false;
    }

    void setHovered(bool hovered) {
        isHovered = hovered;
        shape.setFillColor(hovered ? sf::Color(0, 150, 255) : sf::Color(0, 122, 204));
    }

    bool contains(const sf::Vector2f& point) const {
        return shape.getGlobalBounds().contains(point);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }
};

// Text input box class
class TextBox {
private:
    sf::RectangleShape shape;
    sf::Text text;
    string input;
    bool isSelected;

public:
    TextBox(const sf::Vector2f& size, const sf::Vector2f& position, 
            const sf::Font& font) {
        shape.setSize(size);
        shape.setPosition(position);
        shape.setFillColor(sf::Color::White);
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::Black);

        text.setFont(font);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
        text.setPosition(position.x + 5, position.y + 5);
        
        isSelected = false;
    }

    void setSelected(bool selected) {
        isSelected = selected;
        shape.setOutlineColor(selected ? sf::Color::Blue : sf::Color::Black);
    }

    bool contains(const sf::Vector2f& point) const {
        return shape.getGlobalBounds().contains(point);
    }

    void handleInput(sf::Event& event) {
        if (!isSelected) return;

        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode == '\b' && !input.empty()) {
                input.pop_back();
            }
            else if (event.text.unicode < 128 && event.text.unicode != '\r' && event.text.unicode != '\n') {
                input += static_cast<char>(event.text.unicode);
            }
            text.setString(input);
        }
    }

    string getInput() const {
        return input;
    }

    void clear() {
        input.clear();
        text.setString("");
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }
};

// Function declarations
bool showWelcomeScreen(sf::RenderWindow& window, User& user);
string showMainMenu(sf::RenderWindow& window, const User& user);
string showDifficultyScreen(sf::RenderWindow& window);
void showScores(sf::RenderWindow& window, const Leaderboard& leaderboard);
void showBestSpeed(sf::RenderWindow& window, const Leaderboard& leaderboard);
void showRankings(sf::RenderWindow& window, const Leaderboard& leaderboard);
void showComparison(sf::RenderWindow& window, const Leaderboard& leaderboard, const User& user);

// Function implementations
bool showWelcomeScreen(sf::RenderWindow& window, User& user) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "Error loading font" << std::endl;
        return false;
    }

    // Load background image
    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("background.jpg")) {
        std::cout << "Error loading background image" << std::endl;
    }
    sf::Sprite bgSprite(bgTexture);
    bgSprite.setScale(
        window.getSize().x / (float)bgTexture.getSize().x,
        window.getSize().y / (float)bgTexture.getSize().y
    );

    enum Screen { LOGIN, REGISTER, FORGOT_PASSWORD };
    Screen currentScreen = LOGIN;

// Welcome text
// Welcome text
sf::Text welcomeTitle("WELCOME TO SPEED TYPE", font, 40);
welcomeTitle.setPosition(560, 50); // centered for 1600 width
welcomeTitle.setFillColor(sf::Color(72, 61, 139));

// Login screen elements
TextBox usernameBox(sf::Vector2f(250, 35), sf::Vector2f(675, 200), font);
TextBox passwordBox(sf::Vector2f(250, 35), sf::Vector2f(675, 260), font);
Button loginButton(sf::Vector2f(100, 30), sf::Vector2f(750, 320), "Login", font);
Button toRegisterButton(sf::Vector2f(160, 30), sf::Vector2f(720, 370), "Create Account", font);
Button forgotButton(sf::Vector2f(170, 30), sf::Vector2f(715, 420), "Forgot Password", font);

// Registration screen elements
TextBox regUsernameBox(sf::Vector2f(250, 35), sf::Vector2f(675, 200), font);
TextBox regPasswordBox(sf::Vector2f(250, 35), sf::Vector2f(675, 260), font);
TextBox ageBox(sf::Vector2f(250, 35), sf::Vector2f(675, 320), font);
TextBox schoolBox(sf::Vector2f(250, 35), sf::Vector2f(675, 380), font);

// Centered buttons: 170px wide
float regButtonWidth = 170;
float regButtonX = 715; 
Button regSubmitButton(sf::Vector2f(regButtonWidth, 35), sf::Vector2f(regButtonX, 440), "Register", font);
Button toLoginButton(sf::Vector2f(regButtonWidth, 35), sf::Vector2f(regButtonX, 490), "Back to Login", font);

// Forgot password screen elements
TextBox forgotUsernameBox(sf::Vector2f(250, 35), sf::Vector2f(675, 200), font);
TextBox forgotAgeBox(sf::Vector2f(250, 35), sf::Vector2f(675, 260), font);
TextBox forgotSchoolBox(sf::Vector2f(250, 35), sf::Vector2f(675, 320), font);
TextBox newPasswordBox(sf::Vector2f(250, 35), sf::Vector2f(675, 380), font);
Button forgotSubmitButton(sf::Vector2f(145, 30), sf::Vector2f(730, 440), "Reset Password", font);
Button backToLoginButton(sf::Vector2f(120, 30), sf::Vector2f(740, 490), "Back to Login", font);

sf::Text messageText("", font, 20);
messageText.setPosition(725, 550);
messageText.setFillColor(sf::Color::Red);


    sf::Clock messageClock;
    bool showMessage = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(
                    sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

                // Handle text box selection
                if (currentScreen == LOGIN) {
                    usernameBox.setSelected(usernameBox.contains(mousePos));
                    passwordBox.setSelected(passwordBox.contains(mousePos));
                }
                else if (currentScreen == REGISTER) {
                    regUsernameBox.setSelected(regUsernameBox.contains(mousePos));
                    regPasswordBox.setSelected(regPasswordBox.contains(mousePos));
                    ageBox.setSelected(ageBox.contains(mousePos));
                    schoolBox.setSelected(schoolBox.contains(mousePos));
                }
                else if (currentScreen == FORGOT_PASSWORD) {
                    forgotUsernameBox.setSelected(forgotUsernameBox.contains(mousePos));
                    forgotAgeBox.setSelected(forgotAgeBox.contains(mousePos));
                    forgotSchoolBox.setSelected(forgotSchoolBox.contains(mousePos));
                    newPasswordBox.setSelected(newPasswordBox.contains(mousePos));
                }

                if (currentScreen == LOGIN) {
                    if (loginButton.contains(mousePos)) {
                        if (user.loginUser(usernameBox.getInput(), passwordBox.getInput())) {
                            return true;
                        } else {
                         messageText.setString("Invalid username or password");
                        messageText.setFillColor(sf::Color::Red);
                        sf::FloatRect textBounds = messageText.getLocalBounds();

                        float centerX = (1600 - textBounds.width) / 2;
                        float centerY = (600 - textBounds.height) / 2;

                        messageText.setPosition(centerX, centerY);

                        showMessage = true;
                        messageClock.restart();

                        }
                    }
                    else if (toRegisterButton.contains(mousePos)) {
                        currentScreen = REGISTER;
                        messageText.setString("");
                    }
                    else if (forgotButton.contains(mousePos)) {
                        currentScreen = FORGOT_PASSWORD;
                        messageText.setString("");
                    }
                }
                else if (currentScreen == REGISTER) {
                    if (regSubmitButton.contains(mousePos)) {
                        if (regUsernameBox.getInput().empty() || regPasswordBox.getInput().empty() ||
                            ageBox.getInput().empty() || schoolBox.getInput().empty()) {
                                
                            messageText.setString("All fields are required");
                            messageText.setFillColor(sf::Color::Red);
                             sf::FloatRect textBounds = messageText.getLocalBounds();

                            float centerX = (1600 - textBounds.width) / 2;
                            float centerY = (1100 - textBounds.height) / 2;
                            messageText.setPosition(centerX, centerY);
                            showMessage = true;
                            messageClock.restart();
                        }
                        else {
                            SecurityInfo security = {ageBox.getInput(), schoolBox.getInput()};
                            if (user.registerUser(regUsernameBox.getInput(), regPasswordBox.getInput(), security)) {
                                messageText.setString("Registration successful! Please login.");
                                messageText.setFillColor(sf::Color::Green);
                                sf::FloatRect textBounds = messageText.getLocalBounds();

                                float centerX = (1600 - textBounds.width) / 2;
                                float centerY = (1100 - textBounds.height) / 2;
                                messageText.setPosition(centerX, centerY);
                                showMessage = true;
                                messageClock.restart();
                                
                                // Clear registration fields
                                regUsernameBox.clear();
                                regPasswordBox.clear();
                                ageBox.clear();
                                schoolBox.clear();
                                
                                // Return to login screen
                                currentScreen = LOGIN;
                            } else {
                                messageText.setString("Username already exists");
                                messageText.setFillColor(sf::Color::Red);
                                sf::FloatRect textBounds = messageText.getLocalBounds();

                                float centerX = (1600 - textBounds.width) / 2;
                                float centerY = (600 - textBounds.height) / 2;
                                messageText.setPosition(centerX, centerY);
                                showMessage = true;
                                messageClock.restart();
                            }
                        }
                    }
                    else if (toLoginButton.contains(mousePos)) {
                        // Clear registration fields when going back to login
                        regUsernameBox.clear();
                        regPasswordBox.clear();
                        ageBox.clear();
                        schoolBox.clear();
                        currentScreen = LOGIN;
                        messageText.setString("");
                    }
                }
                else if (currentScreen == FORGOT_PASSWORD) {
                    if (forgotSubmitButton.contains(mousePos)) {
                        if (forgotUsernameBox.getInput().empty() || forgotAgeBox.getInput().empty() ||
                            forgotSchoolBox.getInput().empty() || newPasswordBox.getInput().empty()) {
                            messageText.setString("All fields are required");
                            sf::FloatRect textBounds = messageText.getLocalBounds();

                            float centerX = (1600 - textBounds.width) / 2;
                            float centerY = (1100 - textBounds.height) / 2;
                            messageText.setPosition(centerX, centerY);
                            messageText.setFillColor(sf::Color::Red);
                            showMessage = true;
                            messageClock.restart();
                        }
                        else {
                            SecurityInfo security = {forgotAgeBox.getInput(), forgotSchoolBox.getInput()};
                            if (user.verifySecurityInfo(forgotUsernameBox.getInput(), security)) {
                                if (resetPassword(forgotUsernameBox.getInput(), newPasswordBox.getInput())) {
                                    messageText.setString("Password reset successful!");
                                    messageText.setFillColor(sf::Color::Green);
                                    showMessage = true;
                                    messageClock.restart();
                                    currentScreen = LOGIN;
                                }
                            } else {
                                messageText.setString("Security information incorrect");
                                messageText.setFillColor(sf::Color::Red);
                                showMessage = true;
                                messageClock.restart();
                            }
                        }
                    }
                    else if (backToLoginButton.contains(mousePos)) {
                        currentScreen = LOGIN;
                        messageText.setString("");
                    }
                }
            }

            // Handle text input
            if (currentScreen == LOGIN) {
                usernameBox.handleInput(event);
                passwordBox.handleInput(event);
            }
            else if (currentScreen == REGISTER) {
                regUsernameBox.handleInput(event);
                regPasswordBox.handleInput(event);
                ageBox.handleInput(event);
                schoolBox.handleInput(event);
            }
            else if (currentScreen == FORGOT_PASSWORD) {
                forgotUsernameBox.handleInput(event);
                forgotAgeBox.handleInput(event);
                forgotSchoolBox.handleInput(event);
                newPasswordBox.handleInput(event);
            }
        }

        // Clear message after 3 seconds
        if (showMessage && messageClock.getElapsedTime().asSeconds() > 3.0f) {
            messageText.setString("");
            showMessage = false;
        }

        window.clear();
        window.draw(bgSprite); // Draw background first

        // Draw welcome title on all screens
        window.draw(welcomeTitle);

       if (currentScreen == LOGIN) {
    sf::Text title("Login", font, 30);
    title.setPosition(750, 120); // centered
    title.setFillColor(sf::Color::Black);
    window.draw(title);

    sf::Text usernameLabel("Username:", font, 20);
    usernameLabel.setPosition(675, 172);
    usernameLabel.setFillColor(sf::Color::Black);
    window.draw(usernameLabel);

    sf::Text passwordLabel("Password:", font, 20);
    passwordLabel.setPosition(675, 235);
    passwordLabel.setFillColor(sf::Color::Black);
    window.draw(passwordLabel);

    usernameBox.draw(window);
    passwordBox.draw(window);
    loginButton.draw(window);
    toRegisterButton.draw(window);
    forgotButton.draw(window);
}
else if (currentScreen == REGISTER) {
    sf::Text title("Create Account", font, 30);
    title.setPosition(700, 120); // centered
    title.setFillColor(sf::Color::Black);
    window.draw(title);

    sf::Text usernameLabel("Username:", font, 20);
    usernameLabel.setPosition(675, 172);
    usernameLabel.setFillColor(sf::Color::Black);
    window.draw(usernameLabel);

    sf::Text passwordLabel("Password:", font, 20);
    passwordLabel.setPosition(675, 232);
    passwordLabel.setFillColor(sf::Color::Black);
    window.draw(passwordLabel);

    sf::Text ageLabel("Age:", font, 20);
    ageLabel.setPosition(675, 292);
    ageLabel.setFillColor(sf::Color::Black);
    window.draw(ageLabel);

    sf::Text schoolLabel("School Name:", font, 20);
    schoolLabel.setPosition(675, 352);
    schoolLabel.setFillColor(sf::Color::Black);
    window.draw(schoolLabel);

    regUsernameBox.draw(window);
    regPasswordBox.draw(window);
    ageBox.draw(window);
    schoolBox.draw(window);
    regSubmitButton.draw(window);
    toLoginButton.draw(window);
}
else if (currentScreen == FORGOT_PASSWORD) {
    sf::Text title("Reset Password", font, 30);
    title.setPosition(700, 120); // centered
    title.setFillColor(sf::Color::Black);
    window.draw(title);

    sf::Text usernameLabel("Username:", font, 20);
    usernameLabel.setPosition(675, 172);
    usernameLabel.setFillColor(sf::Color::Black);
    window.draw(usernameLabel);

    sf::Text ageLabel("Age:", font, 20);
    ageLabel.setPosition(675, 232);
    ageLabel.setFillColor(sf::Color::Black);
    window.draw(ageLabel);

    sf::Text schoolLabel("School Name:", font, 20);
    schoolLabel.setPosition(675, 292);
    schoolLabel.setFillColor(sf::Color::Black);
    window.draw(schoolLabel);

    sf::Text newPasswordLabel("New Password:", font, 20);
    newPasswordLabel.setPosition(675, 352);
    newPasswordLabel.setFillColor(sf::Color::Black);
    window.draw(newPasswordLabel);

    forgotUsernameBox.draw(window);
    forgotAgeBox.draw(window);
    forgotSchoolBox.draw(window);
    newPasswordBox.draw(window);
    forgotSubmitButton.draw(window);
    backToLoginButton.draw(window);
}


        window.draw(messageText);
        window.display();
    }
    return false;
}

string showMainMenu(sf::RenderWindow& window, const User& user) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return "exit";
    }

    // Load background image for main menu
    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("background.jpg")) {
        std::cout << "Error loading background image" << std::endl;
    }
    sf::Sprite bgSprite(bgTexture);
    bgSprite.setScale(
        window.getSize().x / (float)bgTexture.getSize().x,
        window.getSize().y / (float)bgTexture.getSize().y
    );

    Button playButton(sf::Vector2f(200, 40), sf::Vector2f(700, 200), "Start Type", font);
    Button scoresButton(sf::Vector2f(200, 40), sf::Vector2f(700, 260), "View Scores", font);
    Button bestButton(sf::Vector2f(200, 40), sf::Vector2f(700, 320), "Best Speed", font);
    Button rankingsButton(sf::Vector2f(200, 40), sf::Vector2f(700, 380), "Rankings", font);
    Button compareButton(sf::Vector2f(200, 40), sf::Vector2f(700, 440), "Compare", font);
    Button exitButton(sf::Vector2f(200, 40), sf::Vector2f(700, 500), "Exit", font);

    sf::Text title("Main Menu", font, 40);  
    title.setPosition(700, 100);  // Centered approximately for "Main Menu"
    title.setFillColor(sf::Color(72, 61, 139));

    sf::Text welcomeText("Welcome, " + user.getUsername(), font, 25);
    welcomeText.setPosition(693, 150);  // Adjusted for centered alignment
    welcomeText.setFillColor(sf::Color::Black);


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return "exit";
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(
                    sf::Mouse::getPosition(window));

                if (playButton.contains(mousePos)) return "play";
                if (scoresButton.contains(mousePos)) return "scores";
                if (bestButton.contains(mousePos)) return "best";
                if (rankingsButton.contains(mousePos)) return "rankings";
                if (compareButton.contains(mousePos)) return "compare";
                if (exitButton.contains(mousePos)) return "exit";
            }
        }

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        playButton.setHovered(playButton.contains(mousePos));
        scoresButton.setHovered(scoresButton.contains(mousePos));
        bestButton.setHovered(bestButton.contains(mousePos));
        rankingsButton.setHovered(rankingsButton.contains(mousePos));
        compareButton.setHovered(compareButton.contains(mousePos));
        exitButton.setHovered(exitButton.contains(mousePos));

        window.clear();
        window.draw(bgSprite);
        window.draw(title);
        window.draw(welcomeText);
        playButton.draw(window);
        scoresButton.draw(window);
        bestButton.draw(window);
        rankingsButton.draw(window);
        compareButton.draw(window);
        exitButton.draw(window);
        
        window.display();
    }
    return "exit";
}

string showDifficultyScreen(sf::RenderWindow& window) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return "exit";
    }

    // Load background image for difficulty screen
    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("background.jpg")) {
        std::cout << "Error loading background image" << std::endl;
    }
    sf::Sprite bgSprite(bgTexture);
    bgSprite.setScale(
        window.getSize().x / (float)bgTexture.getSize().x,
        window.getSize().y / (float)bgTexture.getSize().y
    );

    Button easyButton(sf::Vector2f(200, 40), sf::Vector2f(700, 200), "Beginner", font);
    Button mediumButton(sf::Vector2f(200, 40), sf::Vector2f(700, 260), "Intermediate", font);
    Button hardButton(sf::Vector2f(200, 40), sf::Vector2f(700, 320), "Advance", font);
    Button backButton(sf::Vector2f(200, 40), sf::Vector2f(700, 380), "Back", font);

    sf::Text title("Select Difficulty", font, 30);
    title.setPosition(700, 100);  
    title.setFillColor(sf::Color(72, 61, 139));
     

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return "exit";
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(
                    sf::Mouse::getPosition(window));

                if (easyButton.contains(mousePos)) return "easy";
                if (mediumButton.contains(mousePos)) return "medium";
                if (hardButton.contains(mousePos)) return "hard";
                if (backButton.contains(mousePos)) return "back";
            }
        }

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        easyButton.setHovered(easyButton.contains(mousePos));
        mediumButton.setHovered(mediumButton.contains(mousePos));
        hardButton.setHovered(hardButton.contains(mousePos));
        backButton.setHovered(backButton.contains(mousePos));

        window.clear();
        window.draw(bgSprite);  // Draw background first
        window.draw(title);
        easyButton.draw(window);
        mediumButton.draw(window);
        hardButton.draw(window);
        backButton.draw(window);
        
        window.display();
    }
    return "exit";
}

void showScores(sf::RenderWindow& window, const Leaderboard& leaderboard) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return;
    }

  Button backButton(sf::Vector2f(200, 40), sf::Vector2f(700, 500), "Back", font);

sf::Text title("All Scores", font, 30);
title.setPosition(730, 50);  // Approximate centering
title.setFillColor(sf::Color::Black);

vector<sf::Text> scoreTexts;
auto scores = leaderboard.getAllScores();
size_t total = scores.size();
size_t start = (total > 10) ? total - 10 : 0;
for (size_t i = start; i < total; ++i) {
    const auto& score = scores[i];
    string scoreStr = score.username + " - " + 
                     to_string((int)score.wpm) + " WPM, " +
                     to_string((int)score.accuracy) + "% (" +
                     score.difficulty + ")";
    
    sf::Text text(scoreStr, font, 16);
    text.setPosition(550, 100 + (i - start) * 30);  // Approximate center for score lines
    text.setFillColor(sf::Color::Black);
    scoreTexts.push_back(text);
}


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(
                    sf::Mouse::getPosition(window));

                if (backButton.contains(mousePos)) {
                    return;
                }
            }
        }

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        backButton.setHovered(backButton.contains(mousePos));

        window.clear(sf::Color(240, 240, 240));
        
        window.draw(title);
        for (const auto& text : scoreTexts) {
            window.draw(text);
        }
        backButton.draw(window);
        
        window.display();
    }
}

void showBestSpeed(sf::RenderWindow& window, const Leaderboard& leaderboard) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return;
    }

  Button backButton(sf::Vector2f(200, 40), sf::Vector2f(700, 700), "Back", font);

sf::Text title("Best Speed", font, 30);
title.setPosition(730, 100);  // Centered at 1600 width
title.setFillColor(sf::Color::Black);

sf::Text speedText(to_string((int)leaderboard.getBestSpeed()) + " WPM", font, 40);
speedText.setPosition(730, 200);  // Centered at 1600 width
speedText.setFillColor(sf::Color::Black);


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(
                    sf::Mouse::getPosition(window));

                if (backButton.contains(mousePos)) {
                    return;
                }
            }
        }

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        backButton.setHovered(backButton.contains(mousePos));

        window.clear(sf::Color(240, 240, 240));
        
        window.draw(title);
        window.draw(speedText);
        backButton.draw(window);
        
        window.display();
    }
}

void showRankings(sf::RenderWindow& window, const Leaderboard& leaderboard) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return;
    }

  Button backButton(sf::Vector2f(200, 40), sf::Vector2f(700, 700), "Back", font);

sf::Text title("Top 5 Players", font, 30);
title.setPosition(680, 50);  
title.setFillColor(sf::Color::Black);

vector<sf::Text> rankTexts;
auto top5 = leaderboard.getTop5();
for (size_t i = 0; i < top5.size(); ++i) {
    const auto& score = top5[i];
    string rankStr = to_string(i + 1) + ". " + score.username + 
                    " - " + to_string((int)score.wpm) + " WPM";
    
    sf::Text text(rankStr, font, 20);
    text.setPosition(650, 120 + i * 40);  
    text.setFillColor(sf::Color::Black);
    rankTexts.push_back(text);
}


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(
                    sf::Mouse::getPosition(window));

                if (backButton.contains(mousePos)) {
                    return;
                }
            }
        }

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        backButton.setHovered(backButton.contains(mousePos));

        window.clear(sf::Color(240, 240, 240));
        
        window.draw(title);
        for (const auto& text : rankTexts) {
            window.draw(text);
        }
        backButton.draw(window);
        
        window.display();
    }
}

void showComparison(sf::RenderWindow& window, const Leaderboard& leaderboard, 
                   const User& user) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return;
    }

Button backButton(sf::Vector2f(200, 40), sf::Vector2f(700, 700), "Back", font);

sf::Text title("Your Performance", font, 30);
title.setPosition(650, 50);  
title.setFillColor(sf::Color::Black);

float avgSpeed = leaderboard.getUserAverageSpeed(user.getUsername());
float bestSpeed = leaderboard.getUserBestSpeed(user.getUsername());
float globalBest = leaderboard.getBestSpeed();

sf::Text avgSpeedText("Average Speed: " + to_string((int)avgSpeed) + " WPM", 
                     font, 20);
avgSpeedText.setPosition(600, 150);  
avgSpeedText.setFillColor(sf::Color::Black);

sf::Text bestSpeedText("Your Best: " + to_string((int)bestSpeed) + " WPM", 
                      font, 20);
bestSpeedText.setPosition(600, 200);  
bestSpeedText.setFillColor(sf::Color::Black);

sf::Text globalBestText("Global Best: " + to_string((int)globalBest) + " WPM", 
                       font, 20);
globalBestText.setPosition(600, 250); 
globalBestText.setFillColor(sf::Color::Black);


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(
                    sf::Mouse::getPosition(window));

                if (backButton.contains(mousePos)) {
                    return;
                }
            }
        }

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        backButton.setHovered(backButton.contains(mousePos));

        window.clear(sf::Color(240, 240, 240));
        
        window.draw(title);
        window.draw(avgSpeedText);
        window.draw(bestSpeedText);
        window.draw(globalBestText);
        backButton.draw(window);
        
        window.display();
    }
}

// Add after the Game class declaration and before main()
Game* currentGame = nullptr;  // Global pointer to track current game instance

int main() {
    try {
        // Check if required files exist
        if (!FileHandler::checkRequiredFiles()) {
            cerr << "Error: Required paragraph files are missing. Please ensure all paragraph files exist." << endl;
            return 1;
        }

        // Create window with resize style
        sf::RenderWindow window(sf::VideoMode(1600, 800), "Speed Type", sf::Style::Default);
        window.setFramerateLimit(60);
        
        sf::Image icon;
        if (icon.loadFromFile("icon.png")) {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
        } else {
        std::cerr << "Failed to load icon.png" << std::endl;
        }

        // Create user and leaderboard
        User user;
        Leaderboard leaderboard;

        // Show welcome screen
        if (!showWelcomeScreen(window, user)) {
            return 0;
        }

        // Main game loop
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                else if (event.type == sf::Event::Resized) {
                    // Handle window resize and enforce minimum size
                    sf::Vector2u newSize(event.size.width, event.size.height);
                    if (newSize.x < 800) newSize.x = 800;
                    if (newSize.y < 600) newSize.y = 600;
                    window.setSize(newSize);
                    if (currentGame) {
                        currentGame->handleResize(newSize);
                    }
                }
                else if (event.type == sf::Event::LostFocus) {
                    // Handle window minimize
                    if (currentGame) {
                        currentGame->setMinimized(true);
                    }
                }
                else if (event.type == sf::Event::GainedFocus) {
                    // Handle window restore
                    if (currentGame) {
                        currentGame->setMinimized(false);
                    }
                }

                string menuChoice = showMainMenu(window, user);
                
                if (menuChoice == "exit") {
                    window.close();
                    break;
                    exit(1);
                }
                else if (menuChoice == "play") {
                    string difficulty = showDifficultyScreen(window);
                    if (difficulty == "back") {
                        continue;
                    }
                    else if (difficulty != "exit") {
                        try {
                            Game game(user, difficulty);
                            currentGame = &game;  // Set current game pointer
                            while (window.isOpen()) {
                                sf::Event event;
                                while (window.pollEvent(event)) {
                                    if (event.type == sf::Event::Closed) {
                                        window.close();
                                        return 0;
                                    }
                                    game.processInput(event);
                                }
                                
                                game.render(window);
                                
                                if (game.isFinished()) {
                                    break;
                                }
                            }
                            currentGame = nullptr;  // Clear current game pointer when done
                            // Refresh leaderboard after game ends
                            leaderboard.loadRecords();
                        } catch (const exception& e) {
                            cerr << "Error during game: " << e.what() << endl;
                            currentGame = nullptr;  // Clear current game pointer on error
                            continue;
                        }
                    }
                }
                else if (menuChoice == "scores") {
                    leaderboard.loadRecords(); // Always refresh before showing
                    showScores(window, leaderboard);
                }
                else if (menuChoice == "best") {
                    showBestSpeed(window, leaderboard);
                }
                else if (menuChoice == "rankings") {
                    showRankings(window, leaderboard);
                }
                else if (menuChoice == "compare") {
                    showComparison(window, leaderboard, user);
                }
            }
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}