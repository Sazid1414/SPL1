#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
long long int power(long long int a, long long int b,
					long long int P)
{
	if (b == 1)
		return a;

	else
		return (((long long int)pow(a, b)) % P);
}

int main()
{
	long long int q, G, x, a, y, b, ka, kb;

	q = 23;
	printf("The value of q : %lld\n", q);

	//G = 9; // A primitive root for P, G is taken
	printf("The value of G : %lld\n\n", G);

	// Alice will choose the private key a
	a = 4; // a is the chosen private key
	printf("The private key a for Alice : %lld\n", a);
	x = power(G, a, q); // gets the generated key

	// Bob will choose the private key b
	b = 3; // b is the chosen private key
	printf("The private key b for Bob : %lld\n\n", b);
	y = power(G, b, q);
	ka = power(y, a, q); 
	kb = power(x, b, q); 

	printf("Secret key for the Alice is : %lld\n", ka);
	printf("Secret Key for the Bob is : %lld\n", kb);

	return 0;
}
