// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Framework/Docking/TabManager.h"


class FtestModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// 点击插件的按钮
	void OnButtonClicked();

	// 确保主窗口的资产移动后，下次启动编辑器依然能打开
	void OnAssetMoved(const FAssetData& AssetData, const FString& OldObjectPath);
	// 有资产被删除时
	void OnAssetRemoved(const FAssetData& AssetData);

	// 启动项目时，加载主窗口的资产，默认名字是 EUWBP_MetaData
	void OnPostEngineInit();

	// 关闭所有插件窗口
	void RemoveAllWidget();

	// 项目设置中快捷键被修改时，重新绑定快捷键（热重载）
	void UpdateShortcutBindings();

	// 项目设置变更回调 — OnSettingChanged 委托触发时调用
	// void OnSettingsChanged(UObject* SettingsObject, struct FPropertyChangedEvent& PropertyChangedEvent);

	// Test
	// void TestAction();
	// TSharedRef<SDockTab> SpawnMyTab(const FSpawnTabArgs& Args);
	// End Test
private:
	// Test
	// const FName MyTabName = "MyTab";
	// End Test
	
	TSharedPtr<class FUICommandList> PluginCommands;

	// 保存主窗口资产在内容浏览器中的当前路径
	FString CurrentPath;
	
	/** 项目设置变更委托的句柄，用于解绑 */
	// FDelegateHandle SettingsChangedHandle;
};
