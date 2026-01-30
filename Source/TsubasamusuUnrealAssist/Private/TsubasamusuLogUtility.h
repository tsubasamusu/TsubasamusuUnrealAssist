// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTsubasamusuUnrealAssist, Log, All);

#define TUA_LOG(Format, ...) \
{ \
	UE_LOG(LogTsubasamusuUnrealAssist, Log, TEXT("%s:%d: " Format), TEXT(__FILE__), __LINE__, ##__VA_ARGS__); \
}

#define TUA_WARNING(Format, ...) \
{ \
	UE_LOG(LogTsubasamusuUnrealAssist, Warning, TEXT("%s:%d: " Format), TEXT(__FILE__), __LINE__, ##__VA_ARGS__); \
}

#define TUA_ERROR(Format, ...) \
{ \
	UE_LOG(LogTsubasamusuUnrealAssist, Error, TEXT("%s:%d: " Format), TEXT(__FILE__), __LINE__, ##__VA_ARGS__); \
}
