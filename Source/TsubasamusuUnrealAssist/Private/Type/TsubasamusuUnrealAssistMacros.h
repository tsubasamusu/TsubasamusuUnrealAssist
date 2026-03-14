// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "Misc/EngineVersionComparison.h"

#ifndef UE_VERSION_NEWER_THAN_OR_EQUAL
#define UE_VERSION_NEWER_THAN_OR_EQUAL(MajorVersion, MinorVersion, PatchVersion)\
UE_GREATER_SORT(ENGINE_MAJOR_VERSION, MajorVersion, UE_GREATER_SORT(ENGINE_MINOR_VERSION, MinorVersion, UE_GREATER_SORT(ENGINE_PATCH_VERSION, PatchVersion, true)))
#endif

#define CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED (UE_VERSION_NEWER_THAN_OR_EQUAL(5, 3, 0))
