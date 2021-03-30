/*------------------------------------------------------------------------------
    * File:        main.cpp                                                    *
    * Description: Program to display the mandelbrot set                       *
    * Created:     27 mar 2021                                                 *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
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
    else
    if (argc == 2) // fullscreen or default window sizes
    {
        Mandelbrot Benoit(atoi(argv[1]));
        Benoit.run();
    }
    else
    if (argc == 3) // custom window sizes
    {
        Mandelbrot Benoit(atoi(argv[1]), atoi(argv[2]));
        Benoit.run();
    }
    else printf("Wrong input parameters!\n");
    
    return 0;
}

//------------------------------------------------------------------------------
