#include "filebuffer.h"

#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <atomic>
#include <sstream>

#include "utils.h"

struct FileBuffer::Impl final
{
  // one megabyte for now
  static constexpr size_t FILESIZE = 1024 * 1024;

  // counter for each unique file buffer object within the process
  static std::atomic<int> mThreadId;

  // the address of the memory mapped file
  char* mAddress = nullptr;
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

// TODO alignment?
FileBuffer& FileBuffer::operator<< (const std::string& string) noexcept
{
  if (mImpl)
  {
    memcpy(mImpl->mAddress, string.data(), string.size());
    mImpl->mAddress += string.size() + 1;
  }
  return *this;
}

template <typename T>
FileBuffer& FileBuffer::operator<< (const T& number) noexcept
{
  if (mImpl)
  {
    memcpy(mImpl->mAddress, &number, sizeof(T));
    mImpl->mAddress += sizeof(T);
  }
  return *this;
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

// supported serialisation types (plus std::string)
template FileBuffer& FileBuffer::operator<<(bool const&);
template FileBuffer& FileBuffer::operator<<(int const&);
template FileBuffer& FileBuffer::operator<<(unsigned int const&);
template FileBuffer& FileBuffer::operator<<(float const&);
template FileBuffer& FileBuffer::operator<<(double const&);
