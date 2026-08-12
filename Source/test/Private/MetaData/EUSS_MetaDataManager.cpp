// Fill out your copyright notice in the Description page of Project Settings.


#include "MetaData/EUSS_MetaDataManager.h"

#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilityWidget.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "IContentBrowserSingleton.h"
#include "MetaDataPluginSettings.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Engine/AssetManagerSettings.h"
#include "Runtime/AssetRegistry/Private/AssetRegistry.h"
#include "UObject/AssetRegistryTagsContext.h"

void UEUSS_MetaDataManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// 仅在编辑器环境下有效
	// if (GIsEditor)
	// {
	// 	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	// 	AssetRemovedHandle = AssetRegistry.OnAssetRemoved().AddUObject(this, &UEUSS_MetaDataManager::OnAssetRemoved);
	// }
}

void UEUSS_MetaDataManager::Deinitialize()
{
	// if (GIsEditor)
	// {
	// 	if (FModuleManager::Get().IsModuleLoaded("AssetRegistry"))
	// 	{
	// 		IAssetRegistry& AssetRegistry = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	// 		AssetRegistry.OnAssetRemoved().Remove(AssetRemovedHandle);
	// 	}
	// }
	
	Super::Deinitialize();
}

void UEUSS_MetaDataManager::TestRemove(UObject* Asset, FName TagToAdd)
{
	UAssetManagerSettings* Settings = GetMutableDefault<UAssetManagerSettings>();
	if (!Settings)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get UAssetManagerSettings default object."));
		return;
	}
	
	Settings->MetaDataTagsForAssetRegistry.Remove(TagToAdd);
	
	TSet<FName>& GlobalTagsForAssetRegistry = UObject::GetMetaDataTagsForAssetRegistry();
	GlobalTagsForAssetRegistry.Remove(TagToAdd);
	Settings->TryUpdateDefaultConfigFile();

	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	AssetRegistry.AssetUpdateTags(Asset, EAssetRegistryTagsCaller::FullUpdate);

}

TArray<FMetaDataPluginSetting> UEUSS_MetaDataManager::GetMetaDataPluginSettings()
{
	const UMetaDataPluginSettings* Settings = GetDefault<UMetaDataPluginSettings>();
	return Settings->ScanDirectory;
}

// void UEUSS_MetaData::TestFun()
// {
// 	UAssetManagerSettings* Settings = GetMutableDefault<UAssetManagerSettings>();
// 	if (!Settings)
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Failed to get UAssetManagerSettings default object."));
// 		return;
// 	}
// 	
// 	FString ConfigFile = Settings->GetDefaultConfigFilename();
// 	UE_LOG(LogTemp, Warning, TEXT("Config file: %s"), *ConfigFile);
// }


void UEUSS_MetaDataManager::ScanAssetsInDirectory()
{
	
	TArray<FMetaDataPluginSetting> Settings = GetMetaDataPluginSettings();
	for (FMetaDataPluginSetting Setting : Settings)
	{
		IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
		TArray<FAssetData> AssetDataList;
		AssetRegistry.GetAssetsByPath(FName(*Setting.Directory.Path), AssetDataList, Setting.bRecursive);
		
		for (const FAssetData& AssetData : AssetDataList)
		{
			auto TagsAndValues = AssetData.TagsAndValues;
			
			AddToAssetTagStates(AssetData.GetAsset());
		}
		
	}
}

void UEUSS_MetaDataManager::ScanAssetsInMainWidget(TArray<UObject*> Asset)
{
	for (UObject* Obj : Asset)
	{
		AddToAssetTagStates(Obj);
	}
}

void UEUSS_MetaDataManager::AddToAssetTagStates(UObject* Asset)
{
	if (!IsValid(Asset)) return;

	TMap<FName, FString> ObjectMetaDataMap = UEditorAssetLibrary::GetMetadataTagValues(Asset);
	// TMap<FName, FString>* ObjectMetaDataMap = GetObjectMetaDataMap(Asset);
	// if (!ObjectMetaDataMap) return;
	
	// 4. 构建外层的 Key：使用 FSoftObjectPath
	FSoftObjectPath AssetPath(Asset);

	// TMap<FName, uint32>& TagMapToAdd = AssetTagStates.FindOrAdd(AssetPath);
	TMap<FName, uint32>* TagMapToAdd = AssetTagStates.Find(AssetPath);

	// 表示这个资产当前没有元数据
	if (ObjectMetaDataMap.IsEmpty())
	{
		// 当前资产没有被扫描过
		if (!TagMapToAdd)
		{
			// 添加空值即可，表示此资产已经被扫描过
			AssetTagStates.Add(AssetPath);
		}
		// 被扫描过
		else
		{
			// TagMapToAdd 为空，表示上一次扫描时没有元数据，这次依然没有
			if (TagMapToAdd->IsEmpty()) return;

			// 当前资产实际没有元数据，直接移除保存的所有旧的元数据
			for (const auto& SavedMetaData : *TagMapToAdd)
			{
				RemoveTagFromRegisteredTags(SavedMetaData.Key, Asset);
			}
		}
	}
	// 有元数据
	else
	{
		// 没有被扫描过，表示元数据被手动添加过
		if (!TagMapToAdd)
		{
			TMap<FName, uint32>& TagMap = AssetTagStates.Add(AssetPath);
			for (auto CurrentMetaData : ObjectMetaDataMap)
			{
				AddTagToRegisteredTags(CurrentMetaData.Key, Asset);
				TagMap.Add(CurrentMetaData.Key, GetTypeHash(CurrentMetaData.Value));
			}
		}
		// 被扫描过
		else
		{
			// TagMapToAdd 为空的情况：扫描时没有元数据，手动添加后再次扫描
			if (TagMapToAdd->IsEmpty())
			{
				for (auto CurrentMetaData : ObjectMetaDataMap)
				{
					AddTagToRegisteredTags(CurrentMetaData.Key, Asset);
					TagMapToAdd->Add(CurrentMetaData.Key, GetTypeHash(CurrentMetaData.Value));
				}
			}
			// 不为空，表示上次扫描时添加了元数据，这次扫描需要更新
			else
			{
				// // 移除所有当前资产保存的元数据
				// for (const auto& SavedMetaData : ObjectMetaDataMap)
				// {
				// 	RemoveTagFromRegisteredTags(SavedMetaData.Key);
				// }
				//
				// // 添加当前资产实际的元数据
				// for (const auto& CurrentMetaData : ObjectMetaDataMap)
				// {
				// 	AddTagToRegisteredTags(CurrentMetaData.Key, Asset);
				// }
				
				// 遍历该资产实际的元数据
				for (auto& CurrentMetaData : ObjectMetaDataMap)
				{
					FName CurrentTag = CurrentMetaData.Key;
					FString CurrentValue = CurrentMetaData.Value;
				
					// 计算实际元数据值的哈希
					uint32 CurrentHash = GetTypeHash(CurrentValue);
				
					// 获得保存的元数据值的哈希
					uint32* StoredHash = TagMapToAdd->Find(CurrentTag);

					// 当前元数据没有被标记过
					if (!StoredHash)
					{
						AddTagToRegisteredTags(CurrentTag, Asset);
						TagMapToAdd->Add(CurrentTag, CurrentHash);
					}
					// 元数据值被修改过
					else if (*StoredHash != CurrentHash)
					{
						// 修改值，RegisteredTags 不变
						// 将 Key 的值更新为 CurrentHash
						(*TagMapToAdd)[CurrentTag] = CurrentHash;
					}
					
					// 哈希匹配，表示已标记且未修改过
				}
			}
		}
	}
	
	
}

void UEUSS_MetaDataManager::RemoveFromAssetTagStates(UObject* Asset)
{
	if (!IsValid(Asset)) return;

	// 实际的当前资产的元数据
	// TMap<FName, FString>* ObjectMetaDataMap = GetObjectMetaDataMap(Asset);
	TMap<FName, FString> ObjectMetaDataMap = UEditorAssetLibrary::GetMetadataTagValues(Asset);

	// 4. 构建外层的 Key：使用 FSoftObjectPath
	FSoftObjectPath AssetPath(Asset);
	
	TMap<FName, uint32>* TagMapToRemove = AssetTagStates.Find(AssetPath);
	
	// 保存的当前资产的元数据
	// TMap<FName, uint32>& TagMap = AssetTagStates.FindOrAdd(AssetPath);

	// 表示这个资产当前没有元数据
	if (ObjectMetaDataMap.IsEmpty())
	{
		// 没有被扫描过（不需要被管理）
		if (!TagMapToRemove) return;
	}
	// 有元数据
	else
	{
		// 没有被扫描过，表示元数据被手动添加过
		if (!TagMapToRemove)
		{
			// TODO: 如果从未扫描过，
			// 无法处理，资产必须扫描后才能被管理
			return;
		}
	}
	
	// 已经被扫描过，从 RegisteredTags 中移除管理
	for (const auto& SavedMetaData : *TagMapToRemove)
	{
		RemoveTagFromRegisteredTags(SavedMetaData.Key, Asset);
	}
	
	// 移除此资产的元数据管理
	AssetTagStates.Remove(AssetPath);
	
	
	// 2. 收集当前所有元数据键
	// TSet<FName> CurrentKeys;
	// for (auto& Pair : *ObjectMetaDataMap)
	// {
	// 	CurrentKeys.Add(Pair.Key);
	// }
	
	


	
	// 6. 遍历该资产的所有元数据
	// for (auto& Pair : *ObjectMetaDataMap)
	// {
	// 	FName Key = Pair.Key;
	// 	FString CurrentValue = Pair.Value;
	//
	// 	// 计算当前值的哈希
	// 	uint32 CurrentHash = GetTypeHash(CurrentValue);
	//
	// 	// 7. 检查该键是否已存储，以及哈希是否变化
	// 	uint32* StoredHash = TagMap.Find(Key);
	//
	// 	// 未标记过
	// 	if (!StoredHash)
	// 	{
	// 		// 添加到 RegisteredTags
	// 		
	// 		// 标记为已处理（添加就表示标记）
	// 		TagMap.Add(Key, CurrentHash);
	// 	}
	// 	// 已标记但值被修改
	// 	else if (*StoredHash != CurrentHash)
	// 	{
	// 		// 修改值，RegisteredTags 不变
	// 		// 将 Key 的值更新为 CurrentHash
	// 		TagMap[Key] = CurrentHash;
	// 	}
	// 	
	// 	// 哈希匹配，表示已标记且未修改
	// }

	// 4. 删除已不存在的键
	// TArray<FName> KeysToRemove;
	// for (const auto& Pair : TagMap)
	// {
	// 	if (!CurrentKeys.Contains(Pair.Key))
	// 	{
	// 		KeysToRemove.Add(Pair.Key);
	// 	}
	// }

	// for (const FName& Key : KeysToRemove)
	// {
	//
	// 	// 【删除】该键已被移除
	// 	TagMap.Remove(Key);
	// }

	// 5. 如果 TagMap 变为空，可以选择删除该资产的外层条目（可选）
	// if (TagMap.Num() == 0)
	// {
	// 	AssetTagStates.Remove(AssetPath);
	// }
	
}

void UEUSS_MetaDataManager::OnAssetRemoved(const FAssetData& AssetData)
{
	// 获取被删除资产的路径（例如 /Game/MyAsset.MyAsset）
	UObject* AssetToRemove = AssetData.GetAsset();
	
	// if (!AssetRefs.Contains(AssetToRemove)) return;
	//
	// FSoftObjectPath DeletedPath = AssetData.ToSoftObjectPath();
	// FString DeletedPathStr = DeletedPath.ToString();
	//
	// // 从保存的数组中移除
	// AssetRefs.Remove(AssetToRemove);

	// 资产被删除后，包含的元数据也从 Manager 的管理中移除
	RemoveFromAssetTagStates(AssetToRemove);


	OnAssetDeleted.Broadcast(AssetToRemove);
	
	// 从后向前遍历，安全删除
	// for (int32 i = AssetRefs.Num() - 1; i >= 0; --i)
	// {
	// 	UObject* Obj = AssetRefs[i];
	// 	if (Obj)
	// 	{
	// 		// 比较对象路径是否与删除的资产路径一致
	// 		FSoftObjectPath ObjPath(Obj);
	// 		if (ObjPath == DeletedPath)
	// 		{
	// 			AssetRefs.RemoveAt(i);
	// 			// 触发广播，通知删除
	// 			OnAssetDeleted.Broadcast(DeletedPathStr);
	// 			// 通常一个资产只会对应一个对象，所以找到后即可跳出
	// 			break;
	// 		}
	// 	}
	// 	else
	// 	{
	// 		// 如果对象已被GC置空，顺便清理
	// 		AssetRefs.RemoveAt(i);
	// 	}
	// }
	
}

TMap<FName, FString>* UEUSS_MetaDataManager::GetObjectMetaDataMap(UObject* Asset)
{
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 6
	// 5.6+ 版本改用 FMetaData
	TMap<FName, FString>* ObjectMetaDataMap = FMetaData::GetMapForObject(Asset);
#else
	TMap<FName, FString>* ObjectMetaDataMap = UMetaData::GetMapForObject(Asset);
#endif
	
	return ObjectMetaDataMap;
}

void UEUSS_MetaDataManager::AddTagToRegisteredTags(const FName TagToAdd, UObject* Asset)
{
	if (TagToAdd.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("AddTagToAssetRegistry: Attempt to add None tag."));
		return;
	}
	
	int32& Count = RegisteredTags.FindOrAdd(TagToAdd);

	Count++;

	if (Count == 1)
	{
		// 第一次添加，注册 Tag 到全局
		AddTagToAssetRegistry(TagToAdd, Asset);
	}

	OnMetaDataAdded.Broadcast(Asset);

	UE_LOG(LogTemp, Verbose, TEXT("AddTagToAssetRegistry: Tag '%s' now has count %d"), *TagToAdd.ToString(), Count);

}

void UEUSS_MetaDataManager::RemoveTagFromRegisteredTags(const FName TagToRemove, UObject* Asset)
{
	if (TagToRemove.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveTagFromAssetRegistry: Attempt to remove None tag."));
		return;
	}

	int32* CountPtr = RegisteredTags.Find(TagToRemove);
	if (!CountPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveTagFromAssetRegistry: Tag '%s' not found in registry."), *TagToRemove.ToString());
		return;
	}

	int32& Count = *CountPtr;
	if (Count <= 0)
	{
		// 理论上不应该出现负数，若出现则清理并警告
		UE_LOG(LogTemp, Error, TEXT("RemoveTagFromAssetRegistry: Tag '%s' has non-positive count %d. Removing from map."), *TagToRemove.ToString(), Count);
		RegisteredTags.Remove(TagToRemove);
		return;
	}

	Count--;
	if (Count == 0)
	{
		RegisteredTags.Remove(TagToRemove);
		
		// 引用计数归零，删除
		RemoveTagFromAssetRegistry(TagToRemove, Asset);
		
	}

	OnMetaDataDeleted.Broadcast(Asset);

	UE_LOG(LogTemp, Verbose, TEXT("RemoveTagFromAssetRegistry: Tag '%s' now has count %d"), *TagToRemove.ToString(), Count);
}

void UEUSS_MetaDataManager::AddTagToAssetRegistry(const FName TagToAdd, UObject* Asset)
{
	
/*
	// 获取可修改的默认对象（会映射到 DefaultEditor.ini）
	UAssetManagerSettings* Settings = GetMutableDefault<UAssetManagerSettings>();
	if (!Settings)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get UAssetManagerSettings default object."));
		return;
	}
	
	// 添加标签，避免重复
	if (!Settings->MetaDataTagsForAssetRegistry.Contains(TagToAdd))
	{
		FString Value = TEXT("(\"");
		Value.Append(TagToAdd.ToString());
		Value.Append("\")");
		
		// const FString Path = TEXT("E:/TestProject/TestProject/Config/DefaultGame.ini");
		const FString Path = FPaths::ProjectDir() + TEXT("Config/DefaultGame.ini");
		
		Settings->MetaDataTagsForAssetRegistry.Add(TagToAdd);
		
		TSet<FName>& GlobalTagsForAssetRegistry = UObject::GetMetaDataTagsForAssetRegistry();
		for (FName Tag : Settings->MetaDataTagsForAssetRegistry)
		{
			if (!Tag.IsNone())
			{
				if (!GlobalTagsForAssetRegistry.Contains(Tag))
				{
					GlobalTagsForAssetRegistry.Add(Tag);
				}
				else
				{
					// To catch the case where the same tag is used by different users and their settings are synced after edition
					UE_LOG(LogTemp, Warning, TEXT("Cannot use duplicate metadata tag '%s' for Asset Registry"), *Tag.ToString());
				}
			}
		}
		
		GConfig->SetString(TEXT("/Script/Engine.AssetManagerSettings"), TEXT("MetaDataTagsForAssetRegistry"), *Value, Path);

		// 保存修改到 DefaultEditor.ini （EditorPerProjectUserSettings 配置）
		Settings->SaveConfig();
		
		// 保存后刷新缓存
		GConfig->Flush(true, Path);
		Settings->ReloadConfig();
	
		UE_LOG(LogTemp, Log, TEXT("Added metadata tag '%s' and saved config."), *TagToAdd.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Metadata tag '%s' already exists."), *TagToAdd.ToString());
	}
*/
	
	UAssetManagerSettings* Settings = GetMutableDefault<UAssetManagerSettings>();
	if (!Settings)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get UAssetManagerSettings default object."));
		return;
	}

	if (!Settings->MetaDataTagsForAssetRegistry.Contains(TagToAdd))
	{
		// 添加到配置源
		Settings->MetaDataTagsForAssetRegistry.Add(TagToAdd);
		
		// 添加到全局缓存
		TSet<FName>& GlobalTagsForAssetRegistry = UObject::GetMetaDataTagsForAssetRegistry();
		if (!GlobalTagsForAssetRegistry.Contains(TagToAdd))
		{
			GlobalTagsForAssetRegistry.Add(TagToAdd);
		}
		
		// 增量
		Settings->TryUpdateDefaultConfigFile();
		
		IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
		AssetRegistry.AssetUpdateTags(Asset, EAssetRegistryTagsCaller::FullUpdate);

		
		// FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		// IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		
		// IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
		// AssetRegistry.ScanPathsSynchronous({ FPaths::GetPath(Asset->GetPathName()) }, true);

		// FAssetData AssetData(Asset);
		// TArray<FAssetData> AssetsToSave;
		// AssetsToSave.Add(AssetData);
		// AssetRegistry.AssetsSaved(MoveTemp(AssetsToSave));

		// 手动触发该资产的标签更新，FullUpdate 会模拟保存包时的行为[reference:4]
		// AssetRegistry.AssetUpdateTags(Asset, EAssetRegistryTagsCaller::FullUpdate);
		
	}
	
}

void UEUSS_MetaDataManager::RemoveTagFromAssetRegistry(const FName TagToRemove, UObject* Asset)
{
	// UAssetManagerSettings* Settings = GetMutableDefault<UAssetManagerSettings>();
	// if (!Settings)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Failed to get UAssetManagerSettings default object."));
	// 	return;
	// }
	//
	// // const FString Path = TEXT("E:/TestProject/TestProject/Config/DefaultGame.ini");
	// const FString Path = FPaths::ProjectDir() + TEXT("Config/DefaultGame.ini");
	//
	// FString Tags2;
	//
	// GConfig->GetString(TEXT("/Script/Engine.AssetManagerSettings"),
	// 	TEXT("MetaDataTagsForAssetRegistry"),
	// 	Tags2,
	// 	Path  // 指向项目的 DefaultGame.ini
	// );
	//
	// Tags2 = RemoveTagFromMetaDataString(Tags2, TagToRemove);
	//
	// if (Settings->MetaDataTagsForAssetRegistry.Contains(TagToRemove))
	// {
	// 	
	// 	Settings->MetaDataTagsForAssetRegistry.Remove(TagToRemove);
	//
	// 	TSet<FName>& GlobalTags = UObject::GetMetaDataTagsForAssetRegistry();
	// 	GlobalTags.Empty(); // 清空全局
	// 	// 把剩余的有效标签重新注册
	// 	for (FName Tag : Settings->MetaDataTagsForAssetRegistry)
	// 	{
	// 		if (!Tag.IsNone() && !GlobalTags.Contains(Tag))
	// 		{
	// 			GlobalTags.Add(Tag);
	// 		}
	// 	}
	// 	
	// }
	//
	// Settings->SaveConfig();
	//
	// GConfig->Flush(true, Path);
	//
	// Settings->ReloadConfig();

	UAssetManagerSettings* Settings = GetMutableDefault<UAssetManagerSettings>();
	if (!Settings)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get UAssetManagerSettings default object."));
		return;
	}

	if (Settings->MetaDataTagsForAssetRegistry.Contains(TagToRemove))
	{
		Settings->MetaDataTagsForAssetRegistry.Remove(TagToRemove);

		TSet<FName>& GlobalTagsForAssetRegistry = UObject::GetMetaDataTagsForAssetRegistry();
		GlobalTagsForAssetRegistry.Remove(TagToRemove);
		Settings->TryUpdateDefaultConfigFile();

		IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
		AssetRegistry.AssetUpdateTags(Asset, EAssetRegistryTagsCaller::FullUpdate);
	}

}

FString UEUSS_MetaDataManager::RemoveTagFromMetaDataString(const FString& InputString, const FName& TagToRemove)
{
	// 1. 校验格式：必须以 '(' 开头和 ')' 结尾
	if (!InputString.StartsWith(TEXT("(")) || !InputString.EndsWith(TEXT(")")))
	{
		return InputString; // 格式错误，原样返回
	}

	// 2. 提取括号内的内容
	FString Inner = InputString.Mid(1, InputString.Len() - 2);
	Inner = Inner.TrimStartAndEnd();

	// 3. 解析出所有带引号的字符串项
	TArray<FString> Items;
	bool bInQuote = false;
	FString CurrentItem;

	for (int32 i = 0; i < Inner.Len(); ++i)
	{
		TCHAR Ch = Inner[i];
		if (Ch == '"')
		{
			if (bInQuote)
			{
				// 结束一个项
				Items.Add(CurrentItem);
				CurrentItem.Empty();
				bInQuote = false;
				// 跳过后面的逗号和空格
				while (i + 1 < Inner.Len() && (Inner[i + 1] == ',' || Inner[i + 1] == ' '))
				{
					++i;
				}
			}
			else
			{
				// 开始一个新项
				bInQuote = true;
				CurrentItem.Empty();
			}
		}
		else if (bInQuote)
		{
			CurrentItem.AppendChar(Ch);
		}
		// 不在引号内时忽略逗号、空格等分隔符
	}

	// 4. 移除目标标签（字符串比较）
	FString TargetStr = TagToRemove.ToString();
	Items.RemoveAll([&](const FString& Item) { return Item == TargetStr; });

	// 5. 重新构建格式化的字符串
	FString Result;
	Result.AppendChar('(');
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (i > 0)
		{
			Result.Append(TEXT(","));
		}
		Result.AppendChar('"');
		Result.Append(Items[i]);
		Result.AppendChar('"');
	}
	Result.AppendChar(')');

	return Result;
}

// void UEUSS_MetaDataManager::CreateWidgetBP()
// {
// 	//加载蓝图中的资产（硬编码）
// 	// UEditorUtilityWidgetBlueprint* WidgetBP = LoadObject<UEditorUtilityWidgetBlueprint>(
// 	// 	nullptr, 
// 	// 	TEXT("/test/元数据Widget/EUWBP_MetaData.EUWBP_MetaData") // 替换为指定的蓝图路径
// 	// );
//
// 	
// 	//验证加载是否成功
// 	if (!My_EUBP.Get())
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Failed to load Editor Utility Widget Blueprint."));
// 		return;
// 	}
// 	
// 	// FName UtilityTabName = FName(*(EditorWidget->GetPathName() + EditorWidget->GetFName().ToString()));
// 	
// 	//查找 UtilityWidget 是否创建
// 	UEditorUtilityWidget* UtilityWidget = FindUtilityWidgetFromBlueprint(My_EUBP.Get());
// 	if (UtilityWidget)
// 	{
// 		// 已创建时将窗口显示出来并聚焦
// 		//获得 UtilityWidget 的 父窗口
// 		TSharedPtr<SWidget> ParentWidget = UtilityWidget->GetCachedWidget()->GetParentWidget();
// 		if (ParentWidget.IsValid())
// 		{
// 			// 转换为 SWindow 类型
// 			TSharedRef<SWidget> ParentWidgetRef = ParentWidget.ToSharedRef();
// 			TSharedPtr<SWindow> WidgetWindow = FSlateApplication::Get().FindWidgetWindow(ParentWidgetRef);
// 			
// 			// 将窗口换入前方
// 			WidgetWindow->BringToFront();
// 			// 聚焦窗口
// 			FSlateApplication::Get().SetUserFocus(FSlateApplication::Get().GetUserIndexForKeyboard(), 
// 													   WidgetWindow->GetContent());
// 		}
// 	}
// 	else
// 	{
// 		// 未创建时则创建
// 		SpawnAndRegisterTab(My_EUBP.Get());
// 	}
// 	
// }

// FVector2D UEUSS_MetaDataManager::GetAbsolutePosition(UWidget* Widget)
// {
// 	// 假设你有一个 UUserWidget 的指针 Widget
// 	if (Widget)
// 	{
// 		// 1. 获取控件的几何数据
// 		const FGeometry& WidgetGeometry = Widget->GetCachedGeometry();
//     
// 		// 2. 解析出绝对位置（控件左上角在屏幕上的位置）
// 		// 这个方法返回的是 FVector2D，它的源码可以在 Engine/Source/Runtime/SlateCore/Public/Layout/Geometry.h 中找到[reference:3]
// 		FVector2D AbsolutePosition = WidgetGeometry.GetAbsolutePosition();
//
// 		UE_LOG(LogTemp, Error, TEXT("Widget Position: %s"), TEXT("成功"));
// 		return AbsolutePosition;
// 		// 现在，AbsolutePosition 就是你需要的坐标
// 	}
//
// 	UE_LOG(LogTemp, Error, TEXT("Widget Position: %s"), TEXT("失败"));
// 	return FVector2D();
// }

// UUserWidget* UEUSS_MetaDataManager::OpenModalWindow(TSubclassOf<UUserWidget> BatchAddWidget, UObject* AssetRef, TArray<FIntObjectPair> AssetInfoMap, UEditorUtilityWidget* UtilityWidget, FVector2D WindowSize)
// {
// 	if (!GEditor) return nullptr;
// 	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
// 	if (!EditorWorld) return nullptr;
// 	
// 	// 1. 获取父窗口（通常是主编辑器窗口）
// 	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
//     
// 	// 2. 创建 Slate 窗口
// 	TSharedRef<SWindow> ModalWindow = SNew(SWindow)
// 		.Title(FText::FromString(TEXT("模态窗口")))
// 		.ClientSize(WindowSize);
// 	
// 	// UEditorUtilityWidget* UtilityWidget = SpawnAndRegisterTab(BatchAddWidget.Get());
// 	// 3. 创建新窗口的内容
// 	UUserWidget* WidgetContent = CreateWidget<UUserWidget>(EditorWorld, BatchAddWidget);
// 	if (!WidgetContent) return nullptr;
// 	TSharedRef<SWidget> SlateContent = WidgetContent->TakeWidget();
// 	// TSharedPtr<SWidget> SlateContent_Ptr = UtilityWidget->GetCachedWidget()->GetParentWidget();
// 	// TSharedRef<SWidget> SlateContent = SlateContent_Ptr.ToSharedRef();
// 		
// 	// 4. 将内容设置到窗口
// 	ModalWindow->SetContent(SlateContent);
// 	
// 	OnWidgetCreated.Broadcast(AssetRef, AssetInfoMap, UtilityWidget);
// 	
// 	// 5. 将窗口显示为模态，这会阻塞直到窗口关闭
// 	FSlateApplication::Get().AddModalWindow(ModalWindow, ParentWindow);
//
// 	return WidgetContent;
// }

// UEditorUtilityWidget* UEUSS_MetaDataManager::OpenModalWindow_EUW(UEditorUtilityWidgetBlueprint* InBlueprint, FVector2D WindowSize)
// {
// 	if (!GEditor) return nullptr;
// 	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
// 	if (!EditorWorld) return nullptr;
// 	
// 	// 1. 获取父窗口（通常是主编辑器窗口）
// 	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
//     
// 	// 2. 创建 Slate 窗口
// 	TSharedRef<SWindow> ModalWindow = SNew(SWindow)
// 		.Title(FText::FromString(TEXT("模态窗口")))
// 		.ClientSize(WindowSize);
// 	
// 	UBlueprint* Blueprint = LoadObject<UBlueprint>(NULL, *InBlueprint->GetPathName(), NULL, LOAD_None, NULL);
// 	if (!Blueprint) return nullptr;
//
// 	TSubclassOf<UEditorUtilityWidget> WidgetClass(Blueprint->GeneratedClass);
// 	
// 	UEditorUtilityWidget* EditorUtilityWidget = CreateWidget<UEditorUtilityWidget>(EditorWorld, WidgetClass);
//
// 	TSharedRef<SWidget> SlateContent = EditorUtilityWidget->TakeWidget();
//
// 	ModalWindow->SetContent(SlateContent);
//
// 	OnBatchAddWidgetCreated.Broadcast(EditorUtilityWidget);
// 	
// 	FSlateApplication::Get().AddModalWindow(ModalWindow, ParentWindow);
//
// 	return EditorUtilityWidget;
// }
