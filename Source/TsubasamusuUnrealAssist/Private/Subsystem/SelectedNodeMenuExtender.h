// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystemBase.h"
#include "GraphEditorModule.h"
#include "SelectedNodeMenuExtender.generated.h"

struct FSelectedNodeMenuContext;
using FExtendSelectedNodeMenuDelegate = FGraphEditorModule::FGraphEditorMenuExtender_SelectedNode;

UCLASS()
class USelectedNodeMenuExtender final : public UEditorSubsystem, public TEditorSubsystemBase<USelectedNodeMenuExtender>
{
	GENERATED_BODY()
	
public:
	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& InSubsystemCollectionBase) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface
	
private:
	void RegisterSelectedNodeMenu();
	void UnregisterSelectedNodeMenu() const;
	
	bool IsMyDelegate(const FExtendSelectedNodeMenuDelegate& InExtendSelectedNodeMenuDelegate) const;
	static TArray<TWeakObjectPtr<UEdGraphNode>> GetSelectedNodes(const UEdGraph* InGraph);
	
	FExtendSelectedNodeMenuDelegate ExtendSelectedNodeMenuDelegate;
	TArray<FSelectedNodeMenuContext> SelectedNodeMenuContexts;
};
