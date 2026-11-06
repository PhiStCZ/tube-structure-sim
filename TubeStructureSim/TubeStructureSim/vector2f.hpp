#ifndef vector2f_hpp_
#define vector2f_hpp_
#include <math.h>

struct vector2f
{
    float x;
    float y;

    vector2f() : x(0), y(0) {}
    vector2f(float x, float y) : x(x), y(y) {}
    
    bool operator==(const vector2f& v) const
    {
        return (x == v.x && y == v.y);
    }

    vector2f operator-() const
    {
        return vector2f(-(this->x), -(this->y));
    }

    vector2f& operator+=(const vector2f& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }
    vector2f& operator-=(const vector2f& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    vector2f& operator*=(float alpha)
    {
        x *= alpha;
        y *= alpha;
        return *this;
    }
    vector2f& operator/=(float alpha)
    {
        x /= alpha;
        y /= alpha;
        return *this;
    }

    vector2f operator+(const vector2f& v) const
    {
        vector2f rv(this->x, this->y);
        rv += v;
        return rv;
    }
    vector2f operator-(const vector2f& v) const
    {
        vector2f rv(this->x, this->y);
        rv -= v;
        return rv;
    }
    vector2f operator*(float alpha) const
    {
        vector2f rv(this->x, this->y);
        rv *= alpha;
        return rv;
    }
    vector2f operator/(float alpha) const
    {
        vector2f rv(this->x, this->y);
        rv /= alpha;
        return rv;
    }

    float size() const
    {
        return (sqrtf(x * x + y * y));
    }
    vector2f unit() const
    {
        auto s = size();
        if (s == 0) throw std::exception("zero division");
        return (*this) / s;
    }

    static float distance(const vector2f& v, const vector2f& w)
    {
        return (v - w).size();
    }

    static float dotProduct(const vector2f& v, const vector2f& w)
    {
        return (v.x * w.x) + (v.y * w.y);
    }

    static vector2f rotate90deg_right(const vector2f& v) {
        return { v.y, -v.x };
    }
};

#endif