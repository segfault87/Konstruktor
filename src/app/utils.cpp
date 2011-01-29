#include "utils.h"

namespace Konstruktor
{

float Utils::floatModulo(float dividend, float divisor)
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

int Utils::maximum(const QSet<int> &set)
{
	int max = -1;

	foreach (int i, set) {
		if (i > max)
			max = i;
	}

	return max;
}

}
