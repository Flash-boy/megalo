#ifndef _MEGALO_CONFIG_H_
#define _MEGALO_CONFIG_H_

#include <cctype>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <utility>
#include <functional>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/node/type.h>
#include <yaml-cpp/yaml.h>

#include "log.h"
#include "thread.h"


namespace megalo{

// 配置值基类
class ConfigVarBase{
public:
  typedef std::shared_ptr<ConfigVarBase> ptr;
  ConfigVarBase(const std::string& name,const std::string& description = "")
    :m_name(name)
    ,m_description(description){
      std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
  }
  virtual ~ConfigVarBase(){}

  const std::string& getName() const { return m_name;}
  const std::string& getDescription() const { return m_description;}

  virtual std::string toString() = 0;
  virtual bool fromString(const std::string& val) = 0;
  virtual std::string getTypeName() const = 0;

private:
  std::string m_name;
  std::string m_description;
};


//自定义类型转化
template<class F, class T>
class LexicalCast{
public:
  T operator()(const F& v){
    return boost::lexical_cast<T>(v);
  }
};

// 类型模板转化偏特化（YAML string 转化为 vector<T>）
template<class T>
class LexicalCast<std::string, std::vector<T> >{
public:
  std::vector<T> operator()(const std::string& v){
    YAML::Node node = YAML::Load(v);
    typename std::vector<T> vec;
    std::stringstream ss;
    for(size_t i = 0; i < node.size(); ++i){
      ss.str("");
      ss << node[i];
      vec.push_back(LexicalCast<std::string,T>()(ss.str()));
    }
    return vec;
  }
};

// 类型模板偏特化（vector<T> 转化为 YAML string）
template<class T>
class LexicalCast<std::vector<T>, std::string>{
public:
  std::string operator()(const std::vector<T>& v){
    YAML::Node node(YAML::NodeType::Sequence);
    for(auto& i : v){
      node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};


// 类型模板转化偏特化（YAML string 转化为 list<T>）
template<class T>
class LexicalCast<std::string, std::list<T> >{
public:
  std::list<T> operator()(const std::string& v){
    YAML::Node node = YAML::Load(v);
    typename std::list<T> vec;
    std::stringstream ss;
    for(size_t i = 0; i < node.size(); ++i){
      ss.str("");
      ss << node[i];
      vec.push_back(LexicalCast<std::string,T>()(ss.str()));
    }
    return vec;
  }
};


// 类型模板偏特化（list<T> 转化为 YAML string）
template<class T>
class LexicalCast<std::list<T>, std::string>{
public:
  std::string operator()(const std::list<T>& v){
    YAML::Node node(YAML::NodeType::Sequence);
    for(auto& i : v){
      node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};


// 类型模板转化偏特化（YAML string 转化为 set<T>）
template<class T>
class LexicalCast<std::string, std::set<T> >{
public:
  std::set<T> operator()(const std::string& v){
    YAML::Node node = YAML::Load(v);
    typename std::set<T> vec;
    std::stringstream ss;
    for(size_t i = 0; i < node.size(); ++i){
      ss.str("");
      ss << node[i];
      vec.insert(LexicalCast<std::string,T>()(ss.str()));
    }
    return vec;
  }
};

// 类型模板偏特化（set<T> 转化为 YAML string）
template<class T>
class LexicalCast<std::set<T>, std::string>{
public:
  std::string operator()(const std::set<T>& v){
    YAML::Node node(YAML::NodeType::Sequence);
    for(auto& i : v){
      node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};



// 类型模板转化偏特化（YAML string 转化为 unordered_set<T>）
template<class T>
class LexicalCast<std::string, std::unordered_set<T> >{
public:
  std::unordered_set<T> operator()(const std::string& v){
    YAML::Node node = YAML::Load(v);
    typename std::unordered_set<T> vec;
    std::stringstream ss;
    for(size_t i = 0; i < node.size(); ++i){
      ss.str("");
      ss << node[i];
      vec.insert(LexicalCast<std::string,T>()(ss.str()));
    }
    return vec;
  }
};

// 类型模板偏特化（unordered_set<T> 转化为 YAML string）
template<class T>
class LexicalCast<std::unordered_set<T>, std::string>{
public:
  std::string operator()(const std::unordered_set<T>& v){
    YAML::Node node(YAML::NodeType::Sequence);
    for(auto& i : v){
      node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};



// 类型模板转化偏特化（YAML string 转化为 map<std::string, T>）
template<class T>
class LexicalCast<std::string, std::map<std::string, T> >{
public:
  std::map<std::string, T> operator()(const std::string& v){
    YAML::Node node = YAML::Load(v);
    typename std::map<std::string, T> vec;
    std::stringstream ss;
    for(auto it = node.begin();
        it != node.end(); ++it){
      ss.str("");
      ss << it->second;
      vec.insert(std::make_pair(it->first.Scalar(),
                LexicalCast<std::string,T>()(ss.str())));
    }
    return vec;
  }
};

// 类型模板偏特化（map<std::string,T> 转化为 YAML string）
template<class T>
class LexicalCast<std::map<std::string, T>, std::string>{
public:
  std::string operator()(const std::map<std::string, T>& v){
    YAML::Node node(YAML::NodeType::Map);
    for(auto& i : v){
      node[i.first] = YAML::Load(LexicalCast<T,std::string>()(i.second));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};



// 类型模板转化偏特化（YAML string 转化为 unordered_map<std::string, T>）
template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T> >{
public:
  std::unordered_map<std::string, T> operator()(const std::string& v){
    YAML::Node node = YAML::Load(v);
    typename std::unordered_map<std::string, T> vec;
    std::stringstream ss;
    for(auto it = node.begin();
        it != node.end(); ++it){
      ss.str("");
      ss << it->second;
      vec.insert(std::make_pair(it->first.Scalar(),
                LexicalCast<std::string,T>()(ss.str())));
    }
    return vec;
  }
};

// 类型模板偏特化（unordered_map<std::string,T> 转化为 YAML string）
template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string>{
public:
  std::string operator()(const std::unordered_map<std::string, T>& v){
    YAML::Node node(YAML::NodeType::Map);
    for(auto& i : v){
      node[i.first] = YAML::Load(LexicalCast<T,std::string>()(i.second));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};


// T 值类型
// FromStr 从字符串转换为T类型值的函数对象
// ToStr 从T类型对象转换为字符串的函数对象
template <class T, class FromStr = LexicalCast<std::string, T>
                  ,class ToStr = LexicalCast<T,std::string> >
class ConfigVar: public ConfigVarBase{
public:
  typedef std::shared_ptr<ConfigVar<T> > ptr;
  typedef std::function<void (const T& old_value, const T& new_value)> on_change_cb;
  typedef RWMutex MutexType;

  ConfigVar(const std::string& name
           ,const T& default_val
           ,const std::string& description = "")
    :ConfigVarBase(name,description)
    ,m_val(default_val){
  }

  std::string toString() override {
    try {
      MutexType::ReadLock ll(m_mutex);
      return ToStr()(m_val);
    } catch(boost::bad_lexical_cast& e){
      MEGALO_LOG_ERROR(MEGALO_LOG_ROOT()) << "ConfigVar::toString exception"
        <<  e.what() << " convert: " << typeid(m_val).name() << " to string";
    }
    return "";
  }
  bool fromString(const std::string& val) override {
    try {
      setValue(FromStr()(val));
    } catch(boost::bad_lexical_cast& e){
      MEGALO_LOG_ERROR(MEGALO_LOG_ROOT()) << "ConfigVar::fromString exception"
        <<  e.what() << " convert: string to " << typeid(val).name();
      return false;
    }
    return true;
  }
  const T getValue() {
    MutexType::ReadLock ll(m_mutex);
    return m_val;
  }

  void setValue(const T& v){
    {
      MutexType::ReadLock ll(m_mutex);
      if(m_val == v){
        return;
      }else{
        for(auto& i : m_cbs){
          i.second(m_val, v);
        }
      }
    }

    MutexType::WriteLock ll(m_mutex);
    m_val = v;
  } 

  std::string getTypeName() const override {return typeid(m_val).name();}

  uint64_t addListener(on_change_cb cb) {
    static uint64_t s_fun_id = 0;
    MutexType::WriteLock ll(m_mutex);
    ++s_fun_id;
    m_cbs[s_fun_id] = cb;
    return s_fun_id;
  }

  void delListener(uint64_t key) {
    MutexType::WriteLock ll(m_mutex);
    m_cbs.erase(key);
  }

  on_change_cb getListener(uint64_t key) {
    MutexType::ReadLock ll(m_mutex);
    auto it = m_cbs.find(key);
    return it == m_cbs.end() ? nullptr : it->second;
  }
  void clearListener() {
    MutexType::WriteLock ll(m_mutex);
    m_cbs.clear();
  }

private:
  T m_val;
  //配置变更回调函数 
  std::map<uint64_t, on_change_cb> m_cbs;
  MutexType m_mutex;

};

class Config{
public:
  typedef std::unordered_map<std::string,ConfigVarBase::ptr> ConfigVarMap;
  typedef RWMutex MutexType;
  template<class T>
  static typename ConfigVar<T>::ptr Lookup(const std::string& name, 
        const T& default_val, const std::string& description = ""){
    MutexType::WriteLock ll(GetMutex());
    auto it = GetDatas().find(name);
    if(it != GetDatas().end()){
      auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
      if(tmp){
        MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << "Lookup name=" << name << " exists";
        return tmp;
      }else{
        MEGALO_LOG_ERROR(MEGALO_LOG_ROOT()) << "Lookup name=" << name << " exists but type not " 
                << typeid(T).name() << " real_type=" << it->second->getTypeName();
      }
    }
    if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz_.0123456789")
        != std::string::npos){
      MEGALO_LOG_ERROR(MEGALO_LOG_ROOT()) << "Lookup name invalid " << name;
      throw std::invalid_argument(name);
    }

    typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_val,description));
    GetDatas()[name] = v;
    return v;
  }

  template<class T>
  static typename ConfigVar<T>::ptr Lookup(const std::string& name){
    MutexType::ReadLock ll(GetMutex());
    auto it = GetDatas().find(name);
    if(it == GetDatas().end()){
      return nullptr;
    }
    return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
  }


  static ConfigVarBase::ptr LookupBase(const std::string& name);

  static void LoadFromYaml(const YAML::Node& root);
  static void Visit(std::function<void(ConfigVarBase::ptr)> cb);

private:
  static ConfigVarMap& GetDatas(){
    static ConfigVarMap s_datas;
    return s_datas;
  }
  static MutexType& GetMutex(){
    static MutexType s_mutex;
    return s_mutex;
  }

};

}

#endif
