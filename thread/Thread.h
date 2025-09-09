#pragma once
#include <Windows.h>
#include <atomic>

class IsolatedThread {
private:
    HANDLE m_hThread = NULL;
    HANDLE m_hStopEvent = NULL;
    DWORD m_tlsIndex = TLS_OUT_OF_INDEXES;
    std::atomic<bool> m_running{ false };

    // Статический метод для точки входа потока
    static DWORD WINAPI ThreadProc(LPVOID lpParam);

    // Основной метод выполнения (будет реализован в dllmain)
    DWORD Run();

public:
    IsolatedThread();
    ~IsolatedThread();

    bool Start();
    void Stop();
    bool IsOurThread() const;
    bool IsRunning() const { return m_running.load(); }

    // Запрещаем копирование
    IsolatedThread(const IsolatedThread&) = delete;
    IsolatedThread& operator=(const IsolatedThread&) = delete;
};

// Глобальный экземпляр
extern IsolatedThread g_workerThread;