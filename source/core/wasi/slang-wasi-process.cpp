// slang-unix-process.cpp
#include "../slang-common.h"
#include "../slang-memory-arena.h"
#include "../slang-process.h"
#include "../slang-string-escape-util.h"
#include "../slang-string-util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// WASI headers
#include <wasi/api.h>
#include <time.h>

namespace Slang
{

// WASI does not support fork/exec or pipes. We'll provide stubs for Process and Stream.

class WasiProcess : public Process
{
public:
    virtual bool isTerminated() SLANG_OVERRIDE { return true; }
    virtual bool waitForTermination(Int) SLANG_OVERRIDE { return true; }
    virtual void terminate(int32_t) SLANG_OVERRIDE {}
    virtual void kill(int32_t) SLANG_OVERRIDE {}
    WasiProcess() { m_returnValue = 0; }
};

class WasiStream : public Stream
{
public:
    WasiStream(int fd, FileAccess access)
        : m_fd(fd), m_access(access)
    {
    }
    virtual Int64 getPosition() SLANG_OVERRIDE { return 0; }
    virtual SlangResult seek(SeekOrigin, Int64) SLANG_OVERRIDE { return SLANG_E_NOT_AVAILABLE; }
    virtual SlangResult read(void* buffer, size_t length, size_t& outReadBytes) SLANG_OVERRIDE
    {
        if (!(m_access == FileAccess::Read))
            return SLANG_E_NOT_AVAILABLE;
        size_t nread = 0;
        __wasi_iovec_t iov = {(uint8_t*)buffer, length};
        __wasi_size_t out_nread = 0;
        __wasi_errno_t res = __wasi_fd_read(m_fd, &iov, 1, &out_nread);
        if (res != __WASI_ERRNO_SUCCESS)
            return SLANG_FAIL;
        outReadBytes = out_nread;
        return SLANG_OK;
    }
    virtual SlangResult write(const void* buffer, size_t length) SLANG_OVERRIDE
    {
        if (!(m_access == FileAccess::Write))
            return SLANG_E_NOT_AVAILABLE;
        __wasi_ciovec_t iov = {(const uint8_t*)buffer, length};
        __wasi_size_t nwritten = 0;
        __wasi_errno_t res = __wasi_fd_write(m_fd, &iov, 1, &nwritten);
        if (res != __WASI_ERRNO_SUCCESS || nwritten != length)
            return SLANG_FAIL;
        return SLANG_OK;
    }
    virtual bool isEnd() SLANG_OVERRIDE { return false; }
    virtual bool canRead() SLANG_OVERRIDE { return m_access == FileAccess::Read; }
    virtual bool canWrite() SLANG_OVERRIDE { return m_access == FileAccess::Write; }
    virtual void close() SLANG_OVERRIDE { /* WASI does not require explicit close for stdio */ }
    virtual SlangResult flush() SLANG_OVERRIDE { return SLANG_OK; }

private:
    int m_fd;
    FileAccess m_access;
};

/* !!!!!!!!!!!!!!!!!!!!!! Process !!!!!!!!!!!!!!!!!!!!!!!!!!!! */

/* static */ UnownedStringSlice Process::getExecutableSuffix()
{
    return UnownedStringSlice::fromLiteral("");
}

/* static */ StringEscapeHandler* Process::getEscapeHandler()
{
    return StringEscapeUtil::getHandler(StringEscapeUtil::Style::Space);
}

/* static */ SlangResult Process::create(
    const CommandLine& commandLine,
    Process::Flags,
    RefPtr<Process>& outProcess)
{
    // WASI does not support process creation. Stub with failure.
    outProcess = nullptr;
    return SLANG_E_NOT_AVAILABLE;
}

/* static */ uint64_t Process::getClockFrequency()
{
    return 1000000000;
}

/* static */ uint64_t Process::getClockTick()
{
    __wasi_timestamp_t now = 0;
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 0, &now);
    return now;
}

/* static */ void Process::sleepCurrentThread(Int timeInMs)
{
    __wasi_subclockflags_t flags = 0;
    __wasi_timestamp_t duration = (__wasi_timestamp_t)timeInMs * 1000000;
    nanosleep(__WASI_CLOCKID_MONOTONIC, flags, duration, nullptr);
}

/* static */ SlangResult Process::getStdStream(StdStreamType type, RefPtr<Stream>& out)
{
    switch (type)
    {
    case StdStreamType::In:
        out = new WasiStream(0, FileAccess::Read);
        break;
    case StdStreamType::Out:
        out = new WasiStream(1, FileAccess::Write);
        break;
    case StdStreamType::ErrorOut:
        out = new WasiStream(2, FileAccess::Write);
        break;
    default:
        return SLANG_FAIL;
    }
    return SLANG_OK;
}

uint32_t Process::getId()
{
    // WASI does not expose process IDs. Return 1 as a stub.
    return 1;
}

} // namespace Slang
