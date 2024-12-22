#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <unistd.h>

const int NUM_SNOWFLAKES = 100;
int windowWidth = 800;
int windowHeight = 600;

typedef struct
{
	int x, y;
	int size;
	int speed;
} Snowflake;

//TODO: Избавиться от глобала
Snowflake snowflakes[NUM_SNOWFLAKES];

void InitSnowflakes()
{
	for (int i = 0; i < NUM_SNOWFLAKES; i++)
	{
		snowflakes[i].x = rand() % windowWidth;
		snowflakes[i].y = rand() % windowHeight;
		snowflakes[i].size = rand() % 5 + 2;
		snowflakes[i].speed = rand() % 2 + 1;
	}
}

void DrawSnowflakes(Display* display, Window window, GC gc)
{
	for (int i = 0; i < NUM_SNOWFLAKES; i++)
	{
		XFillArc(display, window, gc,
			snowflakes[i].x, snowflakes[i].y,
			snowflakes[i].size, snowflakes[i].size,
			0, 360 * 64);
	}
}

void UpdateSnowflakes()
{
	for (int i = 0; i < NUM_SNOWFLAKES; i++)
	{
		snowflakes[i].y += snowflakes[i].speed;

		if (snowflakes[i].y > windowHeight)
		{
			snowflakes[i].x = rand() % windowWidth;
			snowflakes[i].y = -5;
		}
	}
}

int main()
{
	Display* display;
	Window window;
	XEvent event;
	GC gc;

	srand(time(NULL));

	display = XOpenDisplay(NULL);
	if (display == NULL)
	{
		fprintf(stderr, "Не удалось открыть дисплей\n");
		return EXIT_FAILURE;
	}

	int screen = DefaultScreen(display);
	window = XCreateSimpleWindow(display, RootWindow(display, screen),
		0, 0, windowWidth, windowHeight, 1,
		BlackPixel(display, screen),
		BlackPixel(display, screen));

	XSelectInput(display, window, StructureNotifyMask);
	gc = XCreateGC(display, window, 0, NULL);

	XSetForeground(display, gc, WhitePixel(display, screen));

	XMapWindow(display, window);

	InitSnowflakes();

	while (true)
	{
		while (XPending(display))
		{
			XNextEvent(display, &event);
			if (event.type == ConfigureNotify)
			{
				windowWidth = event.xconfigure.width;
				windowHeight = event.xconfigure.height;
			}
		}

		UpdateSnowflakes();

		XClearWindow(display, window);
		//TODO: Не фиксированное время, а относительное
		//usleep(15000);
		DrawSnowflakes(display, window, gc);

	}
}

//TODO: При ресайзе окна снег не должен падать штабелями