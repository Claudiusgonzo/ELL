////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueOperations.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "MemoryLayout.h"

// stl
#include <functional>

namespace ell
{
namespace value
{
    class Value;
    class Scalar;

    /// <summary> Creates a for loop over the given layout </summary>
    /// <param name="layout"> The layout used to describe the iteration characteristics. Only active elements are iterated over. </param>
    /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
    void For(utilities::MemoryLayout layout, std::function<void(Scalar)> fn);

} // namespace value
} // namespace ell
