/**
 * \dir
 * Contains the non-generated public header files for libqasm's new API.
 *
 * \file
 * Main include file for libqasm; this is what you should be `#include`ing.
 */

#pragma once

#include "cqasm-v1.hpp"
#include "cqasm-v2.hpp"

/**
 * Toplevel namespace with entry points for the new API.
 */
namespace cqasm {

// This is the toplevel public header for the new API for v1.x. At the time it
// was written, v2 didn't exist yet, and such the v1 namespace didn't exist
// yet. Thus, to maintain API compatibility, we need to pull the v1 namespace
// into this one when this file is used.
using namespace v1;

} // namespace cqasm
