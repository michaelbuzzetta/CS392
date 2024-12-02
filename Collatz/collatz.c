#include <stdio.h>

int collatz(int n) {
    int count = 0;
    
    while (n != 1) {
        printf("%d \n", n); // Print the current integer
        if (n % 2 == 0) {
            n = n / 2;
        } else {
            n = 3 * n + 1;
        }
        count++;
    }
    printf("1\n"); // Print 1 after the sequence terminates
    return count;
}

int main() {
    int number;
    printf("Enter a positive integer: ");
    scanf("%d", &number);
    
    int iterations = collatz(number);
    printf("Number of iterations: %d\n", iterations);
    
    return 0;
}
