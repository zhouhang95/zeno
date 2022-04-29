#pragma once

#include <array>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <zenovis/opengl/shader.h>
#include <zeno/types/CameraObject.h>

namespace zenovis {

struct Camera {
    int m_nx{512}, m_ny{512};
    glm::mat4x4 m_view{1}, m_proj{1};

    float m_near = 0.1f;
    float m_far = 20000.0f;
    float m_fov = 45.f;

    float m_aperature = 0.05f;
    float m_dof = -1.f;

    float m_lodradius = 1.0f;
    glm::vec3 m_lodcenter{0, 0, -1};
    glm::vec3 m_lodfront{0, 0, 1};
    glm::vec3 m_lodup{0, 1, 0};

    float getAspect() const {
        return (float)m_nx / (float)m_ny;
    }

    void setCamera(zeno::CameraData const &cam);
    void setCamera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float fov, float fnear, float ffar, float radius);
    void setCamera(float cx, float cy, float cz, float theta, float phi, float radius, float fov);
    void set_program_uniforms(opengl::Program *pro);
};

} // namespace zenovis
