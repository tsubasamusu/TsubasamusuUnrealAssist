// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UEdGraphNode_Comment;

class FCommentTranslationUtility final
{
public:
	static void AddCommentTranslationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);

private:
	static void AddLanguageSubMenus(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);
	static TArray<FString> GetEditorLanguages();
	
	static void TranslateComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode, const TSharedPtr<const FString> TranslationTargetLanguage);
	static FString GetDeeplJsonRequest(const FString& SourceText, const FString& TargetLanguage);
	static void FixLanguage(FString& OutLanguage);
};
