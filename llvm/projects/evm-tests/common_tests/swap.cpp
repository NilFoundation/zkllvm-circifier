// EVM_RUN: function: test, input: [0x12345678, 0x87654321], result: 1

void swap(long *xp, long *yp);

[[evm]] long test(long a, long b) {
  long aa =a;
  long bb =b;
  swap(&a, &b);
  if (aa == b && bb ==a) {
    return 1;
  } else {
    return 0;
  }
}

void swap(long *xp, long *yp)
{  
    long temp = *xp;  
    *xp = *yp;  
    *yp = temp;  
}  
