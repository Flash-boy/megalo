#include "../megalo/config.h"
#include "../megalo/log.h"

megalo::ConfigVar<int>::ptr g_config_int_value = 
  megalo::Config::Lookup<int>("system.port",(int)8080,"system port");

int main(){
  MEGALO_LOG_INFO(MEGALO_LOG_ROOT()) << g_config_int_value->getValue(); 
  return 0;
}
