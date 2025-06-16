#pragma once
#include <SFML/Graphics.hpp>


using namespace sf;

/* So basically how the animation class is working is that previously i have done all the animations in the character class itself 
and was calling it in the player class but now i have shifted all the logic here and the main logic is that we are using the sprite
sheet and the intrect to display the part of the sprite sheet frame and after the specific time we are moving the sprite to be displayed
to the next frame and when the frame reaches it maximum size then we are resetting the frame back to 0 means start from all over again
so the main things or variables we need to handle these are that sprite variable which will be updated after the specific time and the 
other variables are just the current width , maximum width and the height*/

class Animation {
private:
    Sprite sprite;
    Texture texture;
    float scale_x = 2.5f;
    float scale_y = 2.5f;
    int frame_width = 40;
    int frame_height = 40;
    float current_frame = 0;
    float max_frame = 400;
    Clock animation_clock;
    float animation_speed = 0.02f; 

public:
    Animation() {
        // Setting the sprites to default size that is 2.5
        sprite.setScale(scale_x, scale_y);
    }

    // Seting the width and dimensions of 1 frame 
    void setFrameDimensions(float width, float height) {
        frame_width = width;
        frame_height = height;
    }

    // To enlarge or diminish the sprite size
    void setScale(float x, float y) {
        scale_x = x;
        scale_y = y;
        sprite.setScale(scale_x, scale_y);
    }

    // Generic load texture animation frame
    void loadAnimationFrame(string& texturePath, int currentFrame, int maxFrame, int frameWidth) {
        //Checking if cant load the texture due to error
        if (!texture.loadFromFile(texturePath)) {
            cout << "Error cant load the texture" << endl;
            return;
        }

        //Now assigning the frame width and maximum frame width to the class variable keep in mind every sprite has height of 40
        this->max_frame = maxFrame;
        this->frame_width = frameWidth;
        sprite.setTexture(texture, true);
        sprite.setTextureRect(IntRect(currentFrame, 0, frameWidth, frame_height));
    }

    /* It is the function that is using the clock to  display the annimation of the characters and after every specific time it move to 
    next frame of the sprite being displyed which leads to annimation being displayed*/
    void updateAnimation() {
        if (animation_clock.getElapsedTime().asSeconds() > animation_speed) {
            // Moving the current frame to next frame by adding the width of one frame
            current_frame += frame_width;
            if (current_frame >= max_frame) {
                current_frame = 0;
            }
            sprite.setTextureRect(IntRect(current_frame, 0, frame_width, frame_height));
            animation_clock.restart();
        }
    }

    //Setter and getter of current frame so that they can be called in the animation of idle , moving left , right and jumping functions
    float getCurrentFrame() {
        return current_frame;
    }
    void setCurrentFrame(float frame) {
        current_frame = frame;
        if (current_frame >= max_frame) {
            current_frame = 0;
        }
    }

    

    /* Now these are the different function for different proceses however every function is doing same thing means calling the load
    animation function but all have different paths for different sprites and some have different frame widths so thats why we have 
    created several different functions */

    void idle(string texturePath) {
        if (!texture.loadFromFile(texturePath)) {
            return;
        }
        sprite.setTexture(texture, true);
    }
    void runningRight(int currentFrame, int maxFrame, string texturePath) {
        loadAnimationFrame(texturePath, currentFrame, maxFrame, frame_width);
    }
    void runningLeft(int currentFrame, int maxFrame, string texturePath) {
        loadAnimationFrame(texturePath, currentFrame, maxFrame, frame_width);
    }
    void jumping(int currentFrame, int maxFrame, string texturePath) {
        loadAnimationFrame(texturePath, currentFrame, maxFrame, frame_width);
    }
    void flying(int currentFrame, int maxFrame, string texturePath) {
        loadAnimationFrame(texturePath, currentFrame, maxFrame, frame_width);
    }
    void setPosition(float x, float y) {
        sprite.setPosition(x, y);
    }
    Sprite& getSprite() {
        return sprite;
    }
    float get_width()
    {
        return frame_width;
    }
    float get_height()
    {
        return frame_height;
    }
    void set_animationSpeed(float speed)
    {
        animation_speed = speed;
    }
};