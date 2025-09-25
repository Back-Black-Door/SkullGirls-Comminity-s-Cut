#pragma once
#include <Windows.h>
#include <atomic>
#include <functional>

class IsolatedThread {
private:
    HANDLE m_hThread = NULL;
    HANDLE m_hStopEvent = NULL;
    DWORD m_tlsIndex = TLS_OUT_OF_INDEXES;
    std::atomic<bool> m_running{ false };
    std::function<DWORD(IsolatedThread*)> m_task;

    static DWORD WINAPI ThreadProc(LPVOID lpParam);
    DWORD Run();

public:
    // ����������� ��������� �������, ������� �������� ��������� �� ���� ������
    explicit IsolatedThread(std::function<DWORD(IsolatedThread*)> task);
    ~IsolatedThread();

    bool Start();
    void Stop();
    bool IsOurThread() const;
    bool IsRunning() const { return m_running.load(); }

    // ����� ��� �������� ����� ���������
    bool ShouldStop(DWORD timeoutMs = 0) const {
        return WaitForSingleObject(m_hStopEvent, timeoutMs) == WAIT_OBJECT_0;
    }

    // ����� ��� �������� � ��������� ���������
    bool SleepWithStopCheck(DWORD milliseconds) const {
        return WaitForSingleObject(m_hStopEvent, milliseconds) == WAIT_TIMEOUT;
    }

    // ��������� �����������
    IsolatedThread(const IsolatedThread&) = delete;
    IsolatedThread& operator=(const IsolatedThread&) = delete;
};

extern IsolatedThread ThreadPatchAndMods;