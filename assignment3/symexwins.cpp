#include <deepstate/DeepState.hpp>
using namespace deepstate;

TEST(Symex, Test) {
        
  unsigned a = DeepState_UIntInRange(1,10000);
  unsigned b = DeepState_UIntInRange(1,10000);
  unsigned c = DeepState_UIntInRange(1,10000);
  unsigned result = DeepState_UIntInRange(10,10000);



  
  LOG(TRACE) << "a: " << a << ", b: " << b; // << ", and (a +  b)^2 = " << sumOfSquares * sumOfSquares;
  
  // ASSERT( a < b )
  ASSERT( (a*a*a + b*b*b + c*c*c) != result*result ) << "(" << a <<"^3 +" << b <<"^3 + " << c <<"^3 = "<< result * result;



}
