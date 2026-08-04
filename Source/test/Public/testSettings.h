// // testSettings.h — 插件快捷键项目设置
// // 继承 UDeveloperSettings，将快捷键配置暴露到 项目设置 → 插件 → Test Plugin
// // 快捷键可在编辑器中直接修改，立即生效，无需重启
//
// #pragma once
//
// #include "CoreMinimal.h"
// #include "Engine/DeveloperSettings.h"
// #include "TestPluginChord.h"
// #include "testSettings.generated.h"
//
// /**
//  * UtestSettings — Test 插件的项目设置
//  *
//  * 在编辑器中通过 编辑 → 项目设置 → 插件 → Test Plugin 进行配置。
//  * 快捷键修改后立即生效（热重载）。
//  */
// UCLASS(config=Editor, defaultconfig, meta=(DisplayName="Test Plugin"))
// class TEST_API UtestSettings : public UDeveloperSettings
// {
// 	GENERATED_BODY()
//
// public:
// 	UtestSettings();
//
// 	// ==================== 快捷键配置 ====================
//
// 	/** 打开/聚焦主窗口的快捷键 */
// 	// UPROPERTY(config, EditAnywhere, Category = "Shortcuts")
// 	// FTestPluginChord OpenMainChord;
// 	//
// 	// /** 关闭所有插件窗口的快捷键 */
// 	// UPROPERTY(config, EditAnywhere, Category = "Shortcuts")
// 	// FTestPluginChord CloseAllChord;
//
// 	// ==================== UDeveloperSettings 接口 ====================
//
// 	virtual FName GetCategoryName() const override { return FName(TEXT("Plugins")); }
//
// #if WITH_EDITOR
// 	virtual FText GetSectionText() const override { return NSLOCTEXT("TestPlugin", "TestSettingsSection", "Test Plugin"); }
// #endif
// };
