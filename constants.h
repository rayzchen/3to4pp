#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifndef M_PI
#if __has_cpp_attribute(__cpp_lib_math_constants)
#define M_PI std::numbers::pi
#define M_PI_2 std::numbers::pi / 2
#define M_PI_4 std::numbers::pi / 4
#else
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.785398163397448309616
#endif
#endif

#endif // constants.h
