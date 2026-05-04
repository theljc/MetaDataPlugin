


#include "TestCommands.h"

#include "TestStyle.h"

#define LOCTEXT_NAMESPACE "FTestPluginModule"

FTestCommands::FTestCommands() : TCommands<FTestCommands>(
	"TestCommands",
	NSLOCTEXT("Contexts", "TestCommands", "Test Plugin"),
	NAME_None,
	FTestStyle::GetStyleSetName())
{

}

void FTestCommands::RegisterCommands()
{
	UI_COMMAND(CommandA, "TestCommandA", "Execute Test CommandA", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Shift | EModifierKey::Alt, EKeys::Z));
}

