/*------------------------------------------------------------------------------
    * File:        Mandelbrot.h                                                *
    * Description: Declaration of functions and data types used to display     *
                   the mandelbrot set                                          *
    * Created:     27 mar 2021                                                 *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#define _CRT_SECURE_NO_WARNINGS

#include <SFML/Graphics.hpp>
#include <assert.h>
#include <string.h>
#include <omp.h>

#include <emmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>
#include <xmmintrin.h>

const int DEFAULT_WIDTH  = 640;
const int DEFAULT_HEIGHT = 480;

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

    Mandelbrot ();
    Mandelbrot (char fullscreen_mode);
    Mandelbrot (size_t width, size_t height);
   ~Mandelbrot ();

    void run();

private:

    sf::RenderWindow *window_;
    cmplxborder       border_;
    sf::Vector2i      winsizes_;

    size_t delta_zoom_ = 3500;
    double lim_        = 100;
    size_t itrn_max_   = 3000;

    void initBorders();
    void createWindow(size_t width, size_t height, sf::Uint32 win_style);

    int       GetNewScreen   (screen* newscreen, sf::RenderWindow& window, sf::VertexArray pointmap, sf::Vector2i winsizes);
    void      DrawMandelbrot (sf::VertexArray& pointmap, cmplxborder border, sf::Vector2i winsizes, int itrn_max, double lim);
    void      changeBorders  (cmplxborder* border, screen newscreen, sf::Vector2i winsizes);
    sf::Color getColor       (int32_t itrn, int32_t itrn_max);
    void      savePict       (sf::RenderWindow& window);
};

//------------------------------------------------------------------------------
