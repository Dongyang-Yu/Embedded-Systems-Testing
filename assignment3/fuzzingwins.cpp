#include <deepstate/DeepState.hpp>
using namespace deepstate;
#include <assert.h>

#ifndef TEST_ORIGINAL_LZ4
#include "lz4.h"
#endif

#define MAX_SIZE 64

TEST(Lz4, RoundTrip) {

  // create a sources (char *) of MAX_SIZE bytes text.
  const char *srcBuffer = DeepState_CStrUpToLen(MAX_SIZE, NULL);
  
  // get the srcBuffer size
  const size_t src_size = strlen(srcBuffer) + 1;

  // set buffers boundary
  const size_t compressedCap = LZ4_compressBound(src_size);
  const size_t decompressedCap = LZ4_compressBound(src_size);
  // build allocations for the data
  char *compressedBuffer = (char *)DeepState_Malloc(compressedCap);
  char *decompressedBuffer = (char *)DeepState_Malloc(decompressedCap);

  int cSize = LZ4_compress_default(srcBuffer, compressedBuffer, src_size, compressedCap);
  
  int dSize = LZ4_decompress_safe(compressedBuffer, decompressedBuffer, cSize, decompressedCap);

  LOG(TRACE) << "srcBuffer: " << srcBuffer << "; src_size: " << src_size;
  LOG(TRACE) << "number of bytes written to decompressedBuffer: " << dSize;

  ASSERT_EQ(src_size, dSize) << "ORIGINAL: '" << src_size << "', COMPRESSED: '" << cSize << "', ROUNDTRIPL '" << dSize << "'";
  
  // Free allocated memory
  free(compressedBuffer);
  free(decompressedBuffer);

}
