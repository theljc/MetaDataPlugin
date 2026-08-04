// // TestPluginChordCustomization.cpp — FTestPluginChord 属性自定义显示实现
//
// #include "TestPluginChordCustomization.h"
// #include "TestPluginChord.h"
// #include "DetailWidgetRow.h"
// #include "IDetailChildrenBuilder.h"
// #include "PropertyHandle.h"
// #include "Widgets/Input/SCheckBox.h"
// #include "Widgets/Layout/SWrapBox.h"
// #include "Widgets/Text/STextBlock.h"
// #include "SKeySelector.h"
//
// #define LOCTEXT_NAMESPACE "TestPluginChordCustomization"
//
// TSharedRef<IPropertyTypeCustomization> FTestPluginChordCustomization::MakeInstance()
// {
// 	return MakeShareable(new FTestPluginChordCustomization);
// }
//
// void FTestPluginChordCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
// 	FDetailWidgetRow& HeaderRow,
// 	IPropertyTypeCustomizationUtils& CustomizationUtils)
// {
// 	// 获取子属性句柄
// 	KeyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTestPluginChord, Key));
// 	ShiftHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTestPluginChord, bShift));
// 	CtrlHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTestPluginChord, bCtrl));
// 	AltHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTestPluginChord, bAlt));
// 	CmdHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTestPluginChord, bCmd));
//
// 	// 监听修饰键属性变更，自动刷新显示
// 	if (ShiftHandle.IsValid()) ShiftHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FTestPluginChordCustomization::OnModifierChanged));
// 	if (CtrlHandle.IsValid()) CtrlHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FTestPluginChordCustomization::OnModifierChanged));
// 	if (AltHandle.IsValid()) AltHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FTestPluginChordCustomization::OnModifierChanged));
// 	if (CmdHandle.IsValid()) CmdHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FTestPluginChordCustomization::OnModifierChanged));
//
// 	// 左侧：显示快捷键文本（如 "Shift + Alt + Z"）
// 	HeaderRow.NameContent()
// 	[
// 		PropertyHandle->CreatePropertyNameWidget()
// 	];
//
// 	// 右侧：按键选择器 + 修饰键复选框
// 	HeaderRow.ValueContent()
// 	.MinDesiredWidth(200.0f)
// 	.MaxDesiredWidth(400.0f)
// 	[
// 		SNew(SHorizontalBox)
// 		// 按键选择器
// 		+ SHorizontalBox::Slot()
// 		.AutoWidth()
// 		.VAlign(VAlign_Center)
// 		.Padding(0, 0, 4, 0)
// 		[
// 			SNew(SBox)
// 			.MinDesiredWidth(125.0f)
// 			.MaxDesiredWidth(250.0f)
// 			[
// 				SNew(SKeySelector)
// 				// .CurrentKey(this, &FTestPluginChordCustomization::GetCurrentKey)
// 				// .OnKeyChanged(this, &FTestPluginChordCustomization::OnKeyChanged)
// 				.AllowClear(true)
// 				.Font(CustomizationUtils.GetRegularFont())
// 			]
// 		]
// 		// 修饰键复选框
// 		+ SHorizontalBox::Slot()
// 		.AutoWidth()
// 		.VAlign(VAlign_Center)
// 		[
// 			SNew(SWrapBox)
// 			.UseAllottedSize(true)
// 			+ SWrapBox::Slot()
// 			[
// 				SNew(SCheckBox)
// 				.Style(FAppStyle::Get(), "KeyboardShortcut.ModifierKey")
// 				.ToolTipText(LOCTEXT("ShiftTooltip", "Shift"))
// 				.IsChecked_Lambda([this]() -> ECheckBoxState
// 				{
// 					bool bVal = false;
// 					if (ShiftHandle.IsValid()) ShiftHandle->GetValue(bVal);
// 					return bVal ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
// 				})
// 				.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
// 				{
// 					if (ShiftHandle.IsValid())
// 					{
// 						ShiftHandle->SetValue(State == ECheckBoxState::Checked);
// 						OnModifierChanged();
// 					}
// 				})
// 				[
// 					SNew(STextBlock)
// 					.Text(LOCTEXT("ShiftLabel", "Shift"))
// 					.Font(CustomizationUtils.GetRegularFont())
// 				]
// 			]
// 			+ SWrapBox::Slot()
// 			[
// 				SNew(SCheckBox)
// 				.Style(FAppStyle::Get(), "KeyboardShortcut.ModifierKey")
// 				.ToolTipText(LOCTEXT("CtrlTooltip", "Ctrl"))
// 				.IsChecked_Lambda([this]() -> ECheckBoxState
// 				{
// 					bool bVal = false;
// 					if (CtrlHandle.IsValid()) CtrlHandle->GetValue(bVal);
// 					return bVal ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
// 				})
// 				.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
// 				{
// 					if (CtrlHandle.IsValid())
// 					{
// 						CtrlHandle->SetValue(State == ECheckBoxState::Checked);
// 						OnModifierChanged();
// 					}
// 				})
// 				[
// 					SNew(STextBlock)
// 					.Text(LOCTEXT("CtrlLabel", "Ctrl"))
// 					.Font(CustomizationUtils.GetRegularFont())
// 				]
// 			]
// 			+ SWrapBox::Slot()
// 			[
// 				SNew(SCheckBox)
// 				.Style(FAppStyle::Get(), "KeyboardShortcut.ModifierKey")
// 				.ToolTipText(LOCTEXT("AltTooltip", "Alt"))
// 				.IsChecked_Lambda([this]() -> ECheckBoxState
// 				{
// 					bool bVal = false;
// 					if (AltHandle.IsValid()) AltHandle->GetValue(bVal);
// 					return bVal ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
// 				})
// 				.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
// 				{
// 					if (AltHandle.IsValid())
// 					{
// 						AltHandle->SetValue(State == ECheckBoxState::Checked);
// 						OnModifierChanged();
// 					}
// 				})
// 				[
// 					SNew(STextBlock)
// 					.Text(LOCTEXT("AltLabel", "Alt"))
// 					.Font(CustomizationUtils.GetRegularFont())
// 				]
// 			]
// 			+ SWrapBox::Slot()
// 			[
// 				SNew(SCheckBox)
// 				.Style(FAppStyle::Get(), "KeyboardShortcut.ModifierKey")
// 				.ToolTipText(LOCTEXT("CmdTooltip", "Cmd (Mac)"))
// 				.IsChecked_Lambda([this]() -> ECheckBoxState
// 				{
// 					bool bVal = false;
// 					if (CmdHandle.IsValid()) CmdHandle->GetValue(bVal);
// 					return bVal ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
// 				})
// 				.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
// 				{
// 					if (CmdHandle.IsValid())
// 					{
// 						CmdHandle->SetValue(State == ECheckBoxState::Checked);
// 						OnModifierChanged();
// 					}
// 				})
// 				[
// 					SNew(STextBlock)
// 					.Text(LOCTEXT("CmdLabel", "Cmd"))
// 					.Font(CustomizationUtils.GetRegularFont())
// 				]
// 			]
// 		]
// 	];
// }
//
// void FTestPluginChordCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
// 	IDetailChildrenBuilder& ChildBuilder,
// 	IPropertyTypeCustomizationUtils& CustomizationUtils)
// {
// 	// 不显示子属性（所有编辑都在 Header 行中完成）
// 	// 子属性已通过 Header 中的自定义控件进行编辑
// }
//
// FText FTestPluginChordCustomization::GetChordText() const
// {
// 	FKey CurrentKey = GetCurrentKey();
//
// 	bool bS = false, bC = false, bA = false, bM = false;
// 	if (ShiftHandle.IsValid()) ShiftHandle->GetValue(bS);
// 	if (CtrlHandle.IsValid()) CtrlHandle->GetValue(bC);
// 	if (AltHandle.IsValid()) AltHandle->GetValue(bA);
// 	if (CmdHandle.IsValid()) CmdHandle->GetValue(bM);
//
// 	if (!CurrentKey.IsValid())
// 	{
// 		return LOCTEXT("NoKey", "None");
// 	}
//
// 	FInputChord Chord(CurrentKey);
// 	Chord.bShift = bS;
// 	Chord.bCtrl = bC;
// 	Chord.bAlt = bA;
// 	Chord.bCmd = bM;
//
// 	return Chord.GetInputText();
// }
//
// FKey FTestPluginChordCustomization::GetCurrentKey() const
// {
// 	if (!KeyHandle.IsValid())
// 	{
// 		return EKeys::Invalid;
// 	}
//
// 	FKey Result;
// 	FString KeyStr;
// 	if (KeyHandle->GetValue(KeyStr) == FPropertyAccess::Success)
// 	{
// 		Result = FKey(*KeyStr);
// 	}
// 	return Result;
// }
//
// void FTestPluginChordCustomization::OnKeyChanged(FKey NewKey)
// {
// 	if (KeyHandle.IsValid())
// 	{
// 		// KeyHandle->SetValue(NewKey);
// 	}
// }
//
// void FTestPluginChordCustomization::OnModifierChanged()
// {
// 	// 修饰键变更时刷新显示（更新 chord 文本）
// }
//
// #undef LOCTEXT_NAMESPACE
