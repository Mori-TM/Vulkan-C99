#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define ARRAYSIZE(x)  (sizeof(x) / sizeof((x)[0]))
#define SWAP(T, a, b) do { T tmp = a; a = b; b = tmp; } while (0)
#define MAX_FRAMES_IN_FLIGHT 3
typedef unsigned int bool;
#define true 1
#define false 0
#define E 2.71828182846
#define PI 3.141592653589793238
#define PI2 6.28318530718

#define VERTEX_OFFSET 0
#define FRAGMENT_OFFSET 192

#define SHADOW_MAP_WIDTH 2048
#define SHADOW_MAP_HEIGHT 2048