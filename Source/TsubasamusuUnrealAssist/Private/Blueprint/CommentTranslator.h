// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UEdGraphNode_Comment;

class FCommentTranslator final
{
public:
	static void AddCommentTranslationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);

private:
	static void AddLanguageSubMenus(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);
	static TArray<FString> GetEditorLanguages();
	
	static void TranslateComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode, const TSharedPtr<const FString> InTranslationTargetLanguage);
	static FString GetDeeplJsonRequest(const FString& InSourceText, const FString& InTargetLanguage);
	static void FixLanguage(FString& OutLanguage);
};
