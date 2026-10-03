#include "Utils/MetadataOfAssetsUtils.h"

namespace MetadataOfAssetsUtils
{
	// FString GetWidgetPathFromConfigFile()
	// {
	// 	const FString NormalizedConfigPath = GetNormalizedConfigIniPath();
	// 	FString CurrentPath;
	//
	// 	GConfig->GetString(TEXT("MetaDataPluginConfig"),
	// 		TEXT("CurrentAssetPath"),
	// 		CurrentPath,
	// 		NormalizedConfigPath	// 指向项目的 DefaultGame.ini
	// 	);
	//
	// 	return CurrentPath;
	// }

	// FString GetNormalizedConfigIniPath()
	// {
	// 	static const FString NormalizedConfigPath = FConfigCacheIni::NormalizeConfigIniPath(FPaths::ProjectConfigDir() / TEXT("DefaultGame.ini"));
	// 	return NormalizedConfigPath;
	// }

	UMetaDataPluginSettings* GetMetaDataPluginSettings()
	{
		return GetMutableDefault<UMetaDataPluginSettings>();
	}
}
