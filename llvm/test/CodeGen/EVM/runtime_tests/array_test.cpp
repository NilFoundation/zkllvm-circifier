#include <array>
#include <numeric>

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

// EVM_RUN: function: array_foreach_test, input: [0], result: 0
// EVM_RUN: function: array_foreach_test, input: [2], result: 20
// EVM_RUN: function: array_foreach_test, input: [3], result: 30
[[evm]]
int array_foreach_test(int n) {
  std::array arr = {1, 2, 3, 4};
  auto res = std::accumulate(arr.begin(), arr.end(), 0, [n](int res, int v) {
    return res + v * n;
  });
  return res;
}
