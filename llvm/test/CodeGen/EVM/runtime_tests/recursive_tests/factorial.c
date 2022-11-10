// EVM_RUN: function: factorial, input: [0], result: 1
// EVM_RUN: function: factorial, input: [3], result: 6
// EVM_RUN: function: factorial, input: [8], result: 40320

int factorial(int n)
{
  if (n == 0)
    return 1;
  else
    return(n * factorial(n-1));
}