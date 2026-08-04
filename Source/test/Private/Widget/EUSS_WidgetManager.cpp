// EUW_WidgetManager.cpp — 控件管理子系统实现

#include "Widget/EUSS_WidgetManager.h"
#include "Widget/WidgetInterface_MetaDataPlugin.h"
#include "Blueprint/UserWidget.h"
#include "Editor.h"
#include "EditorUtilityWidget.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "StructUtils/InstancedStruct.h"
#include "Widgets/SWindow.h"

void UEUSS_WidgetManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("[UEUSS_WidgetManager] 子系统已初始化"));
}

void UEUSS_WidgetManager::Deinitialize()
{
	// 清理所有仍处于活跃状态的控件
	UE_LOG(LogTemp, Log, TEXT("[UEUSS_WidgetManager] 子系统即将清空 %d 个活跃控件"), ActiveWidgets.Num());
	
	ActiveWidgets.Empty();

	Super::Deinitialize();
}

// ==================== 聚焦辅助函数 ====================

bool UEUSS_WidgetManager::FocusWidgetTab(UEditorUtilityWidget* UtilityWidget)
{
	if (!UtilityWidget)
	{
		return false;
	}

	TSharedPtr<SWidget> CachedWidget = UtilityWidget->GetCachedWidget();
	if (!CachedWidget.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UEUW_WidgetManager] FocusWidgetTab 失败：CachedWidget 无效"));
		return false;
	}

	// 获取 Widget 的父窗口（即 Slate Tab 所在的 SWindow）
	TSharedPtr<SWidget> ParentWidget = CachedWidget->GetParentWidget();
	if (!ParentWidget.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UEUW_WidgetManager] FocusWidgetTab 失败：ParentWidget 无效"));
		return false;
	}

	TSharedRef<SWidget> ParentWidgetRef = ParentWidget.ToSharedRef();
	TSharedPtr<SWindow> WidgetWindow = FSlateApplication::Get().FindWidgetWindow(ParentWidgetRef);
	if (!WidgetWindow.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UEUW_WidgetManager] FocusWidgetTab 失败：找不到 SWindow"));
		return false;
	}

	// 将窗口换入前方
	WidgetWindow->BringToFront();
	// 聚焦窗口内容
	FSlateApplication::Get().SetUserFocus(
		FSlateApplication::Get().GetUserIndexForKeyboard(),
		WidgetWindow->GetContent());

	UE_LOG(LogTemp, Log, TEXT("[UEUW_WidgetManager] FocusWidgetTab — 已聚焦窗口 [%s]"),
		*UtilityWidget->GetName());

	return true;
}

// void UEUSS_WidgetManager::OnAssetRemoved(const FAssetData& AssetData)
// {
// 	// 获取被删除资产的路径（例如 /Game/MyAsset.MyAsset）
// 	FSoftObjectPath DeletedPath = AssetData.ToSoftObjectPath();
// 	FString DeletedPathStr = DeletedPath.ToString();
//
// 	// 从后向前遍历，安全删除
// 	for (int32 i = AssetRefs.Num() - 1; i >= 0; --i)
// 	{
// 		UObject* Obj = AssetRefs[i];
// 		if (Obj)
// 		{
// 			// 比较对象路径是否与删除的资产路径一致
// 			FSoftObjectPath ObjPath(Obj);
// 			if (ObjPath == DeletedPath)
// 			{
// 				AssetRefs.RemoveAt(i);
// 				// 触发广播，通知删除
// 				OnAssetDeleted.Broadcast(DeletedPathStr);
// 				// 通常一个资产只会对应一个对象，所以找到后即可跳出
// 				break;
// 			}
// 		}
// 		else
// 		{
// 			// 如果对象已被GC置空，顺便清理
// 			AssetRefs.RemoveAt(i);
// 		}
// 	}
// }

// FName UEUSS_WidgetManager::GetWidgetTabName(UUserWidget* Widget)
// {
	// // 获取真实的 TabID — 从 Slate 层级中找到 SDockTab
	// FName RealTabID;
	// if (TSharedPtr<SWidget> CachedSlate = Widget->GetCachedWidget())
	// {
	// 	TSharedPtr<SWidget> Parent = CachedSlate->GetParentWidget();
	// 	while (Parent.IsValid())
	// 	{
	// 		// SDockTab 的类型标识是 "SDockTab"
	// 		if (Parent->GetTypeAsString() == TEXT("SDockingTabStack"))
	// 		{
	// 			TSharedPtr<SDockTab> DockTab = StaticCastSharedPtr<SDockTab>(Parent);
	// 			RealTabID = DockTab->GetLayoutIdentifier().TabType;
	// 			break;
	// 		}
	// 		Parent = Parent->GetParentWidget();
	// 	}
	// }
	//
	// // UE_LOG(LogTemp, Log, TEXT("[EUW_MetaData] 真实 TabID = %hhd"), RealTabID);
	// return RealTabID;

// 	if (!Widget)
// 	{
// 		return FName();
// 	}
//
// 	// 1. 获取 Widget 的 Slate 表示
// 	TSharedPtr<SWidget> CachedSlate = Widget->GetCachedWidget();
// 	if (!CachedSlate.IsValid())
// 	{
// 		return FName();
// 	}
//
// 	// 2. 通过 FSlateApplication 查找所属的 SWindow
// 	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(CachedSlate.ToSharedRef());
// 	if (!ParentWindow.IsValid())
// 	{
// 		return FName();
// 	}
//
// 	// 3. 获取窗口的内容
// 	TSharedRef<SWidget> WindowContent = ParentWindow->GetContent();
//
// 	// 4. 尝试将内容转换为 SDockTab
// 	//    注意：这里可能是 SDockTab 本身，也可能是其他容器，
// 	//    需要根据实际情况调整
// 	TSharedPtr<SDockTab> DockTab = StaticCastSharedPtr<SDockTab>(WindowContent.ToSharedPtr());
// 	if (DockTab.IsValid())
// 	{
// 		return DockTab->GetLayoutIdentifier().TabType;
// 	}
//
// 	// 5. 备选：如果内容不是 SDockTab，可能需要进一步遍历其子控件
// 	//    但大多数情况下，EUW 直接作为 SDockTab 的内容
// 	return FName();
// }

// ==================== CreateMainWidget ====================

void UEUSS_WidgetManager::CreateMainWidget()
{
	// 验证加载是否成功
	if (!EUWBP_MetaData.Get())
	{
		UE_LOG(LogTemp, Error, TEXT("[UEUW_WidgetManager] CreateMainWidget 失败：EUWBP_MetaData 未加载"));
		return;
	}

	// 查找 UtilityWidget 是否已创建
	UEditorUtilityWidget* UtilityWidget = FindUtilityWidgetFromBlueprint(EUWBP_MetaData.Get());
	if (UtilityWidget)
	{
		// 已创建 → 聚焦
		FocusWidgetTab(UtilityWidget);
	}
	else
	{
		// 未创建 → 通过 SpawnAndRegisterTab 创建（也会走到上面的 FocusWidgetTab 逻辑）
		SpawnAndRegisterTab(Get_EUWBP().Get());
	}
}

// ==================== CreateWidget（普通 UMG Widget） ====================

UUserWidget* UEUSS_WidgetManager::CreateDataWidget(TSubclassOf<UUserWidget> WidgetClass, const FInstancedStruct Params)
{
	if (!GEditor)
	{
		UE_LOG(LogTemp, Error, TEXT("[UEUSS_WidgetManager] CreateDataWidget 失败：GEditor 不可用"));
		return nullptr;
	}

	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if (!EditorWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("[UEUSS_WidgetManager] CreateDataWidget 失败：无法获取编辑器 World"));
		return nullptr;
	}

	if (!WidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[UEUSS_WidgetManager] CreateDataWidget 失败：WidgetClass 为空"));
		return nullptr;
	}

	// 1. 创建 Widget（此时 Widget 的 NativeConstruct 会触发，Widget 会自动注册到 ActiveWidgets）
	UUserWidget* Widget = ::CreateWidget<UUserWidget>(EditorWorld, WidgetClass);
	if (!Widget)
	{
		UE_LOG(LogTemp, Error, TEXT("[UEUSS_WidgetManager] CreateDataWidget 失败：无法创建 Widget"));
		return nullptr;
	}
	
	// Widget 只有继承了 IWidgetInterface_MetaDataPlugin，Interface 才有效
	TScriptInterface<IWidgetInterface_MetaDataPlugin> Interface(Widget);
	if (Interface)
	{
		Interface->OnInitialize(Params);
	}

	UE_LOG(LogTemp, Log, TEXT("[UEUSS_WidgetManager] CreateDataWidget 成功创建控件 [%s]，当前活跃控件数：%d"),
		*Widget->GetName(), ActiveWidgets.Num());

	return Widget;
}

UUserWidget* UEUSS_WidgetManager::CreateSubWidget(UEditorUtilityWidgetBlueprint* InBlueprint, const FInstancedStruct Params, bool bOpenAsModal, FVector2D ModalWindowSize)
{
	if (!GEditor)
	{
		UE_LOG(LogTemp, Error, TEXT("[UEUSS_WidgetManager] CreateSubWidget 失败：GEditor 不可用"));
		return nullptr;
	}

	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if (!EditorWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("[UEUSS_WidgetManager] CreateSubWidget 失败：无法获取编辑器 World"));
		return nullptr;
	}

	if (!InBlueprint)
	{
		UE_LOG(LogTemp, Error, TEXT("[UEUSS_WidgetManager] CreateSubWidget 失败：InBlueprint 为空"));
		return nullptr;
	}
	
	SetInCreatingSubWidget(true);
	
	UEditorUtilityWidget* Widget = SpawnAndRegisterTab(InBlueprint);
	if (!Widget)
	{
		SetInCreatingSubWidget(false);

		UE_LOG(LogTemp, Error, TEXT("[UEUSS_WidgetManager] CreateSubWidget 失败：无法创建 Widget"));
		return nullptr;
	}
	
	// Widget 只有继承了 IWidgetInterface_MetaDataPlugin，Interface 才有效
	TScriptInterface<IWidgetInterface_MetaDataPlugin> Interface(Widget);
	if (Interface)
	{
		Interface->OnInitialize(Params);
	}
	
	SetInCreatingSubWidget(false);
	
	UE_LOG(LogTemp, Log, TEXT("[UEUSS_WidgetManager] CreateSubWidget 成功创建控件 [%s]，当前活跃控件数：%d"),
		*Widget->GetName(), ActiveWidgets.Num());
	
	if (bOpenAsModal)
	{
		OpenAsModalWindow(Widget, ModalWindowSize);
	}
	
	// 添加 Widget 和 TabID 的关联
	// AddTo_WidgetToTabName(Widget, TabID);

	// 2. 确保 Widget 已在 ActiveWidgets 中（NativeConstruct 应该已添加，这里做防重复检查）
	// if (!ActiveWidgets.Contains(Widget))
	// {
	// 	AddToActiveWidgets(Widget);
	// 	OnWidgetInstanceCreated.Broadcast(Widget);
	// }

	// 3. 调用 IWidgetInterface::OnOpen 进行初始化（如果 Widget 实现了该接口）
	// if (Widget->Implements<UWidgetInterface>())
	// {
	// 	IWidgetInterface::Execute_OnOpen(Widget, FInstancedStruct());
	// }
	
	return Widget;
}

// ==================== RemoveWidget ====================

// void UEUSS_WidgetManager::RemoveWidget(UUserWidget* Widget)
// {
// 	if (!Widget)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("[UEUW_WidgetManager] RemoveWidget 失败：Widget 为空"));
// 		return;
// 	}
//
// 	// 1. 调用 IWidgetInterface::OnClose 进行清理
// 	if (Widget->Implements<UWidgetInterface_MetaDataPlugin>())
// 	{
// 		IWidgetInterface_MetaDataPlugin::Execute_OnClose(Widget);
// 	}
//
// 	// 2. 从 ActiveWidgets 中移除
// 	if (ActiveWidgets.Contains(Widget))
// 	{
// 		RemoveFromActiveWidgets(Widget);
// 		OnWidgetInstanceRemoved.Broadcast(Widget);
// 	}
//
// 	// 3. 从父容器中移除（会触发 NativeDestruct，其中会做防重复检查）
// 	Widget->RemoveFromParent();
//
// 	UE_LOG(LogTemp, Log, TEXT("[UEUW_WidgetManager] RemoveWidget 已移除控件，当前活跃控件数：%d"), ActiveWidgets.Num());
// }

void UEUSS_WidgetManager::AddToActiveWidgets(UUserWidget* Widget)
{
	ActiveWidgets.Add(Widget);
}

void UEUSS_WidgetManager::RemoveFromActiveWidgets(UUserWidget* Widget)
{
	ActiveWidgets.Remove(Widget);
}

void UEUSS_WidgetManager::OpenAsModalWindow(UUserWidget* Widget, FVector2D WindowSize)
{
	// 获取父窗口
	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
    
	// 创建窗口
	TSharedRef<SWindow> ModalWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("模态窗口")))
		.ClientSize(WindowSize);

	// EUW 转换为 Slate 框架能使用的 SWidget
	TSharedRef<SWidget> SlateContent = Widget->TakeWidget();

	// 设置窗口的内容
	ModalWindow->SetContent(SlateContent);

	// 打开为模态窗口
	FSlateApplication::Get().AddModalWindow(ModalWindow, ParentWindow);

}

void UEUSS_WidgetManager::CloseAllWidgets()
{
	// 复制数组避免迭代时修改 ActiveWidgets（RemoveWidget 会从中移除元素）
	TArray<UUserWidget*> WidgetsCopy = ActiveWidgets;
	for (UUserWidget* Widget : WidgetsCopy)
	{
		if (!Widget) continue;
		
		// UEditorUtilityWidget* EU_Widget = Cast<UEditorUtilityWidget>(Widget);
		// if (EU_Widget)
		// {
		if (WidgetToTabName.Find(Widget))
		{
			CloseTabByID(WidgetToTabName[Widget]);
		}
			// CloseTabByID(GetWidgetTabName(Widget));
		// }
		// else
		// {
		// 只关闭 EUW
		// 	Widget->RemoveFromParent();
		// }
	}

	
}
