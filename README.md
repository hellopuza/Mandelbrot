# Mandelbrot set viewer

![screenshot](picts/8.png "Screenshot")

I wrote a simple program that let's you view the Mandelbrot Set and zoom in.

**Features**

- Beautiful rainbow coloring
- Efficient implementation along with user interaction with the mouse allowing you to scale across the set
- You can take screenshots
- Fast execution of the program using SSE and AVX intrinsics


## Usage

**How to build**

```console
$ make
```
**Requirements:**

- C++ compiler, GNU make,
- SFML

**Therminal**

```console
# if you want to create default window
$ ./Mandelbrot

# if you want to create fullscreen window
$ ./Mandelbrot 1

# if you want to create window with custom sizes
$ ./Mandelbrot 123 456
```

**How to use**

- **Zoom in:** draw a rectangle with the `left mouse` button in the area you want to zoom in. The rectangle will be blue.
- **Zoom out:** draw a rectangle with the `right mouse` button in the area you want to zoom out. The rectangle will be red.
- **Take a screenshot:** Press `Enter`. The screenshot will be saved as "screenshot(<number>).png" in your directory.
- **Point trace:** Press `Space`.
- **Exit the program:** Press `Esc` or close window.
