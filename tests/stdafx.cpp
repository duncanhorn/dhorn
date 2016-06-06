// stdafx.cpp : source file that includes just the standard includes
// tests.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

using namespace dhorn::tests;

size_t object_counter::instance_count = 0;
size_t object_counter::constructed_count = 0;
size_t object_counter::destructed_count = 0;
size_t object_counter::copy_count = 0;
size_t object_counter::move_count = 0;
