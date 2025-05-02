// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuUnrealAssistSettings.h"
#include "CommentNodeType.h"

UTsubasamusuUnrealAssistSettings::UTsubasamusuUnrealAssistSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CommentNodeType = ECommentNodeType::Normal;
}
