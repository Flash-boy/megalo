#ifndef __MEGALO_NONCOPYABLE_H_
#define __MEGALO_NONCOPYABLE_H_


namespace megalo{
class Noncopyable{
public:
  Noncopyable() = default;
  ~Noncopyable() = default;
  Noncopyable(const Noncopyable&) = delete;
  Noncopyable& operator=(const Noncopyable&) = delete;
};
}
#endif
