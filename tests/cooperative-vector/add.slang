//TEST(compute):COMPARE_COMPUTE(filecheck-buffer=CHECK):-vk -output-using-type -xslang -skip-spirv-validation
//TEST(compute):COMPARE_COMPUTE(filecheck-buffer=CHECK):-dx12 -use-dxil -output-using-type -profile cs_6_8 -dx12-experimental -Xslang... -Xdxc -Vd -X.
//TEST(compute):COMPARE_COMPUTE(filecheck-buffer=CHECK):-cpu -output-using-type

// CHECK: type: int32_t
// CHECK-NEXT: 1
// CHECK-NEXT: 3
// CHECK-NEXT: 5
// CHECK-NEXT: 7

//TEST_INPUT:ubuffer(data=[0 0 0 0], stride=4):out,name=outputBuffer
RWStructuredBuffer<int32_t> outputBuffer;

//TEST_INPUT:ubuffer(data=[1 2 3 4], stride=4),name=input1
ByteAddressBuffer input1;

//TEST_INPUT:ubuffer(data=[0 1 2 3], stride=4),name=input2
ByteAddressBuffer input2;

[numthreads(1, 1, 1)]
void computeMain()
{
    CoopVec<int, 4> vec1 = coopVecLoad<4, int32_t>(input1);
    CoopVec<int, 4> vec2 = coopVecLoad<4, int32_t>(input2);

    let result = vec1 + vec2;

    for(int i = 0; i < result.getCount(); ++i)
        outputBuffer[i] = result[i];
}
