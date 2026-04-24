# 顺序表实验简洁前端

## 文件说明

- `seq_list_demo.cpp`：原控制台版顺序表实验程序
- `seq_list_frontend.cpp`：简洁 Windows 图形界面前端
- `seq_list_demo.exe`：控制台目标程序
- `seq_list_frontend.exe`：图形界面目标程序

## 图形界面功能

- 新建、切换、删除多个线性表
- 初始化、销毁、清空、插入、删除、遍历
- 获取元素、查找元素、前驱、后继
- 排序、最大连续子数组和、和为 K 的子数组个数
- 文件保存、文件加载

## 编译方法

在 `zhuangya_dp` 目录下执行：

```bash
g++ -std=c++17 -O2 -Wall -Wextra seq_list_frontend.cpp -o seq_list_frontend.exe -mwindows
```

如果你的编译器对 `fopen_s` 支持不完整，可以改用 MSVC，或者我再给你出一个兼容 MinGW 的版本。

## 使用方法

1. 双击运行 `seq_list_frontend.exe`
2. 在左侧输入表名、下标、数值、文件名、K 值
3. 点击按钮执行对应操作
4. 右下角日志区会显示操作结果，中间列表框显示当前线性表内容

## 注意

- 默认启动时会创建一个名为 `default` 的线性表
- 如果你先执行 Destroy，再进行其他操作，需要重新 Init 或新建表
- 文件保存格式为二进制，适合本程序直接加载