// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UEdGraphNode_Comment;

class TSUBASAMUSUUNREALASSIST_API FCommentGenerationUtility final
{
public:
	
	static void AddCommentGenerationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);

private:

	static void GenerateComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);
};
