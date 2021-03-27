/*------------------------------------------------------------------------------
    * File:        Mandelbrot.cpp                                              *
    * Description: Functions to display the mandelbrot set                     *
    * Created:     1 apr 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#include "Mandelbrot.h"

//------------------------------------------------------------------------------

Mandelbrot::Mandelbrot (size_t width, size_t height, char fullscreen_mode)
{
    initWindow(width, height, fullscreen_mode);

    delta_zoom    = 4500;
    mand_lim      = 100;
    mand_itrn_max = 3000;

    mand_border.Im_up   =  1.3;
    mand_border.Im_down = -1.3;

    mand_border.Re_left  = -(mand_border.Im_up - mand_border.Im_down) * window_sizes.x/window_sizes.y / 5 *3;
    mand_border.Re_right =  (mand_border.Im_up - mand_border.Im_down) * window_sizes.x/window_sizes.y / 5 *2;
}

//------------------------------------------------------------------------------

void Mandelbrot::initWindow(size_t width, size_t height, char fullscreen_mode)
{
    sf::Uint32 win_style;

    if (fullscreen_mode)
    {
        this->window_sizes.x = sf::VideoMode::getDesktopMode().width;
        this->window_sizes.y = sf::VideoMode::getDesktopMode().height;

        win_style = sf::Style::Fullscreen;
    }
    else
    {
        assert (width  < sf::VideoMode::getDesktopMode().width);
        assert (height < sf::VideoMode::getDesktopMode().height);

        this->window_sizes.x = width;
        this->window_sizes.y = height;

        win_style = sf::Style::Default;
    }

    sf::String title_string = "Mandelbrot Set Plotter";
    this->window = new sf::RenderWindow(sf::VideoMode(window_sizes.x, window_sizes.y), title_string, win_style);
    this->window->setVerticalSyncEnabled(true);
}

//------------------------------------------------------------------------------

Mandelbrot::~Mandelbrot()
{
    if (this->window->isOpen()) this->window->close();
    delete this->window;
}

//------------------------------------------------------------------------------

void Mandelbrot::run()
{
    sf::VertexArray pointmap(sf::Points, window_sizes.x * window_sizes.y);

    while (window->isOpen())
    {
        DrawMandelbrot(pointmap, mand_border, window_sizes, mand_itrn_max, mand_lim);

        window->clear();
        window->draw(pointmap);
        window->display();

        screen newscreen;
        if (GetNewScreen(&newscreen, *window, pointmap, window_sizes))
        {
            window->close();
            return;
        }

        changeBorders(&mand_border, newscreen, window_sizes);

        if (newscreen.zoom > 1)
            mand_itrn_max = (int)(mand_itrn_max*(1 + newscreen.zoom/delta_zoom));
        else
            mand_itrn_max = (int)(mand_itrn_max*(1 - 1/(newscreen.zoom*delta_zoom + 1)));
    }
}

//------------------------------------------------------------------------------

int Mandelbrot::GetNewScreen (screen* newscreen, sf::RenderWindow& window, sf::VertexArray pointmap, sf::Vector2i winsizes)
{
    assert(winsizes.x);
    assert(winsizes.y);

    int w = winsizes.x;
    int h = winsizes.y;

    char was_screenshot = 0;

    sf::Vector2i start(-1, -1);
    sf::Vector2i end  (-1, -1);

    sf::RectangleShape rectangle;
    rectangle.setOutlineThickness(1);
    rectangle.setFillColor(sf::Color::Transparent);

    while (1)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            start = sf::Mouse::getPosition(window);
            rectangle.setPosition(start.x, start.y);

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
                        newscreen->zoom = (double)w*h/abs(end.x - start.x)/abs(end.y - start.y);
                    }
                    else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                    {
                        rectangle.setOutlineColor(sf::Color::Red);
                        newscreen->zoom = (double)abs(end.x - start.x)*abs(end.y - start.y)/w/h;
                    }

                    rectangle.setSize(sf::Vector2f(end - start));
                    window.draw(pointmap);
                    window.draw(rectangle);
                    window.display();
                }
                else end.x = -1;
            }
        }

        window.draw(pointmap);

        if (not (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right)) && (end.x != -1))
            break;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if ((event.key.code == sf::Keyboard::Enter) && ! (was_screenshot))
            {
                savePict(window);
                was_screenshot = 1;
                break;
            }

            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape) return 1;
        }
    }


    if (start.x > end.x)
    {
        newscreen->x2 = start.x;
        newscreen->x1 = end.x;
    }
    else
    {
        newscreen->x1 = start.x;
        newscreen->x2 = end.x;
    }

    if (start.y > end.y)
    {
        newscreen->y2 = start.y;
        newscreen->y1 = end.y;
    }
    else
    {
        newscreen->y1 = start.y;
        newscreen->y2 = end.y;
    }

    return 0;
}

//------------------------------------------------------------------------------

void Mandelbrot::changeBorders (cmplxborder* border, screen newscreen, sf::Vector2i winsizes)
{
    assert(border != nullptr);
    assert(winsizes.x);
    assert(winsizes.y);

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

void Mandelbrot::DrawMandelbrot (sf::VertexArray& pointmap, cmplxborder border, sf::Vector2i winsizes, int itrn_max, double lim)
{
    assert(winsizes.x);
    assert(winsizes.y);
    assert(itrn_max);
    assert(lim);

    int width  = winsizes.x;
    int height = winsizes.y;

    double re_step = (border.Re_right - border.Re_left) / width;
    double im_step = (border.Im_up    - border.Im_down) / height;

    __m256d _m_lim = _mm256_set1_pd(lim);
    __m128i ones  = _mm_set1_epi32(1);
    __m128i zeros = _mm_set1_epi32(0);

	__m128i mask32_128_1 = _mm_setr_epi32(0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF );
	__m128i mask32_128_2 = _mm_setr_epi32(0xFFFFFFFF, 0, 0xFFFFFFFF, 0xFFFFFFFF );
	__m128i mask32_128_3 = _mm_setr_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0, 0xFFFFFFFF );
	__m128i mask32_128_4 = _mm_setr_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0 );


    double im0 = border.Im_down;
    
    for (int y = 0; y < height; (++y, im0 += im_step))
    {
        __m256d _m_im0 = { im0, im0, im0, im0 };

        double re0 = border.Re_left;

        for (int x = 0; x < width; (x += 4, re0 += 4*re_step))
        {
            __m256d _m_re0 = { re0, re0 + re_step, re0 + 2*re_step, re0 + 3*re_step };

            __m256d _m_re2 = _mm256_mul_pd(_m_re0, _m_re0);
            __m256d _m_im2 = _mm256_mul_pd(_m_im0, _m_im0);

            __m256d _m_s  = _mm256_mul_pd(_mm256_add_pd(_m_re0, _m_im0), _mm256_add_pd(_m_re0, _m_im0));

            __m128i iterations = _mm_set1_epi32(0);
			__m128i iter_mask  = _mm_set1_epi32(0xFFFFFFFF);

            for (int i = 0; i < itrn_max; ++i)
            {
                __m256d _m_re1 = _mm256_add_pd(_mm256_sub_pd(_m_re2, _m_im2), _m_re0);
                __m256d _m_im1 = _mm256_add_pd(_mm256_sub_pd(_mm256_sub_pd(_m_s, _m_re2), _m_im2), _m_im0);
                _m_re2 = _mm256_mul_pd(_m_re1, _m_re1);
                _m_im2 = _mm256_mul_pd(_m_im1, _m_im1);

                __m256d _m_rad = _mm256_add_pd(_m_re2, _m_im2);

                __m128i rad_cmp = _mm_add_epi32(_mm256_cvtpd_epi32(_mm256_cmp_pd(_m_rad, _m_lim, _CMP_GT_OS)), ones);
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
                pointmap[y*width + x + i].position = sf::Vector2f(x + i, y);
                pointmap[y*width + x + i].color = getColor(*((int32_t*)&iterations + i), itrn_max);
            }
        }
    }
}

//------------------------------------------------------------------------------

sf::Color Mandelbrot::getColor (int32_t itrn, int32_t itrn_max)
{
    if (itrn < itrn_max)
    {
        itrn = itrn*4 % 1530;

             if (itrn < 256 ) return sf::Color( 255,      itrn,      0         );
        else if (itrn < 511 ) return sf::Color( 510-itrn, 255,       0         );
        else if (itrn < 766 ) return sf::Color( 0,        255,       itrn-510  );
        else if (itrn < 1021) return sf::Color( 0,        1020-itrn, 255       );
        else if (itrn < 1276) return sf::Color( itrn-1020, 0,        255       );
        else if (itrn < 1530) return sf::Color( 255,       0,        1529-itrn );
    }

    return sf::Color( 0, 0, 0 );
}

//------------------------------------------------------------------------------

void Mandelbrot::savePict (sf::RenderWindow& window)
{
    static int shot_num = 0;
    char filename[256] = "screenshot";
    char shot_num_str[13] = "";
    sprintf(shot_num_str, "%d", shot_num++);

    strcat(filename, "(");
    strcat(filename, shot_num_str);
    strcat(filename, ")");
    strcat(filename, ".png");

    sf::Texture screen;
    screen.create(window.getSize().x, window.getSize().y);
    screen.update(window);

    sf::RectangleShape rectangle;
    rectangle.setPosition(0, 0);
    rectangle.setSize(sf::Vector2f(window.getSize()));

    if (screen.copyToImage().saveToFile(filename))
        rectangle.setFillColor(sf::Color(10, 10, 10, 150));  // grey screen if ok
    else
        rectangle.setFillColor(sf::Color(255, 10, 10, 200)); // red screen if error

    window.draw(rectangle);
    window.display();

    sf::sleep(sf::milliseconds(300));

    sf::Sprite sprite(screen);
    window.draw(sprite);
    window.display();
}

//------------------------------------------------------------------------------
