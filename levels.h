#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Character.h"
#include "Enemy.h" 
#include "Collectibles.h" 
using namespace sf;
using namespace std;

class levels {
protected:

    char** lvl;
    const int cell_size = 64;
    const int height;
    int width;
    Texture wallTex1;
    Sprite wallSprite1;
    Texture wallTex2;
    Sprite wallSprite2;
    Texture wallTex3;
    Sprite wallSprite3;
    Sprite spike;
    Texture spikeTex;
    Texture backgroundTexture;
    Sprite backgroundSprite;
    float targetCameraX = 0.0f;
    float currentCameraX = 0.0f;
    float cameraLerpFactor = 0.1f; 
    EnemyManager enemyManager;
    CollectibleManager collectible;
    Player player;



public:
    levels() : lvl(nullptr), width(0), height(14) {


    }
    virtual void set_level() = 0;
    virtual int get_width() = 0;
    virtual bool enemy_collision() = 0;
    virtual char get_value(int x, int y) {
        if (x >= 0 && x < height && y >= 0 && y < width)
            return lvl[x][y];
        else
            return '\0';
    }

    virtual Player& get_player() = 0;
    char** get_grid() {
        return lvl;
    }
    virtual void reset_camera_position() {
        targetCameraX = 0.0f;
        currentCameraX = 0.0f;
    }
    virtual void move_enemies(int playerX, int playerY) = 0;
    virtual void render_enemies(RenderWindow& window) = 0;
    virtual ~levels() {
        if (lvl) {
            for (int i = 0; i < height; i++)
                delete[] lvl[i];
            delete[] lvl;
        }
    }
    virtual void render_collectibles(RenderWindow& window) = 0;
    virtual bool is_Boss() = 0;
    virtual void set_destroyVol(float vol) = 0;
    void display(RenderWindow& window)  {

        // Calculating the number of times the background need to be repeated so that we can get number of times we need to draw it 
        int bgWidth = backgroundTexture.getSize().x;
        int repeatCount = (window.getSize().x / bgWidth) + 2;

        for (int i = 0; i < repeatCount; ++i) {
            backgroundSprite.setPosition(-int(currentCameraX * 0.5f) % bgWidth + i * bgWidth, 0);
            window.draw(backgroundSprite);
        }


        /* Now what we are doing is calculating the current position of the player and setting the target of camera to
        always be at the center of the screen and the screen is of 1200 and the columns present in it is nearly 19 so - 9 means 10 and approximaely
        center of the screen so setting target of camera x towards there and moving there not at one go but slowly thats why using lerp factor means adding
        the x value slowly slowly . Basically simply keeping the tring to keep the camera at center around the player */

        int playerColumn = player.get_x(player.get_currentIndex()) / cell_size;
        int targetStartColumn = playerColumn - 9;
        if (targetStartColumn < 0) targetStartColumn = 0;
        targetCameraX = targetStartColumn * cell_size;
        currentCameraX = currentCameraX + (targetCameraX - currentCameraX) * cameraLerpFactor;



        /* Firstly determining the starting column based on camer x from which the columns should be started to be displayed and then finding the end column
         which is current column + 19 as on screen there are 19 columns . Also checking if these are the end columns that are being displaying then limiting
         the columns to last 20 to avoid showing or going out of the bounds
        */
        int startColumn = (int)(currentCameraX / cell_size);
        if (startColumn < 0) startColumn = 0;
        int endColumn = startColumn + 19;
        if (endColumn >= width) endColumn = width - 1;
        if (width > 20 && endColumn == width - 1) {
            startColumn = width - 20;
            if (startColumn < 0) startColumn = 0;
            currentCameraX = startColumn * cell_size;
        }
        float xOffset = currentCameraX;



        /* Now drawing the level height is fixed to be 14 as no verticle scrolling and then talking about the the columns the columns from start to end
        column calculated based upon the camera and then converting the grid coordinated into screen coordinated by using the offset then getting the tile
        or we can say the data store at that index and then drawing the object present there based upon the character stored in that tile either that is
        wall , spikes or collectibles */

        for (int i = 0; i < height; i++) {
            for (int j = startColumn; j <= endColumn; j++) {
                float drawX = j * cell_size - xOffset;
                float drawY = i * cell_size;
                char tile = lvl[i][j];
                if (tile == 'w') {
                    wallSprite1.setPosition(drawX, drawY);
                    window.draw(wallSprite1);
                }
                else if (tile == 's') {
                    wallSprite2.setPosition(drawX, drawY);
                    window.draw(wallSprite2);
                }
                else if (tile == 'p') {
                    wallSprite3.setPosition(drawX, drawY);
                    window.draw(wallSprite3);
                }
                else if (tile == 'k') {
                    spike.setPosition(drawX, drawY);
                    window.draw(spike);
                }
                else if (tile == 'r') {
                    Collectibles* rings = &(collectible.get_collectible(0));
                    rings->get_sprite().setPosition(drawX, drawY);
                    window.draw(rings->get_sprite());
                }
                else if (tile == 'e') {
                    Collectibles* extraLives = &(collectible.get_collectible(1));
                    extraLives->get_sprite().setPosition(drawX, drawY);
                    window.draw(extraLives->get_sprite());
                }
                else if (tile == 'b') {
                    Collectibles* boosts = &(collectible.get_collectible(2));
                    boosts->get_sprite().setPosition(drawX, drawY);
                    window.draw(boosts->get_sprite());
                }
            }
        }


        /* Drawing the player by looping all three players and positioning them based upon the offset  */

        for (int i = 0; i < 3; i++) {
            Sprite playerSprite = player.getSpriteByIndex(i);
            float originalX = player.get_x(i);
            float originalY = player.get_y(i);
            playerSprite.setPosition(originalX - xOffset, originalY);
            window.draw(playerSprite);
        }
        /* At last drawing the enemies and the bullet. Basically checking if the enemy is alive and then calculating the position of the enemy based
        upon the offset of the camera and then rendering them on the screen but there is special logic for beebot and the crab meat as they both fires
        bullet so checking if the bullet is active than adjusting its x coordinate based upon the camera offset while keeping the y value same and then
        drawing the bullet too and then setting the position of the enemies to the orignal - offset position for the drawing purpose and then drawing the
        enemy and when it has been drawned then resetting it back to its orignal position doing the same process for both i == 2 and i == 3 which is beebot
        and the crab meat */

        for (int i = 0; i < enemyManager.get_numEnemies(); i++) {
            Enemy& enemy = enemyManager.get_enemy(i);
            if (enemy.get_isAlive()) {
                float originalX = enemy.get_x();
                enemy.set_x(originalX - xOffset);
                enemy.render(window);
                if (i == 2) {
                    beeBot& bee = static_cast<beeBot&>(enemy);
                    bullet& enemyBullet = bee.get_bullet();
                    if (enemyBullet.isActive()) {
                        float originalBulletX = enemyBullet.get_x();
                        enemyBullet.set_x(originalBulletX - xOffset);
                        enemyBullet.draw(window);
                        enemyBullet.set_x(originalBulletX);
                    }
                }
                else if (i == 3)
                {
                    crabMeat& crab = static_cast<crabMeat&>(enemy);
                    bullet& enemyBullet = crab.get_bullet();
                    if (enemyBullet.isActive()) {
                        float originalBulletX = enemyBullet.get_x();
                        enemyBullet.set_x(originalBulletX - xOffset);
                        enemyBullet.draw(window);
                        enemyBullet.set_x(originalBulletX);
                    }
                }
                enemy.set_x(originalX);
            }
        }
    }

};




//Class level1 inherits from the class levels and other levels also inherit from levels

class level1 : public levels {
private:
    Clock collision_timer;
    bool collectiblesInitialized;
    
    Music lvlMus;

public:
    level1() : collectiblesInitialized(false) {
        /* Simply setting up the sprites and textures and initialising the things and setting width to 200 */

        width = 200;
        wallTex1.loadFromFile("Data/brick1.png");
        wallSprite1.setTexture(wallTex1);

        wallTex2.loadFromFile("Data/brick2.png");
        wallSprite2.setTexture(wallTex2);

        wallTex3.loadFromFile("Data/brick3.png");
        wallSprite3.setTexture(wallTex3);

        spikeTex.loadFromFile("Data/spike.png");
        spike.setTexture(spikeTex);

        backgroundTexture.loadFromFile("Data/bg1.png");
        backgroundSprite.setTexture(backgroundTexture);

        lvl = new char* [height];
        for (int i = 0; i < height; i++)
            lvl[i] = new char[width] {'\0'};

        // Initial layout
        lvl[11][1] = 'w';
        lvl[11][2] = 'w';
        lvl[11][3] = 'w';
        enemyManager.set_numEnemies(4);


    }

    void set_level() override {
        /* Now creating the level manullay creating all the obstacles and after that setting the player on the obstacle so
         w =  wall 
         s = Breakable wall to check the ability of knuckles
         p = platforms  that only have top collision only 
         k = spikes 
         so setting up the level and in the end calling the collectibles spawn function too , to set the collectibles all around the
         array of the level 
         */
        
        lvl[11][5] = 'w';
        lvl[10][5] = 'w';
        lvl[9][5] = 'w';


        //Walls at the bottom and at the top creating boundaries and bottom less pits too
        for (int j = 0; j < width; j++) {
            if (!((j % 33) == 0 || (j % 33) == 1)) {
                lvl[11][j] = 'w';
            }
            else {
                lvl[11][j] = '\0';
            }
            lvl[0][j] = 'w';
        }


        //Border at right and left 
        for (int i = 0; i < height; i++) {
            lvl[i][0] = 'w';
            lvl[i][width - 1] = 'w';
        }


        //Walls at random locations
        for (int i = 0; i < 80; i++) {
            if ((i % 10 == 0)) {
                lvl[8][i] = 'w';
                lvl[8][i + 1] = 'w';
                lvl[8][i + 2] = 'w';
            }
        }

        //Breakable walls at random locations
        for (int i = 0; i < 80; i++) {
            if ((i % 10 == 0)) {
                lvl[8][i] = 's';
                lvl[9][i] = 's';
                lvl[10][i] = 's';
            }
        }


        // Platforms
        for (int i = 30; i < width; i++) {
            if ((i % 10) == 2) {
                lvl[6][i] = 'p';
                lvl[6][i + 1] = 'p';
                lvl[6][i + 2] = 'p';
            }
        }

        //Random obstacles placements
        for (int i = 0; i < width; i++) {
            if ((i % 4) == 0) {
                lvl[1][i] = 'w';
                lvl[2][i + 2] = 'w';
            }
        }

        // A obstacle like _-_ just for the difficulty
        for (int i = 80; i < 150; i++) {
            if ((i % 10 == 4)) {
                lvl[10][i] = 'w';
                lvl[9][i + 1] = 'w';
                lvl[10][i + 2] = 'w';
            }
        }

        //Random obstacles
        for (int i = 150; i < 200; i++) {
            if ((i % 4 == 0)) {
                lvl[3][i] = 'w';
                lvl[7][i + 1] = 'w';
                lvl[7][i + 2] = 'w';
            }
        }

        //Planting spikes
        lvl[10][7] = 'k';
        lvl[10][27] = 'k';
        lvl[10][79] = 'k';
        lvl[10][129] = 'k';
        lvl[10][197] = 'k';


        // Initializing collectibles properly
        if (!collectiblesInitialized) {
            collectible.spawn_collectibles(lvl);
            collectible.renderAll(width);
            collectiblesInitialized = true;
        }

        player.setLevel(lvl, 1); // Setting player for this level
    }

    Player& get_player() override 
    {
        return player;
    }



    //Now below one are just the helper functions means setter getter and rendering functions to call composed objects rendering functions

    void set_destroyVol(float vol)override
    {
        enemyManager.destroy_Vol(vol);
    }

    int get_width() override {
        return width;
    }

    void move_enemies(int playerX, int playerY) override { 
        enemyManager.move(playerX, playerY);
    }

    void render_enemies(RenderWindow& window) override { 
        enemyManager.renderAll(window);
    }

    void render_collectibles(RenderWindow& window) override
    {
        collectible.drawAll(window);
    }


    bool enemy_collision() override //Just getting the bool value if the collision between enemies and the player has taken place or not
    {
        bool check = enemyManager.checkCollisions(player);

        return check;
    }

    bool is_Boss() override // Checking if it is boss level 
    {
        return false;
    }
};



// Level 2
class level2 : public levels {
private:
    Clock collision_timer;
    bool collectiblesInitialized;
  


public:
    level2() : collectiblesInitialized(false) {

        // Setting up the things

        width = 250;
        wallTex1.loadFromFile("Data/brick1.png");
        wallSprite1.setTexture(wallTex1);

        wallTex2.loadFromFile("Data/brick2.png");
        wallSprite2.setTexture(wallTex2);


        wallTex3.loadFromFile("Data/brick3.png");
        wallSprite3.setTexture(wallTex3);


        spikeTex.loadFromFile("Data/spike.png");
        spike.setTexture(spikeTex);
        backgroundTexture.loadFromFile("Data/bg1.png");
        backgroundSprite.setTexture(backgroundTexture);

        lvl = new char* [height];
        for (int i = 0; i < height; i++)
            lvl[i] = new char[width] {'\0'};

        lvl[11][1] = 'w';
        lvl[11][2] = 'w';
        lvl[11][3] = 'w';



        //Level two has high acceleration and low deceleration as it is ice based level
        player.set_acceleration(0.4);
        player.set_deceleration(0.4);

        targetCameraX = 0;
        currentCameraX = 0.0f;
        cameraLerpFactor = 0.1f; 
        enemyManager.set_numEnemies(4);
    }

    void set_level() override {

        /* Now creating the level manullay creating all the obstacles and after that setting the player on the obstacle so
        w =  wall
        s = Breakable wall to check the ability of knuckles
        p = platforms  that only have top collision only
        k = spikes
        so setting up the level and in the end calling the collectibles spawn function too , to set the collectibles all around the
        array of the level
        */

        lvl[11][5] = 'w';
        lvl[10][5] = 'w';
        lvl[11][4] = 'w';
        lvl[11][3] = 'w';
        lvl[11][2] = 'w';


        //Walls , boundaries and the bottom less pits
        for (int j = 0; j < width; j++) {
            if (!((j % 33) == 0 || (j % 33) == 1)) {
                lvl[11][j] = 'w';
            }
            else {
                lvl[11][j] = '\0';
            }
            lvl[0][j] = 'w';

            if ((j % 2) == 0)
                lvl[2][j] = 'w';

        }


        //Side walls like boundaries
        for (int i = 0; i < height; i++) {
            lvl[i][0] = 'w';
            lvl[i][width - 1] = 'w';
        }

        // Creating stairs
        int stairStart = 20;
        lvl[10][stairStart] = 'w';
        lvl[10][stairStart + 1] = 'w';
        lvl[9][stairStart + 3] = 'w';
        lvl[9][stairStart + 4] = 'w';
        lvl[8][stairStart + 6] = 'w';
        lvl[8][stairStart + 7] = 'w';
        lvl[7][stairStart + 9] = 'w';
        lvl[7][stairStart + 10] = 'w';
        lvl[6][stairStart + 12] = 'w';
        lvl[6][stairStart + 13] = 'w';
        lvl[6][stairStart + 15] = 'w';
        lvl[6][stairStart + 17] = 'w';
        lvl[6][stairStart + 19] = 'w';
        lvl[6][stairStart + 21] = 'w';
        lvl[6][stairStart + 23] = 'w';

        //Creating the walls at random
        for (int i = 50; i < 150; i += 20) {
            lvl[6][i] = 'w';
            lvl[6][i + 23] = 'w';
        }

        // Designing "W" just for the difficulty and use tails ability
        int i = 50; 
        lvl[7][i] = 'w';
        lvl[8][i] = 'w';
        lvl[9][i] = 'w';
        lvl[10][i] = 'w';
        lvl[10][i + 1] = 'w';
        lvl[10][i + 2] = 'w';
        lvl[9][i + 3] = 'w';
        lvl[8][i + 4] = 'w';
        lvl[7][i + 5] = 'w';
        lvl[8][i + 6] = 'w';
        lvl[9][i + 7] = 'w';
        lvl[10][i + 8] = 'w';
        lvl[10][i + 9] = 'w';
        lvl[10][i + 10] = 'w';
        lvl[10][i + 11] = 'w';
        lvl[9][i + 11] = 'w';
        lvl[8][i + 11] = 'w';
        lvl[7][i + 11] = 'w';


        //Creating breakable walls and the objects like --- for difficulty only
        for (int i = 80; i < 200; i += 20)
        {
            lvl[11][i] = 's';
            lvl[10][i] = 's';
            lvl[9][i] = 's';
            lvl[9][i - 1] = 's';
            lvl[9][i + 1] = 's';

            lvl[7][i + 3] = 'p';
            lvl[7][i + 4] = 'p';
            lvl[7][i + 5] = 'p';

        }

        //Random objects
        for (int i = 200; (i + 4) < width;i += 10)
        {
            lvl[10][i] = 'w';
            lvl[9][i] = 'w';


            lvl[7][i + 1] = 'w';
            lvl[7][i + 2] = 'w';
            lvl[6][i + 3] = 'w';
            lvl[7][i + 4] = 'w';
        }


        //Platforms
        for (int i = 0; i < 200; i += 20)
        {
            lvl[6][i] = 'p';
            lvl[6][i + 1] = 'p';
            lvl[6][i + 2] = 'p';
        }



        // Planting spikes at the specified locations
        int spikes[] = { 7, 27, 39, 79, 129, 157, 197, 227, 243 };
        for (int s = 0; s < 9; s++) {
            if (spikes[s] < width)
                lvl[10][spikes[s]] = 'k';
        }

        // Now generating the collectibles and rendering them
        if (!collectiblesInitialized) {
            collectible.spawn_collectibles(lvl);
            collectible.renderAll(width);
            collectiblesInitialized = true;
        }

        // Setting player for level 2
        player.setLevel(lvl, 2);
    }


    Player& get_player() override
    {
        return player;
    }





    // Getter , setter and accesors to composed objects
    int get_width() override {
        return width;
    }

    void move_enemies(int playerX, int playerY) override {
        enemyManager.move(playerX, playerY);
    }

    void render_enemies(RenderWindow& window) override {
        enemyManager.renderAll(window);
    }

    void render_collectibles(RenderWindow& window) override
    {
        collectible.drawAll(window);
    }


    bool enemy_collision() override
    {
        bool check = enemyManager.checkCollisions(player);

        return check;
    }

    bool is_Boss() override
    {
        return false;
    }

    void set_destroyVol(float vol)override
    {
        enemyManager.destroy_Vol(vol);
    }
};


// Level 3
class level3 : public levels {
private:
    Clock collision_timer;
    bool collectiblesInitialized;
   

public:

    // Setting the level

    level3() : collectiblesInitialized(false) {
        width = 300;
        wallTex1.loadFromFile("Data/brick1.png");
        wallSprite1.setTexture(wallTex1);

        wallTex2.loadFromFile("Data/brick2.png");
        wallSprite2.setTexture(wallTex2);

        wallTex3.loadFromFile("Data/brick3.png");
        wallSprite3.setTexture(wallTex3);

        spikeTex.loadFromFile("Data/spike.png");
        spike.setTexture(spikeTex);

        backgroundTexture.loadFromFile("Data/bg1.png");
        backgroundSprite.setTexture(backgroundTexture);

        lvl = new char* [height];
        for (int i = 0; i < height; i++)
            lvl[i] = new char[width] {'\0'};

        // Initial layout
        lvl[11][1] = 'w';
        lvl[11][2] = 'w';
        lvl[11][3] = 'w';


        //Reducing the gravity as it is happening in the space and also slowing down the speeds
        player.set_gravity(0.6);
        player.set_maxSpeed(14, 7, 9);

        enemyManager.set_numEnemies(4);
    }

    void set_level() override {

        /* Now creating the level manullay creating all the obstacles and after that setting the player on the obstacle so
        w =  wall
        s = Breakable wall to check the ability of knuckles
        p = platforms  that only have top collision only
        k = spikes
        so setting up the level and in the end calling the collectibles spawn function too , to set the collectibles all around the
        array of the level
        */

        lvl[11][5] = 'w';
        lvl[10][5] = 'w';
        lvl[11][4] = 'w';
        lvl[11][3] = 'w';
        lvl[11][2] = 'w';


        //Platform , bottom less pits and leving the places where i have created a cage and the HI structure
        for (int j = 0; j < width; j++) {
            if (!((j % 15) == 0 || (j % 15) == 1) || ((j % 15) == 2) || ((j % 15) == 3)) {
                if (j > 50 && j < 60)
                {
                    lvl[11][j] = 'k';
                    continue;
                }
                lvl[11][j] = 'w';
            }
            else {
                lvl[11][j] = '\0';
            }
            lvl[0][j] = 'w';

            if ((j % 2) == 0 && j > 50 && j < 150)
            {
                lvl[2][j] = 's';
            }
            if ((j % 2) == 0 && j > 170 && j < 300)
            {
                lvl[2][j] = 's';
            }


        }

        // A cage at the starting of the level
        for (int i = 0; i < 7; i++)
        {
            lvl[4][i] = 's';
            lvl[i][4] = 's';
            lvl[5][i] = 's';
            lvl[i][5] = 's';
            lvl[6][i] = 's';
            lvl[i][6] = 's';
        }



        for (int i = 0; i < height; i++) {
            lvl[i][0] = 'w';
            lvl[i][width - 1] = 'w';
        }



        //Creating HI sign

        for (int i = 0; i < 11; i++)
        {
            lvl[i][150] = 's';
            lvl[i][155] = 's';
            lvl[i][160] = 's';
        }
        for (int i = 150; i <= 155; i++)
        {
            lvl[5][i] = 's';
        }
        for (int i = 158; i < 162; i++)
        {
            lvl[2][i] = 's';
            lvl[10][i] = 's';
        }



        //Random platforms like ---
        for (int i = 200; i < 297; i += 15)
        {
            lvl[6][i] = 'p';
            lvl[6][i + 1] = 'p';
            lvl[6][i + 2] = 'p';

        }

        //Planting spikes over those obstacles defined at above
        for (int i = 200; i < 297; i += 15)
        {
            lvl[5][i + 1] = 'k';

        }


        // Planting spikes
        int spikes[] = { 27, 39, 79, 129, 167, 197, 227, 243 , 297 };
        for (int s = 0; s < 9; s++) {
            if (spikes[s] < width)
                lvl[10][spikes[s]] = 'k';
        }

        // Setting up collectibles
        if (!collectiblesInitialized) {
            collectible.spawn_collectibles(lvl);
            collectible.renderAll(width);
            collectiblesInitialized = true;
        }

        // Setting up the player
        player.setLevel(lvl, 3);
    }


    Player& get_player() override
    {
        return player;
    }

  
    // Getters , setter and accessors to the composed object variables 

    int get_width() override {
        return width;
    }

    void move_enemies(int playerX, int playerY) override {
        enemyManager.move(playerX, playerY);
    }

    void render_enemies(RenderWindow& window) override {
        enemyManager.renderAll(window);
    }

    void render_collectibles(RenderWindow& window) override
    {
        collectible.drawAll(window);
    }


    bool enemy_collision() override
    {
        bool check = enemyManager.checkCollisions(player);

        return check;
    }

    bool is_Boss() override
    {
        return false;
    }
    void set_destroyVol(float vol)override
    {
        enemyManager.destroy_Vol(vol);
    }
};


// Boss level

class BossLevel : public levels {
private:
    Clock collision_timer;
    bool collectiblesInitialized;
  

public:
    BossLevel() {

        //Setting up the level

        width = 300;
        wallTex1.loadFromFile("Data/brick1.png");
        wallSprite1.setTexture(wallTex1);

        wallTex2.loadFromFile("Data/brick2.png");
        wallSprite2.setTexture(wallTex2);


        spikeTex.loadFromFile("Data/spike.png");
        spike.setTexture(spikeTex);
        backgroundTexture.loadFromFile("Data/bg2.png");
        backgroundSprite.setTexture(backgroundTexture);

        lvl = new char* [height];
        for (int i = 0; i < height; i++)
            lvl[i] = new char[width] {'\0'};

        enemyManager.deactivate_enemy();
        enemyManager.set_numEnemies(5);


    }

    void set_level() override {

        /* Now creating the level manullay creating all the obstacles and after that setting the player on the obstacle so
        w =  wall
        s = Breakable wall to check the ability of knuckles
        p = platforms  that only have top collision only
        k = spikes
        so setting up the level and in the end calling the collectibles spawn function too , to set the collectibles all around the
        array of the level
        */


        // Walls for ground
        for (int i = 4; i <= 14; i++)
        {
            lvl[10][i] = 'w';
        }

        //Upper and the side walls 
        for (int i = 0; i < width; i++)
        {
            lvl[0][i] = 's';
        }
        for (int i = 0; i < height; i++) {
            lvl[i][0] = 's';
            lvl[i][18] = 's';
        }

        // Setting up player also setting to the location above the center platform
        player.setLevel(lvl, 4);
        player.set_x(64 * 10);
        player.set_y(400);
        enemyManager.initialise_boss(lvl);

    }


    Player& get_player() override
    {
        return player;
    }



    // Setters , getter and accessors

    int get_width() override {
        return width;
    }

    void move_enemies(int playerX, int playerY) override {
        enemyManager.move_boss(playerX, playerY);
    }


    void render_enemies(RenderWindow& window) override {
        enemyManager.renderAll(window);
    }

    void render_collectibles(RenderWindow& window) override
    {
        collectible.drawAll(window);
    }


    bool enemy_collision() override
    {
        bool check = enemyManager.checkCollisions(player);
        if (check == true)
        {

        }

        return check;
    }

    bool is_Boss() override
    {
        
        return enemyManager.isAlive(4);
    }
    void set_destroyVol(float vol)override
    {
        enemyManager.destroy_Vol(vol);
    }
};