#pragma once
#include <Windows.h>
#include <atomic>

class IsolatedThread {
private:
    HANDLE m_hThread = NULL;
    HANDLE m_hStopEvent = NULL;
    DWORD m_tlsIndex = TLS_OUT_OF_INDEXES;
    std::atomic<bool> m_running{ false };

    // ����������� ����� ��� ����� ����� ������
    static DWORD WINAPI ThreadProc(LPVOID lpParam);

    // �������� ����� ���������� (����� ���������� � dllmain)
    DWORD Run();

public:
    IsolatedThread();
    ~IsolatedThread();

    bool Start();
    void Stop();
    bool IsOurThread() const;
    bool IsRunning() const { return m_running.load(); }

    // ��������� �����������
    IsolatedThread(const IsolatedThread&) = delete;
    IsolatedThread& operator=(const IsolatedThread&) = delete;
};

// ���������� ���������
extern IsolatedThread g_workerThread;