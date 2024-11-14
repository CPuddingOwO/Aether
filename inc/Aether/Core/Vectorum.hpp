#pragma once

#include <SDL3/SDL.h>
#include <imgui.h>
#include <cmath>

namespace Aether {
    template <typename T = float>
    struct Vec2 {
        T x, y;

        // Constructors
        explicit Vec2(T x = 0, T y = 0) : x(x), y(y) {}
        Vec2(std::initializer_list<T> values) {
            auto it = values.begin();
            x = (it != values.end()) ? *it++ : 0;
            y = (it != values.end()) ? *it : 0;
        }

        // Addition operator
        Vec2<T> operator+(const Vec2<T>& other) const {
            return Vec2(x + other.x, y + other.y);
        }

        // Subtraction operator
        Vec2<T> operator-(const Vec2<T>& other) const {
            return Vec2(x - other.x, y - other.y);
        }

        // Multiplication by scalar
        Vec2<T> operator*(T scalar) const {
            return Vec2(x * scalar, y * scalar);
        }

        // Division by scalar
        Vec2<T> operator/(T scalar) const {
            return Vec2(x / scalar, y / scalar);
        }

        // Conversion to ImGui's ImVec2
        ImVec2 toImVec2() const {
            return ImVec2(static_cast<float>(x), static_cast<float>(y));
        }

//         Conversion to SDL's SDL_FPoint
        SDL_FPoint toSDL_FPoint() const {
            return SDL_FPoint{static_cast<float>(x), static_cast<float>(y)};
        }

        // Length (magnitude) of the vector
        T length() const {
            return std::sqrt(x * x + y * y);
        }

        // Normalize the vector
        Vec2<T> normalize() const {
            T len = length();
            return len != 0 ? Vec2<T>(x / len, y / len) : Vec2<T>(0, 0);
        }
    };

// 3D Vector template class
    template <typename T = float>
    struct Vec3 {
        T x, y, z;

        // Constructors
        explicit Vec3(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {}
        Vec3(std::initializer_list<T> values) {
            auto it = values.begin();
            x = (it != values.end()) ? *it++ : 0;
            y = (it != values.end()) ? *it++ : 0;
            z = (it != values.end()) ? *it : 0;
        }

        // Addition operator
        Vec3<T> operator+(const Vec3<T>& other) const {
            return Vec3(x + other.x, y + other.y, z + other.z);
        }

        // Subtraction operator
        Vec3<T> operator-(const Vec3<T>& other) const {
            return Vec3(x - other.x, y - other.y, z - other.z);
        }

        // Multiplication by scalar
        Vec3<T> operator*(T scalar) const {
            return Vec3(x * scalar, y * scalar, z * scalar);
        }

        // Division by scalar
        Vec3<T> operator/(T scalar) const {
            return Vec3(x / scalar, y / scalar, z / scalar);
        }

        // Length (magnitude) of the vector
        T length() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        // Normalize the vector
        Vec3<T> normalize() const {
            T len = length();
            return len != 0 ? Vec3<T>(x / len, y / len, z / len) : Vec3<T>(0, 0, 0);
        }

        // Conversion to ImGui's ImVec2
        ImVec2 toImVec2() const {
            return ImVec2(static_cast<float>(x), static_cast<float>(y));
        }

        // Conversion to SDL's SDL_FPoint
        SDL_FPoint toSDL_FPoint() const {
            return SDL_FPoint{static_cast<float>(x), static_cast<float>(y)};
        }
    };

    template <typename T = float>
    struct Vec4 {
        T x, y, z, w;

        // Constructors
        explicit Vec4(T x = 0, T y = 0, T z = 0, T w = 0) : x(x), y(y), z(z), w(w) {}
        Vec4(std::initializer_list<T> values) {
            auto it = values.begin();
            x = (it != values.end()) ? *it++ : 0;
            y = (it != values.end()) ? *it++ : 0;
            z = (it != values.end()) ? *it++ : 0;
            w = (it != values.end()) ? *it : 0;
        }

        // Addition operator
        Vec4<T> operator+(const Vec4<T>& other) const {
            return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
        }

        // Subtraction operator
        Vec4<T> operator-(const Vec4<T>& other) const {
            return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
        }

        // Multiplication by scalar
        Vec4<T> operator*(T scalar) const {
            return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
        }

        // Division by scalar
        Vec4<T> operator/(T scalar) const {
            return Vec4(x / scalar, y / scalar, z / scalar, w / scalar);
        }

        // Length (magnitude) of the vector
        T length() const {
            return std::sqrt(x * x + y * y + z * z + w * w);
        }

        // Normalize the vector
        Vec4<T> normalize() const {
            T len = length();
            return len != 0 ? Vec4<T>(x / len, y / len, z / len, w / len) : Vec4<T>(0, 0, 0, 0);
        }

        // Conversion to ImGui's ImVec4
        ImVec4 toImVec4() const {
            return ImVec4(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), static_cast<float>(w));
        }

        // Conversion to SDL's SDL_FPoint
        SDL_FPoint toSDL_FPoint() const {
            return SDL_FPoint{static_cast<float>(x), static_cast<float>(y)};
        }
    };
}