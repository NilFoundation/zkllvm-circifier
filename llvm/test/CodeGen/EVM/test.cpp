// EVM_RUN: function: test_global_init, input: [], result: 0

unsigned* Ptr;
unsigned long long G64;
unsigned Arr[8];
unsigned InitedArr[9] = {15, 25, 35, 45, 55, 65, 75, 85, 95};
const unsigned ConstArr[4] = {10, 20, 30, 40};


void setG64(unsigned long long v) {
  G64 = v;
}

int check_globals() {
  if (InitedArr[0] != 15 ||
      InitedArr[1] != 25 ||
      InitedArr[2] != 35 ||
      InitedArr[3] != 45 ||
      InitedArr[4] != 55 ||
      InitedArr[5] != 65 ||
      InitedArr[6] != 75 ||
      InitedArr[7] != 85 ||
      InitedArr[8] != 95) {
    return 1;
  }
  if (ConstArr[0] != 10 ||
      ConstArr[1] != 20 ||
      ConstArr[2] != 30 ||
      ConstArr[3] != 40) {
    return 2;
  }
  return 0;
}

[[evm]]
int test_global_init(int n) {
  if (InitedArr[0] != 15 ||
      InitedArr[1] != 25 ||
      InitedArr[2] != 35 ||
      InitedArr[3] != 45 ||
      InitedArr[4] != 55 ||
      InitedArr[5] != 65 ||
      InitedArr[6] != 75 ||
      InitedArr[7] != 85 ||
      InitedArr[8] != 95) {
    return 1;
  }
  if (ConstArr[0] != 10 ||
      ConstArr[1] != 20 ||
      ConstArr[2] != 30 ||
      ConstArr[3] != 40) {
    return 2;
  }
  if (auto res = check_globals(); res != 0) {
    return res;
  }
  if (Ptr != nullptr) {
    return 21;
  }
  Ptr = (unsigned*)123;
  if (Ptr != (unsigned*)123) {
    return 22;
  }
  setG64(123456LLU);
  if (G64 != 123456) {
    return 3;
  }
  for (int i = 0; i < 8; i++) {
    Arr[i] = i + 30;
  }
  for (int i = 0; i < 8; i++) {
    if (Arr[i] != i + 30) {
      return 4;
    }
  }
  if (auto res = check_globals(); res != 0) {
    return res + 10;
  }
  if (G64 != 123456) {
    return 5;
  }
  return 0;
}