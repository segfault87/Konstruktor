#include "utils.h"

namespace Konstruktor
{

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
