// UW_AssetInfo.cpp — 用户控件基类实现

#include "Widget/UW_BaseDataWidget.h"
#include "Widget/EUSS_WidgetManager.h"
#include "Editor.h"

void UUW_BaseDataWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUW_BaseDataWidget::NativeDestruct()
{
	if (GEditor)
	{
		IWidgetInterface_MetaDataPlugin::OnDeinitialize();
	}
	
	Super::NativeDestruct();
}

void UUW_BaseDataWidget::OnOpen_Implementation(const FInstancedStruct& Params)
{
	// 默认空实现 — 蓝图可重写此函数以接收初始化参数
	UE_LOG(LogTemp, Verbose, TEXT("[UW_BaseDataWidget] OnOpen — 控件 [%s] 收到初始化参数"), *GetName());
}

void UUW_BaseDataWidget::OnClose_Implementation()
{
	// 默认空实现 — 蓝图可重写此函数以进行清理
	UE_LOG(LogTemp, Verbose, TEXT("[UW_BaseDataWidget] OnClose — 控件 [%s] 正在清理"), *GetName());
}

void UUW_BaseDataWidget::OnInitialize(const FInstancedStruct& Params)
{
	// 获得 Subsystem
	UEUSS_WidgetManager* WidgetManager = GEditor->GetEditorSubsystem<UEUSS_WidgetManager>();
	if (!IsValid(WidgetManager)) return;

	// 确保已创建的控件中没有该控件
	if (WidgetManager->GetActiveWidgets().Contains(this)) return;

	// 添加到 ActiveWidgets 中，并触发广播
	WidgetManager->AddToActiveWidgets(this);
	WidgetManager->OnWidgetInstanceCreated.Broadcast(this);
	
	// 蓝图可重写此接口，EUW 创建
	IWidgetInterface_MetaDataPlugin::Execute_OnOpen(this, Params);

	UE_LOG(LogTemp, Warning, TEXT("[UUW_BaseDataWidget] OnInitialize — 已自动注册控件 [%s]"), *GetName());
	
}

void UUW_BaseDataWidget::OnDeinitialize()
{
	UEUSS_WidgetManager* WidgetManager = GEditor->GetEditorSubsystem<UEUSS_WidgetManager>();
	if (!IsValid(WidgetManager)) return;
	
	if (!WidgetManager->GetActiveWidgets().Contains(this)) return;
	
	IWidgetInterface_MetaDataPlugin::Execute_OnClose(this);
	
	WidgetManager->RemoveFromActiveWidgets(this);
	WidgetManager->OnWidgetInstanceRemoved.Broadcast(this);
		
	UE_LOG(LogTemp, Verbose, TEXT("[UUW_BaseDataWidget] OnDeinitialize — 已自动取消注册控件 [%s]"), *GetName());

}
