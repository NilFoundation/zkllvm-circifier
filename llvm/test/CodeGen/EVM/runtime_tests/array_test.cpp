#include <array>

template<typename T, std::size_t N>
void div_array(std::array<T, N> &array) {
  for (auto &Item : array) {
    Item /= 10;
  }
}

// EVM_RUN: function: array_test, input: [0], result: 1
// EVM_RUN: function: array_test, input: [3], result: 4
[[evm]]
int array_test(int n) {
  std::array arr = {10, 20, 30, 40, 50, 60, 70, 80};
  div_array(arr);
  return arr[n];
}
