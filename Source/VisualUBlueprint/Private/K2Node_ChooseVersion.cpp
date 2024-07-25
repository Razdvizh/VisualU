// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_ChooseVersion.h"
#include "VisualVersioningSubsystem.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintFunctionNodeSpawner.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/WildcardNodeUtils.h"
#include "ToolMenu.h"
#include "EdGraph/EdGraphNode.h"

#define LOCTEXT_NAMESPACE "VisualUBlueprint"

UK2Node_ChooseVersion::UK2Node_ChooseVersion(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	ScenarioInfoMembers(),
	AddPinIndex(-1),
	NumAddedPins(0)
{
	AddVisualScenarioInfoMembers(ScenarioInfoMembers);
	FunctionReference.SetFromField<UFunction>(UVisualVersioningSubsystem::StaticClass()->FindFunctionByName(FName(TEXT("ChooseVersion"))), false);
}

void UK2Node_ChooseVersion::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	AddPinIndex = Pins.Num();
}

void UK2Node_ChooseVersion::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
}

void UK2Node_ChooseVersion::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	//UClass* NodeClass = GetClass();
	//if (ActionRegistrar.IsOpenForRegistration(NodeClass))
	//{
	//	UBlueprintFunctionNodeSpawner* Spawner = UBlueprintFunctionNodeSpawner::Create(NodeClass, GetTargetFunction());
	//	check(Spawner);

	//	ActionRegistrar.AddBlueprintAction(NodeClass, Spawner);
	//}
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
				FToolMenuSection& RemovePinSection = Menu->AddSection(ChooseVersionNodeName, ChooseVersionNodeLabel);
				RemovePinSection.AddMenuEntry(
					TEXT("RemovePin"),
					LOCTEXT("RemovePin", "Remove pin"),
					LOCTEXT("RemovePinTooltip", "Remove this input pin"),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateUObject(const_cast<UK2Node_ChooseVersion*>(this), &UK2Node_ChooseVersion::RemoveInputPin, const_cast<UEdGraphPin*>(Context->Pin))
					)
				);
				FToolMenuSection& RemovePinsSection = Menu->AddSection(ChooseVersionNodeName, ChooseVersionNodeLabel);
				RemovePinsSection.AddMenuEntry(
					TEXT("RemovePins"),
					LOCTEXT("RemovePins", "Remove pins"),
					LOCTEXT("RemovePinsTooltip", "Remove all input pins"),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateLambda([this]()
						{
							UK2Node_ChooseVersion* MutableThis = const_cast<UK2Node_ChooseVersion*>(this);

							ForEachAddedPin([MutableThis](UEdGraphPin* Pin)
							{
								MutableThis->RemoveInputPin(Pin);
							});
						})
					)
				);
			}
		}
	}
}

bool UK2Node_ChooseVersion::DoesInputWildcardPinAcceptArray(const UEdGraphPin* Pin) const
{
	return true;
}

bool UK2Node_ChooseVersion::DoesOutputWildcardPinAcceptContainer(const UEdGraphPin* Pin) const
{
	return true;
}

bool UK2Node_ChooseVersion::IncludeParentNodeContextMenu() const
{
	return true;
}

void UK2Node_ChooseVersion::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	if (ensure(Pin))
	{
		for (int32 i = 0; i < NumAddedPins; i++)
		{
			UEdGraphPin* AddedPin = FindPin(GetNameForAdditionalPin(i), EEdGraphPinDirection::EGPD_Input);
			if (!AddedPin)
			{
				if (ScenarioInfoMembers.FindByKey(Pin->PinName))
				{
					AddedPin = Pin;
				}
				else
				{
					continue;
				}
			}

			if (Pin->LinkedTo.IsEmpty())
			{
				Pin->PinType = FWildcardNodeUtils::GetDefaultWildcardPinType();

				const FName InitialPinName = GetNameForAdditionalPin(Pins.IndexOfByKey(Pin) - AddPinIndex);
				Pin->PinName = InitialPinName;
			}
			else if (!FWildcardNodeUtils::IsLinkedToWildcard(Pin))
			{
				const UEdGraphPin* OtherPin = Pin->LinkedTo[0];
				Pin->PinType = OtherPin->PinType;
				Pin->PinName = OtherPin->PinName;
			}

			UEdGraphSchema_K2::ValidateExistingConnections(Pin);
			break;
		}
	}
}

bool UK2Node_ChooseVersion::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	bool bCantConnect = Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason);

	if (!bCantConnect && OtherPin)
	{
		if (FindPin(OtherPin->PinName, EEdGraphPinDirection::EGPD_Input))
		{
			bCantConnect = true;
			OutReason = LOCTEXT("IsConnectionDisallowedReason", "Provided property already connected to the node.").ToString();
		}
	}

	return bCantConnect;
}

void UK2Node_ChooseVersion::AddInputPin()
{
	FWildcardNodeUtils::CreateWildcardPin(this, GetNameForAdditionalPin(NumAddedPins++), EEdGraphPinDirection::EGPD_Input);

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

bool UK2Node_ChooseVersion::CanAddPin() const
{
	return NumAddedPins < FMath::Min(GetMaxInputPinsNum(), ScenarioInfoMembers.Num());
}

void UK2Node_ChooseVersion::RemoveInputPin(UEdGraphPin* Pin)
{
	if (CanRemovePin(Pin))
	{
		if (RemovePin(Pin))
		{
			--NumAddedPins;
			Modify();

			FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
		}
	}
}

bool UK2Node_ChooseVersion::CanRemovePin(const UEdGraphPin* Pin) const
{
	return (
		Pin &&
		NumAddedPins &&
		!Pin->ParentPin &&
		(EEdGraphPinDirection::EGPD_Input == Pin->Direction) &&
		(AddPinIndex <= Pins.IndexOfByKey(Pin))
	);
}

void UK2Node_ChooseVersion::AddVisualScenarioInfoMembers(TArray<FName>& Members)
{
	Members.Add(GET_MEMBER_NAME_CHECKED(FVisualScenarioInfo, Author));
	Members.Add(GET_MEMBER_NAME_CHECKED(FVisualScenarioInfo, Line));
	Members.Add(GET_MEMBER_NAME_CHECKED(FVisualScenarioInfo, Sound));
	Members.Add(GET_MEMBER_NAME_CHECKED(FVisualScenarioInfo, Background));
	Members.Add(GET_MEMBER_NAME_CHECKED(FVisualScenarioInfo, SpritesParams));
	Members.Add(GET_MEMBER_NAME_CHECKED(FVisualScenarioInfo, Flags));
}

void UK2Node_ChooseVersion::ForEachAddedPin(TFunctionRef<void(UEdGraphPin*)> Action) const
{
	const int32 Num = NumAddedPins;
	for (int32 i = Num - 1; i >= 0; i--)
	{
		if (Pins.IsValidIndex(AddPinIndex + i))
		{
			Action(Pins[AddPinIndex + i]);
		}
	}
}

#undef LOCTEXT_NAMESAPCE
