#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MetaDataPluginSettings.generated.h"

USTRUCT(BlueprintType)
struct FMetaDataPluginSetting
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ContentDir, DisplayName = "Directory"))
	FDirectoryPath Directory;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Recursive"))
	bool bRecursive;
};

/**
 * MetaData 插件的项目设置
 *
 */
UCLASS(Config=Game, DefaultConfig)
class TEST_API UMetaDataPluginSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UMetaDataPluginSettings() {};

	// DeleteMode
	// 项目设置中显示的文件夹选取器
	UPROPERTY(Config, EditAnywhere, meta = (DisplayName = "Scan Directory"))
	TArray<FMetaDataPluginSetting> ScanDirectory;

	// DeleteMode
	// ConfigRestartRequired = true 表示修改配置需要重启编辑器
	UPROPERTY(Config, EditAnywhere, meta=(ConfigRestartRequired=true))
	bool bAllowManageMetaDataDelete = false;

	// UPROPERTY(Config, EditAnywhere)
	// TSoftObjectPtr<UEditorUtilityWidgetBlueprint> EUWBP_MetaData;

	
	// 保存主控件对应的资产路径
	UPROPERTY(Config)
	FString MainWidgetPath;

	// 更新配置文件
	void SaveMainWidgetPath(const FString& InMainWidgetPath);
	
	// Project Settings -> Plugins -> MetaDataPlugin
	// 项目设置 -> 插件 -> 元数据插件
	virtual FName GetCategoryName() const override { return FName(TEXT("Plugins")); }

#if WITH_EDITOR
	virtual FText GetSectionText() const override { return NSLOCTEXT("MetaDataPlugin", "MetaDataPluginSettingsSection", "元数据插件"); }
#endif
	
};

