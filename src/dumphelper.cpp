#include "dumphelper.h"

#include <QDir>

#ifdef Q_OS_WIN

#include <Windows.h>
#include <DbgHelp.h>

using MiniDumpWriteDumpFunc = BOOL(WINAPI *)(HANDLE hProcess,
                                             DWORD ProcessId,
                                             HANDLE hFile,
                                             MINIDUMP_TYPE DumpType,
                                             PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                             PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                             PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

static QString g_dumpFilePath;
static MINIDUMP_TYPE g_dumpType = MINIDUMP_TYPE::MiniDumpNormal;
static DumpHelper::DumpNameGenerator g_pDumpNameGenerator = nullptr;
static DumpHelper::AfterWriteDumpCallback g_pAfterWriteDumpCallback = nullptr;
static MiniDumpWriteDumpFunc g_pMiniDumpWriteDump = nullptr;

static inline bool initializeFunctionPointers()
{
    static bool initialized = false;
    if (!initialized) {
        HMODULE module = LoadLibraryW(L"Dbghelp.dll");
        if (module) {
            if (!g_pMiniDumpWriteDump) {
                g_pMiniDumpWriteDump = reinterpret_cast<MiniDumpWriteDumpFunc>(GetProcAddress(module, "MiniDumpWriteDump"));
                if (!g_pMiniDumpWriteDump) {
                    return false;
                }
            }
            initialized = true;
        }
    }

    return initialized;
}

BOOL WINAPI myMiniDumpCallback(PVOID, const PMINIDUMP_CALLBACK_INPUT input, PMINIDUMP_CALLBACK_OUTPUT output)
{
    if (input == nullptr || output == nullptr)
        return FALSE;

    BOOL ret = FALSE;
    switch (input->CallbackType) {
    case IncludeModuleCallback:
    case IncludeThreadCallback:
    case ThreadCallback:
    case ThreadExCallback:
        ret = TRUE;
        break;
    case ModuleCallback: {
        if (!(output->ModuleWriteFlags & ModuleReferencedByMemory)) {
            output->ModuleWriteFlags &= ~ModuleWriteModule;
        }
        ret = TRUE;
    } break;
    default:
        break;
    }

    return ret;
}

LONG WINAPI myUnhandledExceptionFilter(EXCEPTION_POINTERS *exp)
{
    if (initializeFunctionPointers()) {
        QString dumpFilePath = g_dumpFilePath + "/" + g_pDumpNameGenerator();
        HANDLE hFile = ::CreateFileW(dumpFilePath.toStdWString().c_str(), GENERIC_WRITE | GENERIC_READ,
                                     FILE_SHARE_WRITE | FILE_SHARE_READ, nullptr, CREATE_ALWAYS,
                                     FILE_ATTRIBUTE_NORMAL, nullptr);
        MINIDUMP_EXCEPTION_INFORMATION info;
        info.ThreadId = ::GetCurrentThreadId();
        info.ExceptionPointers = exp;
        info.ClientPointers = FALSE;
        MINIDUMP_CALLBACK_INFORMATION mci;
        mci.CallbackRoutine = myMiniDumpCallback;
        mci.CallbackParam = nullptr;
        g_pMiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, g_dumpType, &info, nullptr, &mci);
        ::CloseHandle(hFile);

        if (g_pAfterWriteDumpCallback)
            g_pAfterWriteDumpCallback();
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

#endif //Q_OS_WIN

QString DumpHelper::dumpGenerationPath()
{
    return g_dumpFilePath;
}

void DumpHelper::setDumpGenerationParams(const QString &path, DumpNameGenerator generator, AfterWriteDumpCallback callback, int dumpType)
{
    Q_ASSERT_X(generator, "setDumpGenerationParams", "DumpNameGenerator callback is nullptr!");

    g_dumpFilePath = path;
    g_pDumpNameGenerator = generator;
    g_pAfterWriteDumpCallback = callback;

#ifdef Q_OS_WIN
    g_dumpType = static_cast<MINIDUMP_TYPE>(dumpType);
#endif //Q_OS_WIN

    QDir dir(g_dumpFilePath);
    if (!dir.exists()) {
        dir.mkdir(g_dumpFilePath);
    }
}

void DumpHelper::runDumpProcess()
{
#ifdef Q_OS_WIN
    ::SetUnhandledExceptionFilter(myUnhandledExceptionFilter);
#endif //Q_OS_WIN
}
