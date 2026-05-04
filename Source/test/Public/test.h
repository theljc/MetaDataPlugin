// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FtestModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	// 点击插件的按钮
	void OnButtonClicked();
	
	// 主窗口的资产移动时
	void OnAssetMoved(const FAssetData& AssetData, const FString& OldObjectPath);
	
	// 启动项目时，加载主窗口的资产，默认名字是 EUWBP_MetaData
	void OnPostEngineInit();
	
private:
	TSharedPtr<class FUICommandList> PluginCommands;
	
	// 保存主窗口资产在内容浏览器中的当前路径
	FString CurrentPath;
	
};
