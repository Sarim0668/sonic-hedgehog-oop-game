#pragma once
#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;
using namespace std;

class Menu
{
private:
    Font font;
    Text title;
    Text options[5];
    int Choice = 0;
    bool musicOn = true;
    bool soundOn = true;
    string playerName;
    Texture bg_Texture;
    Sprite bg_Sprite;
    static const int maxEntries = 100;
    string names[maxEntries];
    int scores[maxEntries];
    int entryCount = 0;

public:
    Menu()
    {
        font.loadFromFile("Data/MightySouly-lxggD.ttf");
        if (!bg_Texture.loadFromFile("Data/bg_Menu2.jpg"))
        {
            cout << "Error loading background image!" << endl;
        }
        bg_Sprite.setTexture(bg_Texture);
        bg_Sprite.setPosition(0, 0);

        bg_Sprite.setScale(0.3, 0.4);
        // Title
        title.setFont(font);
        title.setString("SONIC CLASSIC HEROES");
        title.setCharacterSize(70);
        title.setFillColor(Color::Blue);
        title.setPosition(80, 120);

        // Menu Options
        string option[5] = { "New Game", "Continue", "Options", "Leaderboard", "Exit" };

        for (int i = 0; i < 5; ++i)
        {
            options[i].setFont(font);
            options[i].setString(option[i]);
            options[i].setCharacterSize(40);
            if (i == 0)
            {
                options[i].setFillColor(Color::Red); // highlight first option
            }
            else
            {
                options[i].setFillColor(Color::White);
            }
            options[i].setPosition(80, 300 + (i * 80 /* space bw options*/));
        }
    }

    int show(RenderWindow& window)
    {
        while (window.isOpen())
        {
            Event ev;
            while (window.pollEvent(ev))
            {
                if (ev.type == Event::Closed)
                    window.close();
            }

            // Move selection
            if (Keyboard::isKeyPressed(Keyboard::Down))
            {
                Choice = Choice + 1;
                if (Choice > 4)
                { // if Choice exceeds the number of options wrap around to the first option again
                    Choice = 0;
                }
                updateHighlight();
                sleep(milliseconds(200));
            }

            if (Keyboard::isKeyPressed(Keyboard::Up))
            {
                Choice = Choice - 1;
                if (Choice < 0)
                { // if Choice goes below 0 wrap around to the last option
                    Choice = 4;
                }
                updateHighlight();
                sleep(milliseconds(200));
            }

            if (Keyboard::isKeyPressed(Keyboard::Enter))
            {
                if (Choice == 0)
                {
                    if (enterName(window))
                    {
                        return 1;
                    }
                }

                else if (Choice == 1)
                {
                    return 2;
                } // Continue
                else if (Choice == 2)
                {
                    return 3;
                }
                else if (Choice == 3)
                {
                    return 4;
                } // Leaderboard

                else if (Choice == 4)
                {
                    window.close(); // Exit the game
                }
                sleep(milliseconds(200));
            }

            // Render menu
            window.clear();
            window.draw(bg_Sprite);
            window.draw(title);
            for (int i = 0; i < 5; i++)
            {
                window.draw(options[i]);
            }
            window.display();
        }

        return -1; // If window closed then exit
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
    void pop_back(char str[])
    {
        int len = 0;
        while (str[len] != '\0')
        {
            len++;
        }
        if (len > 0)
        {
            str[len - 1] = '\0';
        }
    }
    bool is_empty(char str[])
    {
        bool a = false;
        if (str[0] == '\0')
        {
            a = true;
            return a;
        }
        else
        {
            return a;
        }
    }

    void addChar(char str[], char c)
    {
        int len = 0;
        while (str[len] != '\0')
        {
            len++;
        }
        if (len < 99)
        {
            str[len] = c;
            str[len + 1] = '\0';
        }
    }
    bool enterName(RenderWindow& window)
    {
        char input[100] = "";
        Text inputText, prompt, errorText;

        inputText.setFont(font);
        inputText.setCharacterSize(30);
        inputText.setFillColor(Color::Black);
        inputText.setPosition(100, 200);

        prompt.setFont(font);
        prompt.setCharacterSize(40);
        prompt.setFillColor(Color::Blue);
        prompt.setString("Enter your name:");
        prompt.setPosition(100, 120);

        errorText.setFont(font);
        errorText.setCharacterSize(25);
        errorText.setFillColor(Color::Red);
        errorText.setString("Enter your name first please!");
        errorText.setPosition(100, 250);

        bool showError = false;
        Clock keyTimer;

        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
            }

            if (keyTimer.getElapsedTime().asMilliseconds() > 200)
            {
                for (int key = Keyboard::A; key <= Keyboard::Z; ++key)
                {
                    if (Keyboard::isKeyPressed((Keyboard::Key)key))
                    {
                        addChar(input, (char)('A' + (key - Keyboard::A)));
                        keyTimer.restart();
                        break;
                    }
                }

                for (int key = Keyboard::Num0; key <= Keyboard::Num9; ++key)
                {
                    if (Keyboard::isKeyPressed((Keyboard::Key)key))
                    {
                        addChar(input, (char)('0' + (key - Keyboard::Num0)));
                        keyTimer.restart();
                        break;
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::BackSpace) && !is_empty(input))
                {
                    pop_back(input);
                    keyTimer.restart();
                }

                if (Keyboard::isKeyPressed(Keyboard::Space))
                {
                    addChar(input, ' ');
                    keyTimer.restart();
                }

                if (Keyboard::isKeyPressed(Keyboard::Enter))
                {
                    if (is_empty(input))
                    {
                        showError = true;
                    }
                    else
                    {
                        playerName = input;
                        return true;
                    }
                    keyTimer.restart();
                }
            }

            inputText.setString(input);
            window.clear();
            window.draw(bg_Sprite);
            window.draw(prompt);
            window.draw(inputText);
            if (showError)
                window.draw(errorText);
            window.display();
        }

        return false;
    }

    string get_name()
    {
        return playerName;
    }

    void updateHighlight()
    {
        for (int i = 0; i < 5; ++i)
        {
            if (i == Choice)
            {
                options[i].setFillColor(Color::Red); // highlight selected option
            }
            else
            {
                options[i].setFillColor(Color::White); // reset color for other options
            }
        }
    }

    string getPlayerName() const
    {
        return playerName;
    }

    void readScoresFromFile()
    {
        ifstream inFile("leaderboard.txt");
        if (!inFile)
        {
            cout << "Error: Could not open leaderboard file for reading." << endl;
            return;
        }

        entryCount = 0;
        while (inFile >> names[entryCount] >> scores[entryCount])
        {
            entryCount++;
            if (entryCount >= maxEntries)
                break;
        }

        inFile.close();
    }
    void writeScoresToFile()
    {
        ofstream outFile("leaderboard.txt");
        if (!outFile)
        {
            cout << "Error: Could not open leaderboard file for writing." << endl;
            return;
        }

        for (int i = 0; i < entryCount; i++)
        {
            outFile << names[i] << " " << scores[i] << endl;
        }

        outFile.close();
    }

    void displayLeaderboard(RenderWindow& window)
    {
        Font font;
        font.loadFromFile("Data/Montserrat-Regular.ttf");

        Text title("Leaderboard", font, 40);
        title.setFillColor(Color::Yellow);
        title.setPosition(400, 50);

        // array to store top 10 scores
        Text scoreTexts[10];

        int count = (entryCount < 10) ? entryCount : 10;
        for (int i = 0; i < count; ++i)
        {
            scoreTexts[i].setFont(font);
            scoreTexts[i].setString(names[i] + " - " + intToString(scores[i]));
            scoreTexts[i].setCharacterSize(30);
            scoreTexts[i].setFillColor(Color::White);
            scoreTexts[i].setPosition(400, 120 + i * 40);
        }

        while (window.isOpen())
        {
            Event ev;
            while (window.pollEvent(ev))
            {
                if (ev.type == Event::Closed || (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Escape))
                {
                    return; // Exit leaderboard
                }
            }

            window.clear();
            window.draw(title);
            for (int i = 0; i < count; ++i)
            {
                window.draw(scoreTexts[i]);
            }
            window.display();
        }
    }

    void addScore(const string& playerName, int score)
    {
        // Add the new score
        if (entryCount < maxEntries)
        {
            names[entryCount] = playerName;
            scores[entryCount] = score;
            entryCount++;
        }
        else
        {
            // Replace the lowest score if the new score is higher than that
            int minIndex = 0;
            for (int i = 1; i < entryCount; i++)
            {
                if (scores[i] < scores[minIndex])
                {
                    minIndex = i;
                }
            }
            if (score > scores[minIndex])
            {
                names[minIndex] = playerName;
                scores[minIndex] = score;
            }
        }

        // Sort the leaderboard in descending order
        for (int i = 0; i < entryCount - 1; i++)
        {
            for (int j = i + 1; j < entryCount; j++)
            {
                if (scores[i] < scores[j])
                {

                    int tempScore = scores[i];
                    scores[i] = scores[j];
                    scores[j] = tempScore;

                    string tempName = names[i];
                    names[i] = names[j];
                    names[j] = tempName;
                }
            }
        }
        // Write the updated leaderboard again
        writeScoresToFile();
    }
};