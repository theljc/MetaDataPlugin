// TestPluginChord.h — 快捷键配置结构体
// 将按键和修饰键（Shift/Ctrl/Alt/Cmd）打包为一个逻辑单元

// #pragma once
//
// #include "CoreMinimal.h"
// #include "InputCoreTypes.h"
// #include "TestPluginChord.generated.h"
//
// struct FInputChord;

/**
 * FTestPluginChord — 单个快捷键的配置
 *
 * 包含一个主按键和四个修饰键标志。
 * 提供 ToInputChord() 转换为 Slate 的 FInputChord，
 * 以及 GetChordText() 生成可读的快捷键显示文本（如 "Shift + Alt + Z"）。
 */
// USTRUCT(BlueprintType)
// struct TEST_API FTestPluginChord
// {
// 	GENERATED_BODY()
//
// 	/** 主按键（例如 Z、X、SpaceBar 等）*/
// 	UPROPERTY(config, EditAnywhere, Category = "Chord")
// 	FKey Key;
//
// 	/** 是否需要同时按下 Shift */
// 	UPROPERTY(config, EditAnywhere, Category = "Chord")
// 	bool bShift = false;
//
// 	/** 是否需要同时按下 Ctrl */
// 	UPROPERTY(config, EditAnywhere, Category = "Chord")
// 	bool bCtrl = false;
//
// 	/** 是否需要同时按下 Alt */
// 	UPROPERTY(config, EditAnywhere, Category = "Chord")
// 	bool bAlt = false;
//
// 	/** 是否需要同时按下 Cmd（Mac）*/
// 	UPROPERTY(config, EditAnywhere, Category = "Chord")
// 	bool bCmd = false;
//
// 	/** 转换为 Slate 的 FInputChord */
// 	FInputChord ToInputChord() const;
//
// 	/** 生成可读的快捷键文本，例如 "Shift + Alt + Z" */
// 	FText GetChordText() const;
//
// 	/** 是否配置了有效的按键 */
// 	bool IsValidChord() const { return Key.IsValid(); }
// };
