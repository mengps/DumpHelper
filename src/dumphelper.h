#ifndef DUMPHELPER_H
#define DUMPHELPER_H

#include <QString>

class DumpHelper
{
public:
    /*! Windows MINIDUMP_TYPE */
    enum DumpType
    {
        MiniDumpNormal                         = 0x00000000,
        MiniDumpWithDataSegs                   = 0x00000001,
        MiniDumpWithFullMemory                 = 0x00000002,
        MiniDumpWithHandleData                 = 0x00000004,
        MiniDumpFilterMemory                   = 0x00000008,
        MiniDumpScanMemory                     = 0x00000010,
        MiniDumpWithUnloadedModules            = 0x00000020,
        MiniDumpWithIndirectlyReferencedMemory = 0x00000040,
        MiniDumpFilterModulePaths              = 0x00000080,
        MiniDumpWithProcessThreadData          = 0x00000100,
        MiniDumpWithPrivateReadWriteMemory     = 0x00000200,
        MiniDumpWithoutOptionalData            = 0x00000400,
        MiniDumpWithFullMemoryInfo             = 0x00000800,
        MiniDumpWithThreadInfo                 = 0x00001000,
        MiniDumpWithCodeSegs                   = 0x00002000,
        MiniDumpWithoutAuxiliaryState          = 0x00004000,
        MiniDumpWithFullAuxiliaryState         = 0x00008000,
        MiniDumpWithPrivateWriteCopyMemory     = 0x00010000,
        MiniDumpIgnoreInaccessibleMemory       = 0x00020000,
        MiniDumpWithTokenInformation           = 0x00040000,
        MiniDumpWithModuleHeaders              = 0x00080000,
        MiniDumpFilterTriage                   = 0x00100000,
        MiniDumpWithAvxXStateContext           = 0x00200000,
        MiniDumpWithIptTrace                   = 0x00400000,
        MiniDumpScanInaccessiblePartialPages   = 0x00800000,
        MiniDumpFilterWriteCombinedMemory      = 0x01000000,
        MiniDumpValidTypeFlags                 = 0x01ffffff,
    };

    using DumpNameGenerator = QString(*)();
    using AfterWriteDumpCallback = void(*)();

    /**
     * @brief dumpGenerationPath 获取dump生成路径
     * @return QString
     */
    static QString dumpGenerationPath();

    /**
     * @brief setDumpGenerationParams 设置dump生成参数
     * @param path dump文件生成路径
     * @param generator dump文件名生成器
     * @param callback dump文件生成后的回调
     * @param dumpType dump文件生成类型
     */
    static void setDumpGenerationParams(const QString &path, DumpNameGenerator generator, AfterWriteDumpCallback callback, int dumpType);

    /**
     * @brief runDumpProcess 运行dump处理流程
     */
    static void runDumpProcess();
};

#endif // DUMPHELPER_H
