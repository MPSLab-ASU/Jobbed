unsigned long sqrt_rnd(unsigned long square)
{
	unsigned long op = square;
	unsigned long res = 0;
	unsigned long one = 1uL << 30;

	while (one > op)
		one >>= 2;

	while (one != 0)
	{
		if (op >= res + one)
		{
			op = op - (res + one);
			res = res + 2 * one;
		}
		res >>= 1;
		one >>= 2;
	}

	if (one > res)
		res++;

	return res;
}
