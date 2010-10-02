#include "utils.h"

float KonstruktorUtils::floatModulo(float dividend, float divisor)
{
	if (dividend > 0.0f) {
		while (dividend >= divisor)
			dividend -= divisor;
		return dividend;
	} else if (dividend < 0.0f) {
		while (dividend <= -divisor)
			dividend += divisor;
		return divisor;
	} else {
		return 0.0f;
	}
}
