// TestCommands.cpp — UI 命令注册
// 注册 Command_OpenMain 和 Command_CloseAll 两个命令，快捷键默认从项目设置读取

#include "Command/TestCommands.h"
#include "Style/TestStyle.h"
#include "testSettings.h"

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
	// 从项目设置读取快捷键配置作为默认值
	// const UtestSettings* Settings = GetDefault<UtestSettings>();

	// FInputChord OpenMainChord = Settings ? Settings->OpenMainChord.ToInputChord() : FInputChord(EModifierKey::Shift | EModifierKey::Alt, EKeys::Z);
	// FInputChord CloseAllChord = Settings ? Settings->CloseAllChord.ToInputChord() : FInputChord(EModifierKey::Shift | EModifierKey::Alt, EKeys::X);

	UI_COMMAND(Command_OpenMain, "Command_OpenMain", "Open Main Widget", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Shift | EModifierKey::Alt, EKeys::Z));
	UI_COMMAND(Command_CloseAll, "Command_CloseAll", "Close All Widget", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Shift | EModifierKey::Alt, EKeys::X));
}

#undef LOCTEXT_NAMESPACE
