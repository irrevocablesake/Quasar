#ifndef SPHEREBOX_H
#define SPHEREBOX_H

#include "Point3.h"
#include "Material.h"
#include "Parallelogram.h"
#include "Mesh.h"
#include "AABB.h"
#include "Sphere.h"

#include <memory>
#include <vector>

using std::shared_ptr;

class SphereBox : public Composite
{

public:
    SphereBox()
    {
        int count = 1000;

        auto material = make_shared<Diffuse>(Color3(0.894, 0.443, 0.478));

        for (int i = 0; i < 1000; i++)
        {
            auto sphere = make_shared<Sphere>(Point3(generateRandomNumber( -100 , 65 ), generateRandomNumber(300, 465), generateRandomNumber(300, 465)), 10, material);
            BLAS.add(sphere);
        }

        BLAS.setupAccelerationStructure();
    }

    AABB getBoundingBox() const override
    {
        return BLAS.getBoundingBox();
    }

    bool hit(const Ray &ray, Interval interval, IntersectionManager &intersectionManager) const override
    {
        return BLAS.raycast( ray, interval, intersectionManager );
    }

    int getCount() const override{
        return BLAS.getCount();
    }

private:
    World BLAS;
};

#endif