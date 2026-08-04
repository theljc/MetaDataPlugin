// EUW_MetaData.h — 编辑器工具控件基类
// 继承 UEditorUtilityWidget 和 IWidgetInterface，提供自动注册到 Subsystem 的能力

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "WidgetInterface_MetaDataPlugin.h"
#include "EUW_BaseUtilityWidget.generated.h"

struct FInstancedStruct;
/**
 * UEUW_MetaData — 编辑器工具控件基类
 *
 * 继承自 UEditorUtilityWidget 和 IWidgetInterface。
 * 在 NativeConstruct 中自动注册到 UEUW_WidgetManager 子系统，
 * 在 NativeDestruct 中自动取消注册。
 *
 * 蓝图可以继承此类来创建编辑器工具控件，并重写 OnOpen / OnClose 处理初始化和清理逻辑。
 */
UCLASS(BlueprintType, Blueprintable)
class TEST_API UEUW_BaseUtilityWidget : public UEditorUtilityWidget, public IWidgetInterface_MetaDataPlugin
{
	GENERATED_BODY()

protected:
	// ==================== UMG 生命周期 ====================

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
