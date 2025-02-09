#include <zeno/zeno.h>
#include <zeno/extra/ShaderNode.h>
#include <zeno/types/ShaderObject.h>
#include <zeno/utils/string.h>

namespace zeno {


struct ShaderLinearFit : ShaderNodeClone<ShaderLinearFit> {
    virtual int determineType(EmissionPass *em) override {
        auto in = em->determineType(get_input("in").get());
        auto inMin = em->determineType(get_input("inMin").get());
        auto inMax = em->determineType(get_input("inMax").get());
        auto outMin = em->determineType(get_input("outMin").get());
        auto outMax = em->determineType(get_input("outMax").get());

        if (inMin == 1 && inMax == 1 && outMin == 1 && outMax == 1) {
            return in;
        } else if (inMin == in && inMax == in && outMin == in && outMax == in) {
            return in;
        } else if (inMin == 1 && inMax == 1 && outMin == in && outMax == in) {
            return in;
        } else if (inMin == in && inMax == in && outMin == 1 && outMax == 1) {
            return in;
        } else {
            throw zeno::Exception("vector dimension mismatch in linear fit");
        }
    }

    virtual void emitCode(EmissionPass *em) override {
        auto in = em->determineExpr(get_input("in").get());
        auto inMin = em->determineExpr(get_input("inMin").get());
        auto inMax = em->determineExpr(get_input("inMax").get());
        auto outMin = em->determineExpr(get_input("outMin").get());
        auto outMax = em->determineExpr(get_input("outMax").get());

        auto exp = "(" + in + " - " + inMin + ") / (" + inMax + " - " + inMin + ")";
        if (get_param<bool>("clamped"))
            exp = "clamp(" + exp + ", 0.0, 1.0)";
        em->emitCode(exp + " * (" + outMax + " - " + outMin + ") + " + outMin);
    }
};

ZENDEFNODE(ShaderLinearFit, {
    {
        {"float", "in", "0"},
        {"float", "inMin", "0"},
        {"float", "inMax", "1"},
        {"float", "outMin", "0"},
        {"float", "outMax", "1"},
    },
    {
        {"float", "out"},
    },
    {
        {"bool", "clamped", "0"},
    },
    {"shader"},
});

struct ShaderVecConvert : ShaderNodeClone<ShaderVecConvert> {
    int ty{};

    virtual int determineType(EmissionPass *em) override {
        auto _type = get_param<std::string>("type");
        em->determineType(get_input("in").get());
        if (_type == "vec2") {
            ty = 2;
        }
        else if (_type == "vec3") {
            ty = 3;
        }
        else if (_type == "vec4") {
            ty = 4;
        }
        return ty;
    }

    virtual void emitCode(EmissionPass *em) override {
        std::string exp = em->determineExpr(get_input("in").get());
        em->emitCode(em->funcName("convertTo" + std::to_string(ty)) + "(" + exp + ")");
    }
};

ZENDEFNODE(ShaderVecConvert, {
    {
        { "float", "in", "0"},
    },
    {"out"},
    {
        {"enum vec2 vec3 vec4", "type", "vec3"},
    },
    {"shader"},
});

struct ShaderNormalMap : ShaderNodeClone<ShaderNormalMap> {
    virtual int determineType(EmissionPass *em) override {
        auto in1 = get_input("normalTexel");
        auto in2 = get_input("scale");
        em->determineType(in1.get());
        em->determineType(in2.get());
        return 3;
    }

    virtual void emitCode(EmissionPass *em) override {
        auto in1 = em->determineExpr(get_input("normalTexel").get());
        auto in2 = em->determineExpr(get_input("scale").get());

        return em->emitCode(em->funcName("normalmap") + "(" + in1 + ", " + in2 + ")");
    }
};

ZENDEFNODE(ShaderNormalMap, {
    {
        {"vec3f", "normalTexel", "0.5,0.5,1.0"},
        {"float", "scale", "1"},
    },
    {
        {"vec3f", "out"},
    },
    {},
    {"shader"},
});

}
