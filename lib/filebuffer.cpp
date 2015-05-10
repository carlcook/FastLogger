#include "filebuffer.h"
#include "utils.h"

#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <atomic>
#include <sstream>
#include <time.h>

/// The Pimpl implementation for a file buffer
struct FileBuffer::Impl final
{
  // one megabyte for now
  static constexpr size_t FILESIZE = 1024 * 1024;

  // counter for each unique file buffer object within the process
  static std::atomic<int> mThreadId;

  // the address of the memory mapped file
  char* mAddress = nullptr;

  Impl(const Impl&) = delete;
  Impl() = default;
  Impl(Impl&&) noexcept = default;
  Impl& operator=(const Impl&) = delete;
  Impl& operator=(Impl&&) noexcept = default;
};

// static initialisation
std::atomic<int> FileBuffer::Impl::mThreadId{0};

FileBuffer::FileBuffer()
  : mImpl(utilities::make_unique<Impl>())
{
  std::ostringstream filename;
  filename << "/tmp/fastlogger-" << getpid() << "-" << mImpl->mThreadId++;
  int fd = open(filename.str().c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR );
  if (fd == -1)
  {
    auto errorText = "Failed to open log file for writing: " + std::string(strerror(errno));
    throw std::runtime_error(errorText);
  }
  if ((mImpl->mAddress = (char*)mmap(nullptr, mImpl->FILESIZE, PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
  {
    auto errorText = "Failed to memory map log file: " + std::string(strerror(errno));
    throw std::runtime_error(errorText);
  }
  if (ftruncate(fd, mImpl->FILESIZE) == -1)
  {
    auto errorText = "Failed to truncate log file: " + std::string(strerror(errno));
    throw std::runtime_error(errorText);
  }
  if (close(fd) == -1)
  {
    auto errorText = "Failed to close log file: " + std::string(strerror(errno));
    throw std::runtime_error(errorText);
  }
  bzero(mImpl->mAddress, mImpl->FILESIZE); // rely on all unwritten bytes being zero
}

FileBuffer::FileBuffer(FileBuffer&& buffer) noexcept
{
  mImpl = std::move(buffer.mImpl);
}

FileBuffer::~FileBuffer() noexcept
{
  CloseNoThrow();
}

FileBuffer& FileBuffer::operator= (FileBuffer&& buffer) noexcept
{
  if (this != &buffer)
  {
    CloseNoThrow(); // close previously mapped file (if exists)
    mImpl = std::move(buffer.mImpl);
  }
  return *this;
}

char*& FileBuffer::GetInternalBuffer()
{
  return mImpl->mAddress;
}

void FileBuffer::WriteTimestamp()
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC_COARSE, &now);
  memcpy(mImpl->mAddress, &now, sizeof(now));
  mImpl->mAddress += sizeof(now);
}

void FileBuffer::Close()
{
  if (mImpl)
  {
    if (munmap(mImpl->mAddress, Impl::FILESIZE) == -1)
    {
      auto errorText = "Failed to unmap log file: " + std::string(strerror(errno));
      throw std::runtime_error(errorText);
    }
  }
}

void FileBuffer::CloseNoThrow() noexcept
{
  try
  {
    Close();
  }
  catch (...)
  {
  }
}



