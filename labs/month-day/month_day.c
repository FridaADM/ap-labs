#include <stdio.h>

static char daytab[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

int calendar = 0;

/* month_day function's prototype*/


void month_day(int year, int yearday, int *pmonth, int *pday){
    if(yearday > 366){
	printf("not a valid number of days");
	}
	if(year % 4 == 0){
		calendar = 1;
	}
	else{
		calendar = 0;
	}
	int month = 0;
	for(int i=0; yearday > daytab[calendar][i]; i++){
		month = i;
		yearday -= daytab[calendar][i];
	}
	switch (month+1) {
            case 1:
                printf("January");
                break;
            case 2:
                printf("February");
                break;
            case 3:
                printf("March");
                break;
            case 4:
                printf("April");
                break;
            case 5:
                printf("May");
                break;
            case 6:
                printf("June");
                break;
            case 7:
                printf("July");
                break;
            case 8:
                printf("August");
                break;
            case 9:
                printf("September");
                break;
            case 10:
                printf("October");
                break;
            case 11:
                printf("November");
                break;
            case 12:
                printf("December");
                break;
            default:
                printf("Out of range");
                break;
        }
	printf("%d \n", yearday);

}

int main() {
    month_day(2019, 172, 0, 0);
    return 0;
}
