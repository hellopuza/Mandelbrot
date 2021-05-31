/*------------------------------------------------------------------------------
    * File:        Mandelbrot.cpp                                              *
    * Description: Functions to display the mandelbrot set                     *
    * Created:     27 mar 2021                                                 *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#include "Mandelbrot.h"

//------------------------------------------------------------------------------

Mandelbrot::Mandelbrot () :
    winsizes_ ({DEFAULT_WIDTH, DEFAULT_HEIGHT})
{
    pointmap_ = new sf::VertexArray(sf::Points, winsizes_.x * winsizes_.y);
    window_ = new sf::RenderWindow(sf::VideoMode(winsizes_.x, winsizes_.y), title_string, sf::Style::Close);
    
    initBorders();
}

//------------------------------------------------------------------------------

Mandelbrot::Mandelbrot (size_t width, size_t height) :
    winsizes_ ({(int)width, (int)height})
{
    assert(width);
    assert(height);

    assert(width  < sf::VideoMode::getDesktopMode().width);
    assert(height < sf::VideoMode::getDesktopMode().height);

    pointmap_ = new sf::VertexArray(sf::Points, winsizes_.x * winsizes_.y);
    window_ = new sf::RenderWindow(sf::VideoMode(winsizes_.x, winsizes_.y), title_string, sf::Style::Close);

    initBorders();
}

//------------------------------------------------------------------------------

Mandelbrot::Mandelbrot (char fullscreen_mode) :
    winsizes_ ({DEFAULT_WIDTH, DEFAULT_HEIGHT})
{
    sf::Uint32 win_style = sf::Style::Close;

    if (fullscreen_mode)
    {
        winsizes_.x = sf::VideoMode::getDesktopMode().width;
        winsizes_.y = sf::VideoMode::getDesktopMode().height;

        win_style =  sf::Style::Fullscreen | sf::Style::Close;
    }

    pointmap_ = new sf::VertexArray(sf::Points, winsizes_.x * winsizes_.y);
    window_ = new sf::RenderWindow(sf::VideoMode(winsizes_.x, winsizes_.y), title_string, win_style);

    initBorders();
}

//------------------------------------------------------------------------------

void Mandelbrot::initBorders ()
{
    borders_.Im_up   =  1.3;
    borders_.Im_down = -1.3;

    borders_.Re_left  = -(borders_.Im_up - borders_.Im_down) * winsizes_.x/winsizes_.y / 5 *3;
    borders_.Re_right =  (borders_.Im_up - borders_.Im_down) * winsizes_.x/winsizes_.y / 5 *2;
}

//------------------------------------------------------------------------------

Mandelbrot::~Mandelbrot ()
{
    if (window_->isOpen()) window_->close();
    delete pointmap_;
    delete window_;
}

//------------------------------------------------------------------------------

void Mandelbrot::run ()
{
    window_->setVerticalSyncEnabled(true);

    DrawMandelbrot();

    window_->clear();
    window_->draw(*pointmap_);
    window_->display();

    while (window_->isOpen())
    {
        sf::Event event;
        screen newscreen = {};
        bool was_screenshot = 0;
        while (window_->pollEvent(event))
        {
            if ( ( event.type == sf::Event::Closed) || 
                 ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)) )
            {
                window_->close();
                return;
            }

            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Enter) && (! was_screenshot))
            {
                savePict();
                was_screenshot = 1;
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right))
            {
                if (GetNewScreen(newscreen))
                {
                    changeBorders(newscreen);

                    if (newscreen.zoom > 1)
                        itrn_max_ = (int)(itrn_max_*(1 + newscreen.zoom/delta_zoom_));
                    else
                        itrn_max_ = (int)(itrn_max_*(1 - 1/(newscreen.zoom*delta_zoom_ + 1)));

                    DrawMandelbrot();

                    window_->clear();
                    window_->draw(*pointmap_);
                    window_->display();
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                window_->clear();
                window_->draw(*pointmap_);
                PointTrace(sf::Mouse::getPosition(*window_));
            }
        }
    }
}

//------------------------------------------------------------------------------

int Mandelbrot::GetNewScreen (screen& newscreen)
{
    int w = winsizes_.x;
    int h = winsizes_.y;

    char was_screenshot = 0;

    sf::Vector2i start(-1, -1);
    sf::Vector2i end  (-1, -1);

    sf::RectangleShape rectangle;
    rectangle.setOutlineThickness(1);
    rectangle.setFillColor(sf::Color::Transparent);

#ifdef __linux__

    sf::Texture screen;
    screen.create(w, h);
    screen.update(*window_);

    sf::Sprite sprite(screen);

#endif // __linux__

    while (1)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            start = sf::Mouse::getPosition(*window_);
            rectangle.setPosition(start.x, start.y);

            while (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right))
            {
                end = sf::Mouse::getPosition(*window_) + sf::Vector2i(1, 1);

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

                    #ifdef __linux__
                    window_->draw(sprite);
                    #else
                    window_->draw(*pointmap_);
                    #endif // __linux__

                    window_->draw(rectangle);
                    window_->display();
                }
                else end.x = -1;
            }
        }

        #ifndef __linux__
        window_->draw(*pointmap_);
        #endif // __linux__

        if (end.x != -1) break;
        else return 0;
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

    return 1;
}

//------------------------------------------------------------------------------

void Mandelbrot::changeBorders (screen newscreen)
{
    double releft  = borders_.Re_left;
    double reright = borders_.Re_right;
    double imup    = borders_.Im_up;
    double imdown  = borders_.Im_down;

    if (newscreen.zoom > 1)
    {
        borders_.Re_left  = releft + (reright - releft) * newscreen.x1 / winsizes_.x;
        borders_.Re_right = releft + (reright - releft) * newscreen.x2 / winsizes_.x;
        borders_.Im_down  = imdown + (imup    - imdown) * newscreen.y1 / winsizes_.y;
        borders_.Im_up    = imdown + (imup    - imdown) * newscreen.y2 / winsizes_.y;
    }
    else
    {
        borders_.Re_left  = releft  - (reright - releft) *                newscreen.x1  / (newscreen.x2 - newscreen.x1);
        borders_.Re_right = reright + (reright - releft) * (winsizes_.x - newscreen.x2) / (newscreen.x2 - newscreen.x1);
        borders_.Im_down  = imdown  - (imup    - imdown) *                newscreen.y1  / (newscreen.y2 - newscreen.y1);
        borders_.Im_up    = imup    + (imup    - imdown) * (winsizes_.y - newscreen.y2) / (newscreen.y2 - newscreen.y1);
    }
}

//------------------------------------------------------------------------------

void Mandelbrot::DrawMandelbrot ()
{
    assert(itrn_max_);
    assert(lim_);

    int width  = winsizes_.x;
    int height = winsizes_.y;

    double re_step = (borders_.Re_right - borders_.Re_left) / width;
    double im_step = (borders_.Im_up    - borders_.Im_down) / height;

    __m256d _m_lim = _mm256_set1_pd(lim_);
    __m128i ones   = _mm_set1_epi32(1);
    __m128i zeros  = _mm_set1_epi32(0);

    __m128i mask32_128_1 = _mm_setr_epi32( 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF );
    __m128i mask32_128_2 = _mm_setr_epi32( 0xFFFFFFFF, 0, 0xFFFFFFFF, 0xFFFFFFFF );
    __m128i mask32_128_3 = _mm_setr_epi32( 0xFFFFFFFF, 0xFFFFFFFF, 0, 0xFFFFFFFF );
    __m128i mask32_128_4 = _mm_setr_epi32( 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0 );


    double im0 = borders_.Im_down;

    int x_offset = 0;

    for (int y = 0; y < height; (++y, im0 += im_step, x_offset += width))
    {
        __m256d _m_im0 = { im0, im0, im0, im0 };

        #pragma omp parallel for
        for (int x = 0; x < width; x += 4)
        {
            double re0 = x * re_step + borders_.Re_left;

            __m256d _m_re0 = { re0, re0 + re_step, re0 + 2*re_step, re0 + 3*re_step };

            __m256d _m_re2 = _mm256_mul_pd(_m_re0, _m_re0);
            __m256d _m_im2 = _mm256_mul_pd(_m_im0, _m_im0);

            __m256d _m_s = _mm256_mul_pd(_mm256_add_pd(_m_re0, _m_im0), _mm256_add_pd(_m_re0, _m_im0));

            __m128i iterations = _mm_set1_epi32(0);
            __m128i iter_mask  = _mm_set1_epi32(0xFFFFFFFF);

            for (int i = 0; i < itrn_max_; ++i)
            {
                __m256d _m_re1 = _mm256_add_pd(_mm256_sub_pd(_m_re2, _m_im2), _m_re0);
                __m256d _m_im1 = _mm256_add_pd(_mm256_sub_pd(_mm256_sub_pd(_m_s, _m_re2), _m_im2), _m_im0);
                _m_re2 = _mm256_mul_pd(_m_re1, _m_re1);
                _m_im2 = _mm256_mul_pd(_m_im1, _m_im1);

                __m256d _m_rad2 = _mm256_add_pd(_m_re2, _m_im2);

                __m128i rad_cmp = _mm_add_epi32(_mm256_cvtpd_epi32(_mm256_cmp_pd(_m_rad2, _m_lim, _CMP_GT_OS)), ones);
                rad_cmp = _mm_abs_epi32(_mm_cmpgt_epi32(rad_cmp, zeros));

                iterations = _mm_add_epi32(iterations, _mm_and_si128(iter_mask, rad_cmp));

                if (*((int32_t*)&rad_cmp + 0) == 0) iter_mask = _mm_and_si128(iter_mask, mask32_128_1);
                if (*((int32_t*)&rad_cmp + 1) == 0) iter_mask = _mm_and_si128(iter_mask, mask32_128_2);
                if (*((int32_t*)&rad_cmp + 2) == 0) iter_mask = _mm_and_si128(iter_mask, mask32_128_3);
                if (*((int32_t*)&rad_cmp + 3) == 0) iter_mask = _mm_and_si128(iter_mask, mask32_128_4);

                if (_mm_test_all_zeros(iter_mask, iter_mask)) break;

                _m_s  = _mm256_mul_pd(_mm256_add_pd(_m_re1, _m_im1), _mm256_add_pd(_m_re1, _m_im1));
            }

            for (int i = 0; i < 4; ++i)
            {
                (*pointmap_)[x_offset + x + i].position = sf::Vector2f(x + i, y);
                (*pointmap_)[x_offset + x + i].color = getColor(*((int32_t*)&iterations + i));
            }
        }
    }
}

//------------------------------------------------------------------------------

sf::Color Mandelbrot::getColor (int32_t itrn)
{
    if (itrn < itrn_max_)
    {
        itrn = itrn*4 % 1530;

             if (itrn < 256 ) return sf::Color( 255,       itrn,      0         );
        else if (itrn < 511 ) return sf::Color( 510-itrn,  255,       0         );
        else if (itrn < 766 ) return sf::Color( 0,         255,       itrn-510  );
        else if (itrn < 1021) return sf::Color( 0,         1020-itrn, 255       );
        else if (itrn < 1276) return sf::Color( itrn-1020, 0,         255       );
        else if (itrn < 1530) return sf::Color( 255,       0,         1529-itrn );
    }

    return sf::Color( 0, 0, 0 );
}

//------------------------------------------------------------------------------

void Mandelbrot::savePict ()
{
    static int shot_num = 0;
    char filename[256] = "screenshot";
    char shot_num_str[13] = "";
    sprintf(shot_num_str, "%d", shot_num++);

    strcat(filename, "(");
    strcat(filename, shot_num_str);
    strcat(filename, ")");
    strcat(filename, ".png");
    
    window_->draw(*pointmap_);

    sf::Texture screen;
    screen.create(window_->getSize().x, window_->getSize().y);
    screen.update(*window_);

    sf::RectangleShape rectangle;
    rectangle.setPosition(0, 0);
    rectangle.setSize(sf::Vector2f(window_->getSize()));

    if (screen.copyToImage().saveToFile(filename))
        rectangle.setFillColor(sf::Color(10, 10, 10, 150));  // grey screen if ok
    else
        rectangle.setFillColor(sf::Color(255, 10, 10, 200)); // red screen if error

    window_->draw(rectangle);
    window_->display();

    sf::sleep(sf::milliseconds(300));

    sf::Sprite sprite(screen);
    window_->draw(sprite);
    window_->display();
}

//------------------------------------------------------------------------------

void Mandelbrot::PointTrace (sf::Vector2i point)
{
    double re = borders_.Re_left + (borders_.Re_right - borders_.Re_left) * point.x / winsizes_.x;
    double im = borders_.Im_down + (borders_.Im_up    - borders_.Im_down) * point.y / winsizes_.y;

    double xx = re * re;
    double yy = im * im;
    double w  = (re + im)*(re + im);

    double x1 = re;
    double y1 = im;

    for (int i = 0; (i < 1000) && ((xx + yy) <= lim_); ++i)
    {
        double x2 = xx - yy + re;
        double y2 = w - xx - yy + im;
        xx = x2*x2;
        yy = y2*y2;
        w = (x2 + y2)*(x2 + y2);

        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f((x1 - borders_.Re_left) / (borders_.Re_right - borders_.Re_left) * winsizes_.x,
                                    (y1 - borders_.Im_down) / (borders_.Im_up    - borders_.Im_down) * winsizes_.y), sf::Color::White),

            sf::Vertex(sf::Vector2f((x2 - borders_.Re_left) / (borders_.Re_right - borders_.Re_left) * winsizes_.x,
                                    (y2 - borders_.Im_down) / (borders_.Im_up    - borders_.Im_down) * winsizes_.y), sf::Color::Black)
        };

        x1 = x2;
        y1 = y2;

        window_->draw(line, 2, sf::Lines);
            
        ++i;
    }
    window_->display();
}

//------------------------------------------------------------------------------
