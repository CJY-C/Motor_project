#include "pid_controller.h"

float SmallEnough(float value)
{
	if (value < 0.000001)
		return 0;
	return value;
}
