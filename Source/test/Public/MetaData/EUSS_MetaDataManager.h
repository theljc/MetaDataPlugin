// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Public/EditorUtilitySubsystem.h"
#include "EUSS_MetaDataManager.generated.h"


// 由于动态委托不能传入 TMap 之类的复杂类型，所以用结构体替代
// USTRUCT(BlueprintType)
// struct FIntObjectPair
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	int32 Key;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	UObject* Value;
// };

struct FMetaDataPluginSetting;
// 创建 Widget 后，显示模态框前，将数据进行广播（因为模态框会暂停后续逻辑的执行）
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWidgetCreated, UObject*, AssetRef, const TArray<FIntObjectPair>&, AssetMap, UEditorUtilityWidget*, Widget);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBatchAddWidgetCreated, UEditorUtilityWidget*, EdtiorUtilityWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAssetDeleted, UObject*, Asset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMetaDataAdded, UObject*, Asset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMetaDataDeleted, UObject*, Asset);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMetaDataAdded, UObject*, AddedObject, , MetaData);

class UWidget;
/**
 * 
 */
UCLASS()
class TEST_API UEUSS_MetaDataManager : public UEditorUtilitySubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void TestRemove(UObject* Asset, FName TagToAdd);

	UPROPERTY(BlueprintAssignable)
	FOnMetaDataAdded OnMetaDataAdded;
	
	UPROPERTY(BlueprintAssignable)
	FOnMetaDataDeleted OnMetaDataDeleted;
	
	// 添加标签到资产注册表
	UFUNCTION(BlueprintCallable)
	void AddTagToRegisteredTags(const FName TagToAdd, UObject* Asset);
	
	// 从资产注册表移除标签
	UFUNCTION(BlueprintCallable)
	void RemoveTagFromRegisteredTags(const FName TagToRemove, UObject* Asset);
	
	// 注册标签到全局资产注册表
	void AddTagToAssetRegistry(const FName TagToAdd, UObject* Asset);
	
	// 从全局资产注册表中移除
	void RemoveTagFromAssetRegistry(const FName TagToRemove, UObject* Asset);
	// 从全局移除时，用于移除配置文件中的对应标签
	FString RemoveTagFromMetaDataString(const FString& InputString, const FName& TagToRemove);
	
	
	// 打开模态框
	// UFUNCTION(BlueprintCallable)
	// UUserWidget* OpenModalWindow(TSubclassOf<UUserWidget> BatchAddWidget, UObject* AssetRef, TArray<FIntObjectPair> AssetInfoMap, UEditorUtilityWidget* UtilityWidget, FVector2D WindowSize);
	
	// UFUNCTION(BlueprintCallable)
	// UEditorUtilityWidget* OpenModalWindow_EUW(UEditorUtilityWidgetBlueprint* InBlueprint, FVector2D WindowSize);
	
	
	// 创建主窗口
	// void CreateWidgetBP();
	
	// UFUNCTION(BlueprintCallable)
	// FVector2D GetAbsolutePosition(UWidget* Widget);
	
	// 保存主窗口的蓝图资产
	// UPROPERTY(BlueprintReadWrite, Category = "MetaData Plugin")
	// TSoftObjectPtr<UEditorUtilityWidgetBlueprint> My_EUBP;
	
	// 保存模态框的蓝图资产
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Plugin Settings")
	// TSubclassOf<UUserWidget> BatchAddWidget;

	// UPROPERTY(BlueprintAssignable)
	// FOnWidgetCreated OnWidgetCreated;

	// UPROPERTY(BlueprintAssignable)
	// FOnBatchAddWidgetCreated OnBatchAddWidgetCreated;

	// 广播委托，外部可绑定
	UPROPERTY(BlueprintAssignable)
	FOnAssetDeleted OnAssetDeleted;
	
	UFUNCTION(BlueprintCallable)
	void SetAssetRefs(const TArray<UObject *> NewAssetRefs) { AssetRefs = NewAssetRefs; }
	
	UFUNCTION(BlueprintCallable)
	TArray<UObject *> GetAssetRefs() { return AssetRefs; }

	
	// 扫描项目设置中配置路径的资产
	UFUNCTION(BlueprintCallable)
	void ScanAssetsInDirectory();

	// 获得项目设置中的配置
	TArray<FMetaDataPluginSetting> GetMetaDataPluginSettings();
	
	// 扫描已添加到主窗口中的资产
	UFUNCTION(BlueprintCallable)
	void ScanAssetsInMainWidget(TArray<UObject*> Asset);

	void AddToAssetTagStates(UObject* Asset);

	// 资产被删除时，从已注册的标签中移除
	void RemoveFromAssetTagStates(UObject* Asset);
	// 资产删除回调
	void OnAssetRemoved(const FAssetData& AssetData);
	
	// UFUNCTION(BlueprintCallable)
	// void TestFun();
private:
	TMap<FName, FString>* GetObjectMetaDataMap(UObject* Asset);
	
	// 以引用计数的方式保存已注册的标签
	TMap<FName, int32> RegisteredTags;
	
	// 保存所有已添加的资产
	TArray<TObjectPtr<UObject>> AssetRefs;

	// 此变量用于标记资产的元数据，外层 key 表示资产路径，内层 key 表示元数据的键，内层 value 表示元数据的值的哈希值
	TMap<FSoftObjectPath, TMap<FName, uint32>> AssetTagStates;
	
	// 委托句柄，用于解绑
	// FDelegateHandle AssetRemovedHandle;
	
	
};
