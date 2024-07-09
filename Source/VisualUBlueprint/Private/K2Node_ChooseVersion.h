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

	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;

	virtual bool IncludeParentNodeContextMenu() const override;

	virtual void AddInputPin() override;

	virtual bool CanAddPin() const override;

	virtual void RemoveInputPin(UEdGraphPin* Pin) override;

	bool CanRemovePin(const UEdGraphPin* Pin) const override;

private:
	int8 PinIndex;

};
