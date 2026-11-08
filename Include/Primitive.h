#pragma once

#include "Ray.h"

class Primitive
{
private:

public:
    virtual bool Intersect(Ray ray, Intersection &isect) const = 0;
};
