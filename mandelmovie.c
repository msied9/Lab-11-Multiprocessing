/**
 * File: mandelmovie.c
 * Author: Mia Siedentopf
 * Assignment: CPE-2600 Lab 11 - Multiprocessing
 * Section:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARG_LEN 64
#define MAX_NAME_LEN 64

static void show_help(void);
static void start_frame(int frame, double x, double y, double scale,
                        int max, int width, int height);
static int wait_for_one_child(void);

int main(int argc, char *argv[])
{
    // Default settings. The point (0.286932, 0.014287) is the example
    // zoom location from the mandel help text.
    int procs = 1;          // how many children may run at once
    int frames = 50;        // how many images to make
    double xcenter = 0.286932;
    double ycenter = 0.014287;
    double start_scale = 4;     // first frame shows the whole set
    double end_scale = 0.0005;  // last frame is zoomed way in
    int max = 1000;
    int width = 1000;
    int height = 1000;

    // Read the command line options with getopt
    int c;
    while ((c = getopt(argc, argv, "n:f:x:y:s:e:m:W:H:h")) != -1)
    {
        switch (c)
        {
            case 'n': procs = atoi(optarg); break;
            case 'f': frames = atoi(optarg); break;
            case 'x': xcenter = atof(optarg); break;
            case 'y': ycenter = atof(optarg); break;
            case 's': start_scale = atof(optarg); break;
            case 'e': end_scale = atof(optarg); break;
            case 'm': max = atoi(optarg); break;
            case 'W': width = atoi(optarg); break;
            case 'H': height = atoi(optarg); break;
            case 'h':
            default:
                show_help();
                return 1;
        }
    }

    // Check the values so bad input gives a clear message
    if (procs < 1 || frames < 1 || width < 1 || height < 1 || max < 1 ||
        start_scale <= 0 || end_scale <= 0)
    {
        fprintf(stderr, "Error: values must be positive. Use -h for help.\n");
        return 1;
    }
    if (procs > frames)
    {
        procs = frames;     // more children than frames makes no sense
    }

    printf("mandelmovie: %d frames, %d processes at a time\n", frames, procs);

    int running = 0;    // children currently alive
    int failures = 0;   // children that did not finish successfully

    for (int frame = 1; frame <= frames; frame++)
    {
        // Zoom smoothly: multiply the scale by the same factor each frame
        // so the zoom looks steady. Frame 1 uses start_scale and the last
        // frame uses end_scale.
        double scale = start_scale;
        if (frames > 1)
        {
            double fraction = (double)(frame - 1) / (frames - 1);
            scale = start_scale * pow(end_scale / start_scale, fraction);
        }

        // If the limit of children is reached, wait for one to finish
        // before starting another.
        if (running == procs)
        {
            failures += wait_for_one_child();
            running--;
        }

        start_frame(frame, xcenter, ycenter, scale, max, width, height);
        running++;
    }

    // The parent must wait for all remaining children before it exits
    while (running > 0)
    {
        failures += wait_for_one_child();
        running--;
    }

    if (failures > 0)
    {
        fprintf(stderr, "Warning: %d frame(s) failed\n", failures);
        return 1;
    }

    printf("Done. Make the movie with: ffmpeg -i mandel%%d.jpg mandel.mpg\n");
    return 0;
}

/**
 * Forks a child that runs mandel to make one frame.
 * The output file is mandel<frame>.jpg so ffmpeg can find the frames.
 */
static void start_frame(int frame, double x, double y, double scale,
                        int max, int width, int height)
{
    // Build the argument strings BEFORE forking
    char xs[MAX_ARG_LEN], ys[MAX_ARG_LEN], ss[MAX_ARG_LEN];
    char ms[MAX_ARG_LEN], ws[MAX_ARG_LEN], hs[MAX_ARG_LEN];
    char outfile[MAX_NAME_LEN];

    snprintf(xs, sizeof(xs), "%.12f", x);
    snprintf(ys, sizeof(ys), "%.12f", y);
    snprintf(ss, sizeof(ss), "%.12f", scale);
    snprintf(ms, sizeof(ms), "%d", max);
    snprintf(ws, sizeof(ws), "%d", width);
    snprintf(hs, sizeof(hs), "%d", height);
    snprintf(outfile, sizeof(outfile), "mandel%d.jpg", frame);

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    if (pid == 0)
    {
        // Child: replace this process with mandel. execl only returns
        // if it fails.
        execl("./mandel", "mandel", "-x", xs, "-y", ys, "-s", ss,
              "-m", ms, "-W", ws, "-H", hs, "-o", outfile, (char *)NULL);
        perror("execl ./mandel");
        _exit(1);
    }
    // Parent returns right away so it can start the next child
}

/**
 * Waits for any one child to finish.
 * Returns 0 if it finished normally, 1 if it failed.
 */
static int wait_for_one_child(void)
{
    int status;
    pid_t done = wait(&status);
    if (done < 0)
    {
        perror("wait");
        return 1;
    }
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
        return 0;
    }
    fprintf(stderr, "Child %d did not finish normally\n", (int)done);
    return 1;
}

/**
 * Shows the help message
 */
static void show_help(void)
{
    printf("Use: mandelmovie [options]\n");
    printf("Where options are:\n");
    printf("-n <procs>   Number of processes working at once. (default=1)\n");
    printf("-f <frames>  Number of images to make. (default=50)\n");
    printf("-x <coord>   X coordinate to zoom in on. (default=0.286932)\n");
    printf("-y <coord>   Y coordinate to zoom in on. (default=0.014287)\n");
    printf("-s <scale>   Scale of the first image. (default=4)\n");
    printf("-e <scale>   Scale of the last image. (default=0.0005)\n");
    printf("-m <max>     Max iterations per point. (default=1000)\n");
    printf("-W <pixels>  Image width. (default=1000)\n");
    printf("-H <pixels>  Image height. (default=1000)\n");
    printf("-h           Show this help text.\n");
    printf("\nExample: ./mandelmovie -n 5\n");
}