# Lab 11: Multiprocessing

Name: Mia Siedentopf
Course: CPE-2600 Systems Programming, Fall 2026
Section: 121

## Overview

`mandelmovie` makes 50 Mandelbrot images by running the provided `mandel`
program once per frame. Each frame zooms in a little more on the point
x=0.286932, y=0.014287. The parent uses `fork()` to start a child for each
frame, and each child uses `execl()` to run `mandel`. The number of children
running at once is set with `-n` (read with `getopt`). When that limit is
reached, the parent calls `wait()` for one child to finish before starting
the next, and at the end it waits for all children to finish.

## Build and run

    make
    ./mandelmovie -n 5
    ffmpeg -i mandel%d.jpg mandel.mpg

## Results

| Processes | Time (seconds) |
| --- | --- |
| 1 | 232.671 |
| 2 | 89.579 |
| 5 | 38.503 |
| 10 | 30.118 |
| 20 | 27.797 |

## Discussion

DISCUSSION

## Movie

`mandel.mp4` is in this repo.