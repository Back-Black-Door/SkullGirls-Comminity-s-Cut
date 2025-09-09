#include <iostream>

#include "Thread.h"
#include "../dll_proxy/dll_proxy.h"
#include "../Console.h"

// Глобальный экземпляр
IsolatedThread g_workerThread;

IsolatedThread::IsolatedThread() {
    m_tlsIndex = TlsAlloc();
    m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_hStopEvent == NULL) {
        // Обработка ошибки создания события
        throw std::runtime_error("Failed to create stop event");
    }
}

IsolatedThread::~IsolatedThread() {
    Stop();
    if (m_tlsIndex != TLS_OUT_OF_INDEXES) {
        TlsFree(m_tlsIndex);
    }
    if (m_hStopEvent) {
        CloseHandle(m_hStopEvent);
        m_hStopEvent = NULL;
    }
}

DWORD WINAPI IsolatedThread::ThreadProc(LPVOID lpParam) {
    IsolatedThread* pThis = static_cast<IsolatedThread*>(lpParam);

    __try {
        if (pThis->m_tlsIndex != TLS_OUT_OF_INDEXES) {
            TlsSetValue(pThis->m_tlsIndex, reinterpret_cast<LPVOID>(1));
        }

        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
        return pThis->Run();
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return GetExceptionCode();
    }
}

bool IsolatedThread::Start() {
    if (m_running.load()) {
        return false;
    }

    ResetEvent(m_hStopEvent);

    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
    sa.bInheritHandle = FALSE;

    m_hThread = CreateThread(&sa, 0, ThreadProc, this, 0, NULL);
    if (m_hThread == NULL) {
        DWORD error = GetLastError();
        Console::DLL_DebugWriteOutput(("CreateThread failed with error: " + std::to_string(error) + "\n").c_str());
        return false;
    }

    m_running = true;
    return true;
}

void IsolatedThread::Stop() {
    if (!m_running.load()) {
        return;
    }

    m_running = false;
    SetEvent(m_hStopEvent);

    if (m_hThread) {
        DWORD waitResult = WaitForSingleObject(m_hThread, 5000); // 5 секунд таймаут
        if (waitResult == WAIT_TIMEOUT) {
            Console::DLL_DebugWriteOutput("Warning: Thread termination timeout\n");
            TerminateThread(m_hThread, 0);
        }
        CloseHandle(m_hThread);
        m_hThread = NULL;
    }
}

bool IsolatedThread::IsOurThread() const {
    if (m_tlsIndex == TLS_OUT_OF_INDEXES) {
        return false;
    }

    LPVOID value = TlsGetValue(m_tlsIndex);
    return (value != NULL) && (value != (LPVOID)0xDEADBEEF); // Добавляем проверку на специальное значение
}


extern "C" __declspec(naked) void __stdcall ProxyFunction_ThreadStart()
{
    __asm {
        // Сохраняем все регистры
        pushad
        pushfd
    }

    g_workerThread.Start();

    __asm {
        // Восстанавливаем регистры
        popfd
        popad

        // Переходим к оригинальной функции
        jmp[PROXY_FUNC_ADRESS + 10 * 4]
    }
}