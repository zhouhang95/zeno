#include <zen/zen.h>
#include <cstdio>
#include <zen/NumericObject.h>

struct MyObject : zen::IObject {
    int value;
};


struct MakeMyObject : zen::INode {
    virtual void apply() override {
        printf("MakeMyObject::apply() called!\n");
        int value = std::get<int>(get_param("value"));
        auto obj = std::make_unique<MyObject>();
        obj->value = value;
        set_output("obj", obj);
    }
};

static int defMakeMyObject = zen::defNodeClass<MakeMyObject>("MakeMyObject",
   { /* inputs: */ {
   }, /* outputs: */ {
       "obj",
   }, /* params: */ {
       {"int", "value", "233 0"},  // defl min max; defl min; defl
   }, /* category: */ {
       "demo_project",
   }});


struct MyAdd : zen::INode {
  virtual void apply() override {
      printf("MyAdd::apply() called!\n");
      auto lhs = get_input("lhs")->as<MyObject>();
      auto rhs = get_input("rhs")->as<MyObject>();
      auto result = std::make_unique<MyObject>();
      result->value = lhs->value + rhs->value;
      set_output("result", result);
  }
};

static int defMyAdd = zen::defNodeClass<MyAdd>("MyAdd",
    { /* inputs: */ {
        "lhs", "rhs",
    }, /* outputs: */ {
        "result",
    }, /* params: */ {
    }, /* category: */ {
    "demo_project",
    }});


struct MyPrint : zen::INode {
    virtual void apply() override {
        printf("MyPrint::apply() called!\n");
        if (has_input("obj")) {
            auto obj = get_input("obj")->as<MyObject>();
            printf("MyPrint: object value is %d\n", obj->value);
        } else {
            printf("input socket `obj` not connected!\n");
        }
    }
};

static int defMyPrint = zen::defNodeClass<MyPrint>("MyPrint",
   { /* inputs: */ {
           "obj",
   }, /* outputs: */ {
   }, /* params: */ {
   }, /* category: */ {
           "demo_project",
   }});
