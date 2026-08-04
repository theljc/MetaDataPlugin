// WidgetInterface.h — 提供所有控件都需要的通用接口

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StructUtils/InstancedStruct.h"
#include "WidgetInterface_MetaDataPlugin.generated.h"

/**
 * UWidgetInterface — UHT 接口包装类
 * 实际的接口函数定义在 IWidgetInterface 中
 */
UINTERFACE()
class TEST_API UWidgetInterface_MetaDataPlugin : public UInterface
{
	GENERATED_BODY()
};

/**
 * IWidgetInterface — 所有控件的通用接口
 *
 * 提供 OnOpen（初始化）和 OnClose（清理）两个生命周期回调。
 * Subsystem 在创建/移除控件时会调用这两个函数。
 */
class TEST_API IWidgetInterface_MetaDataPlugin
{
	GENERATED_BODY()

public:
	/**
	 * OnOpen — 当 Widget 被 Subsystem 创建后调用，用于初始化
	 * 传入 FInstancedStruct 参数，不同 Widget 可以接收不同类型的初始化数据
	 *
	 * @param Params 初始化参数，由调用方传入的自定义结构体
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Interface")
	void OnOpen(const FInstancedStruct& Params);

	/**
	 * OnClose — 当 Subsystem 移除 Widget 时调用，用于解绑事件、清理资源
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Interface")
	void OnClose();

	// construct 时的初始化
	virtual void OnInitialize(const FInstancedStruct& Params);

	// Destruct 时的清理
	virtual void OnDeinitialize();
	
};
