// EUW_MetaData.cpp — 编辑器工具控件基类实现

#include "Widget/EUW_BaseUtilityWidget.h"
#include "Widget/EUSS_WidgetManager.h"
#include "Editor.h"
#include "StructUtils/InstancedStruct.h"

#define LOCTEXT_NAMESPACE "EditorUtilitySubsystem"

void UEUW_BaseUtilityWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (GEditor)
	{
		UEUSS_WidgetManager* WidgetManager = GEditor->GetEditorSubsystem<UEUSS_WidgetManager>();

		if (WidgetManager->GetInCreatingSubWidget()) return;

		/** 主窗口控件创建时，才需要在这里执行初始化 */
		// 这个类继承了 IWidgetInterface_MetaDataPlugin，所以 Interface 有效
		TScriptInterface<IWidgetInterface_MetaDataPlugin> Interface(this);
		if (Interface)
		{
			Interface->OnInitialize(FInstancedStruct());
		}
		
	}
}

void UEUW_BaseUtilityWidget::NativeDestruct()
{
	if (GEditor)
	{
		IWidgetInterface_MetaDataPlugin::OnDeinitialize();
	}

	Super::NativeDestruct();
}

void UEUW_BaseUtilityWidget::OnOpen_Implementation(const FInstancedStruct& Params)
{
	// 默认空实现 — 蓝图可重写此函数以接收初始化参数
	UE_LOG(LogTemp, Verbose, TEXT("[UEUW_BaseUtilityWidget] OnOpen — 控件 [%s] 收到初始化参数"), *GetName());
}

void UEUW_BaseUtilityWidget::OnClose_Implementation()
{
	// 默认空实现 — 蓝图可重写此函数以进行清理
	UE_LOG(LogTemp, Verbose, TEXT("[UEUW_BaseUtilityWidget] OnClose — 控件 [%s] 正在清理"), *GetName());
}

void UEUW_BaseUtilityWidget::OnInitialize(const FInstancedStruct& Params)
{
	// 获得 Subsystem
	UEUSS_WidgetManager* WidgetManager = GEditor->GetEditorSubsystem<UEUSS_WidgetManager>();
	if (!IsValid(WidgetManager)) return;

	// 确保已创建的控件中没有该控件
	if (WidgetManager->GetActiveWidgets().Contains(this)) return;

	// 添加到 ActiveWidgets 中，并触发广播
	WidgetManager->AddToActiveWidgets(this);
	WidgetManager->OnWidgetInstanceCreated.Broadcast(this);
	
	/* 通过 EUW 蓝图中的 运行编辑器工具 按钮打开，TabID 是 UEditorUtilityWidgetBlueprint->GetFName()
	 * GetClass()->GetFName();
	 * 使用插件按钮，或者通过右键资产点击运行编辑器工具，TabID 如下
	 * FName RegistrationName = NewTabID.IsNone() ? FName(*(InBlueprint->GetPathName() + LOCTEXT("ActiveTabSuffix", "_ActiveTab").ToString())) : FName(*(InBlueprint->GetPathName() + NewTabID.ToString()));
	 */

	// 手动构建 TabID，只适用于通过插件按钮和右键资产点击运行编辑器工具打开的控件
	FString RegistrationName = GetClass()->ClassGeneratedBy.GetPathName() + LOCTEXT("ActiveTabSuffix", "_ActiveTab").ToString();
	WidgetManager->AddTo_WidgetToTabName(this, FName(*RegistrationName));

	// 蓝图可重写此接口，EUW 创建
	IWidgetInterface_MetaDataPlugin::Execute_OnOpen(this, Params);

	UE_LOG(LogTemp, Warning, TEXT("[UEUW_BaseUtilityWidget] OnInitialize — 已自动注册控件 [%s]"), *GetName());
	
}

void UEUW_BaseUtilityWidget::OnDeinitialize()
{
	UEUSS_WidgetManager* WidgetManager = GEditor->GetEditorSubsystem<UEUSS_WidgetManager>();
	if (!IsValid(WidgetManager)) return;
	
	if (!WidgetManager->GetActiveWidgets().Contains(this)) return;
	
	IWidgetInterface_MetaDataPlugin::Execute_OnClose(this);
		
	WidgetManager->RemoveFromActiveWidgets(this);
	WidgetManager->OnWidgetInstanceRemoved.Broadcast(this);
		
	WidgetManager->RemoveFrom_WidgetToTabName(this);
	
	UE_LOG(LogTemp, Verbose, TEXT("[UEUW_BaseUtilityWidget] OnDeinitialize — 已自动取消注册控件 [%s]"), *GetName());

}

#undef LOCTEXT_NAMESPACE
