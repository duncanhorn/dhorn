/*
 * d3d.h
 *
 * Duncan Horn
 *
 * Includes many useful classes and functions for using the DirectX library. This file is simply a front for including
 * other files, though it is not recommended that clients #include those files directly as many of them rely on the
 * inclusion of other files that are #include'd from d3d.h such as Windows.h, xnamath.h, etc.
 */
#pragma once

#ifndef WIN32
static_assert(false, "ERROR: DirectX not supported on non-Windows operating systems");
#endif  /* WIN32 */

/* Windows/DirectX library includes */
#pragma warning(push)
#pragma warning(disable:4838)
#include <Windows.h>
#include <DirectXMath.h>
#pragma warning(pop)



/* Additional header files */
#include "d3d/vector.h"
