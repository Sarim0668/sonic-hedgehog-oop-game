#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include"Animation.h"

using namespace sf;
using namespace std;


class Character {
protected:
    float speed;
    Animation animation; // Composing animation as if the characters doent exists then animation of that character also doesnt exists
    int x, y;
    float max_speed;

public:
    Character() {
        speed = 10;
        x = 100;
        y = 100;
    }

    virtual ~Character() {}

    // Defining virtual functions as the character is an abstract class
    virtual void loadTexture() = 0;
    virtual void loadTexture_runningR(int x) = 0;
    virtual void loadTexture_runningL(int x) = 0;
    virtual void flying_texture(int x) = 0;
    virtual int getSpeed() = 0;
    virtual void jumping_texture(int x) = 0;

    virtual int get_x() { return x; }
    virtual void set_x(int num) { x = num; }
    virtual void set_y(int num) { y = num; }
    virtual int get_y() { return y; }
    virtual void set_maxSpeed(float value) { max_speed = value; };



    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
        animation.setPosition(x, y);// Setting the position of the sprite in the animation class as now it ishandling all the working
    }

    // Getting the sprite from the animation class to draw 
    Sprite& getSprite() {
        return animation.getSprite();
    }

    // Drawing the sprite
    void draw(RenderWindow& window) {
        window.draw(getSprite());
    }

    Animation& getAnimation() {
        return animation;
    }

};


// Now the three classes Sonic , Tails and Knuckles inherit from the parent class of Character
class Sonic : public Character {
public:
    Sonic() {
        speed = this->speed = 18;
    }

    //Basically loading the texture for the animation only by passing the locations of the textures and getting speed

    void loadTexture() override {
        animation.idle("Data/0left_still.png");
    }

    void loadTexture_runningR(int x) override {
        animation.runningRight(x, 400, "Data/0jog_right.png");
    }

    void loadTexture_runningL(int x) override {
        animation.runningLeft(x, 400, "Data/0jog_left.png");
    }

    void jumping_texture(int x) override {
        animation.jumping(x, 400, "Data/0upR.png");
    }



    int getSpeed() override {
        return speed;
    }

    void flying_texture(int x) override {

    }
};


class Tails : public Character {
private:

public:
    Tails() {
        speed = 10;
        animation.setFrameDimensions(48, 40);
    }

    void loadTexture() override {
        animation.idle("Data/tails.png");
    }

    void loadTexture_runningR(int x) override {
        animation.runningRight(x, 489, "Data/t.png");
    }

    void loadTexture_runningL(int x) override {
        animation.runningLeft(x, 489, "Data/tails_left.png");
    }

    int getSpeed() override {
        return speed;
    }

    void jumping_texture(int x) override {
        animation.jumping(x, 489, "Data/tails_jump.png");
    }

    void flying_texture(int x) override {
        animation.flying(x, 489, "Data/tails_fly.png");
    }
};


class Knuckles : public Character {
private:
    char** lvl;

public:
    Knuckles(char** arr = nullptr) {
        speed = 12;
        lvl = arr;
        animation.setFrameDimensions(48.5, 40);
    }

    void loadTexture() override {
        animation.idle("Data/knuckle.png");
    }


    void loadTexture_runningR(int x) override {
        animation.runningRight(x, 388, "Data/knuckles_right.png");
    }

    void loadTexture_runningL(int x) override {
        animation.runningLeft(x, 388, "Data/knuckles_left.png");
    }

    void jumping_texture(int x) override {
        animation.jumping(x, 388, "Data/knuckles_jump.png");
    }

    int getSpeed() override {
        return speed;
    }

    void flying_texture(int x) override {

    }
};



class Player
{
protected:
    Character* characters[3];
    int currentIndex = 0;
    char** lvl;
    int hp = 3;
    float x = 100, y = 100;
    float velocityY = 0;
    bool onGround = true;
    float gravity = 1.3;
    float terminal_Velocity = 20;
    float jumpStrength = -20;
    int screen_x = 1200;
    int screen_y = 900;

    float raw_img_x = 24;
    float raw_img_y = 35;
    float scale_x = 2.5;
    float scale_y = 2.5;
    int Pheight = raw_img_y * scale_y;
    int Pwidth = raw_img_x * scale_x;
    int hit_box_factor_x = 8 * scale_x;
    int hit_box_factor_y = 5 * scale_y;
    const int cell_size = 64;
    int score = 0, level_score = 0;
    Clock jump_clock;
    bool isJumping = false, is_ball = false, is_flying = false;
    float frame = 40;
    float current_frame = 40;
    float frame_width = 400;
    Clock clock, flyClock;
    int num = 0;
    bool is_alive = true;
    bool ability_activated = false;
    float current_speed = 5.0f;
    float max_speed[3] = { 18.0, 10.0, 12.0 };
    float boost_timer = 0.0f;
    bool boost_active = false;
    float boost_duration = 15.0; // Special ability for 15 seconds
    Clock boostClock;

    int level = 0;
    float fly_time = 7;
    Clock hitting_clock, flying_clock;
    bool boost_collected = false;
    bool is_invincible = false;
    float decelerationRate = 0.8;
    float acceleration = 0.2;

    //Sound for the different objects
    SoundBuffer jump_buffer, collectible_buffer, hurt_buffer, switch_buffer;
    Sound jump_sound, collectible_sound, hurt_sound, switch_sound;





public:

    Player() {

        //Initialising all the things 
        characters[0] = new Sonic();
        characters[1] = new Tails();
        characters[2] = new Knuckles(lvl);
        for (int i = 0; i < 3; i++) {
            characters[i]->loadTexture();
            characters[i]->set_x(x);
            characters[i]->set_y(y);
            characters[i]->setPosition(x, y);
        }

        jump_buffer.loadFromFile("Data/Global/jump.wav");
        jump_sound.setBuffer(jump_buffer);
        collectible_buffer.loadFromFile("Data/Global/Ring.wav");
        collectible_sound.setBuffer(collectible_buffer);
        hurt_buffer.loadFromFile("Data/Global/Hurt.wav");
        hurt_sound.setBuffer(hurt_buffer);
        switch_buffer.loadFromFile("Data/Global/Switch.wav");
        switch_sound.setBuffer(switch_buffer);
    }

    ~Player() {
        for (int i = 0; i < 3; i++) {
            delete characters[i];
        }
    }

    void switchCharacter() {
        /* Switching character logic same and basic like adding the current index each time the user presses z but
        if the counts exceeds 2 so thats why doing modulus with 3 so that even increases we get the number / index of
        the currently active character */
        currentIndex = (currentIndex + 1) % 3;

        // Setting the x, y, sprites and the frames of that character for smooth animation and movement
        characters[currentIndex]->set_x(x);
        characters[currentIndex]->set_y(y);
        characters[currentIndex]->setPosition(x, y);
        switch_sound.play();
    }

    bool check_alive() // Checking if the player is alive or not
    {
        if (hp < 1)
            return false;
        return true;
    }

    void move(bool is_right) {
        // Handling and calling the animation function just here for the animation
        if (!is_flying && !isJumping && !is_ball && !(ability_activated && currentIndex == 1)) {
            if (is_right) {
                characters[currentIndex]->loadTexture_runningR(characters[currentIndex]->getAnimation().getCurrentFrame());
            }
            else {
                characters[currentIndex]->loadTexture_runningL(characters[currentIndex]->getAnimation().getCurrentFrame());
            }
        }
        else if (is_flying && currentIndex == 1) {
            characters[currentIndex]->flying_texture(characters[currentIndex]->getAnimation().getCurrentFrame());
        }
        else {
            characters[currentIndex]->jumping_texture(characters[currentIndex]->getAnimation().getCurrentFrame());
        }
        //Updating the animation at every time as the update animation is based upon the clock 
        characters[currentIndex]->getAnimation().updateAnimation();



        /* Simply moving player based upon the accelerration the current speed is 0 and the speed is incrementing based upon the
       acceleration and direction like if moving left the speed is subtracting means magnitude is increasing but the negative sign
       means moving back in x direction as x is decreasing and same goes for the right direction we add x as , x is increasing towrds
       the right and limiting the speed to max speed of the characters if it exceeds it*/

        if (is_right) {
            current_speed += acceleration;
            if (current_speed > max_speed[currentIndex]) current_speed = max_speed[currentIndex];
        }
        else {
            current_speed -= acceleration;
            if (current_speed < -max_speed[currentIndex]) current_speed = -max_speed[currentIndex];
        }

        // Calculating the new position
        float newX = get_x(currentIndex) + current_speed;
        float newY = get_y(currentIndex);

        // Now detecting the collision with obstacles firstly calculating the left and right x values then finding top , bottom and mid y values than checking for collision
        int left_x = (int)(newX + hit_box_factor_x) / cell_size;
        int right_x = (int)(newX + hit_box_factor_x + Pwidth) / cell_size;

        int top_y = (int)(newY + hit_box_factor_y) / cell_size;
        int mid_y = (int)(newY + hit_box_factor_y + Pheight / 2) / cell_size;
        int bottom_y = (int)(newY + hit_box_factor_y + Pheight) / cell_size;

        // Finding the values store there to check if there is any obstacle or not
        char left_top = lvl[top_y][left_x];
        char left_mid = lvl[mid_y][left_x];
        char left_bottom = lvl[bottom_y][left_x];

        char right_top = lvl[top_y][right_x];
        char right_mid = lvl[mid_y][right_x];
        char right_bottom = lvl[bottom_y][right_x];

      
        if (current_speed < 0) {  //It means that if we are moving towards left
            if (left_mid == 'w' || left_mid == 's' || left_mid == 'k') {
                newX = (left_x + 1) * cell_size - hit_box_factor_x; // See above eq for left_x making x the subject
                current_speed = 0;
            }
        }
        if (current_speed > 0) { 
            if (right_mid == 'w' || right_mid == 's' || right_mid == 'k') {
                newX = right_x * cell_size - hit_box_factor_x - Pwidth;
                current_speed = 0;
            }

        }


        //Now using knuckles ability check for the collision with s walls and if present then breaking them using W

        if (Keyboard::isKeyPressed(Keyboard::W) && ability_activated && currentIndex == 2)
        {
            if (left_top == 's') {
                lvl[top_y][left_x] = '\0';
            }
            if (left_mid == 's') {
                lvl[mid_y][left_x] = '\0';
            }
            if (left_bottom == 's') {
                lvl[bottom_y][left_x] = '\0';
            }
            if (right_top == 's') {
                lvl[top_y][right_x] = '\0';
            }
            if (right_mid == 's') {
                lvl[mid_y][right_x] = '\0';
            }
            if (right_bottom == 's') {
                lvl[bottom_y][right_x] = '\0';
            }
        }



        /* Simply hitbox factor of player has already being calculated so now simply checking for the collision
        of player with right or left side of the rings and if there is then setting ring calculated as true so
        that score can be updated and then setting that location of array to /0 if the ring has been calculated so
        that it can become inactive */

        bool ringCollected = false;

       
        if (left_top == 'r') {
            lvl[top_y][left_x] = '\0';
            ringCollected = true;
        }
        if (left_mid == 'r') {
            lvl[mid_y][left_x] = '\0';
            ringCollected = true;
        }
        if (left_bottom == 'r') {
            lvl[bottom_y][left_x] = '\0';
            ringCollected = true;
        }
        if (right_top == 'r') {
            lvl[top_y][right_x] = '\0';
            ringCollected = true;
        }
        if (right_mid == 'r') {
            lvl[mid_y][right_x] = '\0';
            ringCollected = true;
        }
        if (right_bottom == 'r') {
            lvl[bottom_y][right_x] = '\0';
            ringCollected = true;
        }

        // If a ring was collected then increasing the score
        if (ringCollected) {
            collectible_sound.play();
            score += 10;
            level_score += 10;
            cout << "RIngs collected" << endl;
            cout << "Total Score: " << score << endl;
            cout << "Level Score: " << level_score << endl;

        }


        // Doing same as the rings just know increasing hp
        bool extraLifeCollected = false;

      
        if (left_top == 'e') {
            lvl[top_y][left_x] = '\0';
            extraLifeCollected = true;
        }
        if (left_mid == 'e') {
            lvl[mid_y][left_x] = '\0';
            extraLifeCollected = true;
        }
        if (left_bottom == 'e') {
            lvl[bottom_y][left_x] = '\0';
            extraLifeCollected = true;
        }
        if (right_top == 'e') {
            lvl[top_y][right_x] = '\0';
            extraLifeCollected = true;
        }
        if (right_mid == 'e') {
            lvl[mid_y][right_x] = '\0';
            extraLifeCollected = true;
        }
        if (right_bottom == 'e') {
            lvl[bottom_y][right_x] = '\0';
            extraLifeCollected = true;
        }

        // If an extra life was collected then increasing the hp
        if (extraLifeCollected) {
            hp++;
            collectible_sound.play();

        }


        //Same checking for the collision with boost too

        boost_collected = false;

        if (left_top == 'b') {
            lvl[top_y][left_x] = '\0';
            boost_collected = true;
        }
        if (left_mid == 'b') {
            lvl[mid_y][left_x] = '\0';
            boost_collected = true;
        }
        if (left_bottom == 'b') {
            lvl[bottom_y][left_x] = '\0';
            boost_collected = true;
        }
        if (right_top == 'b') {
            lvl[top_y][right_x] = '\0';
            boost_collected = true;
        }
        if (right_mid == 'b') {
            lvl[mid_y][right_x] = '\0';
            boost_collected = true;
        }
        if (right_bottom == 'b') {
            lvl[bottom_y][right_x] = '\0';
            boost_collected = true;
        }


        /* Now applying the different abilities for the different characters for 15 seconds like + 4 speed for sonic, +4 flying time and
         making knucles invincible
         */

        if (boost_collected) {
            collectible_sound.play();
            boostClock.restart();
            boost_active = true;
            if (currentIndex == 0) {
                current_speed += 4.0f;
                cout << "Sonic speed increased" << endl;
            }
            else if (currentIndex == 1) {
                fly_time += 4;
            }
            else if (currentIndex == 2) {
                is_invincible = true;
                cout << "Knuckleeee is INVINCIIIIIIIIIIIIIII" << endl;
            }

        }


        // Now checking if the 15 seconds has been exceeded so resetting the abilities back to normal

        if (boost_active)
        {
            if (boostClock.getElapsedTime().asSeconds() >= 15)
            {
                boost_active = false;
                if (currentIndex == 0)
                    current_speed -= 4;
                else if (currentIndex == 1)
                    fly_time = 7;
                else
                    is_invincible = false;
            }
        }


        // Atlast updating the positions of the player
        set_x(newX);
        x = get_x(currentIndex);

        if (!(ability_activated && currentIndex == 1)) {
            characters[currentIndex]->setPosition(x, y);  // Displaying the character until tails is flying as it was causing errors
        }
    }



    void applyFriction(bool is_right) {


        //Now applying the frition based upon direction means if negative than adding and else subtracting
        if (current_speed > 0) {
            // Checking it doesnt gets negative if it does limiting to 0
            if (current_speed > decelerationRate) {
                current_speed -= decelerationRate;
            }
            else {
                current_speed = 0;
            }
        }
        else if (current_speed < 0) {
            if (current_speed < -decelerationRate) {
                current_speed += decelerationRate;
            }
            else {
                current_speed = 0;
            }
        }

        //If current speed is not 0 means the friction will be applied so handling the friction and collisions
        if (current_speed != 0) {
            float newX = get_x(currentIndex) + current_speed;
            float newY = get_y(currentIndex);


            // Doing same collision checks as in the move function so that it doesnt overlaps obstacles

            int left_x = (int)(newX + hit_box_factor_x) / cell_size;
            int top_y = (int)(get_y(currentIndex) + hit_box_factor_y) / cell_size;
            int mid_y = (int)(get_y(currentIndex) + hit_box_factor_y + Pheight / 2) / cell_size;
            int bottom_y = (int)(get_y(currentIndex) + hit_box_factor_y + Pheight) / cell_size;
            int right_x = (int)(newX + hit_box_factor_x + Pwidth) / cell_size;
            // Checking collision based on movement direction
            if (current_speed < 0) { 
                char left_mid = lvl[mid_y][left_x];
                if (left_mid == 'w' || left_mid == 's' || left_mid == 'k') {
                    newX = (left_x + 1) * cell_size - hit_box_factor_x;
                    current_speed = 0;
                }
            }
            else if (current_speed > 0) { 
                char right_mid = lvl[mid_y][right_x];
                if (right_mid == 'w' || right_mid == 's' || right_mid == 'k') {
                    newX = right_x * cell_size - hit_box_factor_x - Pwidth;
                    current_speed = 0;
                }
            }
            // Updating the position
            set_x(newX);
            x = get_x(currentIndex);

            // Displaying the texture by using animation class
            if (!(ability_activated && currentIndex == 1)) {
                if (current_speed > 0) {
                    characters[currentIndex]->loadTexture_runningR(characters[currentIndex]->getAnimation().getCurrentFrame());
                }
                else if (current_speed < 0) {
                    characters[currentIndex]->loadTexture_runningR(characters[currentIndex]->getAnimation().getCurrentFrame());
                }
                characters[currentIndex]->setPosition(x, y);
            }
        }

    }

    void follow_chars(bool isJumping, bool is_right) {
        /* Gettng the indes of the inative character ans storing them in followers array than defining the gap between all of them through
        spacing variable then using static variables and other variables to store and defining the things that are essential for the logic 
        of following the character. follow_delays is the one that is used to add the delay between both of the chracter to follow because 
        otherwise both were following at the same time and was not lookin good and using the clocks to check for the delay time has been 
        exceeded so that there positions can be updated then using a started following array to store that if the not following then can
        be set to true to start following once delay time has been reached then using two float variables maximum and minimum spacing , max
        spacing store the max distance the follow characters can be away from the main character it is done because when the follow character
        gets stuck with the wall so if the main character kept on moving forward and the follow characters gets out of the screen so 
        telporting it the min distance away from the main character */

        int followers[2] = { (currentIndex + 1) % 3, (currentIndex + 2) % 3 };
        float spacing = 80.0f;
        static float follow_delays[2] = { 1.0f, 2.0f };
        static Clock delay_clocks[2];
        static bool started_following[2] = { false, false };
        static float current_positions[2] = { 0.0f, 0.0f };
        static bool positions_initialized[2] = { false, false };
        float speed = 5.0f;
        float max_distance = 750.0f;
        float min_spacing = 50.0f;


        
        //Handling movement of both of the character
        for (int i = 0; i < 2; i++) {

            // For both character checking if the delay time has been exceeded so that started following can be set to true
            int follower = followers[i];
            if (!started_following[i]) {
                if (delay_clocks[i].getElapsedTime().asSeconds() >= follow_delays[i]) {
                    started_following[i] = true;
                }
                else {
                    continue; 
                }
            }

            /* Calculating the position of the follower away from the main character based upon direction if moving right then the follower
            should be on the left and vice versa */

            float targetX = x;
            if (is_right) {
                targetX -= spacing * (i + 1); 
            }
            else {
                targetX += spacing * (i + 1); 
            }


            // Setting up the starting positions of the followers
            if (!positions_initialized[i]) {
                current_positions[i] = characters[follower]->get_x();
                positions_initialized[i] = true;
            }

            // Check if character is too far away from the main character then teleporting it min distance away from main character
            float currentDistance = abs(x - current_positions[i]);
            if (currentDistance > max_distance) {
                current_positions[i] = is_right ? (x - min_spacing) : (x + min_spacing);
            }
            else {
                /* Now basically simply moving the characters based upon there directions like if moving right
                then adding the speed until it reaches the target x not adding the difference directly as we dont
                need a teleportation but smooth transmission and also checking if the speed gets larger then target
                x while moving to right setting it to target x and same goes for moving left just subtracting*/
                float old_x = current_positions[i];
                if (current_positions[i] < targetX) {
                    current_positions[i] += speed;
                    if (current_positions[i] > targetX) {
                        current_positions[i] = targetX;
                    }
                }
                else if (current_positions[i] > targetX) {
                    current_positions[i] -= speed;
                    if (current_positions[i] < targetX) {
                        current_positions[i] = targetX;
                    }
                }

                /* Here is the same above used collision logic checking if the follow characters has collided 
                with the obstacles then stopping them there at the edge and if they gets stuck there and the main
                character has moved forward so they will automatically be teleported near them by the logic used
                above */
                float newX = current_positions[i];
                float newY = characters[follower]->get_y();


                int left_x = (int)(newX + hit_box_factor_x) / cell_size;
                int right_x = (int)(newX + hit_box_factor_x + Pwidth) / cell_size;
                int top_y = (int)(newY + hit_box_factor_y) / cell_size;
                int mid_y = (int)(newY + hit_box_factor_y + Pheight / 2) / cell_size;
                int bottom_y = (int)(newY + hit_box_factor_y + Pheight) / cell_size;
                char left_mid = lvl[mid_y][left_x];
                char right_mid = lvl[mid_y][right_x];
                float movement = newX - old_x;
                if (movement < 0) { 
                    if (left_mid == 'w' || left_mid == 's' || left_mid == 'k') {
                        current_positions[i] = (left_x + 1) * cell_size - hit_box_factor_x; 
                    }
                }
                if (movement > 0) { 
                    if (right_mid == 'w' || right_mid == 's' || right_mid == 'k') {
                        current_positions[i] = right_x * cell_size - hit_box_factor_x - Pwidth; 
                    }
                }
            }

           
            // At last handling the animation
            float jumping_frame = 45.0f;
            if (isJumping) {
                characters[follower]->jumping_texture(jumping_frame);
            }
            else {
                if (is_right) {
                    characters[follower]->loadTexture_runningR(characters[follower]->getAnimation().getCurrentFrame());
                }
                else {
                    characters[follower]->loadTexture_runningL(characters[follower]->getAnimation().getCurrentFrame());
                }
                characters[follower]->getAnimation().updateAnimation();
            }
            characters[follower]->set_x(current_positions[i]);
            characters[follower]->setPosition(current_positions[i], characters[follower]->get_y());
        }
    }



    void jump() { // Function to jump simply applies jump strength to velocityY which moves it upwards
        if (onGround) {
            velocityY = jumpStrength;
            onGround = false;
            isJumping = true;  // Set jumping flag to true
            jump_sound.play();

        }
    }


    // Apply gravity logic Mainly is same as the provided one just little ammendments have been done
    void applyGravity() {


        /* Handlig the flying logic of tails here so what is done is checking is that is flying is true and the current index is
        1 so making tails fly moving it upwrds by W and downwards by S and further applying gravity if nothing is applied */
        if (is_flying && currentIndex == 1)
        {
            if (flyClock.getElapsedTime().asSeconds() > 0.016f) {
                if (Keyboard::isKeyPressed(Keyboard::W)) {
                    velocityY -= 0.8f;
                    if (velocityY < -8.0f)
                        velocityY = -8.0f;
                }
                else if (Keyboard::isKeyPressed(Keyboard::S)) {
                    velocityY += 0.8f;
                    if (velocityY > 5.0f)
                        velocityY = 5.0f;
                }
                else {
                    velocityY *= 0.95f;
                }

                velocityY += 0.2f; // Applying gravity while flying too

                flyClock.restart();
            }
        }

        //Now the main code below is the one already provided in the structure code 

        // Apply current velocity to position
        float offset_y = y;
        offset_y += velocityY;

        // Boundary check for player falling off the screen
        if (offset_y + Pheight > screen_y) {
            offset_y = screen_y - Pheight;
            velocityY = 0;
            onGround = true;
            isJumping = false;
        }


        if (offset_y < 0) {
            offset_y = 0;
            velocityY = 0;
        }

        char bottom_left_down = lvl[(int)(offset_y + hit_box_factor_y + Pheight) / cell_size][(int)(x + hit_box_factor_x) / cell_size];
        char bottom_right_down = lvl[(int)(offset_y + hit_box_factor_y + Pheight) / cell_size][(int)(x + hit_box_factor_x + Pwidth) / cell_size];
        char bottom_mid_down = lvl[(int)(offset_y + hit_box_factor_y + Pheight) / cell_size][(int)(x + hit_box_factor_x + Pwidth / 2) / cell_size];

        /* Handle falling into spikes reducing health and showing the animation of the collision*/
        if (bottom_left_down == 'k' || bottom_mid_down == 'k' || bottom_right_down == 'k') {
            hurt_sound.play();
            hp--;
            set_x(get_x(currentIndex) + 120);
            x = get_x(currentIndex);
            y = 7 * 64;
            velocityY = 0;
            onGround = false;
            characters[currentIndex]->setPosition(x, y);
            characters[currentIndex]->set_y(y);
            return;
        }

        //Falling in bottomless pits leads instant death
        if (y > (11 * 64))
        {
            hp = 0;
        }

        // Ground collision check
        if (bottom_left_down == 'w' || bottom_mid_down == 'w' || bottom_right_down == 'w' ||
            bottom_left_down == 's' || bottom_mid_down == 's' || bottom_right_down == 's' ||
            bottom_left_down == 'p' || bottom_mid_down == 'p' || bottom_right_down == 'p') {

            //Basically my sprite was geeting inside the sprite of box so setting it just above the box
            int bottomTileY = ((int)(offset_y + hit_box_factor_y + Pheight)) / cell_size;
            int tileTop = bottomTileY * cell_size;
            y = tileTop - hit_box_factor_y - Pheight;
            onGround = true;
            isJumping = false;


        }
        else {
            y = offset_y;
            set_y(y);
            onGround = false;
        }

        // Apply gravity if not on ground and not actively flying upward
        if (!onGround) {
            if (!is_flying) {
                velocityY += gravity;
                if (velocityY >= terminal_Velocity)
                    velocityY = terminal_Velocity;
            }
            // When flying, the gravity is already applied in the flying controls section
        }
        else {
            velocityY = 0;
        }

        // Top collision check for the walls
        char top_left = lvl[(int)(y + hit_box_factor_y) / cell_size][(int)(x + hit_box_factor_x) / cell_size];
        char top_mid = lvl[(int)(y + hit_box_factor_y) / cell_size][(int)(x + hit_box_factor_x + Pwidth / 2) / cell_size];
        char top_right = lvl[(int)(y + hit_box_factor_y) / cell_size][(int)(x + hit_box_factor_x + Pwidth) / cell_size];

        //If moving up means velocity is negative then checking for top collision
        if (velocityY < 0) {
            if (top_left == 'w' || top_mid == 'w' || top_right == 'w' ||
                top_left == 's' || top_mid == 's' || top_right == 's' ||
                top_left == 'k' || top_mid == 'k' || top_right == 'k') {

                // Blocking the jump and repositioning just below the block
                int topTileY = ((int)(y + hit_box_factor_y)) / cell_size;
                int nextTileY = (topTileY + 1) * cell_size;
                y = nextTileY - hit_box_factor_y + 0.1f;

                velocityY = 0;
            }
        }

        // Updating the current character's position first
        characters[currentIndex]->set_y(y);
        characters[currentIndex]->setPosition(characters[currentIndex]->get_x(), y);

       
        for (int i = 0; i < 3; i++) {
            if (i == currentIndex) continue; 
            float currentX = characters[i]->get_x();
            characters[i]->set_y(y);
            characters[i]->setPosition(currentX, y);
        }
    }



    void setLevel(char** newLevel, int lev) { // To handle the changing of the levels
        lvl = newLevel; //Changing level by assigning the pointer to other level
        level = lev;
    }

    void useAbility() // So that the ability can be used through main by pressing F key
    {
        ability_activated = true;
        if (currentIndex == 1)
            is_flying = true;
        else if (currentIndex == 0)
        {
            max_speed[0] = 25;
        }
    }



    //Using is_ball and is_jumping so that when player and enemies collision take place the enemy gets the damage not player
    void set_isBall(bool value)
    {
        is_ball = value;
    }
    bool get_isBall()
    {
        return is_ball;
    }

    bool get_isJumping()
    {
        return isJumping;
    }








    //Setters and Getters
    bool get_invincible()
    {
        return is_invincible;
    }
    void set_invincible(bool value)
    {
        is_invincible = value;
    }
    void set_ability_activated(bool set)
    {
        ability_activated = set;
        is_flying = set;
        max_speed[0] = 18;
    }

    int get_hitBoxFactor_x()
    {
        return hit_box_factor_x;
    }
    int get_hitBoxFactor_y()
    {
        return hit_box_factor_y;
    }
    int get_Pheight()
    {
        return Pheight;
    }
    int get_Pwidth()
    {
        return Pwidth;
    }

    int get_hp()
    {
        return hp;
    }

    int get_currentIndex() {
        return currentIndex;
    }

    int get_x(int index) {
        return characters[index]->get_x();
    }

    void set_x(int num) {
        characters[currentIndex]->set_x(num);
    }
    int get_y(int index) {
        return characters[index]->get_y();
    }

    void set_y(int num) {
        characters[currentIndex]->set_y(num);
        y = num;
    }

    bool is_ground() {
        return onGround;
    }

    Sprite& getSpriteByIndex(int index) {
        return characters[index]->getSprite();
    }
    int get_totalScore()
    {
        return score;
    }

    float getSpeed() {
        return characters[currentIndex]->getSpeed();
    }

    void set_hp(int prev_hp)
    {
        hp = prev_hp;
    }

    void set_acceleration(float acc)
    {
        acceleration = acc;
    }
    void set_deceleration(float dec)
    {
        acceleration = dec;
    }
    void set_gravity(float value)
    {
        gravity = value;
    }
    void set_maxSpeed(float sonic_speed, float tails_speed, float knuckles_speed)
    {
        max_speed[0] = sonic_speed;
        max_speed[1] = tails_speed;
        max_speed[2] = knuckles_speed;
    }




    //Below these two function are defined to show the animation of collision between player and enemy
    void set_onGround(bool setter)
    {
        onGround = setter;
    }

    void set_velocityY(int num)
    {
        velocityY = num;
    }

    void teleportCurrentCharacter(int x, int y)  // To show the animtion of collision
    {
        set_x(x);
        set_y(y);
        characters[currentIndex]->setPosition(x, y);
        characters[currentIndex]->set_y(y);
    }

    float get_flyTime()
    {
        return fly_time;
    }

    bool get_boostActive()
    {
        if (currentIndex == 2 && boost_collected)
            cout << "KUCKLESSSSSSSSSSSSSSS" << endl;
        return boost_active;
    }


    //To maintain the score throughout different levels

    int get_levelScore()
    {
        return level_score;
    }
    int get_score()
    {
        return score;
    }
    void reset_levelScore()
    {
        level_score = 0;
    }
    void set_totalScore(int value)
    {
        score = value;
    }

    //To handle the sounds toggling

    Sound& get_HurtSound()
    {
        return hurt_sound;
    }

    void set_jumpVol(float vol)
    {
        jump_sound.setVolume(vol);
    }
    void set_collectibleVol(float vol)
    {
        collectible_sound.setVolume(vol);
    }
    void set_hurtVol(float vol)
    {
        hurt_sound.setVolume(vol);
    }
    void set_switch_charVol(float vol)
    {
        switch_sound.setVolume(vol);
    }
};