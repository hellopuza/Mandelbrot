/*------------------------------------------------------------------------------
    * Description: Program to display mandelbrot set                           *
    * Created:     1 apr 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#include "Mandelbrot.h"

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    if (argc == 1) // default window sizes
    {
        Mandelbrot Benoit;
        Benoit.run();
    }
    if (argc == 2) // fullscreen or default window sizes
    {
        Mandelbrot Benoit(atoi(argv[1]));
        Benoit.run();
    }
    if (argc == 3) // custom window sizes
    {
        Mandelbrot Benoit(atoi(argv[1]), atoi(argv[2]));
        Benoit.run();
    }
    
    return 0;
}

//------------------------------------------------------------------------------
