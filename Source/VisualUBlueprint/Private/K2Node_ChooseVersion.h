// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_AddPinInterface.h"
#include "K2Node_CallFunction.h"
#include "EdGraph/EdGraphPin.h"
#include "K2Node_ChooseVersion.generated.h"

class FBlueprintActionDatabaseRegistrar;
class UEdGraphPin;

/**
 * Custom blueprint node for UVisualVersioningSubsystem::ChooseVersion
 */
UCLASS()
class UK2Node_ChooseVersion : public UK2Node_CallFunction, public IK2Node_AddPinInterface
{
	GENERATED_BODY()
	
public:
	UK2Node_ChooseVersion(const FObjectInitializer& ObjectInitializer);

	virtual void AllocateDefaultPins() override;

	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;

	virtual bool DoesInputWildcardPinAcceptArray(const UEdGraphPin* Pin) const override;

	virtual bool DoesOutputWildcardPinAcceptContainer(const UEdGraphPin* Pin) const override;

	virtual bool IncludeParentNodeContextMenu() const override;

	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;

	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;

	virtual void AddInputPin() override;

	virtual bool CanAddPin() const override final;

	virtual void RemoveInputPin(UEdGraphPin* Pin) override;

	bool CanRemovePin(const UEdGraphPin* Pin) const override;

protected:
	virtual void AddVisualScenarioInfoMembers(TArray<FName>& Members);

	void ForEachAddedPin(TFunctionRef<void(UEdGraphPin*)> Action) const;

private:
	TArray<FName> ScenarioInfoMembers;

	/*
	* Location to which AddInputPin can add its first pin.
	* Next locations are guaranteed to contain added input pins only.
	*/
	int32 AddPinIndex;

	int32 NumAddedPins;

};
