// #pragma once
//
// class SProgressBar;
//
// class FAssetSyncTask : public TSharedFromThis<FAssetSyncTask>
// {
// public:
// 	FAssetSyncTask(const TArray<FAssetData>& InAssets, int32 InBatchSize = 32)
// 		: Assets(InAssets)
// 		, BatchSize(InBatchSize)
// 		, CurrentIndex(0)
// 	{
// 	}
//
// 	void Start();
// 	void Cancel();
//
// private:
// 	bool Tick(float DeltaTime);
// 	void Finish(bool bCancelled);
//
// 	TArray<FAssetData> Assets;
// 	int32 BatchSize;
// 	int32 CurrentIndex;
//
// 	TSharedPtr<SProgressBar> ProgressWidget;
// 	TSharedPtr<SNotificationItem> ProgressNotification;
// 	FTSTicker::FDelegateHandle TickHandle;
// };
