#ifndef UTILS_H
#define UTILS_H

#include <memory>

namespace utilities
{

template <typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&&... params)
{
  return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

}
#endif // UTILS_H
