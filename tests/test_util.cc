#include "../megalo/megalo.h"
#include <assert.h>


megalo::Logger::ptr g_logger = MEGALO_LOG_ROOT();

void test_assert(){
  MEGALO_LOG_INFO(g_logger) << megalo::BacktraceToString(10);
  MEGALO_ASSERT(false);
}


int main(int argc, char** argv){
  test_assert();
  return 0;
}
