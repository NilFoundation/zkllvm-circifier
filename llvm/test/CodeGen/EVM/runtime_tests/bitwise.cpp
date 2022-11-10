// EVM_RUN: function: bits, input: [0xffffffff], result: 1
// EVM_RUN: function: bits, input: [0xfff8ffff], result: 0
long bits(long num)
{
  long count = 0, n = 0, i = 0;

  n = num;
  if (num == 0)
  {
    return 0;
  }
  while (n)
  {
    count ++;
    n = n >> 1;
  }
  for (i = 0; i < count; i++)
  {
    if (((num >> i) & 1) == 1)
    {
      continue;
    }
    else
    {
      return 0;
    }
  }
  return 1;
}

// EVM_RUN: function: changebits, input: [0x11223344, 0x55667788, 0x00000000C, 0x00000013], result: 0x11267344
long changebits(long num1, long num2, long pos1, long pos2)
{
  long temp1, temp_1, buffer2, bit1 = 0, bit2 = 0, counter = 0, a = 1;

  temp1 = num1;
  temp_1 = num1;
  buffer2 = num2;
  for (;pos1 <= pos2;pos1++)
  {
    a = 1;
    num1 = temp_1;
    num2 = buffer2;
    while (counter <= pos1)
    {
      if (counter  == pos1)
        bit1 = (num1&1);    //placing the bit of position 1 in bit1
      counter++;
      num1 >>= 1;
    }
    counter = 0;
    while (counter <= pos1)
    {
      if (counter == pos1)
        bit2 = (num2&1);        //placing the bit of position 2 in bit2
      counter++;
      num2 >>= 1;
    }
    counter = 0;
    if (bit1 == bit2);
    else
    {
      while (counter++<pos1)
        a = a << 1;
      temp1 ^= a;    //placing the repplaced bit longeger longo temp1 variable
    }
    counter = 0;
  }
  return temp1;
}

// EVM_RUN: function: n_bit_position, input: [0x87654321, 15], result: 0
// EVM_RUN: function: n_bit_position, input: [0x87654321, 14], result: 1
long n_bit_position(long number,long position)
{
  long result = (number>>(position));
  return result & 1;
}

// EVM_RUN: function: num_bits, input: [0x12345678, 0x87654321], result: 0xe
int num_bits(int n, int m)
{
  int i, count = 0, a, b;

  for (i = 32 - 1;i >= 0;i--)
  {
    a = (n >> i)& 1;
    b = (m >> i)& 1;
    if (a != b)
      count++;
  }
  return count;
}

// EVM_RUN: function: swap, input: [0x00000001, 0x00000000, 0x00000001F], result: 0x80000000
long  swap(long n, long p, long q)
{
  // see if the bits are same. we use XOR operator to do so.
  if (((n & ((long)1 << p)) >> p) ^ ((n & ((long)1 << q)) >> q))
  {
    n ^= ((long)1 << p);
    n ^= ((long)1 << q);
  }

  return n;
}
