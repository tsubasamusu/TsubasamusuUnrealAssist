// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UEdGraphNode_Comment;
struct FSelectedNodeMenuContext;
struct FSelectedNodeSubMenuContext;

class FCommentTranslator final
{
public:
	static FSelectedNodeMenuContext CreateSelectedNodeMenuContext();
	
private:
	static TArray<FSelectedNodeSubMenuContext> CreateSelectedNodeSubMenuContext();
	static TMap<FString, FText> GetEditorLanguages();
	static void TranslateComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode, const FString& InTranslationTargetLanguage);
	static void FixLanguage(FString& OutLanguage);
};
