#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Character.h"
using namespace sf;
using namespace std;


class bullet {
private:
    Sprite sprite;
    Texture texture;
    float x = 0, y = 0;
    float velocityX = 0, velocityY = 0;
    float speed = 5.0f;
    bool is_active = false;

public:
    void initialize(Texture& tex) {
        texture = tex;
        sprite.setTexture(texture);
        sprite.setScale(1.5f, 1.5f);
    }

    /* It basically starts from the position where the enemy is present and if it is crabmeat then it move upwards and 
    if it is bee bot then the projectile moves dowanwards thats why i am using direction up in here */
    void fire(float startX, float startY, bool directionUp = false) {
        x = startX;
        y = startY;
        is_active = true;
        velocityX = 0;
        velocityY = directionUp ? -speed : speed;  
        sprite.setPosition(x, y);
    }

    //Simple move function move in the direction of velocity x and y and the velocity x is mostly 0 and y is the one which we act upon
    void move() {
        if (is_active) {
            x += velocityX;
            y += velocityY;

            sprite.setPosition(x, y);

            // Making i=is active false if it goes out of bounds
            if (x < 0 || x > 10000 || y < 0 || y > 10000) {
                is_active = false;
            }
        }
    }

    void draw(RenderWindow& window) {
        if (is_active) {
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }

    void deactivate() { is_active = false; }
    bool isActive() const { return is_active; }
    int get_x() { return x; }
    void set_x(int value) {
        x = value;
        sprite.setPosition(x, y);
    }

    //Getting the four corners if the sprite to handle the bullet and the player collision
    int getLeft() const { return (int)x; }
    const int width = 20;
    const int height = 29;
    int getRight() const { return (int)(x + width); }
    int getBottom() const { return (int)(y + height); }
    int getTop() const { return (int)y; }
};




//Abstract enemy class
class Enemy {  
protected:
    int hp;
    float x, y;
    bool isAlive;
    Sprite sprite;
    Texture texture;
    Sprite bullet_sprite;
    Texture bullet_tex;
    bool bullet_active = false;
    float bullet_x, bullet_y;
    Clock projectiles_timer;
    bullet defaultBullet; 

public:
    Enemy() {
        x = y = 0;
        isAlive = true;
    }

    bool isEnemyAlive() { 
        return isAlive;
    }
    void deactivate_enemy()// Mainly for boss level as it only has 1 enemy
    {
        isAlive = false;
    }

    virtual bool checkCollision(Player& player) = 0;
    virtual void move(int player_x, int player_y) = 0;
    virtual void loadTexture() = 0;
    virtual void loadTexture_runningR(int frameX) = 0;
    virtual void loadTexture_runningL(int frameX) = 0;
    virtual int get_x() = 0;
    virtual int get_y() = 0;

    virtual bullet& get_bullet() {
        return defaultBullet; 
    }

    virtual ~Enemy() {} 
    //Functions to set the positions and rendering the sprites
    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
        sprite.setPosition(x, y);
    }

    virtual void render(RenderWindow& window) {
        if (isAlive) {  // Only render if the enemy is alive
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }

    Sprite& getSprite() {
        return sprite;
    }

    Sprite& getBulletSprite()
    {
        return bullet_sprite;
    }

    bool get_isAlive()
    {
        return isAlive;
    }

    void set_x(int num)
    {
        x = num;
    }
    void set_y(int num)
    {
        y = num;
    }

};

// Bet brain enemy it follows the player 
class BatBrain : public Enemy {
private:
    Clock timer;
    int frameCounter;
    const int frameWidth = 33;  
    const int frameHeight = 26;
    const float moveSpeed = 20.0f;
    int current_frame;
    Texture textureR, textureL;  

public:
    BatBrain(float startX = 1000, float startY = 400) {

        //Simply initialising the variables
        timer.restart();
        x = startX;
        y = startY;
        hp = 3;
        isAlive = true;
        
        frameCounter = 0;
        current_frame = 0;
        if (!textureR.loadFromFile("Data/bat_right.png"))
            cout << "Failed to load bat_right.png\n";
        if (!textureL.loadFromFile("Data/batBrain.png"))
            cout << "Failed to load batBrain.png\n";
    }

    void loadTexture() override {
       
    }

    void loadTexture_runningR(int xFrame) override {
        sprite.setTexture(textureR);
        sprite.setTextureRect(IntRect(xFrame, 0, frameWidth, frameHeight));
        sprite.setScale(2.0f, 2.0f);
    }

    void loadTexture_runningL(int xFrame) override {
        sprite.setTexture(textureL);
        sprite.setTextureRect(IntRect(xFrame, 0, frameWidth, frameHeight));
        sprite.setScale(2.0f, 2.0f);
    }

    void move(int player_x, int player_y) override {
        if (!isAlive) return;  // Only moving if the enemy is alive

        if (timer.getElapsedTime().asSeconds() > 0.15f) {
            //Simply moving left and right  based upon the position of the enemy simply adding and subtracting the values in x
            if (x > player_x) {
                loadTexture_runningL(current_frame);
                x -= moveSpeed;
            }
            else if (x < player_x) {
                loadTexture_runningR(current_frame);
                x += moveSpeed;
            }

            //Showing animation of the movement
            current_frame += frameWidth;
            if (current_frame >= 132)  
                current_frame = 0;

            timer.restart();
            setPosition(x, y);
        }
    }

    

    int get_x() override {
        return int(x);
    }

    int get_y() override {
        return int(y);
    }
    bool checkCollision(Player& player) override {
        // Skip collision detection if the enemy is already dead
        if (!isAlive) return false;

        /* Simply checking the collision by just finding the 4 points of the player and the 4 points of the enemy and 
        checking are they overlapping if yes then it is the collision taking place */

        int playerLeft = (int)(player.get_x(player.get_currentIndex()) + player.get_hitBoxFactor_x());
        int playerRight = playerLeft + player.get_Pwidth();
        int playerTop = (int)(player.get_y(player.get_currentIndex()) + player.get_hitBoxFactor_y());  
        int playerBottom = playerTop + player.get_Pheight();

        int enemyLeft = int(x);
        int enemyRight = enemyLeft + int(frameWidth) * 2;
        int enemyTop = int(y); 
        int enemyBottom = enemyTop + frameHeight * 2;

        // Check if they are overlapping if yes then it is the collision and reducing the hp of the enemy
        if (playerRight > enemyLeft &&
            playerLeft < enemyRight &&
            playerBottom > enemyTop &&
            playerTop < enemyBottom) {
            cout << "Collision detected" << endl;
            hp--;
            if (hp <= 0)
                isAlive = false;  // Marking the enemy as dead
            return true;
        }
        return false;
    }
   

    
};


//Motobug also follow the player but on the ground
class Motobug : public Enemy
{
private:
    Clock timer;
    int frameCounter;
    const int frameWidth = 39;  
    const int frameHeight = 29;
    const float moveSpeed = 20.0f;
    int current_frame = 0;
    bool activated = false;

public:

    Motobug(float startX = 4000, float startY = 660) {

        // Simply initialising the variables
        timer.restart();
        x = startX;
        y = startY;
        frameCounter = 0;
        loadTexture();
        sprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));
        sprite.setScale(2.0f, 2.0f);
        setPosition(x, y);
        hp = 2;
        isAlive = false;
    }
    // Loading the sprites for moving left and right and also handling the animation of moving
    void loadTexture() override {
        if (!texture.loadFromFile("Data/motoBug.png")) {
            cout << "Failed to load enemy texture\n";
        }
        sprite.setTexture(texture);
    }

    void loadTexture_runningR(int xFrame) override {
        texture.loadFromFile("Data/bug_right.png");
        sprite.setTextureRect(IntRect(xFrame, 0, frameWidth, frameHeight));
        sprite.setScale(2.0f, 2.0f);
    }

    void loadTexture_runningL(int xFrame) override {
        texture.loadFromFile("Data/motoBug.png");
        sprite.setTextureRect(IntRect(xFrame, 0, frameWidth, frameHeight));
        sprite.setScale(2.0f, 2.0f);
    }

    void move(int player_x, int player_y) override {

        //Activating when player exceeds 4000 in x direction
        if (player_x >= 4000 && !activated) {
            isAlive = true;
            activated = true;
        }
       

        //Only moving when alive
        if (!isAlive) return;

        //Moving in left and right direction simply adding x if moving right and if moving left vice versa
        if (timer.getElapsedTime().asSeconds() > 0.2f) {
            if (x > player_x && x > 4000) {
                loadTexture_runningL(current_frame);
                x -= moveSpeed;
            }
            else if (x < player_x && x < 10000) {
                loadTexture_runningR(current_frame);
                x += moveSpeed;
            }

            //Showing animation of moving
            current_frame += frameWidth;
            if (current_frame >= 3 * frameWidth) {
                current_frame = 0;
            }

            timer.restart();
            setPosition(x, y);
        }
    }


    int get_x() override {
        return int(x);
    }

    int get_y() override {
        return int(y);
    }

    bool checkCollision(Player& player) override {
        // Checking the collision only if the enemy is active
        if (!isAlive) return false;


        //Calculating the hitbox of the player bottom , top , left and right
        int playerLeft = (int)(player.get_x(player.get_currentIndex()) + player.get_hitBoxFactor_x());
        int playerRight = playerLeft + player.get_Pwidth();
        int playerTop = (int)(player.get_y(player.get_currentIndex()) + player.get_hitBoxFactor_y());
        int playerBottom = playerTop + player.get_Pheight();

        //Calculating the position of enemy (hitboxes)
        int enemyLeft = int(x); // Current position
        int enemyRight = enemyLeft + int(frameWidth); // Current position + frames width which gives us right point
        int enemyTop = int(y); // Current y position
        int enemyBottom = enemyTop + frameHeight; // Current y + height of frame which gives us the bottom 

        // Checking if the player and enemy are overlapping so collision can take place
        if (playerRight > enemyLeft &&
            playerLeft < enemyRight &&
            playerBottom > enemyTop &&
            playerTop < enemyBottom) {
            cout << "Collision detected" << endl;
            hp--; // If collision decreasing the hp
            if (hp <= 0)
                isAlive = false;  // Deactivating the enemy
            return true;
        }
        return false;
    }



};

class beeBot : public Enemy {
private:
    Clock timer, fireClock;
    int frameCounter;
    const int frameWidth = 46;
    const int frameHeight = 24;
    const float moveSpeed = 20.0f;
    const float verticalSpeed = 15.0f; 
    int current_frame = 0;
    Texture textureR, textureL;
    bool movingRight = true;
    bool movingDown = true; 
    float originalY; 
    const float verticalRange = 100.0f; 
    bullet enemyBullet;
    Texture bullet_tex;

public:
    beeBot(float startX = 1000, float startY = 200) {
        //Initialising the variables

        timer.restart();
        fireClock.restart(); 
        x = startX;
        y = startY;
        originalY = startY; 
        frameCounter = current_frame = 0;

        textureR.loadFromFile("Data/bee_bot.png");
        textureL.loadFromFile("Data/bee_bot.png");

        sprite.setTexture(textureL);
        sprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));
        sprite.setScale(2.0f, 2.0f);
        setPosition(x, y);


        if (!bullet_tex.loadFromFile("Data/bullet_2.png")) {
            cout << "Failed to load bullet texture" << endl;
        }
        enemyBullet.initialize(bullet_tex);

        hp = 5;
        isAlive = true;
    }

    void loadTexture_runningR(int yFrame) override {
        sprite.setTexture(textureR);
        sprite.setTextureRect(IntRect(0, yFrame, frameWidth, frameHeight));
        sprite.setScale(2.0f, 2.0f);
    }

    void loadTexture() override {};

    void loadTexture_runningL(int yFrame) override {
        sprite.setTexture(textureL);
        sprite.setTextureRect(IntRect(0, yFrame, frameWidth, frameHeight));
        sprite.setScale(2.0f, 2.0f);
    }

    void move(int player_x, int player_y) override {
        /* It is simply movement logic first is that moving left and right is controlled by simple bool variable of movingright . If it is moving 
        right means less than the range of section we add the speed and if more than we change the direction backward and reduce the x value . Now 
        about verticle movement we use the movingDown variable and we move the bee down until the range has been reached and then we change the 
        direction to upward where the range is exceeded. Now when they both are acting togather they are fowming zig zag movement */
       // if (player_x >= 1000 && !isAlive) isAlive = true;
        //if (player_x >= 5000) isAlive = false;

        if (!isAlive) return;

        // Always move bullet if active
        enemyBullet.move();

        // Firing a bullet every 5 seconds and this is done by calling the fire function of the bullet class
        if (fireClock.getElapsedTime().asSeconds() > 5.0f) {
            // Calculate the center bottom of the bee for bullet spawn point
            float bulletStartX = x + (frameWidth) / 2;
            float bulletStartY = y + (frameHeight);
            enemyBullet.fire(bulletStartX, bulletStartY, false);
            fireClock.restart();
        }

        if (timer.getElapsedTime().asSeconds() > 0.3f) {
            // Horizontal movement simply moving left and right 
            if (movingRight) {
                loadTexture_runningR(current_frame);
                x += moveSpeed;
                if (x >= 5000) movingRight = false;
            }
            else {
                loadTexture_runningL(current_frame);
                x -= moveSpeed;
                if (x <= 1000) movingRight = true;
            }

            // Vertical zig-zag movement
            if (movingDown) {
                y += verticalSpeed;
                if (y >= originalY + verticalRange) movingDown = false;
            }
            else {
                y -= verticalSpeed;
                if (y <= originalY - verticalRange) movingDown = true;
            }

            //Handling the animation of the movement
            current_frame += frameHeight;
            if (current_frame > 110) current_frame = 0;

            timer.restart();
            setPosition(x, y);
        }
    }

    bullet& get_bullet() {
        return enemyBullet;
    }

    int get_x() override { return int(x); }
    int get_y() override { return int(y); }

    void set_x(float newX) {
        x = newX;
        setPosition(x, y);
    }

    bool checkCollision(Player& player) override {
        if (!isAlive) return false;

        /* Checking the collision with bullet and as well as the enemy sprite itself both uses the same logic as used previously 
        we calculates the four corners of the hitboxes for both then we check for overlapping if yes then collision take place
        and if no then no collision takes place and if the player collides with either the enemy itself or the bullet 
        it loses hp */

        if (enemyBullet.isActive()) {
            int bulletLeft = enemyBullet.getLeft();
            int bulletRight = enemyBullet.getRight();
            int bulletTop = enemyBullet.getTop();
            int bulletBottom = enemyBullet.getBottom();

            int pIndex = player.get_currentIndex();
            int playerLeft = (int)(player.get_x(pIndex) + player.get_hitBoxFactor_x());
            int playerRight = playerLeft + player.get_Pwidth();
            int playerTop = (int)(player.get_y(pIndex) + player.get_hitBoxFactor_y());
            int playerBottom = playerTop + player.get_Pheight();

            if (playerRight > bulletLeft &&
                playerLeft < bulletRight &&
                playerBottom > bulletTop &&
                playerTop < bulletBottom) {
                enemyBullet.deactivate();
                return true;
            }
        }

        // Bee collision same as the above logic
        int playerLeft = (int)(player.get_x(player.get_currentIndex()) + player.get_hitBoxFactor_x());
        int playerRight = playerLeft + player.get_Pwidth();
        int playerTop = (int)(player.get_y(player.get_currentIndex()) + player.get_hitBoxFactor_y());
        int playerBottom = playerTop + player.get_Pheight();

        int beeLeft = (int)x;
        int beeRight = beeLeft + frameWidth * 2; 
        int beeTop = (int)y;
        int beeBottom = beeTop + frameHeight * 2; 

        if (playerRight > beeLeft &&
            playerLeft < beeRight &&
            playerBottom > beeTop &&
            playerTop < beeBottom) {
            hp--;
            if (hp < 1) isAlive = false;
            return true;
        }

        return false;
    }

    void draw(RenderWindow& window) {
        if (isAlive) {
            window.draw(sprite);
            enemyBullet.draw(window);
        }
    }

    void render(RenderWindow& window) override {
        if (isAlive) {
            sprite.setPosition(x, y);
            window.draw(sprite);
            enemyBullet.draw(window);
        }
    }
};

class crabMeat : public Enemy {
private:
    Clock timer, fireClock;
    int frameCounter;
    const int frameWidth = 40;
    const int frameHeight = 29;
    const float moveSpeed = 20.0f;
    int current_frame = 0;
    Texture textureR, textureL;
    bool movingRight = true;
    bullet enemyBullet;
    Texture bullet_tex;

public:
    crabMeat(float startX = 7000, float startY = 640) {
        //Simple initialising the variables

        timer.restart();
        fireClock.restart();
        x = startX;
        y = startY;
        frameCounter = current_frame = 0;

        textureR.loadFromFile("Data/crabmeat.png");
        textureL.loadFromFile("Data/crabmeat.png");

        sprite.setTexture(textureL);
        sprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));
        sprite.setScale(2.0f, 2.0f);
        setPosition(x, y);
        if (!bullet_tex.loadFromFile("Data/bullet_2.png")) {
            cout << "Failed to load bullet texture" << endl;
        }
        enemyBullet.initialize(bullet_tex);

        hp = 4;
        isAlive = false;
    }

    void loadTexture_runningR(int xFrame) override {
        sprite.setTexture(textureR);
        sprite.setTextureRect(IntRect(xFrame , 0, frameWidth, frameHeight));
        sprite.setScale(2.0f, 2.0f);
    }

    void loadTexture() override {};

    void loadTexture_runningL(int xFrame) override {
        sprite.setTexture(textureL);
        sprite.setTextureRect(IntRect( xFrame, 0, frameWidth, frameHeight));
        sprite.setScale(2.0f, 2.0f);
    }

    void move(int player_x, int player_y) override {
        

        if (!isAlive) return;

        // Always move bullet if active
        enemyBullet.move();

        // Firing a bullet every 5 seconds and this is done by calling the fire function of the bullet class
        if (fireClock.getElapsedTime().asSeconds() > 5.0f) {
            // Calculate the center bottom of the crab for bullet spawn point
            float bulletStartX = x + (frameWidth) / 2;
            float bulletStartY = y + (frameHeight); // Bottom of crab
            enemyBullet.fire(bulletStartX, bulletStartY ,true);
            fireClock.restart();
        }

        //Just showing the animation of the bee
        if (timer.getElapsedTime().asSeconds() > 0.1f) {
            if (movingRight) {
                loadTexture_runningR(current_frame);
                x += moveSpeed;
                if (x >= 10000) movingRight = false;
            }
            else {
                loadTexture_runningL(current_frame);
                x -= moveSpeed;
                if (x <= 5000) movingRight = true;
            }

            current_frame += frameHeight;
            if (current_frame > 460) current_frame = 0;

            timer.restart();
            setPosition(x, y);
        }
    }

    bullet& get_bullet() {
        return enemyBullet;
    }

    int get_x() override { return int(x); }
    int get_y() override { return int(y); }

    void set_x(float newX) {
        x = newX;
        setPosition(x, y);
    }

    bool checkCollision(Player& player) override {
        if (!isAlive) return false;

        /* Checking the collision with bullet and as well as the enemy sprite itself both uses the same logic as used previously
        we calculates the four corners of the hitboxes for both then we check for overlapping if yes then collision take place
        and if no then no collision takes place and if the player collides with either the enemy itself or the bullet
        it loses hp */

        if (enemyBullet.isActive()) {
            int bulletLeft = enemyBullet.getLeft();
            int bulletRight = enemyBullet.getRight();
            int bulletTop = enemyBullet.getTop();
            int bulletBottom = enemyBullet.getBottom();

            int pIndex = player.get_currentIndex();
            int playerLeft = (int)(player.get_x(pIndex) + player.get_hitBoxFactor_x());
            int playerRight = playerLeft + player.get_Pwidth();
            int playerTop = (int)(player.get_y(pIndex) + player.get_hitBoxFactor_y());
            int playerBottom = playerTop + player.get_Pheight();

            if (playerRight > bulletLeft &&
                playerLeft < bulletRight &&
                playerBottom > bulletTop &&
                playerTop < bulletBottom) {
                enemyBullet.deactivate();
                return true;
            }
        }
        int playerLeft = (int)(player.get_x(player.get_currentIndex()) + player.get_hitBoxFactor_x());
        int playerRight = playerLeft + player.get_Pwidth();
        int playerTop = (int)(player.get_y(player.get_currentIndex()) + player.get_hitBoxFactor_y());
        int playerBottom = playerTop + player.get_Pheight();

        int crabLeft = (int)x;
        int crabRight = crabLeft + frameWidth * 2; 
        int crabTop = (int)y;
        int crabBottom = crabTop + frameHeight * 2;

        if (playerRight > crabLeft &&
            playerLeft < crabRight &&
            playerBottom > crabTop &&
            playerTop < crabBottom) {
            hp--;
            
            if (hp <= 0) isAlive = false;
            return true;
        }

        return false;
    }

    void draw(RenderWindow& window) {
        if (isAlive) {
            window.draw(sprite);
            enemyBullet.draw(window);
        }
    }

    void render(RenderWindow& window) override {
        if (isAlive) {
            sprite.setPosition(x, y);
            window.draw(sprite);
            enemyBullet.draw(window);
        }
    }
};




//Eggstinger for boss level
class EggStinger : public Enemy {
private:
    // Two timers for the horizontal movement and for checking if 10 seconds exceeded so boss can move down
    Clock moving_timer;
    Clock droping_timer;
    int frameCounter;
    const int frameWidth = 70;  
    const int frameHeight = 67;
    const float moveSpeed = 20.0f;
    int current_frame;
    Texture textureR, textureL; 
    bool movingRight = true;

    //Checking is boss moving up , down or horizontal
    bool isDropping = false;
    bool isLifting = false;    
    char** lvl; // So boss knows where the w means wall is present
    int cell_size = 64;    

public:
    EggStinger(char** grid, float startX = 500, float startY = 200) {

        //Initialising the variables 

        moving_timer.restart();
        x = startX;
        y = startY;
        hp = 5;
        isAlive = true;
        frameCounter = 0;
        current_frame = 0;

        // Load both textures once
        if (!textureR.loadFromFile("Data/egg.png"))
            cout << "Failed to load bat_right.png\n";
        if (!textureL.loadFromFile("Data/egg_left.png"))
            cout << "Failed to load batBrain.png\n";

        lvl = grid;
        droping_timer.restart();  
    }

    void loadTexture() override {
        
    }

    // Loading the sprites for moving left and right and also handling the animation of moving
    void loadTexture_runningR(int xFrame) override {
        sprite.setTexture(textureR);
        sprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));
        sprite.setScale(1.5f, 1.5f);
    }

    void loadTexture_runningL(int xFrame) override {
        sprite.setTexture(textureL);
        sprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));
        sprite.setScale(1.5f, 1.5f);
    }

    void move(int player_x, int player_y) override {
        if (!isAlive) return;

        float moving_time = moving_timer.getElapsedTime().asSeconds(); 
        float droping_time = droping_timer.getElapsedTime().asSeconds();

        // Moving horizontally if the enemy is not moving down neither moving up
        if (!isDropping && !isLifting) {

            y = 100; // Roaming aroung y = 100

            // Horizontal movement between x=50 and x=1000 when the time is less then 10 seconds
            if (moving_time > 0.05f) {
                if (movingRight) {
                    x += moveSpeed;
                    loadTexture_runningR(67);
                    if (x >= 1000) {
                        movingRight = false;
                    }
                }
                else {
                    x -= moveSpeed;
                    loadTexture_runningL(67);
                    if (x <= 50) {
                        movingRight = true;
                    }
                }

                // Setting the position of the enemy accordingly 
                setPosition(x, y);
                moving_timer.restart();

            }

            // Checking if the time has exceeded 10 seconds so moving the enemy above the player and then droping
            if (droping_time >= 10.0f) {
                isDropping = true;
                // Move directly above player's x position
                x = player_x;

            }
        }
        // Now handling the droping logic
        else if (isDropping) {
            // Moving the player downwards in the direction of the player location was found
            if (moving_time > 0.05f) {
                y += moveSpeed;
                setPosition(x, y);
                loadTexture_runningR(67);

                moving_timer.restart();
            }

            //Calculating the location of the enemy has reached the location of the wall that is 640 as it is on 10 row and (64 * 10) = 640
            int cell_x = x / cell_size;
            int cell_y = y / cell_size;

            // Checking if the enemy has collided with wall or has it reached 640 in the y direction if yes then moving up
            if ((cell_y < 16 && cell_x < 16 && lvl[cell_y][cell_x] == 'w') || y >= 640) {
                isDropping = false;
                isLifting = true;
                cout << "Found wall at position: " << cell_x << ", " << cell_y << endl;
            }
        }
        // Now coming back up after colliding with the wall 
        else if (isLifting) {
            if (moving_time > 0.05f) {
                // Moving upward 
                y -= moveSpeed;

                loadTexture_runningR(67);

                // Calculating the position below the enemy so that we can show the animation of the box moving upward
                int cell_x = x / cell_size;
                int cell_y = (y + frameHeight) / cell_size;

                // Mark the position as a wall (carrying it upward)
                if (cell_y < 16 && cell_x < 16) {
                   //Now creating the wall texture at current position and removing it from the old position
                    lvl[cell_y][cell_x] = 'w';

                    // Removing from the old location
                    if (cell_y + 1 < 16) {
                        lvl[cell_y + 1][cell_x] = ' ';
                    }
                }

                setPosition(x, y);
                moving_timer.restart();

                // Now at last checking if we have reached the top so that the wall can be destroyed and enemy come back to horizontal movement logic
                if (y <= 100) {
                    y = 100;
                    setPosition(x, y);

                    // Atlast destroying the block
                    cell_x  = x / cell_size;
                    cell_y = (y + frameHeight) / cell_size;
                    lvl[cell_y][cell_x] = ' ';
                    

                    // Now setting is lifting to false too 
                    isLifting = false;
                    droping_timer.restart();  
                    cout << "Returned to patrol, destroyed wall" << endl;
                }
            }
        }
    }

    // Getters not used any where for now
    int get_x() override {
        return int(x);
    }

    int get_y() override {
        return int(y);
    }

    bool checkCollision(Player& player) override {
        // Checking the collision only if the enemy is active
        if (!isAlive) return false;


        //Calculating the hitbox of the player bottom , top , left and right
        int playerLeft = (int)(player.get_x(player.get_currentIndex()) + player.get_hitBoxFactor_x());
        int playerRight = playerLeft + player.get_Pwidth();
        int playerTop = (int)(player.get_y(player.get_currentIndex()) + player.get_hitBoxFactor_y());
        int playerBottom = playerTop + player.get_Pheight();

        //Calculating the position of enemy (hitboxes)
        int enemyLeft = int(x); 
        int enemyRight = enemyLeft + int(frameWidth); 
        int enemyTop = int(y);
        int enemyBottom = enemyTop + frameHeight;

        // Checking if the player and enemy are overlapping so collision can take place
        if (playerRight > enemyLeft &&
            playerLeft < enemyRight &&
            playerBottom > enemyTop &&
            playerTop < enemyBottom) {
            cout << "Collision detected" << endl;
            hp--; // If collision decreasing the hp
            cout << "Boss hp: " << hp;
            if (hp <= 0)
                isAlive = false;  // Deactivating the enemy
            
            return true;
            
        }
        return false;
    }
};






class EnemyManager {
private:
    Enemy* enemies[5]; 
    bool collision_enemy = false;
    int num_enemies;
    Clock collision_timer;
    SoundBuffer destroy_buffer;
    Sound destroy_sound;
public:
    EnemyManager() { // Initialising all other enemies (composed inside it)
        enemies[0] = new BatBrain();
        enemies[1] = new Motobug();
        enemies[2] = new beeBot();
        enemies[3] = new crabMeat();

        destroy_buffer.loadFromFile("Data/Global/Destroy.wav");
        destroy_sound.setBuffer(destroy_buffer);
    }

    void set_numEnemies(int x) // As there is 1 enemy in last level and other has 4 enemies
    {
        num_enemies = x;
    }
    int get_numEnemies()
    {
        return num_enemies;
    }
    void initialise_boss(char** lvl)
    {
        enemies[4] = new EggStinger(lvl);
    }
    void move(int playerX, int playerY) { // Moving other players except the boss as boss comes only in last level
        
        for (int i = 0; i < 4; ++i) {
            enemies[i]->move(playerX, playerY); 
        }

       
    }
    Enemy& get_enemy(int index)
    {
        return *(enemies[index]);
    }

    void move_boss(int playerX, int playerY)
    {
        enemies[4]->move(playerX, playerY); // Moving boss
    }

    void renderAll(RenderWindow& window) { // Rendering all of the enemies
        for (int i = 0; i < num_enemies; ++i) {
            enemies[i]->render(window);
        }
    }

    bool checkCollisions(Player& player) {
        // Checking for the collision if the player is not invincible
        if (!collision_enemy) {
            for (int i = 0; i < num_enemies; ++i) {
                // Checking if any enemy has collided with the player
                if (enemies[i]->checkCollision(player)) {
                   
                    collision_enemy = true;  //If collision taking place then setting it to true so player become invincible

                    if (player.get_isBall() == false && player.get_isJumping() == false)
                    {
                        //Only subtracting  while not in jumping form
                        player.set_hp(player.get_hp() - 1);
                        cout << "HP reducedddddd" << endl;
                        player.get_HurtSound().play();
                    }
                    else
                        destroy_sound.play();

                    /*Below 4 lines are just to show the animation of the collision its proper processing will be explained in
                     player class in gravity function where the idea was orignally generated from */
                    player.set_velocityY(0);
                    player.set_onGround(false);
                    player.teleportCurrentCharacter(player.get_x(player.get_currentIndex()) + 120, 3 * 64);
                    collision_timer.restart();  // Starting invincible timer
                    break;  // Exiting the loop after first collision is detected
                }
            }
        }
        else {
            // Checking if player has been invincible for 10 seconds if yes then making it visible
            if (collision_timer.getElapsedTime().asSeconds() > 1) {
                collision_enemy = false;  // Reset collision state after cooldown
            }
        }

        return collision_enemy;  // Returning true if collision has taken place
    }


    bool isAlive(int index)
    {
        cout << "Boss dies " << endl;
        return (enemies[index]->get_isAlive());
        
    }

    
    void destroy_Vol(float vol)
    {
        destroy_sound.setVolume(vol);
    }


    void deactivate_enemy() 
    {
        enemies[0]->deactivate_enemy();
        enemies[1]->deactivate_enemy();
        enemies[2]->deactivate_enemy();
        enemies[3]->deactivate_enemy();
    }


};
