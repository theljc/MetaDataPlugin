#include "Settings/MetaDataPluginSettings.h"


void UMetaDataPluginSettings::SaveMainWidgetPath(const FString& InMainWidgetPath)
{
	MainWidgetPath = InMainWidgetPath;
	TryUpdateDefaultConfigFile();
}
