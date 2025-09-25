#include <iostream>

#include "Thread.h"
#include "../dll_proxy/dll_proxy.h"
#include "../Console.h"

IsolatedThread::IsolatedThread(std::function<DWORD(IsolatedThread*)> task)
    : m_task(task)
{
    m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_tlsIndex = TlsAlloc();
}

IsolatedThread::~IsolatedThread() {
    Stop();
    if (m_hStopEvent) {
        CloseHandle(m_hStopEvent);
    }
    if (m_tlsIndex != TLS_OUT_OF_INDEXES) {
        TlsFree(m_tlsIndex);
    }
}

DWORD WINAPI IsolatedThread::ThreadProc(LPVOID lpParam) {
    IsolatedThread* pThis = static_cast<IsolatedThread*>(lpParam);
    return pThis->Run();
}

DWORD IsolatedThread::Run() {
    TlsSetValue(m_tlsIndex, this);
    m_running = true;

    DWORD result = 0;
    if (m_task) {
        result = m_task(this);
    }

    m_running = false;
    return result;
}

bool IsolatedThread::Start() {
    if (m_running) return false;

    ResetEvent(m_hStopEvent);
    m_hThread = CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
    return m_hThread != NULL;
}

void IsolatedThread::Stop() {
    if (!m_running || !m_hThread) return;

    SetEvent(m_hStopEvent);

    // Ждем завершения потока с таймаутом
    if (WaitForSingleObject(m_hThread, 5000) == WAIT_TIMEOUT) {
        // Принудительное завершение, если поток не отвечает
        TerminateThread(m_hThread, 0);
    }

    CloseHandle(m_hThread);
    m_hThread = NULL;
    m_running = false;
}

bool IsolatedThread::IsOurThread() const {
    return TlsGetValue(m_tlsIndex) == (LPVOID)this;
}