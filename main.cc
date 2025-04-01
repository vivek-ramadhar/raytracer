#include "rtweekend.h"

#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "sphere.h"


int main() {
    hittable_list world;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73)); 
    auto green = make_shared<lambertian>(color(.12, .45, .15)); 
    auto light = make_shared<diffuse_light>(color(15,15,15));

    // Cornell box sides
}
