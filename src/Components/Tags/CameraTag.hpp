#pragma once

#include <cstddef>

namespace Components
{
namespace Tags
{

struct CameraTag
{
    size_t Influence;

    CameraTag() : Influence(1) { }
};

}
}
