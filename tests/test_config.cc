#include "../megalo/config.h"
#include "../megalo/log.h"

#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/node/type.h>
#include <yaml-cpp/yaml.h>

megalo::ConfigVar<int>::ptr g_config_int_value = 
  megalo::Config::Lookup("system.port",(int)8080,"system port");

//megalo::ConfigVar<float>::ptr g_config_xint_value = 
//  megalo::Config::Lookup("system.port",(float)8080,"system port");

megalo::ConfigVar<float>::ptr g_config_float_value = 
  megalo::Config::Lookup("system.value",(float)10.2f,"system value");

megalo::ConfigVar<std::vector<int>>::ptr g_config_int_vec_value = 
  megalo::Config::Lookup("system.int_vec", std::vector<int>{1,2},"system int vec");

megalo::ConfigVar<std::list<int>>::ptr g_config_int_list_value = 
  megalo::Config::Lookup("system.int_list", std::list<int>{1,2},"system int list");

megalo::ConfigVar<std::set<int>>::ptr g_config_int_set_value = 
  megalo::Config::Lookup("system.int_set", std::set<int>{1,2},"system int set");

megalo::ConfigVar<std::unordered_set<int>>::ptr g_config_int_uset_value = 
  megalo::Config::Lookup("system.int_uset", std::unordered_set<int>{1,2},"system int uset");

megalo::ConfigVar<std::map<std::string, int>>::ptr g_config_str_int_map_value = 
  megalo::Config::Lookup("system.str_int_map", std::map<std::string, int>{{"one",1},{"two",2}},"system str_int map");

megalo::ConfigVar<std::unordered_map<std::string, int>>::ptr g_config_str_int_umap_value = 
  megalo::Config::Lookup("system.str_int_umap", std::unordered_map<std::string, int>{{"one",1},{"two",2}},"system str_int umap");

void print_yaml(const YAML::Node& node, int level){
  if(node.IsScalar()){
    MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << std::string(level*4,' ') << node.Scalar() << " - " << node.Type() << " - " << level;
  }else if(node.IsNull()){
    MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << std::string(level*4,' ') << "NULL" << " - " << node.Type() << " - " << level;
  }else if(node.IsMap()){
    for(auto it = node.begin();
        it != node.end(); ++it){
      MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << std::string(level*4,' ') << it->first << " - " << it->second.Type() << " - " << level;
      print_yaml(it->second,level+1);
    }
  }else if(node.IsSequence()){
    for(size_t i = 0;i < node.size();++i){
      MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << std::string(level*4,' ') << i << " - " << node[i].Type() << " - " << level;
      print_yaml(node[i],level+1); 
    }
  }
}


void test_yaml(){
  YAML::Node root = YAML::LoadFile("/home/mars/Project/megalo/bin/conf/test.yml");
  print_yaml(root,0);

  MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << root.Scalar();
}

void test_config(){
  MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) <<"Before:" << g_config_int_value->getValue(); 
  MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) <<"Before:" << g_config_float_value->getValue(); 


#define XX(g_var, name, prefix) \
  { \
    auto& v = g_var->getValue(); \
    for(auto& i : v){ \
      MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << #prefix " " #name ": " << i; \
    } \
    MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString(); \
  } 

#define XX_M(g_var, name, prefix) \
  { \
    auto& v = g_var->getValue(); \
    for(auto& i : v){ \
      MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << #prefix " " #name ": {" \
            << i.first << " - " << i.second << "}"; \
    } \
    MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString(); \
  }

  XX(g_config_int_vec_value, int_vec, before);
  XX(g_config_int_list_value, int_list, before);
  XX(g_config_int_set_value, int_set, before);
  XX(g_config_int_uset_value, int_uset, before);
  XX_M(g_config_str_int_map_value, str_int_map, before);

  YAML::Node root = YAML::LoadFile("/home/mars/Project/megalo/bin/conf/test.yml");
  megalo::Config::LoadFromYaml(root);

  MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) <<"After:" << g_config_int_value->getValue(); 
  MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) <<"After:" << g_config_float_value->getValue(); 

  XX(g_config_int_vec_value, int_vec, after);
  XX(g_config_int_list_value, int_list, after);
  XX(g_config_int_set_value, int_set, after);
  XX(g_config_int_uset_value, int_uset, after);
  XX_M(g_config_str_int_map_value, str_int_map, after);
       
}


class Person{
public:
  Person() {}
  std::string toString() const {
    std::stringstream ss;
    ss << "[Person name=" << m_name 
       << " age=" << m_age 
       << " sex=" << m_sex 
       << "]";
    return ss.str();
  }

  bool operator==(const Person& other) const{
    return m_name == other.m_name
        && m_age == other.m_age
        && m_sex == other.m_sex;
  }
  
  std::string m_name;
  int m_age = 0;
  bool m_sex = 0;
};

namespace megalo{

template<>
class LexicalCast<std::string, Person>{
public:
  Person operator()(const std::string& v){
    YAML::Node node = YAML::Load(v);
    Person p;
    p.m_name = node["name"].as<std::string>();
    p.m_age = node["age"].as<int>();
    p.m_sex = node["sex"].as<bool>();
    return p;
  }
};


template<>
class LexicalCast<Person, std::string>{
public:
  std::string operator()(const Person& v){
    YAML::Node node(YAML::NodeType::Map);
    node["name"] = v.m_name;
    node["age"] = v.m_age;
    node["sex"] = v.m_sex;
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};

}

megalo::ConfigVar<Person>::ptr g_person = 
  megalo::Config::Lookup("class.person", Person(), "system person");

megalo::ConfigVar<std::map<std::string, Person> >::ptr g_person_map = 
  megalo::Config::Lookup("class.map", std::map<std::string, Person>{}, "system person map");

void test_class(){

  g_person->addListener(10,[](const Person& old_value, const Person& new_value){
        MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << "old_value=" << old_value.toString()
        << " new_value=" << new_value.toString();
      });

#define XX_MP(g_var, prefix) \
  { \
    auto& v = g_var->getValue(); \
    for(auto& i : v){ \
      MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << #prefix ": " << i.first << " - " << i.second.toString(); \
    } \
    MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << #prefix ": size=" << v.size(); \
  }

  MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << "before: " << g_person->getValue().toString() << " - " << g_person->toString();
  XX_MP(g_person_map, class.map before);

  YAML::Node root = YAML::LoadFile("/home/mars/Project/megalo/bin/conf/test.yml");
  megalo::Config::LoadFromYaml(root);

  MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << "after: " << g_person->getValue().toString() << " - " << g_person->toString();
  XX_MP(g_person_map, class.map after);
}
int main(){
  //test_yaml();
  //test_config();
  test_class();
  return 0;
}
