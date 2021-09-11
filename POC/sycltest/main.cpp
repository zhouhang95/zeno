#include <CL/sycl.hpp>
#include <iostream>
#include <memory>
#include <array>
#include "virtual_ptr.hpp"

namespace sycl = cl::sycl;


struct SyclHandler {
    sycl::handler &m_handler;

    SyclHandler(sycl::handler &handler) : m_handler(handler) {}

    template <auto Mode = sycl::access::mode::read_write, class T>
    [[nodiscard]] auto remapPointer(T *ptr) {
        auto axr = m_mapper.get_access<Mode,
             sycl::access::mode::global_buffer, T>(ptr, m_handler);
        return axr;
    }

    template <class Key, class Range, class Kernel>
    decltype(auto) parallel_for(Range &&range, Kernel &&kernel) {
        return m_handler.parallel_for(range, kernel);
    }
};


struct SyclSession {
    sycl::queue m_queue;
    sycl::codeplay::PointerMapper m_mapper;

    template <class F>
    decltype(auto) submit(F const &f) {
        return m_queue.submit(f);
    }

    decltype(auto) wait() {
        return m_queue.wait();
    }

    [[nodiscard]] void *syclMalloc(size_t size) {
        return sycl::codeplay::SYCLmalloc(size, m_mapper);
    }

    void syclFree(void *ptr) {
        sycl::codeplay::SYCLfree(ptr, m_mapper);
    }

    template <auto Mode = sycl::access::mode::read_write, class T>
    [[nodiscard]] auto remapPointer(T *ptr) {
        auto axr = m_mapper.get_access<Mode,
             sycl::access::mode::host_buffer, T>(ptr);
        return axr;
    }
};


[[nodiscard]] static SyclSession *syclSession() {
    static std::unique_ptr<SyclSession> g_session = std::make_unique<SyclSession>();
    return g_session.get();
}


template <class T>
struct SyclAllocator {
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <class U>
    struct rebind {
        using other = SyclAllocator<U>;
    };

    SyclAllocator() = default;
    ~SyclAllocator() = default;

    [[nodiscard]] pointer allocate(size_type numObjects) {
        return static_cast<pointer>(syclSession()->syclMalloc(sizeof(T) * numObjects));
    }

    [[nodiscard]] pointer allocate(size_type numObjects, const void *hint) {
        return allocate(numObjects);
    }

    void deallocate(pointer p, size_type numObjects) {
        syclSession()->syclFree(p);
    }

    [[nodiscard]] size_type max_size() const {
        return std::numeric_limits<size_type>::max();
    }

    template <class U, class... Args>
    void construct(U *p, Args &&... args) {
        new(p) U(std::forward<Args>(args)...);
    }

    template <class U>
    void destroy(U *p) {
        p->~U();
    }
};

class kernel0;

int main() {
    auto *sess = syclSession();

    SyclAllocator<int> svm;
    int *arr = svm.allocate(32);

    sess->submit([&] (SyclHandler &hdl) {
        auto axr = hdl.remapPointer(arr);
        handler.parallel_for<kernel0>(sycl::range<1>(32), [=](sycl::item<1> id) {
            axr[id[0]] = id[0];
        });
    });

    {
        auto axr = sess.remapPointer(arr);
        for (int i = 0; i < 32; i++) {
            printf("%d\n", axr[i]);
        }
    }

    svm.deallocate(arr, 32);

    return 0;
}
