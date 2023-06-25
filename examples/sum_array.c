#include <stdio.h>

int sum_array(int *arr, int n)
{
    int sum = 0;
    for (int i = 0; i < n; i++)
        sum += arr[i];
    return sum;
}

int main(void)
{
    int arr[5] = {1, 2, 3, 4, 5};
    {
        const int sum = sum_array(arr, 5);
        printf("sum = %d\n", sum);
    }

    return 0;
}