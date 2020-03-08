#include <stddef.h>
#include <stdint.h>

#include "cfunction.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Size >= 255) {
    validate_hostname( (char *)Data, (int)Size) ;
  }
  return 0;
}
