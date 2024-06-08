#include "VisualUEditor.h"
#include "SpriteStructCustomization.h"
#include "PropertyEditorModule.h"

IMPLEMENT_MODULE(FVisualUEditorModule, VisualUEditor);

void FVisualUEditorModule::StartupModule()
{
	//FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	//PropertyEditorModule.RegisterCustomPropertyTypeLayout(TEXT("Sprite"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FSpriteStructCustomization::MakeInstance));
    //PropertyEditorModule.NotifyCustomizationModuleChanged();
}

void FVisualUEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded(TEXT("PropertyEditor")))
	{
		//FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		//PropertyEditorModule.UnregisterCustomPropertyTypeLayout(TEXT("Sprite"));
		//PropertyEditorModule.NotifyCustomizationModuleChanged();
	}
}
