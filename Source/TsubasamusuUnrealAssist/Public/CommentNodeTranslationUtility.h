// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UEdGraphNode_Comment;

class TSUBASAMUSUUNREALASSIST_API FCommentNodeTranslationUtility
{
public:
	
	static void AddCommentNodeTranslationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);

private:

	static void TranslateCommentNode(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);
};
