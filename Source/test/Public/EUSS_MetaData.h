// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Public/EditorUtilitySubsystem.h"
#include "EUSS_MetaData.generated.h"

class UWidget;
/**
 * 
 */
UCLASS()
class TEST_API UEUSS_MetaData : public UEditorUtilitySubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	// 添加标签到资产注册表
	UFUNCTION(BlueprintCallable)
	void AddTagFromAssetRegistry(const FName TagToAdd);
	
	// 从资产注册表移除标签
	UFUNCTION(BlueprintCallable)
	void RemoveTagFromAssetRegistry(const FName TagToRemove);
	
	// 打开模态框
	UFUNCTION(BlueprintCallable)
	void OpenModalDialog();
	
	// 注册标签到全局资产注册表
	void OnTagFirstAdded(const FName TagToAdd);
	
	// 从全局资产注册表中移除
	void OnTagRemoved(const FName TagToRemove);
	// 从全局移除时，用于移除配置文件中的对应标签
	FString RemoveTagFromMetaDataString(const FString& InputString, const FName& TagToRemove);
	
	// 创建主窗口
	void CreateWidgetBP();
	
	UFUNCTION(BlueprintCallable)
	FVector2D GetAbsolutePosition(UWidget* Widget);
	
	// 保存主窗口的蓝图资产
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Plugin Settings")
	TSoftObjectPtr<UEditorUtilityWidgetBlueprint> My_EUBP;
	
	// 保存模态框的蓝图资产
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Plugin Settings")
	TSubclassOf<UUserWidget> BatchAddWidget;

private:
	// 以引用计数的方式保存已注册的标签
	TMap<FName, int32> RegisteredTags;
	
	
};
