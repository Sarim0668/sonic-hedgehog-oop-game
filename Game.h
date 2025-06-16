#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include "Menu.h"
#include "Character.h"
#include "levels.h"
#include "Enemy.h"
#include "Collectibles.h"

using namespace sf;
using namespace std;

class Game {
private:
    RenderWindow window;
    Menu menu;
    levels* currentLevel;
    Player* player;
    float acceleration = 0.2;
    float raw_img_x = 24;
    float raw_img_y = 35;
    float scale_x = 2.5;
    float scale_y = 2.5;
    int Pheight = raw_img_y * scale_y;
    int Pwidth = raw_img_x * scale_x;
    int hit_box_factor_x = 8 * scale_x;
    int hit_box_factor_y = 5 * scale_y;
    int currentLevelNumber = 1;
    const int cell_size = 64;
    char** lvl;
    Text timerText;
    int totalTime = 120;
    Text hpText;
    bool z_pressed = false;
    Clock ability_timer, ball_timer, follow_char_timer, gameClock, clock;
    bool power_active = false;
    SoundBuffer menu_buffer;
    Sound menu_sound;
    Music bossLevelMus;
    Music level1Mus;
    Music level2Mus;
    Music level3Mus;
    bool music_started = false;
    float volumes[10] = { 50, 50, 50, 50, 50, 50, 50, 50, 50, 50 };

public:

    Game() : window(VideoMode(1200, 900), "Sonic the Hedgehog-OOP", Style::Close) {
        window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(60);

        currentLevel = new level1();
        currentLevel->set_level();
        player = &currentLevel->get_player();
        lvl = currentLevel->get_grid();
        currentLevelNumber = 1;

        menu.readScoresFromFile();
    }
    string intToString(int num)
    {
        if (num == 0)
        {
            return "0";
        }
        string result = "";
        bool negative = false;
        if (num < 0)
        {
            negative = true;
            num = -num;
        }
        while (num > 0)
        {
            result = char((num % 10) + '0') + result;
            num /= 10;
        }
        if (negative)
        {
            result = "-" + result;
        }
        return result;
    }

    int gameOverScreen(RenderWindow& window, const string& playerName, int score) {

        menu.addScore(playerName, score); // Add score to the leaderboard
        Texture gameOverTexture;
        gameOverTexture.loadFromFile("Data/GameOver2.jpeg");
        Sprite gameOverSprite;
        gameOverSprite.setTexture(gameOverTexture);
        gameOverSprite.setScale(1.9, 1.9);

        Font font;
        font.loadFromFile("Data/Montserrat-Regular.ttf");

        Text restartText;
        restartText.setFont(font);
        restartText.setString("Press R to return to Menu\nPress X to Exit");
        restartText.setCharacterSize(30);
        restartText.setFillColor(Color::Red);
        restartText.setPosition(435, 630);

        while (window.isOpen()) {
            Event ev;
            while (window.pollEvent(ev)) {
                if (ev.type == Event::Closed) { // it means the user clicked the x 
                    window.close();
                    return -1; // Exit the game
                }
                if (ev.type == Event::KeyPressed) {
                    if (ev.key.code == Keyboard::R) {
                        return 1; // Return to menu
                    }
                    if (ev.key.code == Keyboard::X) {
                        window.close();
                        return -1; // Exit the game
                    }
                }
            }

            window.clear();
            window.draw(gameOverSprite);
            window.draw(restartText);
            window.display();
        }

        return -1; // it will do default exit if window is closed
    }

    void showLevelTransition() {
        Texture levelUpTexture;
        if (!levelUpTexture.loadFromFile("Data/LevelUp.jpeg")) {
            cout << "Error: Failed to load level transition image: Data/levelup.jpeg" << endl;
            return;
        }

        Sprite levelUpSprite;
        levelUpSprite.setTexture(levelUpTexture);
        levelUpSprite.setScale((float)(window.getSize().x) / levelUpTexture.getSize().x, (float)(window.getSize().y) / levelUpTexture.getSize().y);

        while (clock.getElapsedTime().asSeconds() < 3.0) { // Show for 3 seconds
            window.clear();
            window.draw(levelUpSprite);
            window.display();
        }
    }

    void showYouWin() {
        Texture YouWinTexture;
        if (!YouWinTexture.loadFromFile("Data/YouWin.jpeg")) {
            cout << "Error: Failed to load YouWin Image: Data/YouWin.jpeg" << endl;
        }

        Sprite YouWinSprite;
        YouWinSprite.setTexture(YouWinTexture);
        YouWinSprite.setScale((float)(window.getSize().x) / YouWinTexture.getSize().x, (float)(window.getSize().y) / YouWinTexture.getSize().y);
        clock.restart();
        while (clock.getElapsedTime().asSeconds() < 3.0) { // Show for 3 seconds
            window.clear();
            window.draw(YouWinSprite);
            window.display();
        }
    }

    void startNewGame() {
        totalTime = 120; // Reset total time
        gameClock.restart(); // Restart the game clock

        menu_buffer.loadFromFile("Data/Global/Sega.wav");
        menu_sound.setBuffer(menu_buffer);

        bossLevelMus.openFromFile("Data/Global/bossLevel.ogg");
        bossLevelMus.setVolume(50);

        level1Mus.openFromFile("Data/Global/level1.ogg");
        level1Mus.setVolume(50);

        level2Mus.openFromFile("Data/Global/level2.ogg");
        level2Mus.setVolume(50);

        level3Mus.openFromFile("Data/Global/level3.ogg");
        level3Mus.setVolume(50);

        Font font;
        font.loadFromFile("Data/Montserrat-Regular.ttf");

        Text scoreText;
        scoreText.setFont(font);
        scoreText.setCharacterSize(30);
        scoreText.setFillColor(Color::Green);
        scoreText.setStyle(Text::Bold);
        scoreText.setPosition(1200 - 170, 30);

        timerText.setFont(font);
        timerText.setCharacterSize(25);
        timerText.setFillColor(Color::Red);
        timerText.setStyle(Text::Bold);
        timerText.setPosition(1200 - 60, 60);
        timerText.setString("120");

        hpText.setFont(font);
        hpText.setCharacterSize(25);
        hpText.setFillColor(Color::Green);
        hpText.setStyle(Text::Bold);
        hpText.setPosition(1200 - 96, 80);
        cout << currentLevelNumber << endl;
        Event ev;
        while (window.isOpen()) {
            while (window.pollEvent(ev)) {
                if (ev.type == Event::Closed) {
                    window.close();
                    return;
                }
            }

            // Handling the music here means changing the music based upon the level player is in 
            if (!music_started && currentLevelNumber == 4)
            {
                music_started = true;
                bossLevelMus.play();
                bossLevelMus.setLoop(true);
                cout << currentLevelNumber << endl;

            }
            if (!music_started && currentLevelNumber == 1)
            {
                music_started = true;
                level1Mus.play();
                level1Mus.setLoop(true);
                cout << currentLevelNumber << endl;
            }
            if (!music_started && currentLevelNumber == 2)
            {
                music_started = true;
                level2Mus.play();
                level2Mus.setLoop(true);
                cout << currentLevelNumber << endl;
            }
            if (!music_started && currentLevelNumber == 3)
            {
                music_started = true;
                level3Mus.play();
                level3Mus.setLoop(true);
                cout << currentLevelNumber << endl;
            }

            Events();

            // Check if the player is alive
            if (!player->check_alive()) {
                int status = gameOverScreen(window, menu.get_name(), player->get_score());
                if (status == 1) {
                    // Return to menu
                    return;
                }
                else if (status == -1) {
                    // Exit the game
                    window.close();
                    return;
                }
            }

            // Calling the move enemies function to move them
            currentLevel->move_enemies(player->get_x(player->get_currentIndex()), player->get_y(player->get_currentIndex()));

            //Checking if it is knuckles and he has absorbed the boost so he gets invincible
            if (!(player->get_invincible())) {
                currentLevel->enemy_collision();
            }

            // Continously calling the update level to check if the level end has been reached so update the level
            updateLevel();
            window.clear();
            currentLevel->display(window);
            scoreText.setString("Score: " + intToString(player->get_score()));
            window.draw(scoreText);

            // Timer update logic
            int timeLeft = totalTime - (int)gameClock.getElapsedTime().asSeconds();
            if (timeLeft < 0) {
                timeLeft = 0;
                int status = gameOverScreen(window, menu.get_name(), player->get_score());
                if (status == 1) {
                    // Return to menu
                    return;
                }
                else if (status == -1) {
                    // Exit the game
                    window.close();
                    return;
                }
            }
            timerText.setString(intToString(timeLeft));
            window.draw(timerText);

            hpText.setString("HP: " + intToString(player->get_hp()));
            window.draw(hpText);



            window.display();
        }
    }

    void run() {
        while (true) {
            int menuStatus = loadMenu();

            if (menuStatus == -1) {
                // Exit the game
                window.close();
                return;
            }
            else if (menuStatus == 1) {
                // Start a new game
                startNewGame();
            }
            else {
                // Continue the game
                continue;
            }
        }
    }

    int loadMenu() {
        int selected = menu.show(window);
        menu_sound.play();

        if (selected == 1) {
            return 1; // New Game
        }
        else if (selected == 2) {
            return 2; // Continue
        }
        else if (selected == 3) { // Options
            int status = showOptions(window);
            if (status == -1 || status == 1) {
                cout << "Exiting Options menu..." << endl;
                return 0; // Return to the main menu
            }
            return 3; // Return to options 
        }
        else if (selected == 4) {
            menu.displayLeaderboard(window); // Show leaderboard
            return 4;
        }
        else {
            return -1; // Exit the game
        }
    }

    void Events() {
        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            int menuStatus = loadMenu();

            if (menuStatus == -1) {
                // Exit the game
                window.close();
                return;
            }
            else if (menuStatus == 1) {
                // Start a new game
                startNewGame();
            }
            else if (menuStatus == 3) {
                // Options menu
                int status = showOptions(window);
                if (status == -1 || status == 1) {
                    cout << "Exiting Options menu..." << endl;
                    return; // Return to the main menu
                }
            }
        }

        //Here all the events mainly the button pressing is handled

        // Switch Character with Z
        if (Keyboard::isKeyPressed(Keyboard::Z)) {
            if (!z_pressed) {
                player->switchCharacter();
                z_pressed = true;
            }
        }
        else {
            z_pressed = false;
        }

        bool keyPressed = false;
        bool is_right = true;

        // Move Left (A)
        if (Keyboard::isKeyPressed(Keyboard::A)) {
            keyPressed = true;
            player->move(false);
            if (currentLevel)
                player->follow_chars(player->get_isJumping(), false);
            is_right = false;
        }
        // Move Right (D)
        else if (Keyboard::isKeyPressed(Keyboard::D)) {
            keyPressed = true;
            player->move(true);
            player->follow_chars(player->get_isJumping(), true);
            is_right = true;
        }
        else {
            player->applyFriction(is_right);
        }

        // Ability Ball Mode (W)
        if (Keyboard::isKeyPressed(Keyboard::W) && !power_active) {
            player->set_isBall(true);
            ball_timer.restart();
        }

        // Disable Ball mode after 3 seconds
        if (player->get_isBall() && ball_timer.getElapsedTime().asSeconds() > 3) {
            player->set_isBall(false);
        }

        // Jump
        if (Keyboard::isKeyPressed(Keyboard::Space)) {
            player->jump();
        }

        // Use Ability (F)
        if (Keyboard::isKeyPressed(Keyboard::F) && !power_active) {
            player->useAbility();
            ability_timer.restart();
            power_active = true;
            cout << "Power ACTIVATED" << endl;
        }

        // Auto disable after the flytime seconds means that is 7 and if tails has boost then that is 11
        if (power_active && ability_timer.getElapsedTime().asSeconds() > player->get_flyTime()) {
            player->set_ability_activated(false);
            power_active = false;
            cout << "Power DEACTIVATED" << endl;
        }

        if (power_active) {
            player->useAbility();
        }
        player->applyGravity(); // Calling gravity function all the time
    }

    void updateLevel() {

        bool gameEnd = false;
        if (currentLevelNumber == 4 && (currentLevel->is_Boss() == false) && gameEnd == false) {
            cout << "You Win Screen displayed" << endl;

            gameEnd = true;
            showYouWin();
            window.close();
        }

        int player_x = player->get_x(player->get_currentIndex());

        if (player_x > ((currentLevel->get_width() * cell_size) - 800) && player->get_levelScore() > 220) {
            levels* newLevel = nullptr;

            /* Switching the levels and keeping the score hp constant */
            switch (currentLevelNumber) {
            case 1:
                showLevelTransition(); // Show Level Up screen
                newLevel = new level2();
                music_started = false;
                level1Mus.stop();
                gameClock.restart();
                break;
            case 2:
                showLevelTransition();
                newLevel = new level3();
                music_started = false;
                level2Mus.stop();
                gameClock.restart();
                break;
            case 3:
                showLevelTransition();
                newLevel = new BossLevel();
                music_started = false;
                level3Mus.stop();
                gameClock.restart();
                break;
            default:
                cout << "No more levels. Game completed!" << endl;
            }

            newLevel->set_level();

            // Transfer player stats
            int hp = player->get_hp();
            int charIndex = player->get_currentIndex();
            int totalScore = player->get_totalScore();

            player = &newLevel->get_player();
            player->set_hp(hp);
            player->set_x(64 * 2);
            player->set_totalScore(totalScore);


            delete currentLevel;

            currentLevel = newLevel;
            currentLevelNumber++;
            cout << "Level shifted to Level " << currentLevelNumber << endl;
        }
    }
    float max(float a, float b) {
        if (a > b) {
            return a;
        }
        else {
            return b;
        }
    }

    float min(float a, float b) {
        if (a < b) {
            return a;
        }
        else {
            return b;
        }
    }

    int showOptions(RenderWindow& window) {
        Font font;
        if (!font.loadFromFile("Data/Montserrat-Regular.ttf")) {
            cout << "Error: Could not load font for Options menu." << endl;
            return -1;
        }

        Text title("Options", font, 50);
        title.setFillColor(Color::Blue);
        title.setPosition(400, 50);

        Text instructions("Use Up/Down to Select, Left/Right to Adjust Volume\nPress Escape to Save and Return", font, 20);
        instructions.setFillColor(Color::White);
        instructions.setPosition(200, 700);

        const int numOptions = 10;
        string optionNames[numOptions] = {
            "Jump Sound", "Switch Character Sound", "Collectible Sound", "Hurt Sound",
            "Destroy Sound", "Level 1 Music", "Level 2 Music", "Level 3 Music",
            "Boss Level Music", "Menu Sound"
        };

        int selectedOption = 0;
        Text options[numOptions];

        for (int i = 0; i < numOptions; i++) {
            options[i].setFont(font);
            options[i].setCharacterSize(30);
            options[i].setPosition(300, 150 + i * 50);
        }

        while (window.isOpen()) {
            Event ev;
            while (window.pollEvent(ev)) {
                if (ev.type == Event::Closed) {
                    window.close();
                    return -1;
                }
                if (ev.type == Event::KeyPressed) {
                    if (ev.key.code == Keyboard::Up) {
                        selectedOption = (selectedOption - 1 + numOptions) % numOptions;
                    }
                    else if (ev.key.code == Keyboard::Down) {
                        selectedOption = (selectedOption + 1) % numOptions;
                    }
                    else if (ev.key.code == Keyboard::Left) {
                        volumes[selectedOption] = max(0.0, volumes[selectedOption] - 5.0);
                    }
                    else if (ev.key.code == Keyboard::Right) {
                        volumes[selectedOption] = min(100.0, volumes[selectedOption] + 5.0);
                    }
                    else if (ev.key.code == Keyboard::Enter) {
                        for (int i = 0; i < numOptions; i++) {
                            changeVolume(i + 1, volumes[i]);
                        }
                        return 1;  // Returning to main menu
                    }
                    else if (ev.key.code == Keyboard::Escape) {
                        // Return to the main menu 
                        return 1;
                    }
                }
            }

            for (int i = 0; i < numOptions; i++) {
                options[i].setString(optionNames[i] + ": " + intToString((int)volumes[i]));
                options[i].setFillColor(i == selectedOption ? Color::Cyan : Color::White);
            }
            window.clear();
            window.draw(title);
            for (int i = 0; i < numOptions; i++) {
                window.draw(options[i]);
            }
            window.draw(instructions);
            window.display();
        }
        return -1;
    }

    void changeVolume(int option, float volume) {
        // 1. Jump sound volume
        // 2. Switch character sound volume
        // 3. Collectible sound volume
        // 4. Hurt sound volume
        // 5. Destroy sound volume
        // 6. level 1 Music volume
        // 7. level 2 Music volume
        // 8. level 3 Music volume
        // 9. Boss level Music volume
        // 10. Menu sound volume
        switch (option) {
        case 1:
            player->set_jumpVol(volume);
            break;
        case 2:
            player->set_switch_charVol(volume);
            break;
        case 3:
            player->set_collectibleVol(volume);
            break;
        case 4:
            player->set_hurtVol(volume);
            break;
        case 5:
            currentLevel->set_destroyVol(volume);
            break;
        case 6:
            level1Mus.setVolume(volume);
            break;
        case 7:
            level2Mus.setVolume(volume);
            break;
        case 8:
            level3Mus.setVolume(volume);
            break;
        case 9:
            bossLevelMus.setVolume(volume);
            break;
        case 10:
            menu_sound.setVolume(volume);
            break;
        }
    }
};