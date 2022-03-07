
#pragma once

#include <knoting/engine.h>
#include <knoting/input_manager.h>

namespace knot {
class Window;
}

namespace knot {
class Untie {
   public:
    Untie();

    void run();

   private:
    std::unique_ptr<knot::Engine> m_engine;
    std::unique_ptr<knot::Engine> m_loadEngine;
};

}  // namespace knot
