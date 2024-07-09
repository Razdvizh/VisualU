// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_ChooseVersion.h"
#include "VisualVersioningSubsystem.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintFunctionNodeSpawner.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ToolMenu.h"
#include "EdGraph/EdGraphNode.h"


#define LOCTEXT_NAMESPACE "VisualUBlueprint"

UK2Node_ChooseVersion::UK2Node_ChooseVersion(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	PinIndex(-1)
{
	FunctionReference.SetFromField<UFunction>(UVisualVersioningSubsystem::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UVisualVersioningSubsystem, ChooseVersion)), false);
}

void UK2Node_ChooseVersion::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* NodeClass = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(NodeClass))
	{
		UBlueprintFunctionNodeSpawner* Spawner = UBlueprintFunctionNodeSpawner::Create(NodeClass, GetTargetFunction());
		check(Spawner);

		ActionRegistrar.AddBlueprintAction(NodeClass, Spawner);
	}
}

void UK2Node_ChooseVersion::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);

	if (!Context->bIsDebugging)
	{
		static FName ChooseVersionNodeName = FName("ChooseVersionNode");
		const FText ChooseVersionNodeLabel = LOCTEXT("ChooseVersionNodeLabel", "Choose Version");
		if (Context->Pin)
		{
			if (CanRemovePin(Context->Pin))
			{
				FToolMenuSection& Section = Menu->AddSection(ChooseVersionNodeName, ChooseVersionNodeLabel);
				Section.AddMenuEntry(
					"RemovePin",
					LOCTEXT("RemovePin", "Remove pin"),
					LOCTEXT("RemovePinTooltip", "Remove this input pin"),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateUObject(const_cast<UK2Node_ChooseVersion*>(this), &UK2Node_ChooseVersion::RemoveInputPin, const_cast<UEdGraphPin*>(Context->Pin))
					)
				);
			}
		}
	}
}

bool UK2Node_ChooseVersion::IncludeParentNodeContextMenu() const
{
	return true;
}

void UK2Node_ChooseVersion::AddInputPin()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, GetNameForAdditionalPin(++PinIndex));

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

bool UK2Node_ChooseVersion::CanAddPin() const
{
	return PinIndex < GetMaxInputPinsNum();
}

void UK2Node_ChooseVersion::RemoveInputPin(UEdGraphPin* Pin)
{
	if (CanRemovePin(Pin))
	{
		RemovePin(Pin);
		Modify();
	}

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

bool UK2Node_ChooseVersion::CanRemovePin(const UEdGraphPin* Pin) const
{
	return (
		Pin &&
		Pin->ParentPin == nullptr &&
		(INDEX_NONE != Pins.IndexOfByKey(Pin)) &&
		(EEdGraphPinDirection::EGPD_Input == Pin->Direction)
	);
}

#undef LOCTEXT_NAMESAPCE
