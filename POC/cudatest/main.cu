#include <cstdio>
#include "impl_cuda.h"
//#include "impl_host.h"
#include "Vector.h"
#include "HashListGrid.h"

using namespace fdb;

int main() {
#if 1
    H21B3_Grid<vec3f> vel, new_vel;
    float dt = 0.01f;

    advect(vel, vel, new_vel, dt);
    std::swap(vel, new_vel);

#else
    Vector<int> a;
    a.resize(5, 40);
    {
        auto av = a.view();
        parallel_for(a.size(), [=] FDB_DEVICE (size_t i) {
            printf("- %ld %d\n", i, av[i]);
            av[i] = 42;
        });
    }
    a.resize(8, 4);
    {
        auto av = a.view();
        parallel_for(a.size(), [=] FDB_DEVICE (size_t i) {
            printf("+ %ld %d\n", i, av[i]);
        });
    }

#endif

    synchronize();
    return 0;
}
