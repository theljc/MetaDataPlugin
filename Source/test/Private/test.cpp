// Copyright Epic Games, Inc. All Rights Reserved.

#include "test.h"

#include "BlueprintEditorLibrary.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "MetaData/EUSS_MetaDataManager.h"
#include "LevelEditor.h"
#include "Command/TestCommands.h"
#include "Style/TestStyle.h"
#include "testSettings.h"
#include "TestPluginChord.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Widget/EUSS_WidgetManager.h"

// 属性自定义
#include "PropertyEditorModule.h"
#include "Interfaces/IMainFrameModule.h"
#include "Widgets/Layout/SConstraintCanvas.h"

#define LOCTEXT_NAMESPACE "FtestModule"

void FtestModule::StartupModule()
{
	// 初始化
	FTestCommands::Register();
	FTestStyle::Initialize();
	FTestStyle::ReloadTextures();

	// 2. 注册 Tab 的生成器（通常在 StartupModule 中）
	// FGlobalTabmanager::Get()->RegisterTabSpawner(MyTabName, FOnSpawnTab::CreateRaw(this, &FtestModule::SpawnMyTab))
		// .SetDisplayName(FText::FromString("My Tab"));

	
	// 创建命令列表
	PluginCommands = MakeShareable(new FUICommandList);

	// 用 MainFrame 模块，确保聚焦到 EUW 时也能触发快捷键
	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
    
	// 获取主框架的命令列表
	PluginCommands = MainFrameModule.GetMainFrameCommandBindings();
	
	// 绑定快捷键和回调函数
	UpdateShortcutBindings();

	// 订阅项目设置变更 — 实现快捷键热重载（修改后立即生效，无需重启编辑器）
	// SettingsChangedHandle = GetMutableDefault<UtestSettings>()->OnSettingChanged().AddRaw(
	// 	this, &FtestModule::OnSettingsChanged);

	// 注册 FTestPluginChord 的属性自定义显示（Keyboard Shortcuts 风格）
	// FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	// PropertyEditor.RegisterCustomPropertyTypeLayout(
	// 	FTestPluginChord::StaticStruct()->GetFName(),
		// FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTestPluginChordCustomization::MakeInstance));

	// 获得关卡编辑器模块，通过此模块可以访问编辑器的各个扩展点
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	// 扩展关卡编辑器的工具栏
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

		// 将按钮添加在名为 "Play" 的工具栏区块之后
		ToolbarExtender->AddToolBarExtension("Play"
			, EExtensionHook::After
			, PluginCommands
			, FToolBarExtensionDelegate::CreateLambda([](FToolBarBuilder& Builder)
				{
					Builder.AddToolBarButton(FTestCommands::Get().Command_OpenMain);
					// Builder.AddToolBarButton(FTestCommands::Get().Command_CloseAll);
				}));

		// 使按钮出现在工具栏
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}

	// 持有此引用以确保命令在编辑器会话期间持续有效
	{
		TSharedRef<FUICommandList> LevelEditorCommandList = LevelEditorModule.GetGlobalLevelEditorActions();
		LevelEditorCommandList->Append(PluginCommands.ToSharedRef());
	}

	// 获得资产注册表模块
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	// 绑定资产删除事件
	AssetRegistry.OnAssetRemoved().AddRaw(this, &FtestModule::OnAssetRemoved);
	// 移动资产时也会触发重命名事件
	AssetRegistry.OnAssetRenamed().AddRaw(this, &FtestModule::OnAssetMoved);
	
	// 引擎初始化完成后的回调
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FtestModule::OnPostEngineInit);
}

void FtestModule::ShutdownModule()
{
	// 取消订阅项目设置变更
	// if (SettingsChangedHandle.IsValid())
	// {
	// 	GetMutableDefault<UtestSettings>()->OnSettingChanged().Remove(SettingsChangedHandle);
	// 	SettingsChangedHandle.Reset();
	// }

	// 取消注册属性自定义
	// if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	// {
	// 	FPropertyEditorModule& PropertyEditor = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	// 	PropertyEditor.UnregisterCustomPropertyTypeLayout(FTestPluginChord::StaticStruct()->GetFName());
	// }

	FTestCommands::Unregister();
	FTestStyle::Shutdown();
}

// ==================== 快捷键热重载 ====================

// void FtestModule::OnSettingsChanged(UObject* SettingsObject, FPropertyChangedEvent& PropertyChangedEvent)
// {
// 	// 项目设置中快捷键被修改 → 立即重新绑定
// 	UE_LOG(LogTemp, Log, TEXT("[FtestModule] 项目设置快捷键已变更，热重载中..."));
// 	UpdateShortcutBindings();
// }

// 3. 实现 Spawn 回调，返回一个 SDockTab
// TSharedRef<SDockTab> FtestModule::SpawnMyTab(const FSpawnTabArgs& Args)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("%s"), *Args.GetTabId().TabType.ToString());
// 	
// 	return SNew(SDockTab)
// 		.Label(FText::FromString("New Tab"))
// 		[
// 			SNew(SConstraintCanvas)
// 			+ SConstraintCanvas::Slot()
// 			.Alignment(0.5f)
// 			.Anchors(0.5f)
// 			.Offset(FMargin(0.0f, 0.0f, 300.f, 100.f))
// 			.AutoSize(true)
// 			[
// 				SNew(STextBlock)
// 				.Text(FText::FromString("This is a Tab content!"))
// 			]
// 		];
// }

// void FtestModule::TestAction()
// {
// 	TSharedPtr<SDockTab> NewTab = FGlobalTabmanager::Get()->TryInvokeTab(MyTabName);
// }

void FtestModule::UpdateShortcutBindings()
{
	if (!PluginCommands.IsValid())
	{
		return;
	}

	// 先清空已有的绑定
	// PluginCommands->UnmapAction(FTestCommands::Get().Command_OpenMain);
	// PluginCommands->UnmapAction(FTestCommands::Get().Command_CloseAll);
	//
	// // 从项目设置读取快捷键配置
	// const UtestSettings* Settings = GetDefault<UtestSettings>();
	// FInputChord OpenMainChord = Settings ? Settings->OpenMainChord.ToInputChord() : FInputChord(EModifierKey::Shift | EModifierKey::Alt, EKeys::Z);
	// FInputChord CloseAllChord = Settings ? Settings->CloseAllChord.ToInputChord() : FInputChord(EModifierKey::Shift | EModifierKey::Alt, EKeys::X);
	//
	// // 设置命令的活跃快捷键（实时生效）
	// FTestCommands::Get().Command_OpenMain->SetActiveChord(OpenMainChord, EMultipleKeyBindingIndex::Primary);
	// FTestCommands::Get().Command_CloseAll->SetActiveChord(CloseAllChord, EMultipleKeyBindingIndex::Primary);

	// 绑定执行函数
	PluginCommands->MapAction(
		FTestCommands::Get().Command_OpenMain,
		FExecuteAction::CreateRaw(this, &FtestModule::OnButtonClicked));

	PluginCommands->MapAction(
		FTestCommands::Get().Command_CloseAll,
		FExecuteAction::CreateRaw(this, &FtestModule::RemoveAllWidget));
		// FExecuteAction::CreateRaw(this, &FtestModule::RemoveAllWidget));

	// UE_LOG(LogTemp, Log, TEXT("[FtestModule] 快捷键已热更新：OpenMain=%s, CloseAll=%s"),
	// 	*OpenMainChord.GetInputText().ToString(),
	// 	*CloseAllChord.GetInputText().ToString());
}

// ==================== 控件操作 ====================

void FtestModule::RemoveAllWidget()
{
	UEUSS_WidgetManager* WidgetManager = GEditor->GetEditorSubsystem<UEUSS_WidgetManager>();
	if (WidgetManager)
	{
		WidgetManager->CloseAllWidgets();
	}
}

void FtestModule::OnButtonClicked()
{
	// 点击按钮时，获得子系统，并创建主窗口
	UEUSS_WidgetManager* WidgetManager = GEditor->GetEditorSubsystem<UEUSS_WidgetManager>();
	if (WidgetManager)
	{
		WidgetManager->CreateMainWidget();
	}
}

void FtestModule::OnAssetMoved(const FAssetData& AssetData, const FString& OldObjectPath)
{
	// 为了保证下次启动项目时能读取并加载主窗口的资产，需要保存到配置文件
	const FString Path = FPaths::ProjectDir() + TEXT("Config/DefaultGame.ini");

	// 读取配置文件
	GConfig->GetString(TEXT("MetaDataPluginConfig"),
		TEXT("CurrentAssetPath"),
		CurrentPath,
		Path  // 指向项目的 DefaultGame.ini
	);

	// 没有值时，表示未移动过资产，需要设置一个默认值，用于后续判断移动的资产是否是主窗口的资产
	if (CurrentPath.IsEmpty())
	{
		CurrentPath = TEXT("/test/元数据Widget/EUWBP_MetaData.EUWBP_MetaData");
	}

	if (CurrentPath == OldObjectPath)
	{
		// 移动的是主窗口的资产，则保存新的路径
		GConfig->SetString(TEXT("MetaDataPluginConfig"), TEXT("CurrentAssetPath"), *AssetData.GetSoftObjectPath().ToString(), Path);
	}

	// 刷新配置文件，使改动立即生效
	GConfig->Flush(true, Path);
}

void FtestModule::OnAssetRemoved(const FAssetData& AssetData)
{
	UEUSS_MetaDataManager* MetaDataManager = GEditor->GetEditorSubsystem<UEUSS_MetaDataManager>();
	if (MetaDataManager)
	{
		MetaDataManager->OnAssetRemoved(AssetData);
	}
}

void FtestModule::OnPostEngineInit()
{
	UEUSS_WidgetManager* EUSS_WidgetManager = GEditor->GetEditorSubsystem<UEUSS_WidgetManager>();
	if (EUSS_WidgetManager)
	{
		const FString Path = FPaths::ProjectDir() + TEXT("Config/DefaultGame.ini");

		// 读取配置文件的主窗口资产的路径
		GConfig->GetString(TEXT("MetaDataPluginConfig"),
			TEXT("CurrentAssetPath"),
			CurrentPath,
			Path  // 指向项目的 DefaultGame.ini
		);

		// 设置一个默认值
		FSoftObjectPath EUWBP_Path(TEXT("/test/元数据Widget/EUWBP_MetaData.EUWBP_MetaData"));

		// 判断是否读取到了配置文件中的资产路径
		if (!CurrentPath.IsEmpty())
		{
			// 读取成功，则将配置文件中的资产路径设置为主窗口的资产路径，失败则使用默认值
			EUWBP_Path = FSoftObjectPath(CurrentPath);
		}

		// 赋值给软引用
		EUSS_WidgetManager->Set_EUWBP(TSoftObjectPtr<UEditorUtilityWidgetBlueprint>(EUWBP_Path));

		// 加载资产才能使软引用一开始就生效
		EUSS_WidgetManager->Get_EUWBP().LoadSynchronous();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FtestModule, test)
