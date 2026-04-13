# MouseJumpTool

[English](../README.en.md) | [简体中文](README.cn.md) | [日本語](README.ja.md) | [한국어](README.ko.md)

A Windows tool that helps you quickly switch your mouse cursor between multiple monitors.

## 功能特性

- **自動檢測多顯示器**: 自動識別當前系統中的所有顯示器
- **倒數計時**: 在下一個顯示器上顯示倒數計時提示，提示用戶進行滑鼠移動
- **光標指示器**: 顯示當前光標位置，支援圓角矩形和圓形兩種樣式
- **單實例運行**: 確保同一時間只有一個實例運行
- **托盤圖示**: 系統托盤圖示支援啟動、退出操作

## 使用方式

1. 運行程序，程序會自動檢測並顯示所有連接的顯示器
2. 當滑鼠移動到目標顯示器邊緣時，會在下一個顯示器上顯示倒數
3. 倒數結束後，光標會跳到目標顯示器
4. 使用系統托盤圖示可以退出程序

### 快速切換

- 按 `Ctrl + Shift + 1` 切換到第 1 個螢幕
- 按 `Ctrl + Shift + 2` 切換到第 2 個螢幕
- 按 `Ctrl + Shift + 3` 切換到第 3 個螢幕
- 以此類推，用於切換到更多螢幕

## 系統要求

- Windows 7 及以上版本
- 支援多顯示器配置

## 構建

使用 Visual Studio 打開 `MyMouneTools.sln` 解決方案文件進行編譯。

## 許可證

MIT License

---

## Languages

- [English](../README.en.md)
- [简体中文](README.cn.md)
- [日本語](README.ja.md)
- [한국어](README.ko.md)
