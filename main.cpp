// ATTENTION: This program will NOT run... I have not included the necessary image files
// as this is mostly for viewing. Also, I put the code all in one main.cpp file for ease of access

#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <map>
#include <sstream>
using namespace std;

int main() {

    //read height, width, and num of mines
    ifstream configIn("files/config.cfg");
    string colS;
    string rowS;
    string mine_countS;
    getline(configIn, colS);
    int col = stoi(colS);
    getline(configIn, rowS);
    int row = stoi(rowS);
    getline(configIn, mine_countS);
    int mine_count = stoi(mine_countS);

    int width = col * 32;
    int height = (row * 32) + 100;

    sf::RenderWindow welcomeWindow(sf::VideoMode(width, height), "Welcome Window");
    sf::Font font;
    font.loadFromFile("files/font.ttf");

    sf::Text welcomeText;
    welcomeText.setFont(font);
    welcomeText.setString("Enter your name: ");
    welcomeText.setCharacterSize(30);
    welcomeText.setFillColor(sf::Color::White);
    welcomeText.setPosition(width/2, height/2);

    sf::FloatRect welcomeTextRect = welcomeText.getLocalBounds();

    welcomeText.setOrigin(welcomeTextRect.left + welcomeTextRect.width/2.0f, welcomeTextRect.top + welcomeTextRect.height/2.0f);
    welcomeText.setPosition(width/2.0f, height/2.0f - 75);

    // create user input text and string
    sf::Text userInput;
    userInput.setFont(font);
    userInput.setCharacterSize(30);
    userInput.setFillColor(sf::Color::White);
    userInput.setPosition(width/2.0f - 57, height/2.0f - 45);

    //Welcome Text
    sf::Text WelcomeToMinesweeper;
    WelcomeToMinesweeper.setFont(font);
    WelcomeToMinesweeper.setString("WELCOME TO MINESWEEPER!");
    WelcomeToMinesweeper.setCharacterSize(32);
    WelcomeToMinesweeper.setFillColor(sf::Color::White);
    WelcomeToMinesweeper.setStyle(sf::Text::Underlined);
    WelcomeToMinesweeper.setPosition(width/2.0f - 230, height/2.0f - 150);
    string name = "";
    const int MAX_CHARS = 10;


    //tile struct
    struct Tile{
        map<int, sf::Texture> nums;
        sf::Texture tile_hiddenTexture;
        sf::Texture tile_revealedTexture;
        sf::Texture flagTexture;
        sf::Texture mineTexture;
        sf::Sprite digitSprite;
        sf::Sprite tileSprite;
        sf::Sprite mineSprite;
        sf::Sprite flagSprite;
        bool is_clicked;
        bool is_a_mine;
        bool is_right_clicked;
        bool is_flag;
        int surrounding_mines;

        Tile(map<int, sf::Texture> nums_){
            surrounding_mines = 0;
            nums = nums_;
            is_a_mine = false;
            is_clicked = false;
            is_right_clicked = false;
            is_flag = false;
            tile_hiddenTexture.loadFromFile("files/images/tile_hidden.png");
            tile_revealedTexture.loadFromFile("files/images/tile_revealed.png");
            tileSprite.setTexture(tile_hiddenTexture);
            flagTexture.loadFromFile("files/images/flag.png");
            mineTexture.loadFromFile("files/images/mine.png");
            mineSprite.setTexture(mineTexture);
            flagSprite.setTexture(flagTexture);

        }

        void Display(int x, int y, sf::RenderWindow &Window){
            //default Display (just the hidden tile)
            tileSprite.setPosition(x, y);
            flagSprite.setPosition(x, y);
            Window.draw(tileSprite);

            //right click Display
            if (!is_flag) {
                // Draw tileSprite if not flagged
                Window.draw(tileSprite);
            } else {
                // Draw flagSprite if flagged
                Window.draw(flagSprite);
            }

            //left click Display
            if (is_a_mine && is_clicked && !is_flag){
                mineSprite.setPosition(x, y);
                Window.draw(mineSprite);
            }
            else if (is_clicked && !is_flag){
                    tileSprite.setTexture(tile_revealedTexture);
                    digitSprite.setPosition(x, y);
                    digitSprite.setTexture(nums[surrounding_mines]);
                    Window.draw(tileSprite);
                    Window.draw(digitSprite);
            }
        }

        void clicked(){
            if(!is_flag) {
                is_clicked = true;
            }
        }

        void right_clicked(){
            if (!is_clicked){
                is_flag = !is_flag;
            }
        }
    };

    //Board struct
    struct Board{
        vector<vector<Tile*>> boardVect;
        int row;
        int col;
        int num_mines;
        int num_flags;
        sf::Texture tile_hiddenTexture_;
        bool debug_mode_pressed;
        bool gameover;
        bool gameWin;
        map<int, sf::Texture> nums;

        //randomly places mines throughout boardVect
        void randomlyPlaceMines(std::vector<std::vector<Tile*>>& matrix, int count) {
            int numRows = matrix.size();
            if (numRows == 0) return;
            int numCols = matrix[0].size();
            if (numCols == 0) return;

            // Seed the random number generator
            srand(static_cast<unsigned int>(time(nullptr)));

            int totalTiles = numRows * numCols;
            // Ensure count does not exceed the total number of tiles
            count = std::min(count, totalTiles);

            // Initialize a vector to keep track of which tiles have been chosen
            std::vector<bool> chosen(totalTiles, false);

            // Randomly select count tiles to set as mines
            for (int i = 0; i < count; ++i) {
                int index;
                // Find an index that hasn't been chosen yet
                do {
                    index = rand() % totalTiles;
                } while (chosen[index]);
                chosen[index] = true;

                // Calculate row and column from index
                int row = index / numCols;
                int col = index % numCols;

                matrix[row][col]->is_a_mine = true;
            }
        }

        //intializes board
        Board(int row_, int col_, int num_mines_, map<int, sf::Texture> nums_){
            num_flags = 0;
            debug_mode_pressed = false;
            gameover = false;
            gameWin = false;
            nums = nums_;
            num_mines = num_mines_;
            tile_hiddenTexture_.loadFromFile("files/images/tile_hidden.png");
            row = row_;
            col = col_;
            boardVect.resize(row);
            for (int i = 0; i < row; i++){
                for (int j = 0; j < col; j++){
                    Tile *tile = new Tile(nums);
                    boardVect[i].push_back(tile);
                }
            }
            randomlyPlaceMines(boardVect, num_mines);
        }

        void drawRevealed(sf::RenderWindow &Window){
            int x = 0;
            int y = 0;
            for (int i = 0; i < row; i++) {
                for (int j = 0; j < col; j++) {
                    int temp = boardVect[row][col] -> surrounding_mines;
                    bool tempB = boardVect[row][col] -> is_clicked;
                    bool tempM = boardVect[row][col] -> is_a_mine;
                    boardVect[row][col] -> surrounding_mines = 0;
                    boardVect[row][col] -> is_clicked = true;
                    boardVect[row][col] -> is_a_mine = false;
                    boardVect[row][col] -> Display(x, y, Window);
                    x += boardVect[0][0]->tileSprite.getGlobalBounds().width;
                    boardVect[row][col] -> surrounding_mines = temp;
                    boardVect[row][col] -> is_clicked = tempB;
                    boardVect[row][col] -> is_a_mine = tempM;
                }
                x = 0;
                y += boardVect[0][0]->tileSprite.getGlobalBounds().height;
            }
        }

        void reset(int row_, int col_, int num_mines_, map<int, sf::Texture> nums_){
            boardVect = {};
            num_flags = 0;
            debug_mode_pressed = false;
            gameover = false;
            gameWin = false;
            nums = nums_;
            num_mines = num_mines_;
            tile_hiddenTexture_.loadFromFile("files/images/tile_hidden.png");
            row = row_;
            col = col_;
            boardVect.resize(row);
            for (int i = 0; i < row; i++){
                for (int j = 0; j < col; j++){
                    Tile *tile = new Tile(nums);
                    boardVect[i].push_back(tile);
                }
            }
            randomlyPlaceMines(boardVect, num_mines);
        }

        int getSurroundingValues(int row, int col) {
            int count = 0;
            vector<Tile*> surroundingValues;
            int numRows = boardVect.size();
            if (numRows == 0) return 0;
            int numCols = boardVect[0].size();
            if (numCols == 0) return 0;

            // Check all surrounding indices
            for (int i = row - 1; i <= row + 1; ++i) {
                for (int j = col - 1; j <= col + 1; ++j) {
                    // Exclude the index itself
                    if (i == row && j == col) continue;
                    // Check if the index is within bounds
                    if (i >= 0 && i < numRows && j >= 0 && j < numCols) {
                        surroundingValues.push_back(boardVect[i][j]);
                    }
                }
            }
            for(int i = 0; i < surroundingValues.size(); i++){
                if (surroundingValues[i]->is_a_mine){
                    count++;
                }
            }
            return count;
        }

        vector<Tile*> getSurroundingTiles(int row, int col) {
            int count = 0;
            vector<Tile*> surroundingValues;
            int numRows = boardVect.size();
            int numCols = boardVect[0].size();

            // Check all surrounding indices
            for (int i = row - 1; i <= row + 1; ++i) {
                for (int j = col - 1; j <= col + 1; ++j) {
                    // Exclude the index itself
                    if (i == row && j == col) continue;
                    // Check if the index is within bounds
                    if (i >= 0 && i < numRows && j >= 0 && j < numCols) {
                        surroundingValues.push_back(boardVect[i][j]);
                    }
                }
            }
            return surroundingValues;
        }

        //Displays board
        void displayBoard(sf::RenderWindow &Window){
            int x = 0;
            int y = 0;

            //Display board normally if debug mode isn't pressed
            //else display all the mines
            if (!debug_mode_pressed) {
                for (int i = 0; i < row; i++) {
                    for (int j = 0; j < col; j++) {
                        boardVect[i][j]->Display(x, y, Window);
                        x += boardVect[0][0]->tileSprite.getGlobalBounds().width;
                    }
                    x = 0;
                    y += boardVect[0][0]->tileSprite.getGlobalBounds().height;
                }
            }
            else {
                    for (int i = 0; i < row; i++){
                        for (int j = 0; j < col; j++){
                            if (boardVect[i][j] -> is_a_mine){
                                boardVect[i][j] -> is_clicked = true;
                                boardVect[i][j] -> Display(x, y, Window);
                                boardVect[i][j] -> is_clicked = false;
                                x += boardVect[0][0] -> tileSprite.getGlobalBounds().width;
                            }
                            else{
                                boardVect[i][j] -> Display(x, y, Window);
                                x += boardVect[0][0] -> tileSprite.getGlobalBounds().width;
                            }
                        }
                        x = 0;
                        y += boardVect[0][0] -> tileSprite.getGlobalBounds().height;
                    }
            }
        }
        void revealAdjacentTiles(int rowT, int colT) {
            int numRows = boardVect.size();
            int numCols = boardVect[0].size();

            // Check if tile is within bounds
            if (rowT < 0 || rowT >= numRows || colT < 0 || colT >= numCols)
                return;

            // Check if tile has already been clicked or is a mine
            if (boardVect[rowT][colT]->is_clicked || boardVect[rowT][colT]->is_a_mine)
                return;

            // Calculate the number of surrounding mines for the current tile
            boardVect[rowT][colT]->surrounding_mines = getSurroundingValues(rowT, colT);

            // Mark the current tile as clicked
            boardVect[rowT][colT]->clicked();

            // If the current tile has no surrounding mines, recursively reveal adjacent tiles
            if (boardVect[rowT][colT]->surrounding_mines == 0) {
                for (int i = -1; i <= 1; ++i) {
                    for (int j = -1; j <= 1; ++j) {
                        int newRow = rowT + i;
                        int newCol = colT + j;
                        // Skip the current tile itself
                        if (i == 0 && j == 0)
                            continue;
                        // Recursively reveal adjacent tiles
                        revealAdjacentTiles(newRow, newCol);
                    }
                }
            }
        }

        void gamewin(){
            for (int i = 0; i < row; i++) {
                for (int j = 0; j < col; j++) {
                    if (boardVect[i][j] -> is_a_mine or boardVect[i][j] -> is_clicked){
                        continue;
                    }
                    else{
                        return;
                    }
                }
            }
            gameWin = true;
        }


        //checks what tile object was clicked and updates it accordingly
        void tileClicked(float mouse_x, float mouse_y, sf::RenderWindow &Window, sf::Mouse mouse, map<int, sf::Texture> digits, sf::Texture &SmileyTexture, sf::Sprite &SmileySprite){
            if (mouse.isButtonPressed(mouse.Right)){
                for (int i = 0; i < row; i++){
                    for (int j = 0; j < col; j++){
                        if (boardVect[i][j] -> tileSprite.getGlobalBounds().contains(mouse_x, mouse_y)){
                            boardVect[i][j] -> right_clicked();
                            if (boardVect[i][j] -> is_flag){
                                num_flags++;
                            }
                            else{
                                num_flags--;
                            }
                        }
                    }
                }
            }
            else {
                for (int i = 0; i < row; i++) {
                    for (int j = 0; j < col; j++) {
                        if (boardVect[i][j]->tileSprite.getGlobalBounds().contains(mouse_x, mouse_y)) {
                            // Calculate the surrounding mines before revealing adjacent tiles
                            boardVect[i][j]->surrounding_mines = getSurroundingValues(i, j);

                            // Check if the clicked tile has no surrounding mines and hasn't been clicked yet
                            if (boardVect[i][j]->surrounding_mines == 0 && !boardVect[i][j]->is_clicked && !boardVect[i][j] -> is_a_mine) {
                                // If no surrounding mines, reveal all surrounding tiles recursively
                                revealAdjacentTiles(i, j);
                            } else if(!boardVect[i][j] -> is_clicked && !boardVect[i][j]->is_a_mine){
                                // If the clicked tile has surrounding mines or has already been clicked,
                                // simply reveal the clicked tile and update its texture accordingly
                                boardVect[i][j]->clicked();
                                boardVect[i][j]->digitSprite.setTexture(digits[boardVect[i][j]->surrounding_mines]);
                                // Update other tile textures as needed
                            }
                            else if(boardVect[i][j] -> is_a_mine and !boardVect[i][j] -> is_flag){
                                gameover = true;
                            }
                        }
                    }
                }
                gamewin();
            }
        }

        //destructor for boardVect
        ~Board(){
            for (int i = 0; i < row; i++){
                for (auto ptr : boardVect[i]){
                    delete ptr;
                }
            }
        }


    };

    struct user{
        int place;
        string time;
        string name;

        user(int place_, string time_, string name_){
            place = place_;
            time = time_;
            name = name_;
        }
    };

    struct leaderboard{
        vector<user> users;
        string csvLine;
        string token;
        int place;
        string time;
        string name;


        leaderboard(ifstream &leaderboardIn){
            place = 1;
            while(getline(leaderboardIn, csvLine)){
                stringstream line(csvLine);
                getline(line, time, ',');
                getline(line, name, ',');
                user temp(place, time, name);
                place++;
                users.push_back(temp);
            }
        }

        void Display(sf::RenderWindow &Window){
            float x = 400;
            float y = 612/2 + 20;
            sf::Font font;
            font.loadFromFile("files/font.ttf");

            sf::Text LeaderboardText;
            LeaderboardText.setFont(font);
            LeaderboardText.setString("LEADERBOARD");
            LeaderboardText.setPosition(x - (LeaderboardText.getGlobalBounds().width/2), y - 140);
            LeaderboardText.setCharacterSize(20);
            LeaderboardText.setFillColor(sf::Color::White);
            LeaderboardText.setStyle(sf::Text::Underlined);
            Window.draw(LeaderboardText);

            for(int i = 0; i < 5; i++){
                sf::Text user;
                user.setFont(font);
                user.setCharacterSize(18);
                user.setFillColor(sf::Color::White);
                user.setString(to_string(i + 1) + ".\t" + users[i].time + "\t" + users[i].name);
                user.setPosition(x - 130, y);
                Window.draw(user);
                y += 20;
            }
        }
    };

    //TEXTURES

    //faces
    sf::Texture face_happyTexture;
    face_happyTexture.loadFromFile("files/images/face_happy.png");
    sf::Sprite faceSprite(face_happyTexture);
    faceSprite.setPosition((col/2) * 32 - 32, 32 * (row + 0.5));
    sf::Texture face_loseTexture;
    face_loseTexture.loadFromFile("files/images/face_lose.png");
    sf::Texture face_winTexture;
    face_winTexture.loadFromFile("files/images/face_win.png");

    //debug sprite/texture
    sf::Texture debugTexture;
    debugTexture.loadFromFile("files/images/debug.png");
    sf::Sprite debugSprite(debugTexture);
    debugSprite.setPosition(col*32 - 304, 32 * (row + 0.5));

    //pause and play
    sf::Texture pauseTexture;
    pauseTexture.loadFromFile("files/images/pause.png");
    sf::Sprite pauseSprite(pauseTexture);
    pauseSprite.setPosition(col*32 - 240, 32 * (row + 0.5));
    sf::Texture playTexture;
    playTexture.loadFromFile("files/images/play.png");

    //leaderboard text and sprite
    sf::Texture leaderboardTexture;
    leaderboardTexture.loadFromFile("files/images/leaderboard.png");
    sf::Sprite leaderboardSprite(leaderboardTexture);
    leaderboardSprite.setPosition(col*32 - 176, 32 * (row + 0.5));

    //digits
    map<int, sf::Texture> digits;
    sf::Texture ZeroTexture;
    ZeroTexture.loadFromFile("files/images/digits.png", sf::IntRect(0, 0, 21, 32));
    digits[0] = ZeroTexture;
    sf::Texture OneTexture;
    OneTexture.loadFromFile("files/images/digits.png", sf::IntRect(21, 0, 21, 32));
    digits[1] = OneTexture;
    sf::Texture TwoTexture;
    TwoTexture.loadFromFile("files/images/digits.png", sf::IntRect(42, 0, 21, 32));
    digits[2] = TwoTexture;
    sf::Texture ThreeTexture;
    ThreeTexture.loadFromFile("files/images/digits.png", sf::IntRect(63, 0, 21, 32));
    digits[3] = ThreeTexture;
    sf::Texture FourTexture;
    FourTexture.loadFromFile("files/images/digits.png", sf::IntRect(84, 0, 21, 32));
    digits[4] = FourTexture;
    sf::Texture FiveTexture;
    FiveTexture.loadFromFile("files/images/digits.png", sf::IntRect(105, 0, 21, 32));
    digits[5] = FiveTexture;
    sf::Texture SixTexture;
    SixTexture.loadFromFile("files/images/digits.png", sf::IntRect(126, 0, 21, 32));
    digits[6] = SixTexture;
    sf::Texture SevenTexture;
    SevenTexture.loadFromFile("files/images/digits.png", sf::IntRect(147, 0, 21, 32));
    digits[7] = SevenTexture;
    sf::Texture EightTexture;
    EightTexture.loadFromFile("files/images/digits.png", sf::IntRect(168, 0, 21, 32));
    digits[8] = EightTexture;
    sf::Texture NineTexture;
    NineTexture.loadFromFile("files/images/digits.png", sf::IntRect(189, 0, 21, 32));
    digits[9] = NineTexture;
    sf::Texture NegativeTexture;
    NegativeTexture.loadFromFile("files/images/digits.png", sf::IntRect(210, 0, 21, 32));

    //num Textures
    map<int, sf::Texture> nums;
    sf::Texture num0Texture;
    num0Texture.loadFromFile("files/images/tile_revealed.png");
    nums[0] = num0Texture;
    sf::Texture num1Texture;
    num1Texture.loadFromFile("files/images/number_1.png");
    nums[1] = num1Texture;
    sf::Texture num2Texture;
    num2Texture.loadFromFile("files/images/number_2.png");
    nums[2] = num2Texture;
    sf::Texture num3Texture;
    num3Texture.loadFromFile("files/images/number_3.png");
    nums[3] = num3Texture;
    sf::Texture num4Texture;
    num4Texture.loadFromFile("files/images/number_4.png");
    nums[4] = num4Texture;
    sf::Texture num5Texture;
    num5Texture.loadFromFile("files/images/number_5.png");
    nums[5] = num5Texture;
    sf::Texture num6Texture;
    num6Texture.loadFromFile("files/images/number_6.png");
    nums[6] = num6Texture;
    sf::Texture num7Texture;
    num7Texture.loadFromFile("files/images/number_7.png");
    nums[7] = num7Texture;
    sf::Texture num8Texture;
    num8Texture.loadFromFile("files/images/number_8.png");
    nums[8] = num8Texture;

    //counter sprites
    sf::Sprite counter0Sprite(digits[0]);
    counter0Sprite.setPosition(33, 32*(row + 0.5) + 16);
    sf::Sprite counter1Sprite(digits[5]);
    counter1Sprite.setPosition(54, 32* (row + 0.5) + 16);
    sf::Sprite counter2Sprite(digits[2]);
    counter2Sprite.setPosition(75, 32* (row + 0.5) + 16);

    //timer sprites
    sf::Sprite timer0Sprite(ZeroTexture);
    timer0Sprite.setPosition((col*32) - 97, 32 * (row+0.5) + 16);
    sf::Sprite timer1Sprite(ZeroTexture);
    timer1Sprite.setPosition((col*32) - 97 + 21, 32 * (row+0.5) + 16);
    sf::Sprite timer2Sprite(ZeroTexture);
    timer2Sprite.setPosition((col*32) - 54, 32 * (row+0.5) + 16);
    sf::Sprite timer3Sprite(ZeroTexture);
    timer3Sprite.setPosition((col*32) - 54 + 21, 32 * (row+0.5) + 16);


    //Code for the GUI begins here
    ifstream leaderboardIn("files/leaderboard.txt");
    leaderboard test(leaderboardIn);
    while(welcomeWindow.isOpen()){
        sf::Event event;
        while(welcomeWindow.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                welcomeWindow.close();
            }

            if(event.type == sf::Event::TextEntered){
                if(event.text.unicode >= 65 && event.text.unicode <= 90 ||
                   event.text.unicode >= 97 && event.text.unicode <= 122){
                    if(name.size() < MAX_CHARS){
                        if(name.size() == 0){
                            char temp = (char)(event.text.unicode);
                            temp = toupper(temp);
                            name += static_cast<char>(temp);
                            userInput.setString(name + '|');

                        }
                        else{
                            char temp = (char)(event.text.unicode);
                            temp = tolower(temp);
                            name += static_cast<char>(temp);
                            userInput.setString(name + '|');
                        }


                    }
                }
            }

            if(event.type == sf::Event::KeyPressed){
                if(event.key.code == sf::Keyboard::Enter && !name.empty()){
                    
                    //Game Window
                    welcomeWindow.close();
                    sf::RenderWindow gameWindow(sf::VideoMode(width, height), "Minesweeper");
                    Board board(row, col, mine_count, nums);
                    Tile tile(nums);
                    sf::Clock clock;
                    float totalElapsedTime = 0.0f; // Total elapsed time since the game started
                    int minutes = 0, seconds = 0;
                    bool is_paused = false;
                    bool processEvent = true;
                    int resetCount = 0;
                    bool leaderboardClicked = false;
                    user CurrentUser(0, to_string(totalElapsedTime), name);

                    while(gameWindow.isOpen()) {
                        sf::Event event1;
                        sf::Mouse mouse;
                        while (gameWindow.pollEvent(event1)) {
                            if (event1.type == sf::Event::Closed) {
                                gameWindow.close();
                            }
                            if (processEvent && event1.type == sf::Event::MouseButtonPressed) {
                                int mousex = mouse.getPosition(gameWindow).x;
                                int mousey = mouse.getPosition(gameWindow).y;
                                if(!is_paused) {
                                    board.tileClicked(mousex, mousey, gameWindow, mouse, nums, face_loseTexture,
                                                      faceSprite);

                                    //Counter texture = total - flags
                                    int counterDisplay = mine_count - board.num_flags;
                                    int difference = abs(mine_count - board.num_flags);
                                    if (counterDisplay < 0) {
                                        counter0Sprite.setTexture(NegativeTexture);
                                    }
                                    counter1Sprite.setTexture(digits[difference / 10]);
                                    counter2Sprite.setTexture(digits[difference % 10]);
                                    if (board.gameover) {
                                        faceSprite.setTexture(face_loseTexture);
                                        board.debug_mode_pressed = true;
                                        gameWindow.setActive(false);
                                        processEvent = false;
                                    }
                                    if (board.gameWin) {
                                        faceSprite.setTexture(face_winTexture);
                                        gameWindow.setActive(false);
                                        processEvent = false;
                                    }
                                }

                                    //debug mode button
                                    if (debugSprite.getGlobalBounds().contains(mousex, mousey)) {
                                        board.debug_mode_pressed = !board.debug_mode_pressed;
                                    }
                                    if (pauseSprite.getGlobalBounds().contains(mousex, mousey)) {
                                        is_paused = !is_paused;
                                    }
                                    if (leaderboardSprite.getGlobalBounds().contains(mousex, mousey) or board.gameWin) {
                                        leaderboardClicked = !leaderboardClicked;
                                        if (board.gameWin) {
                                            CurrentUser.name += '*';
                                            // Format minutes and seconds with leading zeros if needed
                                            string formattedMinutes = (minutes < 10) ? "0" + to_string(minutes)
                                                                                     : to_string(minutes);
                                            string formattedSeconds = (seconds < 10) ? "0" + to_string(seconds)
                                                                                     : to_string(seconds);

                                            CurrentUser.time = formattedMinutes + ":" + formattedSeconds;

                                            bool inserted = false;
                                            for (int i = 0; i < test.users.size(); i++) {
                                                if (CurrentUser.time < test.users[i].time) {
                                                    test.users.insert(test.users.begin() + i, CurrentUser);
                                                    CurrentUser.place = i + 1;
                                                    inserted = true;
                                                    break; // Exit the loop once CurrentUser is inserted
                                                }
                                            }
                                            // If CurrentUser wasn't inserted, it means it's the slowest so far
                                            if (!inserted) {
                                                test.users.push_back(CurrentUser);
                                                CurrentUser.place = test.users.size();
                                            }
                                        }
                                        sf::RenderWindow leaderboardWindow(sf::VideoMode(width, height), "Leaderboard");
                                        while (leaderboardWindow.isOpen()) {
                                            clock.restart();
                                            sf::Event event2;
                                            while (leaderboardWindow.pollEvent(event2)) {
                                                if (event2.type == sf::Event::Closed) {
                                                    leaderboardClicked = !leaderboardClicked;
                                                    leaderboardWindow.close();
                                                }
                                            }
                                            leaderboardWindow.clear(sf::Color::Blue);
                                            test.Display(leaderboardWindow);
                                            leaderboardWindow.display();
                                        }
                                    }
                                    if (faceSprite.getGlobalBounds().contains(mousex, mousey)) {
                                        board.reset(row, col, mine_count, nums);
                                        resetCount++;
                                    }

                            }
                        }

                    //PAUSE BUTTON
                    if (!is_paused && !board.gameover) {
                            totalElapsedTime += clock.restart().asSeconds();
                            pauseSprite.setTexture(pauseTexture);
                    }
                    else{
                        clock.restart();
                        pauseSprite.setTexture(playTexture);
                    }
                        //Update timer sprites
                        seconds = static_cast<int>(totalElapsedTime) % 60;
                        minutes = static_cast<int>(totalElapsedTime) / 60;
                        timer0Sprite.setTexture(digits[minutes / 10]);
                        timer1Sprite.setTexture(digits[minutes % 10]);
                        timer2Sprite.setTexture(digits[seconds / 10]);
                        timer3Sprite.setTexture(digits[seconds % 10]);


                        //draw everything
                        gameWindow.clear();
                        board.displayBoard(gameWindow);
                        gameWindow.draw(faceSprite);
                        gameWindow.draw(debugSprite);
                        gameWindow.draw(pauseSprite);
                        gameWindow.draw(leaderboardSprite);
                        gameWindow.draw(counter0Sprite);
                        gameWindow.draw(counter1Sprite);
                        gameWindow.draw(counter2Sprite);
                        gameWindow.draw(timer0Sprite);
                        gameWindow.draw(timer1Sprite);
                        gameWindow.draw(timer2Sprite);
                        gameWindow.draw(timer3Sprite);
                        gameWindow.display();

                    }

                } else if(event.key.code == sf::Keyboard::Backspace){
                    if (name.size() != 0){
                        name.pop_back();
                        userInput.setString(name + '|');
                    }
                }
            }
        }

        welcomeWindow.clear(sf::Color::Blue);
        welcomeWindow.draw(welcomeText);
        welcomeWindow.draw(userInput);
        welcomeWindow.draw(WelcomeToMinesweeper);
        welcomeWindow.display();
    }

    return 0;
}
