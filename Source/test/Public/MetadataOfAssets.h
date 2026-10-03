// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Framework/Docking/TabManager.h"

class FUICommandList;

class FMetadataOfAssetsModule : public IModuleInterface
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
	// 有资产被保存时
	// void OnAssetSaved(const FString& String, UPackage* Package, FObjectPostSaveContext Context);

	// 引擎初始化完成的回调
	void OnPostEngineInit();

	// 关闭所有创建的窗口
	void RemoveAllWidget();

	// 项目设置中快捷键被修改时，重新绑定快捷键（热重载）
	void BindCommandActions();

	// 加载主窗口的资产
	// void LoadMainEUWBP();
	
	// 扫描配置路径中的资产
	void ScanAssetsWithConfiguration();

private:
	
	// 绑定命令和具体的操作
	TSharedPtr<FUICommandList> PluginCommands;

};
