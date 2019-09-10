#include <stdio.h>

void fahrenheit_celcius(int start, int end, int increment ){
    int fahrenheit;
    for (fahrenheit = start; fahrenheit <= end; fahrenheit = fahrenheit + increment)
	printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahrenheit, (5.0/9.0)*(fahrenheit-32));
}

int main()
{
    fahrenheit_celcius(0, 100, 10);
    return 0;
}
