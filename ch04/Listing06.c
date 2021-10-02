int resolve_year(int year2dig, int yearnow)
{
    int result = yearnow - yearnow%100 + year2dig;

    if (result >= yearnow) {
	if (result > yearnow+50)
            result -= 100;
    } else {
        if (result <= yearnow-50)
            result += 100;
    }

    return result;
}
