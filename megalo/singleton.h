#ifndef _MEGALO_SINGLETON_H_
#define _MEGALO_SINGLETON_H_

#include <memory>


namespace megalo{


//T 类型
//X 为了创建多个实例的Tag标签
//N 同一个Tag创建的多个实例索引

template<class T,class X = void,int N = 0>
class Singleton{
public:
  static T* GetInstance(){
    static T v;
    return &v;
  }
};

//T 类型
//X 为了创建多个实例的Tag标签
//N 同一个Tag创建的多个实例索引
template<class T,class X = void,int N = 0>
class SingletonPtr{
public:
  static std::shared_ptr<T> GetInstance(){
    static std::shared_ptr<T> v(new T);
    return v;
  }
};
  
}

#endif
