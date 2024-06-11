#include "stdint.h"

// Used to send the 3 newest altitude data points
typedef struct LastThreePoints {
	uint16_t point1;
	uint16_t point2;
	uint16_t point3;
} LastThreePoints;
