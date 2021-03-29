/*------------------------------------------------------------------------------
    * Description: Program to display mandelbrot set                           *
    * Created:     1 apr 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#include "Mandelbrot.h"

int main()
{
    //Mandelbrot Benoit(640, 480);
    Mandelbrot Benoit(1);

    Benoit.run();
    
    return 0;
}

//------------------------------------------------------------------------------