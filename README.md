# DumpHelper - 简单崩溃转储生成工具

基于 Qt 和 Windows API 实现的轻量级崩溃生成工具，用于捕获应用程序异常并生成 MiniDump 文件。

## 功能特性

- 支持多种MiniDump类型（完整内存/线程信息/模块头等）
- 自定义dump文件生成路径
- 支持自定义文件名生成策略
- 处理结束回调
- 跨平台支持（当前实现Windows版本）

## 如何使用

1. 克隆仓库
2. 拷贝`dumphelper.h`和`dumphelper.cpp` 到你的项目中
3. 调用 `DumpHelper::setDumpGenerationParams` 配置相关参数
3. 调用 `DumpHelper::runDumpProcess()` 启用崩溃捕获

## 快速开始

```cpp
// 初始化配置
DumpHelper::setDumpGenerationParams(app.applicationDirPath() + "/crash", // 存储路径
                                        []() -> QString { // 文件名生成器
                                            return QString("%1_%2.dmp").arg("crash", QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
                                        },
                                        []{ // 事后回调
                                            qDebug() << "Crash! The dump files in:" << DumpHelper::dumpGenerationPath();
                                            QDesktopServices::openUrl(QUrl::fromLocalFile(DumpHelper::dumpGenerationPath()));
                                        },
                                        DumpHelper::MiniDumpScanMemory |
                                            DumpHelper::MiniDumpWithIndirectlyReferencedMemory);
// 启用崩溃捕获
DumpHelper::runDumpProcess();
```

## 自定义文件名

```cpp
QString customNameGenerator() {
    return QString("appcrash_%1.dmp").arg(QTime::currentTime().toString("hhmmsszzz"));
}
```

## 注意事项

1. Windows平台需要确保：
   - 存在 `dbghelp.dll`
2. 完整内存dump文件较大，建议配合压缩使用(通过AfterWriteDumpCallback)
3. 生产环境建议组合使用：`MiniDumpWithThreadInfo | MiniDumpWithUnloadedModules`

## 许可证

使用 `MIT LICENSE`

## 环境

Windows 11, Qt Version >= 5