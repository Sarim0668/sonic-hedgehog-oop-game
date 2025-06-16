#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

class Collectibles
{
protected:
    Sprite sprite;
    Texture texture;
    int x[50];
    int y[50];
    bool isActive[50];
    int maxItems;
    int cell_size;
    char** lvl;
    bool occupied[14][300];

public:
    Collectibles(char** l, int maxCount)
    {
        lvl = l;
        maxItems = maxCount;
        cell_size = 64;

        for (int i = 0; i < maxItems; i++)
        {
            x[i] = 0;
            y[i] = 0;
            isActive[i] = false;
        }

        for (int i = 0; i < 14; i++)
            for (int j = 0; j < 300; j++)
                occupied[i][j] = false;
    }

    virtual void loadTexture() = 0;
    virtual void generate(int width) = 0;
    virtual void draw(RenderWindow& window) = 0;

    void getSpriteCellSize(int& cellsWide, int& cellsHigh)
    {
        int spriteWidth = texture.getSize().x;
        int spriteHeight = texture.getSize().y;

        float scaleX = sprite.getScale().x;
        float scaleY = sprite.getScale().y;
        // Calculate how many grid cells the sprite covers
        cellsWide = (int)((spriteWidth * scaleX) / cell_size);
        cellsHigh = (int)((spriteHeight * scaleY) / cell_size);

        if (cellsWide < 1)
        {
            cellsWide = 1;
        }
        if (cellsHigh < 1)
        {
            cellsHigh = 1;
        }
    }
    /*this function will check if in the particular position is no element present means if it is free or not*/
    bool isAreaFree(int row, int col, int cellsWide, int cellsHigh, int width = 1)
    {
        int gridW = width;

        if (row < 0 || col < 0 || row + cellsHigh > 14 || col + cellsWide > gridW)
            return false;

        for (int i = 0; i < maxItems; i++)
        {
            if (isActive[i])
            {
                if (x[i] == col && y[i] == row)
                {
                    return false;
                }
            }
        }

        for (int i = 0; i < cellsHigh; i++)
        {
            for (int j = 0; j < cellsWide; j++)
            {
                int r = row + i;
                int c = col + j;

                // bounds checking
                if (r < 0 || r >= 14 || c < 0 || c >= gridW)
                    return false;

                if (occupied[r][c] || lvl[r][c] != '\0')
                {
                    return false;
                }
            }
        }

        return true;
    }
    /*this function will mark area as occupied or not*/
    void markArea(int row, int col, int cellsWide, int cellsHigh)
    {
        for (int i = 0; i < cellsHigh; i++)
        {
            for (int j = 0; j < cellsWide; j++)
            {
                occupied[row + i][col + j] = true;
            }
        }
    }

    int get_maxItems()
    {
        return maxItems;
    }

    bool get_isActive(int index)
    {
        return isActive[index];
    }

    int get_x(int index)
    {
        return x[index];
    }
    int get_y(int index)
    {
        return y[index];
    }
    void set_x(int index, int num)
    {
        x[index] = num;
    }
    void set_y(int index, int num)
    {
        y[index] = num;
    }

    Sprite& get_sprite()
    {
        return sprite;
    }
};

class Ring : public Collectibles
{
public:
    Ring(char** l) : Collectibles(l, 30) {}

    void loadTexture() override
    {
        texture.loadFromFile("Data/ring.png");
        sprite.setTexture(texture);
        sprite.setScale(1.0, 1.0);
    }

    void generate(int w) override
    {
        int ringCount = 0;
        int width = w;
        srand(static_cast<unsigned>(time(0)));
        int cellsWide, cellsHigh;
        getSpriteCellSize(cellsWide, cellsHigh);

        while (ringCount < maxItems)
        {
            int row = rand() % (9 - 4 + 1) + 4;
            // Restrict columns to middle
            int minCol = width * 0.2;
            int maxCol = width * 0.8;
            int col = minCol + rand() % ((maxCol - minCol) - cellsWide);

            if (isAreaFree(row, col, cellsWide, cellsHigh, width))
            {
                markArea(row, col, cellsWide, cellsHigh);
                x[ringCount] = col;
                y[ringCount] = row;
                isActive[ringCount] = true;
                lvl[row][col] = 'r';
                ringCount++;
            }
        }
    }

    void draw(RenderWindow& window) override
    {
        for (int i = 0; i < maxItems; i++)
        {
            // Only draw if active AND the corresponding grid cell still has 'r'
            if (isActive[i])
            {
                /* Check if the ring was collected means in collision in player i am doing that if the collision happens set grid[x][y] = '/0' so i am not spawning it
                   there and doing the same thing for others*/
                if (lvl[y[i]][x[i]] != 'r')
                {
                    isActive[i] = false;
                    continue;
                }

                sprite.setPosition(x[i] * cell_size, y[i] * cell_size);
                window.draw(sprite);
            }
        }
    }
};

class ExtraLife : public Collectibles
{
public:
    ExtraLife(char** l) : Collectibles(l, 5) {}

    void loadTexture() override
    {
        texture.loadFromFile("Data/extra_life.png");
        sprite.setTexture(texture);
        sprite.setScale(0.1, 0.1);
    }

    void generate(int w) override
    {
        int lifeCount = 0;
        int width = w;
        srand(static_cast<unsigned>(time(0)) + 100);

        int cellsWide, cellsHigh;
        getSpriteCellSize(cellsWide, cellsHigh);

        while (lifeCount < maxItems)
        {
            int row = rand() % (9 - 4 + 1) + 4;
            // Restrict columns to middle 
            int minCol = width * 0.2;
            int maxCol = width * 0.8;
            int col = minCol + rand() % ((maxCol - minCol) - cellsWide);

            if (isAreaFree(row, col, cellsWide, cellsHigh, width))
            {
                markArea(row, col, cellsWide, cellsHigh);
                x[lifeCount] = col;
                y[lifeCount] = row;
                isActive[lifeCount] = true;
                lvl[row][col] = 'e';
                lifeCount++;
            }
        }
    }

    void draw(RenderWindow& window) override
    {
        for (int i = 0; i < maxItems; i++)
        {
            if (isActive[i])
            {
                sprite.setPosition(x[i] * cell_size, y[i] * cell_size);
                window.draw(sprite);

                if (lvl[y[i]][x[i]] != 'e')
                {
                    isActive[i] = false;
                    continue;
                }
            }
        }
    }
};

class SpecialBoost : public Collectibles
{
public:
    SpecialBoost(char** l) : Collectibles(l, 7) {}

    void loadTexture() override
    {
        texture.loadFromFile("Data/boost.png");
        sprite.setTexture(texture);
        sprite.setScale(0.3, 0.3);
    }

    void generate(int w) override
    {
        int boostCount = 0;
        int width = w;
        srand(static_cast<unsigned>(time(0)) + 200);

        int cellsWide, cellsHigh;
        getSpriteCellSize(cellsWide, cellsHigh);

        while (boostCount < maxItems)
        {
            int row = rand() % (9 - 4 + 1) + 4;
            // Restrict columns to middle
            int minCol = width * 0.2;
            int maxCol = width * 0.8;
            int col = minCol + rand() % ((maxCol - minCol) - cellsWide);
            if (isAreaFree(row, col, cellsWide, cellsHigh, width))
            {
                markArea(row, col, cellsWide, cellsHigh);
                x[boostCount] = col;
                y[boostCount] = row;
                isActive[boostCount] = true;
                lvl[row][col] = 'b';
                boostCount++;
            }
        }
    }

    void draw(RenderWindow& window) override
    {
        for (int i = 0; i < maxItems; i++)
        {
            if (isActive[i])
            {
                // will only draw if already there is no collectible there
                if (lvl[y[i]][x[i]] != 'b')
                {
                    isActive[i] = false;
                    continue;
                }
                sprite.setPosition(x[i] * cell_size, y[i] * cell_size);
                window.draw(sprite);
            }
        }
    }
};

class CollectibleManager
{
private:
    Collectibles* collectibles[3];
    bool initialized;

public:
    CollectibleManager() : initialized(false)
    {
        collectibles[0] = nullptr;
        collectibles[1] = nullptr;
        collectibles[2] = nullptr;
    }

    ~CollectibleManager()
    {
        for (int i = 0; i < 3; i++)
        {
            if (collectibles[i] != nullptr)
            {
                delete collectibles[i];
                collectibles[i] = nullptr;
            }
        }
    }

    Collectibles& get_collectible(int index)
    {
        return *(collectibles[index]);
    }

    void spawn_collectibles(char** chr)
    {
        for (int i = 0; i < 3; i++)
        {
            if (collectibles[i] != nullptr)
            {
                delete collectibles[i];
                collectibles[i] = nullptr;
            }
        }

        collectibles[0] = new Ring(chr);
        collectibles[1] = new ExtraLife(chr);
        collectibles[2] = new SpecialBoost(chr);

        for (int i = 0; i < 3; i++)
        {
            if (collectibles[i] != nullptr)
            {
                collectibles[i]->loadTexture();
            }
        }

        initialized = true;
    }

    void renderAll(int width)
    {
        if (!initialized)
        {
            return;
        }
        for (int i = 0; i < 3; i++)
        {
            if (collectibles[i] != nullptr)
            {
                collectibles[i]->generate(width);
            }
        }
    }

    void drawAll(RenderWindow& window)
    {
        if (!initialized)
        {
            return;
        }
        for (int i = 0; i < 3; i++)
        {
            if (collectibles[i] != nullptr)
            {
                collectibles[i]->draw(window);
            }
        }
    }
};