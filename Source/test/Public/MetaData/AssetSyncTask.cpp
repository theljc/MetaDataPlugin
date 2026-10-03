// #include "AssetSyncTask.h"
//
// #include "Framework/Notifications/NotificationManager.h"
// #include "Widgets/Notifications/SNotificationList.h"
// #include "Widgets/Notifications/SProgressBar.h"
//
// void FAssetSyncTask::Start()
// {
//     if (Assets.Num() == 0)
//     {
//         Finish(false);
//         return;
//     }
//
//     // 创建进度通知
//     FNotificationInfo Info(FText::FromString(TEXT("正在同步资产元数据...")));
//     Info.bFireAndForget = false;       // 不自动消失
//     Info.bUseThrobber = false;
//     Info.bUseSuccessFailIcons = false;
//     Info.bAllowThrottleWhenFrameRateIsLow = false;
//     Info.ExpireDuration = 0.0f;
//
//     // 进度条控件
//     ProgressWidget = SNew(SProgressBar)
//         .Percent(0.0f);
//
//     Info.Widget = ProgressWidget;
//
//     ProgressNotification = FSlateNotificationManager::Get().AddNotification(Info);
//     if (ProgressNotification.IsValid())
//     {
//         ProgressNotification->SetCompletionState(SNotificationItem::CS_Pending);
//     }
//
//     // 绑定 Tick，每帧执行一次
//     TickHandle = FTSTicker::GetCoreTicker().AddTicker(
//         FTickerDelegate::CreateSP(AsShared(), &FAssetSyncTask::Tick),
//         0.0f // 每帧都执行
//     );
// }
//
// bool FAssetSyncTask::Tick(float DeltaTime)
// {
//     if (CurrentIndex >= Assets.Num())
//     {
//         Finish(false);
//         return false; // 停止 Tick
//     }
//
//     // 处理一批
//     const int32 End = FMath::Min(CurrentIndex + BatchSize, Assets.Num());
//     UEUSS_WidgetManager* Manager = GEditor ? GEditor->GetEditorSubsystem<UEUSS_WidgetManager>() : nullptr;
//
//     for (int32 i = CurrentIndex; i < End; ++i)
//     {
//         UObject* Asset = Assets[i].GetAsset(); // 同步加载单个资产
//         if (IsValid(Asset) && Manager)
//         {
//             Manager->SyncAsset(Asset);
//         }
//     }
//     CurrentIndex = End;
//
//     // 更新进度条
//     if (ProgressWidget.IsValid())
//     {
//         ProgressWidget->SetPercent((float)CurrentIndex / (float)Assets.Num());
//     }
//
//     return true; // 继续 Tick
// }
//
// void FAssetSyncTask::Finish(bool bCancelled)
// {
//     // 停止 Tick
//     if (TickHandle.IsValid())
//     {
//         FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
//         TickHandle.Reset();
//     }
//
//     // 关闭通知
//     if (ProgressNotification.IsValid())
//     {
//         ProgressNotification->SetCompletionState(
//             bCancelled ? SNotificationItem::CS_Fail : SNotificationItem::CS_Success);
//         ProgressNotification->ExpireAndFadeout();
//         ProgressNotification.Reset();
//     }
//
//     ProgressWidget.Reset();
// }
//
// void FAssetSyncTask::Cancel()
// {
//     Finish(true);
// }
