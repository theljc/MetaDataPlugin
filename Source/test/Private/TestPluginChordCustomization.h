// // TestPluginChordCustomization.h — FTestPluginChord 的属性自定义显示
// // 参考编辑器偏好设置中的 Keyboard Shortcuts 样式
//
// #pragma once
//
// #include "CoreMinimal.h"
// #include "IPropertyTypeCustomization.h"
//
// class SKeySelector;
//
// /**
//  * FTestPluginChordCustomization — 自定义 FTestPluginChord 在 Details 面板中的显示
//  *
//  * 将原本散乱的 FKey + 4 个 bool 属性整合为紧凑的一行显示：
//  *   左侧：快捷键名称（如 "Shift + Alt + Z"）
//  *   右侧：按键选择器 + 修饰键复选框
//  * 样式参考 编辑器偏好设置 → Keyboard Shortcuts
//  */
// class FTestPluginChordCustomization : public IPropertyTypeCustomization
// {
// public:
// 	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
//
// 	// IPropertyTypeCustomization interface
// 	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
// 		FDetailWidgetRow& HeaderRow,
// 		IPropertyTypeCustomizationUtils& CustomizationUtils) override;
//
// 	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
// 		IDetailChildrenBuilder& ChildBuilder,
// 		IPropertyTypeCustomizationUtils& CustomizationUtils) override;
//
// private:
// 	/** 获取当前显示的快捷键文本 */
// 	FText GetChordText() const;
//
// 	/** 获取当前选择的按键 */
// 	FKey GetCurrentKey() const;
//
// 	/** 按键被修改时的回调 */
// 	void OnKeyChanged(FKey NewKey);
//
// 	/** 修饰键变更时的回调 */
// 	void OnModifierChanged();
//
// 	/** 持有的 Key 属性的句柄 */
// 	TSharedPtr<IPropertyHandle> KeyHandle;
//
// 	/** 持有的修饰键属性的句柄 */
// 	TSharedPtr<IPropertyHandle> ShiftHandle;
// 	TSharedPtr<IPropertyHandle> CtrlHandle;
// 	TSharedPtr<IPropertyHandle> AltHandle;
// 	TSharedPtr<IPropertyHandle> CmdHandle;
// };
