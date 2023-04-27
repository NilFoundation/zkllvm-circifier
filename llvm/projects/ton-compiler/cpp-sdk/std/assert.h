#ifndef ASSERT_H
#define ASSERT_H
inline constexpr void noop(){}
// inline constexpr void assert_fail(){1/0;}
// inline constexpr void assert(bool expr) {
//     if(expr) noop();
//     else assert_fail();
// }
#define assert(x) noop()
#endif