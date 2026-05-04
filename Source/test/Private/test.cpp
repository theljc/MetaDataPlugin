// Copyright Epic Games, Inc. All Rights Reserved.

#include "test.h"

#include "BlueprintEditorLibrary.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EUSS_MetaData.h"
#include "LevelEditor.h"
#include "TestCommands.h"
#include "TestStyle.h"
#include "AssetRegistry/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "FtestModule"

void FtestModule::StartupModule()
{
	// 初始化
	FTestCommands::Register();
	FTestStyle::Initialize();
	FTestStyle::ReloadTextures();
	
	// 创建命令列表
	PluginCommands = MakeShareable(new FUICommandList);
	
	// 将命令和执行函数关联，按钮点击时触发回调函数
	PluginCommands->MapAction(
		FTestCommands::Get().CommandA,
		FExecuteAction::CreateRaw(this, &FtestModule::OnButtonClicked),
		FCanExecuteAction());
	
	// 获得关卡编辑器模块，通过此模块可以访问编辑器的各个扩展点
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	// 扩展关卡编辑器的菜单
	// {
	// 	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
	// 	MenuExtender->AddMenuExtension("WindowLayout"
	// 		, EExtensionHook::After
	// 		, PluginCommands
	// 		, FMenuExtensionDelegate::CreateLambda([](FMenuBuilder& Builder)
	// 			{
	// 				Builder.AddMenuEntry(FTestCommands::Get().CommandA);
	// 			}));
	// 	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	// }
	
	// 扩展关卡编辑器的工具栏
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		
		// 将按钮添加在名为 "Play" 的工具栏区块之后
		ToolbarExtender->AddToolBarExtension("Play"
			, EExtensionHook::After
			, PluginCommands
			, FToolBarExtensionDelegate::CreateLambda([](FToolBarBuilder& Builder)
				{
					Builder.AddToolBarButton(FTestCommands::Get().CommandA);
				
				}));
		
		// 使按钮出现在工具栏
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}

	// 将 CommandA 添加入关卡编辑器的 GlobalLevelEditorActions 中，这样可以触发快捷键
	{
		// 返回关卡编辑器使用的全局 FUICommandList 引用，编辑器的主快捷键绑定在这个命令列表上
		TSharedRef<FUICommandList> LevelEditorCommandList = LevelEditorModule.GetGlobalLevelEditorActions();
		
		// 映射快捷键触发回调函数
		LevelEditorCommandList->MapAction(
			FTestCommands::Get().CommandA,
			FExecuteAction::CreateRaw(this, &FtestModule::OnButtonClicked),
			FCanExecuteAction());
	}

	// 获得资产注册表
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
    
	// 绑定资产重命名事件
	AssetRegistry.OnAssetRenamed().AddRaw(this, &FtestModule::OnAssetMoved);
	// 引擎初始化完成后的回调
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FtestModule::OnPostEngineInit);
	
}

void FtestModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FTestCommands::Unregister();
	FTestStyle::Shutdown();
}

void FtestModule::OnButtonClicked()
{
	// 点击按钮时，获得子系统，并创建主窗口
	UEUSS_MetaData* My_EUSS = GEditor->GetEditorSubsystem<UEUSS_MetaData>();
	if (My_EUSS)
	{
		My_EUSS->CreateWidgetBP();
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

void FtestModule::OnPostEngineInit()
{
	UEUSS_MetaData* My_EUSS = GEditor->GetEditorSubsystem<UEUSS_MetaData>();
	if (My_EUSS)
	{
		const FString Path = FPaths::ProjectDir() + TEXT("Config/DefaultGame.ini");
		
		// 读取配置文件的主窗口资产的路径
		GConfig->GetString(TEXT("MetaDataPluginConfig"),
			TEXT("CurrentAssetPath"),
			CurrentPath,
			Path  // 指向项目的 DefaultGame.ini
		);
		
		// 设置一个默认值
		FSoftObjectPath My_EUBP_Path(TEXT("/test/元数据Widget/EUWBP_MetaData.EUWBP_MetaData"));
		
		// 判断是否读取到了配置文件中的资产路径
		if (!CurrentPath.IsEmpty())
		{
			// 读取成功，则将配置文件中的资产路径设置为主窗口的资产路径，失败则使用默认值
			My_EUBP_Path = FSoftObjectPath(CurrentPath);
		}
		
		// 赋值给软引用
		My_EUSS->My_EUBP = My_EUBP_Path;
		
		// 加载资产才能使软引用一开始就生效
		auto a = My_EUSS->My_EUBP.LoadSynchronous();
		
	}
}

// FString FtestModule::SplitObjectPath(FString ObjectPath, TCHAR InChar)
// {
// 	int32 DotIndex;
//
// 	if (ObjectPath.FindLastChar(InChar, DotIndex))
// 	{
// 		FString DirPart = ObjectPath.Left(DotIndex);    // 点之前的部分
// 		FString NamePart = ObjectPath.Mid(DotIndex + 1); // 点之后的部分
// 		return DirPart;
// 	}
// 	
// 	return FString();
// }

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FtestModule, test)