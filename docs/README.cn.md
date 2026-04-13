# MouseJumpTool

[English](../README.en.md) | [日本語](README.ja.md) | [繁體中文](README.tw.md) | [한국어](README.ko.md)

A Windows tool that helps you quickly switch your mouse cursor between multiple monitors.

## 功能特性

- **自动检测多显示器**: 自动识别当前系统中的所有显示器
- **倒计时提示**: 在下一个显示器上显示倒计时提示，提示用户进行鼠标移动
- **光标指示器**: 显示当前光标位置，支持圆角矩形和圆形两种样式
- **单实例运行**: 确保同一时间只有一个实例运行
- **托盘图标**: 系统托盘图标支持启动、退出操作

## 使用方式

1. 运行程序，程序会自动检测并显示所有连接的显示器
2. 当鼠标移动到目标显示器边缘时，会在下一个显示器上显示倒计时
3. 倒计时结束后，光标会跳转到目标显示器
4. 使用系统托盘图标可以退出程序

### 快速切换

- 按 `Ctrl + Shift + 1` 切换到第 1 个屏幕
- 按 `Ctrl + Shift + 2` 切换到第 2 个屏幕
- 按 `Ctrl + Shift + 3` 切换到第 3 个屏幕
- 以此类推，用于切换到更多屏幕

## 系统要求

- Windows 7 及以上版本
- 支持多显示器配置

## 构建

使用 Visual Studio 打开 `MyMouneTools.sln` 解决方案文件进行编译。

## 许可证

MIT License

---

## Languages

- [English](../README.en.md)
- [日本語](README.ja.md)
- [繁體中文](README.tw.md)
- [한국어](README.ko.md)
