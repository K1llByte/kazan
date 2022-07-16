#include "cvar.hpp"

namespace kzn
{
    class CVarSystemImpl : public CVarSystem
    {

    };

    CVarSystem* CVarSystem::get()
    {
        static CVarSystemImpl cvar_sys{};
        return &cvar_sys;
    }
} // namespace kzn
