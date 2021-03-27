/*------------------------------------------------------------------------------
    * File:        Mandelbrot.h                                                *
    * Description: Declaration of functions and data types used to display     *
                   the mandelbrot set                                          *
    * Created:     1 apr 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#define _CRT_SECURE_NO_WARNINGS

#include <SFML/Graphics.hpp>
#include <assert.h>
#include <string.h>

#include <emmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>
#include <xmmintrin.h>


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

//------------------------------------------------------------------------------

class Mandelbrot
{
public:

    Mandelbrot (size_t width, size_t height, char fullscreen_mode);
   ~Mandelbrot ();

    void run();

private:

    sf::RenderWindow *window;
    cmplxborder       mand_border;
    sf::Vector2i      window_sizes;

    size_t delta_zoom;
    double mand_lim;
    size_t mand_itrn_max;

    void initWindow(size_t width, size_t height, char fullscreen_mode);

    int       GetNewScreen   (screen* newscreen, sf::RenderWindow& window, sf::VertexArray pointmap, sf::Vector2i winsizes);
    void      DrawMandelbrot (sf::VertexArray& pointmap, cmplxborder border, sf::Vector2i winsizes, int itrn_max, double lim);
    void      changeBorders  (cmplxborder* border, screen newscreen, sf::Vector2i winsizes);
    sf::Color getColor       (int32_t itrn, int32_t itrn_max);
    void      savePict       (sf::RenderWindow& window);
};

//------------------------------------------------------------------------------
