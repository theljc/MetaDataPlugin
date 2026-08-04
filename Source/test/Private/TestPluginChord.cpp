// TestPluginChord.cpp — 快捷键配置结构体实现

// #include "TestPluginChord.h"
// #include "Framework/Commands/InputChord.h"

// FInputChord FTestPluginChord::ToInputChord() const
// {
// 	FInputChord Chord(Key);
// 	Chord.bShift = bShift;
// 	Chord.bCtrl = bCtrl;
// 	Chord.bAlt = bAlt;
// 	Chord.bCmd = bCmd;
// 	return Chord;
// }
//
// FText FTestPluginChord::GetChordText() const
// {
// 	if (!Key.IsValid())
// 	{
// 		return FText::FromString(TEXT("None"));
// 	}
//
// 	return ToInputChord().GetInputText();
// }
