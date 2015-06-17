#ifndef VECTORS_H_DEF
#define VECTORS_H_DEF

#include <cmath>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// 2D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector2f
{
    float x;
    float y;

    // ctors
    Vector2f() : x(0), y(0) {};
    Vector2f(float x, float y) : x(x), y(y) {};

    // utils functions
    void        set(float x, float y);
    float       length() const;                         //
    float       distance(const Vector2f& vec) const;     // distance between two vectors
    Vector2f&    normalize();                            //
    float       dot(const Vector2f& vec) const;          // dot product
    bool        equal(const Vector2f& vec, float e) const; // compare with epsilon

    // operators
    Vector2f     operator-() const;                      // unary operator (negate)
    Vector2f     operator+(const Vector2f& rhs) const;    // add rhs
    Vector2f     operator-(const Vector2f& rhs) const;    // subtract rhs
    Vector2f&    operator+=(const Vector2f& rhs);         // add rhs and update this object
    Vector2f&    operator-=(const Vector2f& rhs);         // subtract rhs and update this object
    Vector2f     operator*(const float scale) const;     // scale
    Vector2f     operator*(const Vector2f& rhs) const;    // multiply each element
    Vector2f&    operator*=(const float scale);          // scale and update this object
    Vector2f&    operator*=(const Vector2f& rhs);         // multiply each element and update this object
    Vector2f     operator/(const float scale) const;     // inverse scale
    Vector2f&    operator/=(const float scale);          // scale and update this object
    bool        operator==(const Vector2f& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Vector2f& rhs) const;   // exact compare, no epsilon
    bool        operator<(const Vector2f& rhs) const;    // comparison for sort
    float       operator[](int index) const;            // subscript operator v[0], v[1]
    float&      operator[](int index);                  // subscript operator v[0], v[1]
	Vector2f&	operator=(const Vector2f& rhs);

    friend Vector2f operator*(const float a, const Vector2f vec);
    friend std::ostream& operator<<(std::ostream& os, const Vector2f& vec);
};



///////////////////////////////////////////////////////////////////////////////
// 3D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector3f
{
    float x;
    float y;
    float z;

    // ctors
    Vector3f() : x(0), y(0), z(0) {};
    Vector3f(float x, float y, float z) : x(x), y(y), z(z) {};

    // utils functions
    void        set(float x, float y, float z);
    float       length() const;                         //
    float       distance(const Vector3f& vec) const;     // distance between two vectors
    Vector3f&    normalize();                            //
    float       dot(const Vector3f& vec) const;          // dot product
    Vector3f     cross(const Vector3f& vec) const;        // cross product
    bool        equal(const Vector3f& vec, float e) const; // compare with epsilon

    // operators
    Vector3f     operator-() const;                      // unary operator (negate)
    Vector3f     operator+(const Vector3f& rhs) const;    // add rhs
    Vector3f     operator-(const Vector3f& rhs) const;    // subtract rhs
    Vector3f&    operator+=(const Vector3f& rhs);         // add rhs and update this object
    Vector3f&    operator-=(const Vector3f& rhs);         // subtract rhs and update this object
    Vector3f     operator*(const float scale) const;     // scale
    Vector3f     operator*(const Vector3f& rhs) const;    // multiplay each element
    Vector3f&    operator*=(const float scale);          // scale and update this object
    Vector3f&    operator*=(const Vector3f& rhs);         // product each element and update this object
    Vector3f     operator/(const float scale) const;     // inverse scale
    Vector3f&    operator/=(const float scale);          // scale and update this object
    bool        operator==(const Vector3f& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Vector3f& rhs) const;   // exact compare, no epsilon
    bool        operator<(const Vector3f& rhs) const;    // comparison for sort
    float       operator[](int index) const;            // subscript operator v[0], v[1]
    float&      operator[](int index);                  // subscript operator v[0], v[1]
	Vector3f&	operator=(const Vector3f& rhs);

    friend Vector3f operator*(const float a, const Vector3f vec);
    friend std::ostream& operator<<(std::ostream& os, const Vector3f& vec);
};



///////////////////////////////////////////////////////////////////////////////
// 4D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector4f
{
    float x;
    float y;
    float z;
    float w;

    // ctors
    Vector4f() : x(0), y(0), z(0), w(0) {};
    Vector4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};

    // utils functions
    void        set(float x, float y, float z, float w);
    float       length() const;                         //
    float       distance(const Vector4f& vec) const;     // distance between two vectors
    Vector4f&    normalize();                            //
    float       dot(const Vector4f& vec) const;          // dot product
    bool        equal(const Vector4f& vec, float e) const; // compare with epsilon

    // operators
    Vector4f     operator-() const;                      // unary operator (negate)
    Vector4f     operator+(const Vector4f& rhs) const;    // add rhs
    Vector4f     operator-(const Vector4f& rhs) const;    // subtract rhs
    Vector4f&    operator+=(const Vector4f& rhs);         // add rhs and update this object
    Vector4f&    operator-=(const Vector4f& rhs);         // subtract rhs and update this object
    Vector4f     operator*(const float scale) const;     // scale
    Vector4f     operator*(const Vector4f& rhs) const;    // multiply each element
    Vector4f&    operator*=(const float scale);          // scale and update this object
    Vector4f&    operator*=(const Vector4f& rhs);         // multiply each element and update this object
    Vector4f     operator/(const float scale) const;     // inverse scale
    Vector4f&    operator/=(const float scale);          // scale and update this object
    bool        operator==(const Vector4f& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Vector4f& rhs) const;   // exact compare, no epsilon
    bool        operator<(const Vector4f& rhs) const;    // comparison for sort
    float       operator[](int index) const;            // subscript operator v[0], v[1]
    float&      operator[](int index);                  // subscript operator v[0], v[1]
	Vector4f&	operator=(const Vector4f& rhs);

    friend Vector4f operator*(const float a, const Vector4f vec);
    friend std::ostream& operator<<(std::ostream& os, const Vector4f& vec);
};



// fast math routines from Doom3 SDK
inline float invSqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int*)&x;          // get bits for floating value
    i = 0x5f3759df - (i>>1);    // gives initial guess
    x = *(float*)&i;            // convert bits back to float
    x = x * (1.5f - xhalf*x*x); // Newton step
    return x;
}



///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector2f
///////////////////////////////////////////////////////////////////////////////
inline Vector2f Vector2f::operator-() const {
    return Vector2f(-x, -y);
}

inline Vector2f Vector2f::operator+(const Vector2f& rhs) const {
    return Vector2f(x+rhs.x, y+rhs.y);
}

inline Vector2f Vector2f::operator-(const Vector2f& rhs) const {
    return Vector2f(x-rhs.x, y-rhs.y);
}

inline Vector2f& Vector2f::operator+=(const Vector2f& rhs) {
    x += rhs.x; y += rhs.y; return *this;
}

inline Vector2f& Vector2f::operator-=(const Vector2f& rhs) {
    x -= rhs.x; y -= rhs.y; return *this;
}

inline Vector2f Vector2f::operator*(const float a) const {
    return Vector2f(x*a, y*a);
}

inline Vector2f Vector2f::operator*(const Vector2f& rhs) const {
    return Vector2f(x*rhs.x, y*rhs.y);
}

inline Vector2f& Vector2f::operator*=(const float a) {
    x *= a; y *= a; return *this;
}

inline Vector2f& Vector2f::operator*=(const Vector2f& rhs) {
    x *= rhs.x; y *= rhs.y; return *this;
}

inline Vector2f Vector2f::operator/(const float a) const {
    return Vector2f(x/a, y/a);
}

inline Vector2f& Vector2f::operator/=(const float a) {
    x /= a; y /= a; return *this;
}

inline bool Vector2f::operator==(const Vector2f& rhs) const {
    return (x == rhs.x) && (y == rhs.y);
}

inline bool Vector2f::operator!=(const Vector2f& rhs) const {
    return (x != rhs.x) || (y != rhs.y);
}

inline bool Vector2f::operator<(const Vector2f& rhs) const {
    if(x < rhs.x) return true;
    if(x > rhs.x) return false;
    if(y < rhs.y) return true;
    if(y > rhs.y) return false;
    return false;
}

inline float Vector2f::operator[](int index) const {
    return (&x)[index];
}

inline float& Vector2f::operator[](int index) {
    return (&x)[index];
}

inline Vector2f& Vector2f::operator=(const Vector2f& rhs){
	set( rhs.x, rhs.y );
	return *this;
}

inline void Vector2f::set(float x, float y) {
    this->x = x; this->y = y;
}

inline float Vector2f::length() const {
    return sqrtf(x*x + y*y);
}

inline float Vector2f::distance(const Vector2f& vec) const {
    return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y));
}

inline Vector2f& Vector2f::normalize() {
    //@@const float EPSILON = 0.000001f;
    float xxyy = x*x + y*y;
    //@@if(xxyy < EPSILON)
    //@@    return *this;

    //float invLength = invSqrt(xxyy);
    float invLength = 1.0f / sqrtf(xxyy);
    x *= invLength;
    y *= invLength;
    return *this;
}

inline float Vector2f::dot(const Vector2f& rhs) const {
    return (x*rhs.x + y*rhs.y);
}

inline bool Vector2f::equal(const Vector2f& rhs, float epsilon) const {
    return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon;
}

inline Vector2f operator*(const float a, const Vector2f vec) {
    return Vector2f(a*vec.x, a*vec.y);
}

inline std::ostream& operator<<(std::ostream& os, const Vector2f& vec) {
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}
// END OF Vector2f /////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector3f
///////////////////////////////////////////////////////////////////////////////
inline Vector3f Vector3f::operator-() const {
    return Vector3f(-x, -y, -z);
}

inline Vector3f Vector3f::operator+(const Vector3f& rhs) const {
    return Vector3f(x+rhs.x, y+rhs.y, z+rhs.z);
}

inline Vector3f Vector3f::operator-(const Vector3f& rhs) const {
    return Vector3f(x-rhs.x, y-rhs.y, z-rhs.z);
}

inline Vector3f& Vector3f::operator+=(const Vector3f& rhs) {
    x += rhs.x; y += rhs.y; z += rhs.z; return *this;
}

inline Vector3f& Vector3f::operator-=(const Vector3f& rhs) {
    x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this;
}

inline Vector3f Vector3f::operator*(const float a) const {
    return Vector3f(x*a, y*a, z*a);
}

inline Vector3f Vector3f::operator*(const Vector3f& rhs) const {
    return Vector3f(x*rhs.x, y*rhs.y, z*rhs.z);
}

inline Vector3f& Vector3f::operator*=(const float a) {
    x *= a; y *= a; z *= a; return *this;
}

inline Vector3f& Vector3f::operator*=(const Vector3f& rhs) {
    x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this;
}

inline Vector3f Vector3f::operator/(const float a) const {
    return Vector3f(x/a, y/a, z/a);
}

inline Vector3f& Vector3f::operator/=(const float a) {
    x /= a; y /= a; z /= a; return *this;
}

inline bool Vector3f::operator==(const Vector3f& rhs) const {
    return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}

inline bool Vector3f::operator!=(const Vector3f& rhs) const {
    return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
}

inline bool Vector3f::operator<(const Vector3f& rhs) const {
    if(x < rhs.x) return true;
    if(x > rhs.x) return false;
    if(y < rhs.y) return true;
    if(y > rhs.y) return false;
    if(z < rhs.z) return true;
    if(z > rhs.z) return false;
    return false;
}

inline Vector3f& Vector3f::operator=(const Vector3f& rhs){
	set( rhs.x, rhs.y, rhs.z );
	return *this;
}

inline float Vector3f::operator[](int index) const {
    return (&x)[index];
}

inline float& Vector3f::operator[](int index) {
    return (&x)[index];
}

inline void Vector3f::set(float x, float y, float z) {
    this->x = x; this->y = y; this->z = z;
}

inline float Vector3f::length() const {
    return sqrtf(x*x + y*y + z*z);
}

inline float Vector3f::distance(const Vector3f& vec) const {
    return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y) + (vec.z-z)*(vec.z-z));
}

inline Vector3f& Vector3f::normalize() {
    //@@const float EPSILON = 0.000001f;
    float xxyyzz = x*x + y*y + z*z;
    //@@if(xxyyzz < EPSILON)
    //@@    return *this; // do nothing if it is ~zero vector

    //float invLength = invSqrt(xxyyzz);
    float invLength = 1.0f / sqrtf(xxyyzz);
    x *= invLength;
    y *= invLength;
    z *= invLength;
    return *this;
}

inline float Vector3f::dot(const Vector3f& rhs) const {
    return (x*rhs.x + y*rhs.y + z*rhs.z);
}

inline Vector3f Vector3f::cross(const Vector3f& rhs) const {
    return Vector3f(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x);
}

inline bool Vector3f::equal(const Vector3f& rhs, float epsilon) const {
    return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon && fabs(z - rhs.z) < epsilon;
}

inline Vector3f operator*(const float a, const Vector3f vec) {
    return Vector3f(a*vec.x, a*vec.y, a*vec.z);
}

inline std::ostream& operator<<(std::ostream& os, const Vector3f& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}
// END OF Vector3f /////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector4f
///////////////////////////////////////////////////////////////////////////////
inline Vector4f Vector4f::operator-() const {
    return Vector4f(-x, -y, -z, -w);
}

inline Vector4f Vector4f::operator+(const Vector4f& rhs) const {
    return Vector4f(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w);
}

inline Vector4f Vector4f::operator-(const Vector4f& rhs) const {
    return Vector4f(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w);
}

inline Vector4f& Vector4f::operator+=(const Vector4f& rhs) {
    x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;
}

inline Vector4f& Vector4f::operator-=(const Vector4f& rhs) {
    x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;
}

inline Vector4f Vector4f::operator*(const float a) const {
    return Vector4f(x*a, y*a, z*a, w*a);
}

inline Vector4f Vector4f::operator*(const Vector4f& rhs) const {
    return Vector4f(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
}

inline Vector4f& Vector4f::operator*=(const float a) {
    x *= a; y *= a; z *= a; w *= a; return *this;
}

inline Vector4f& Vector4f::operator*=(const Vector4f& rhs) {
    x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this;
}

inline Vector4f Vector4f::operator/(const float a) const {
    return Vector4f(x/a, y/a, z/a, w/a);
}

inline Vector4f& Vector4f::operator/=(const float a) {
    x /= a; y /= a; z /= a; w /= a; return *this;
}

inline bool Vector4f::operator==(const Vector4f& rhs) const {
    return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
}

inline bool Vector4f::operator!=(const Vector4f& rhs) const {
    return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
}

inline bool Vector4f::operator<(const Vector4f& rhs) const {
    if(x < rhs.x) return true;
    if(x > rhs.x) return false;
    if(y < rhs.y) return true;
    if(y > rhs.y) return false;
    if(z < rhs.z) return true;
    if(z > rhs.z) return false;
    if(w < rhs.w) return true;
    if(w > rhs.w) return false;
    return false;
}

inline float Vector4f::operator[](int index) const {
    return (&x)[index];
}

inline float& Vector4f::operator[](int index) {
    return (&x)[index];
}

inline Vector4f& Vector4f::operator=(const Vector4f& rhs){
	set( rhs.x, rhs.y, rhs.z, rhs.w );
	return *this;
}

inline void Vector4f::set(float x, float y, float z, float w) {
    this->x = x; this->y = y; this->z = z; this->w = w;
}

inline float Vector4f::length() const {
    return sqrtf(x*x + y*y + z*z + w*w);
}

inline float Vector4f::distance(const Vector4f& vec) const {
    return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y) + (vec.z-z)*(vec.z-z) + (vec.w-w)*(vec.w-w));
}

inline Vector4f& Vector4f::normalize() {
    //NOTE: leave w-component untouched
    //@@const float EPSILON = 0.000001f;
    float xxyyzz = x*x + y*y + z*z;
    //@@if(xxyyzz < EPSILON)
    //@@    return *this; // do nothing if it is zero vector

    //float invLength = invSqrt(xxyyzz);
    float invLength = 1.0f / sqrtf(xxyyzz);
    x *= invLength;
    y *= invLength;
    z *= invLength;
    return *this;
}

inline float Vector4f::dot(const Vector4f& rhs) const {
    return (x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w);
}

inline bool Vector4f::equal(const Vector4f& rhs, float epsilon) const {
    return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon &&
           fabs(z - rhs.z) < epsilon && fabs(w - rhs.w) < epsilon;
}

inline Vector4f operator*(const float a, const Vector4f vec) {
    return Vector4f(a*vec.x, a*vec.y, a*vec.z, a*vec.w);
}

inline std::ostream& operator<<(std::ostream& os, const Vector4f& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
    return os;
}
// END OF Vector4f /////////////////////////////////////////////////////////////

struct UV2f
{
	float u, v;
};

struct Color3f 
{
	float r, g, b; 
	operator float*() { return (float*)&r; }
	operator const float*() const { return (const float*)&r; }
};

struct Color4f 
{
	float r, g, b, a; 
	Color4f() : r(0.0f),g(0.0f),b(0.0f),a(0.0f) {};
	Color4f(float r, float g, float b, float a) : r(r),g(g),b(b),a(a) {};
	operator float*() { return (float*)&r; }
	operator const float*() const { return (const float*)&r; }
};

#endif
