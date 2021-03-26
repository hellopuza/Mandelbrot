/*------------------------------------------------------------------------------
    * Description: Program to display mandelbrot set                           *
    * Created:     1 mar 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#include <SFML/Graphics.hpp>
#include <assert.h>
#include <stdio.h>

//#define FULLSCREENMODE

struct screen
{
    int    x1   = -1;
    int    x2   = -1;
    int    y1   = -1;
    int    y2   = -1;
    double zoom =  0;
};

struct cmplxborder
{
    double Re_left  = 0;
    double Re_right = 0;
    double Im_up    = 0;
    double Im_down  = 0;
};

screen GetNewScreen   (sf::RenderWindow& window, sf::Sprite sprite, sf::Vector2i winsizes);
void   DrawMandelbrot (sf::VertexArray& pointmap, cmplxborder border, sf::Vector2i winsizes, int itrn, double lim);
void   changeBorders  (cmplxborder* border, screen newscreen, sf::Vector2i winsizes);

const double null = 1e-7;

//------------------------------------------------------------------------------

int main()
{
    sf::Vector2i winsizes;

#ifdef FULLSCREENMODE

    winsizes.x = sf::VideoMode::getDesktopMode().width;
    winsizes.y = sf::VideoMode::getDesktopMode().height;

    sf::Uint32 win_style = sf::Style::Fullscreen;

#else

    winsizes.x = 640;
    winsizes.y = 480;

    sf::Uint32 win_style = sf::Style::Default;

#endif // FULLSCREENMODE


    sf::String title_string = "Mandelbrot Set Plotter";
    sf::RenderWindow window(sf::VideoMode(winsizes.x, winsizes.y), title_string, win_style);
    sf::VertexArray pointmap(sf::Points, winsizes.x * winsizes.y);

    int itrn = 3000;
    int dz   = 4500;

    double lim = 200;

    cmplxborder border;

    border.Im_up   =  1.3;
    border.Im_down = -1.3;

    border.Re_left  = -(border.Im_up - border.Im_down) * winsizes.x/winsizes.y / 5 *3;
    border.Re_right =  (border.Im_up - border.Im_down) * winsizes.x/winsizes.y / 5 *2;

    sf::Sprite mand_sprite;
    mand_sprite.setTextureRect(sf::IntRect(0, 0, winsizes.x, winsizes.y));
    mand_sprite.setPosition(0, 0);

    while (window.isOpen())
    {
        DrawMandelbrot(pointmap, border, winsizes, itrn, lim);

        window.clear();
        window.draw(pointmap);
        window.display();

        sf::Texture mand_texture;
        mand_texture.create(winsizes.x, winsizes.y);
        mand_texture.update(window);
        mand_sprite.setTexture(mand_texture);

        screen newscreen = GetNewScreen(window, mand_sprite, winsizes);
        if (newscreen.x1 == -1)
        {
            window.close();
            return 0;
        }

        changeBorders(&border, newscreen, winsizes);

        if (newscreen.zoom > 1)
            itrn = (int)(itrn*(1 + newscreen.zoom/dz));
        else
            itrn = (int)(itrn*(1 - 1/(newscreen.zoom*dz + 1)));
    }

    return 0;
}

//------------------------------------------------------------------------------

screen GetNewScreen(sf::RenderWindow& window, sf::Sprite sprite, sf::Vector2i winsizes)
{
    int w = winsizes.x;
    int h = winsizes.y;

    assert(w);
    assert(h);

    screen newscreen;

    sf::Vector2i start(-1, -1);
    sf::Vector2i end  (-1, -1);

    sf::RectangleShape rectangle;
    rectangle.setOutlineThickness(1);
    rectangle.setFillColor(sf::Color::Transparent);

    int OK = 1;

    while (OK)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            start = sf::Mouse::getPosition(window);
            rectangle.setPosition(start.x, start.y);
            sf::sleep(sf::milliseconds(1));

            while (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right))
            {
                end = sf::Mouse::getPosition(window) + sf::Vector2i(1, 1);

                if ((abs(end.x - start.x) > 8) && (abs(end.y - start.y) > 8))
                {
                    double sx = start.x;
                    double sy = start.y;
                    double ex = end.x;
                    double ey = end.y;


                    if ( ((end.y > start.y) && (end.x > start.x)) ||
                         ((end.y < start.y) && (end.x < start.x))   )
                    {
                        end.x = (int)((w*h*(ey - sy) + w*w*ex + h*h*sx)/(w*w + h*h));
                        end.y = (int)((w*h*(ex - sx) + w*w*sy + h*h*ey)/(w*w + h*h));
                    }
                    else
                    {
                        end.x = (int)((w*h*(sy - ey) + w*w*ex + h*h*sx)/(w*w + h*h));
                        end.y = (int)((w*h*(sx - ex) + w*w*sy + h*h*ey)/(w*w + h*h));
                    }


                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                    {
                        rectangle.setOutlineColor(sf::Color::Blue);
                        newscreen.zoom = (double)w*h/abs(end.x - start.x)/abs(end.y - start.y);
                    }
                    else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                    {
                        rectangle.setOutlineColor(sf::Color::Red);
                        newscreen.zoom = (double)abs(end.x - start.x)*abs(end.y - start.y)/w/h;
                    }

                    rectangle.setSize(sf::Vector2f(end - start));
                    window.draw(rectangle);
                    window.display();
                    window.draw(sprite);
                }
                else end.x = -1;
            }
        }

        if (not (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right)) && (end.x != -1))
        {
            rectangle.setSize(sf::Vector2f(end - start));
            window.draw(rectangle);
            window.display();
            window.draw(sprite);
            OK = 0;
        }

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape) return newscreen;
        }
    }


    if (start.x > end.x)
    {
        newscreen.x2 = start.x;
        newscreen.x1 = end.x;
    }
    else
    {
        newscreen.x1 = start.x;
        newscreen.x2 = end.x;
    }

    if (start.y > end.y)
    {
        newscreen.y2 = start.y;
        newscreen.y1 = end.y;
    }
    else
    {
        newscreen.y1 = start.y;
        newscreen.y2 = end.y;
    }

    return newscreen;
}

//------------------------------------------------------------------------------

void changeBorders (cmplxborder* border, screen newscreen, sf::Vector2i winsizes)
{
        double releft  = border->Re_left;
        double reright = border->Re_right;
        double imup    = border->Im_up;
        double imdown  = border->Im_down;

        if (newscreen.zoom > 1)
        {
            border->Re_left  = releft + (reright - releft) * newscreen.x1 / winsizes.x;
            border->Re_right = releft + (reright - releft) * newscreen.x2 / winsizes.x;
            border->Im_down  = imdown + (imup    - imdown) * newscreen.y1 / winsizes.y;
            border->Im_up    = imdown + (imup    - imdown) * newscreen.y2 / winsizes.y;
        }
        else
        {
            border->Re_left  = releft  - (reright - releft) *               newscreen.x1  / (newscreen.x2 - newscreen.x1);
            border->Re_right = reright + (reright - releft) * (winsizes.x - newscreen.x2) / (newscreen.x2 - newscreen.x1);
            border->Im_down  = imdown  - (imup    - imdown) *               newscreen.y1  / (newscreen.y2 - newscreen.y1);
            border->Im_up    = imup    + (imup    - imdown) * (winsizes.y - newscreen.y2) / (newscreen.y2 - newscreen.y1);
        }
}

//------------------------------------------------------------------------------

void DrawMandelbrot (sf::VertexArray& pointmap, cmplxborder border, sf::Vector2i winsizes, int itrn, double lim)
{
    assert(winsizes.x);
    assert(winsizes.y);
    assert(itrn);
    assert(lim);

    int width  = winsizes.x;
    int height = winsizes.y;

    for (int x = 0; x <= width; ++x)
    {
        double re = border.Re_left + (border.Re_right - border.Re_left) * x/width ;
        double x2 = re*re;

        for (int y = 0; y <= height; ++y)
        {
            double im = border.Im_down + (border.Im_up - border.Im_down) * y/height;

            double y2 = im*im;
            double s  = (re + im)*(re + im);

            int i = 0;

            for (; (i < itrn) && ((x2 + y2) <= lim); ++i)
            {
                double x = x2 - y2 + re;
                double y = s - x2 - y2 + im;
                x2 = x*x;
                y2 = y*y;
                s = (x + y)*(x + y);
            }

            sf::Color color;
            if (i < itrn)
            {
                i = i*4 % 1530;

                     if (i < 256 ) color = sf::Color( 255,    i,      0       );
                else if (i < 511 ) color = sf::Color( 510-i,  255,    0       );
                else if (i < 766 ) color = sf::Color( 0,      255,    i - 510 );
                else if (i < 1021) color = sf::Color( 0,      1020-i, 255     );
                else if (i < 1276) color = sf::Color( i-1020, 0,      255     );
                else if (i < 1530) color = sf::Color( 255,    0,      1529-i  );
            }
            else color = sf::Color( 0, 0, 0 );

            pointmap[y*width + x].position = sf::Vector2f(x, y);
            pointmap[y*width + x].color = color;
        }
    }
}

//------------------------------------------------------------------------------
