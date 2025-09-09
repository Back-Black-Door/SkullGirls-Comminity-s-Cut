#pragma once
#pragma once
#include <Windows.h>

class CriticalSection
{
public:
    CriticalSection()
    {
        InitializeCriticalSection(&m_cs);
    }

    ~CriticalSection()
    {
        DeleteCriticalSection(&m_cs);
    }

    void Lock()
    {
        EnterCriticalSection(&m_cs);
    }

    void Unlock()
    {
        LeaveCriticalSection(&m_cs);
    }

    // «апрещаем копирование и присваивание
    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;

private:
    CRITICAL_SECTION m_cs;
};

class ScopedLock
{
public:
    explicit ScopedLock(CriticalSection& cs) : m_cs(cs)
    {
        m_cs.Lock();
    }

    ~ScopedLock()
    {
        m_cs.Unlock();
    }

    // «апрещаем копирование и присваивание
    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;

private:
    CriticalSection& m_cs;
};