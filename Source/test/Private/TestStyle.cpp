// Fill out your copyright notice in the Description page of Project Settings.


#include "TestStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FTestStyle::StyleInstance = NULL;

void FTestStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FTestStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FTestStyle::GetStyleSetName()
{
	return TEXT("TestStyle");
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FTestStyle::Create()
{
	//创建一个新的Style实例：
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet(GetStyleSetName()));

	//设置资源目录，为本插件的Resources目录
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("test")->GetBaseDir() / TEXT("Resources"));

	//注册图标：
	Style->Set("TestCommands.CommandA", new IMAGE_BRUSH(TEXT("Icon128"), Icon40x40));

	return Style;
}

#undef IMAGE_BRUSH


void FTestStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}
