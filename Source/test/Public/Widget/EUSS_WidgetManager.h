// EUW_WidgetManager.h — 控件管理子系统
// 继承 UEditorUtilitySubsystem，负责管理所有创建的 UUserWidget，提供回调

#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Public/EditorUtilitySubsystem.h"
#include "InstancedStruct.h"
#include "MetaData/EUSS_MetaDataManager.h"
#include "EUSS_WidgetManager.generated.h"

struct FInstancedStruct;
class UUserWidget;
class UEditorUtilityWidget;
class UEditorUtilityWidgetBlueprint;

/**
 * FOnWidgetInstanceCreated — 有 Widget 被创建时触发的委托
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetInstanceCreated, UUserWidget*, Widget);

/**
 * FOnWidgetInstanceRemoved — 有 Widget 被移除时触发的委托
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetInstanceRemoved, UUserWidget*, Widget);

/**
 * UEUSS_WidgetManager — 编辑器控件管理子系统
 *
 * 负责统一管理所有通过本子系统创建的 UUserWidget。
 * 提供 CreateWidget / RemoveWidget 函数，以及 OnWidgetCreated / OnWidgetRemoved 委托回调。
 * 控件通过 IWidgetInterface 接口接收 OnOpen / OnClose 生命周期回调。
 *
 * 重写 SpawnAndRegisterTab，在创建 EUW 之前判断是否已打开：
 *   - 已打开 → 聚焦窗口
 *   - 未打开 → 调用 Super 创建
 */
UCLASS()
class TEST_API UEUSS_WidgetManager : public UEditorUtilitySubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	

	/** 有 Widget 被创建时触发 */
	UPROPERTY(BlueprintAssignable, Category = "Widget Manager")
	FOnWidgetInstanceCreated OnWidgetInstanceCreated;

	/** 有 Widget 被移除时触发 */
	UPROPERTY(BlueprintAssignable, Category = "Widget Manager")
	FOnWidgetInstanceRemoved OnWidgetInstanceRemoved;

	/** 有资产被删除时触发 */
	// UPROPERTY(BlueprintAssignable, Category = "Widget Manager")
	// FOnAssetDeleted OnAssetDeleted;

	/**
	 * CreateWidget — 创建普通 UMG Widget，蓝图可调用
	 *
	 * @param WidgetClass 要创建的 Widget 类（蓝图传入 TSubclassOf<UUserWidget>）
	 * @param Params      不同 Widget 创建时需要的不同参数（通过 FInstancedStruct 传递）
	 * @return            创建完成的 UUserWidget 实例
	 */
	UFUNCTION(BlueprintCallable, Category = "Widget Manager")
	UUserWidget* CreateDataWidget(TSubclassOf<UUserWidget> WidgetClass, const FInstancedStruct Params);

	/** 用于创建 Sub Widget
	 * @param InBlueprint      要创建的子 Widget 蓝图
	 * @param Params      子 Widget 创建时需要的不同参数（通过 FInstancedStruct 传递）
	 * @param bOpenAsModal   是否以模态窗口打开
	 * @param ModalWindowSize 模态窗口的大小
	 * @return             创建完成的 UUserWidget 实例
	 */
	UFUNCTION(BlueprintCallable, Category = "Widget Manager",  meta = (AdvancedDisplay="ModalWindowSize, bOpenAsModal"))
	UUserWidget* CreateSubWidget(UEditorUtilityWidgetBlueprint* InBlueprint, const FInstancedStruct Params, bool bOpenAsModal, FVector2D ModalWindowSize);

	/**
	 * RemoveWidget — 移除 Widget，蓝图可调用，默认由 Widget 的 NativeDestruct 函数触发
	 *
	 * @param Widget 要移除的 Widget 实例
	 */
	// UFUNCTION(BlueprintCallable, Category = "Widget Manager")
	// void RemoveWidget(UUserWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "Widget Manager")
	void CloseAllWidgets();
	
	// UFUNCTION(BlueprintCallable, Category = "Widget Manager")
	void AddToActiveWidgets(UUserWidget* Widget);

	// UFUNCTION(BlueprintCallable, Category = "Widget Manager")
	void RemoveFromActiveWidgets(UUserWidget* Widget);

	void OpenAsModalWindow(UUserWidget* Widget, FVector2D WindowSize);
	

	void AddTo_WidgetToTabName(UUserWidget* Widget, FName TabName) { WidgetToTabName.Add(Widget, TabName); }
	void RemoveFrom_WidgetToTabName(UUserWidget* Widget) { WidgetToTabName.Remove(Widget); }

	/** 创建主窗口（检查是否已打开 → 聚焦，否则创建） */
	void CreateMainWidget();
	
	/**
	 * FocusWidgetTab — 将已存在的 EditorUtilityWidget 窗口聚焦到前台
	 *
	 * @param UtilityWidget 已存在的 EditorUtilityWidget 实例
	 * @return 聚焦成功返回 true
	 */
	bool FocusWidgetTab(UEditorUtilityWidget* UtilityWidget);

	// 有资产被删除时触发
	// void OnAssetRemoved(const FAssetData& AssetData);
	
	/** Getter */
	// UFUNCTION(BlueprintCallable)
	// TArray<UObject *> GetAssetRefs() { return AssetRefs; }
	
	// UFUNCTION(BlueprintCallable, Category = "Widget Manager")
	TArray<UUserWidget*> GetActiveWidgets() const { return ActiveWidgets; }

	TMap<TObjectPtr<UUserWidget>, FName> GetWidgetToTabName() const { return WidgetToTabName; }

	TSoftObjectPtr<UEditorUtilityWidgetBlueprint> Get_EUWBP() { return EUWBP_MetaData; }

	bool GetInCreatingSubWidget() { return bInSubWidgetCreating; }


	/** Setter */
	UFUNCTION(BlueprintCallable, Category = "Widget Manager")
	void Set_EUWBP(TSoftObjectPtr<UEditorUtilityWidgetBlueprint> InEUWBP_MetaData) { EUWBP_MetaData = InEUWBP_MetaData; }

	// UFUNCTION(BlueprintCallable)
	// void SetAssetRefs(const TArray<UObject *> NewAssetRefs) { AssetRefs = NewAssetRefs; }
	
	void SetInCreatingSubWidget(const bool bInCreating) { bInSubWidgetCreating = bInCreating; }

	UFUNCTION(BlueprintCallable)
	void Test();
	
private:

	// FName GetWidgetTabName(UUserWidget* Widget);

	/** 已创建并正在管理的控件列表 */
	TArray<TObjectPtr<UUserWidget>> ActiveWidgets;

	/** 主窗口蓝图资产 */
	TSoftObjectPtr<UEditorUtilityWidgetBlueprint> EUWBP_MetaData;

	// UEditorUtilityWidget 和 TabID 的映射
	TMap<TObjectPtr<UUserWidget>, FName> WidgetToTabName;

	// 用于判断是否正在创建子控件，DataWidget 不需要判断
	bool bInSubWidgetCreating = false;

	/** 资产相关 */
	// 保存所有已添加的资产
	// TArray<TObjectPtr<UObject>> AssetRefs;
	
};
