//
// Duncan Horn
//
// stdafx.h
//
// Unit tests for dhorn header files
//
#pragma once

#define _SCL_SECURE_NO_WARNINGS

#include "targetver.h"

// Headers for CppUnitTest
#pragma warning(push)
#pragma warning(disable:4499)
#pragma warning(disable:4389) // Signed unsigned mismatch due to char16_t/char32_t being unsigned, but char is signed
#include "CppUnitTest.h"
#pragma warning(pop)

#include "object_counter.h"
