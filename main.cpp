#include <iostream>
#include <string>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>

using namespace std;

class Buffer final
{
private:
  static constexpr size_t FILESIZE = 1024 * 1024;
  char* mAddress = nullptr;

  void CloseNoThrow() noexcept
  {
    try
    {
      Close();
    }
    catch (...)
    {
    }
  }

public:
  Buffer()
  {
    int fd = open("/dev/shm/fastlog", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR );
    if (fd == -1)
    {
        auto errorText = "Failed to open log file for writing: " + std::string(strerror(errno));
        throw runtime_error(errorText);
    }

    if ((mAddress = (char*)mmap(nullptr, FILESIZE, PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        auto errorText = "Failed to memory map log file: " + std::string(strerror(errno));
        throw runtime_error(errorText);
    }

    if (ftruncate(fd, FILESIZE) == -1)
    {
        auto errorText = "Failed to truncate log file: " + std::string(strerror(errno));
        throw runtime_error(errorText);
    }

    if (close(fd) == -1)
    {
        auto errorText = "Failed to close log file: " + std::string(strerror(errno));
        throw runtime_error(errorText);
    }

    // rely on all unwritten bytes being zero
    bzero(mAddress, FILESIZE);
  }

  Buffer(const Buffer&) = delete; // can't copy a buffer (only one instance owns the mapped file)

  Buffer(Buffer&& buffer) noexcept // can move a buffer
  {
    buffer.mAddress = mAddress;
    mAddress = nullptr;
  }

  ~Buffer() noexcept
  {
    CloseNoThrow();
    mAddress = nullptr;
  }

  Buffer& operator= (const Buffer&) = delete; // can't assign from another buffer

  Buffer& operator= (Buffer&& buffer) noexcept
  {
    if (this != &buffer)
    {
      CloseNoThrow();
      mAddress = buffer.mAddress;
      buffer.mAddress = nullptr;
    }
    return *this;
  }

  Buffer& operator<< (const std::string& string) noexcept
  {
    if (mAddress != nullptr)
    {
      memcpy(mAddress, string.data(), string.size());
      mAddress += string.size() + 1;
    }
    return *this;
  }

  template <typename T>
  Buffer& operator<< (const T& number) noexcept
  {
    if (mAddress != nullptr)
    {
      memcpy(mAddress, &number, sizeof(T));
      mAddress += sizeof(T);
    }
    return *this;
  }

  template <typename T>
  void Serialise(const T& message)
  {
    // TODO lock
    *this << message.GetMessageType();
    message.Serialise(*this);
    // TODO unlock
  }

  void Close()
  {
    if (mAddress != nullptr)
    {
      if (munmap(mAddress, FILESIZE) == -1)
      {
          auto errorText = "Failed to unmap log file: " + std::string(strerror(errno));
          throw runtime_error(errorText);
      }
    }
  }

};

class Message
{
public:
  std::string mTraderName;
  int mTraderIndex;
  float mFooFactor;

  int GetMessageType() const
  {
    return 1;
  }

  void Serialise(Buffer& buffer) const
  {
    buffer << mTraderName;
    buffer << mTraderIndex;
    buffer << mFooFactor;
  }
};

int main()
{
  Buffer buffer;

  // TODO: use buffer from two different threads

  Message message;
  message.mTraderName = "CarlCook";
  message.mTraderIndex = 1;
  message.mFooFactor = 12.57;

  buffer.Serialise(message);


  // write sample program to read (to verify)

  // journalling?
}

