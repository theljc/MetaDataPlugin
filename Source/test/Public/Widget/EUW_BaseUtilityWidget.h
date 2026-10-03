#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "WidgetInterface_MetaDataPlugin.h"
#include "EUW_BaseUtilityWidget.generated.h"

struct FInstancedStruct;

/**
 * 本插件的所有 EUW 都继承自该基类
 */
UCLASS(BlueprintType, Blueprintable)
class TEST_API UEUW_BaseUtilityWidget : public UEditorUtilityWidget, public IWidgetInterface_MetaDataPlugin
{
	GENERATED_BODY()

protected:

	/**
	 * NativeConstruct — Widget 被构造时调用
	 * 自动将自身注册到 UEUW_WidgetManager 的 ActiveWidgets 中，并触发 OnWidgetCreated 委托
	 */
	virtual void NativeConstruct() override;

	/**
	 * NativeDestruct — Widget 被销毁时调用
	 * 自动从 UEUW_WidgetManager 的 ActiveWidgets 中移除自身，并触发 OnWidgetRemoved 委托
	 */
	virtual void NativeDestruct() override;

public:
	
// IWidgetInterface

	/**
	 * OnOpen — 默认实现（可在蓝图中重写）
	 * 当 Widget 被 Subsystem 的 CreateWidget 创建后调用
	 */
	virtual void OnOpen_Implementation(const FInstancedStruct& Params) override;

	/**
	 * OnClose — 默认实现（可在蓝图中重写）
	 * 当 Subsystem 移除 Widget 时调用，用于解绑事件、清理资源
	 */
	virtual void OnClose_Implementation() override;

	virtual void OnInitialize(const FInstancedStruct& Params) override;

	virtual void OnDeinitialize() override;
	
	// End IWidgetInterface
	
};
