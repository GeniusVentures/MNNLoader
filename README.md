# MNNLoader
Parsing and loading any format file support by MNN library </br>
This library built-in for support IPFS system loading/parsing any MNN model file. </br>
The first design is just support a MNN format. </br>
**TODO**: We should thinking about general formal we are support (other format). Then we can re-name the project to FileLoader (Suggest) </br>

## Design


## Depend Library

- MNN: [https://github.com/alibaba/MNN](https://github.com/alibaba/MNN)
- Google Test: [https://github.com/google/googletest](https://github.com/google/googletest)

## Build with Linux
```sh
$ cd build
$ cmake ..
-- C flags: 
-- CXX flags: 
-- C Debug flags: -g
-- CXX Debug flags: -g
-- C Release flags: -O3 -DNDEBUG
-- CXX Release flags: -O3 -DNDEBUG
-- Installing MNNLOADER Headers ...
-- Installing: /home/tannguyen/workspace/GNUS/thirdparty/MNNLoader/.build/MNNLoader/include/MNNCommon.hpp
-- Installing: /home/tannguyen/workspace/GNUS/thirdparty/MNNLoader/.build/MNNLoader/include/MNNLoader.hpp
-- >>> Added Library: MNNLoader!
-- Configuring done
-- Generating done
-- Build files have been written to: /home/tannguyen/workspace/GNUS/thirdparty/MNNLoader/.build

$ make
Consolidate compiler generated dependencies of target mnnloader
[ 66%] Built target MNN
[ 75%] Linking CXX static library MNNLoader/lib/libmnnloader.a
[ 83%] Built target mnnloader
Consolidate compiler generated dependencies of target MNNExample
[ 91%] Linking CXX executable MNNExample
[100%] Built target MNNExample
```

## Build with Windows
TODO: Need to update

## Build with Mac OS
TODO: Need to update

## Support GTest


## Run the example
```bash
$ ./MNNExample ~/workspace/GNUS/thirdparty/MNNLoader/test/example.mnn 
The device support i8sdot:0, support fp16:0, support i8mm: 0
        **Tensor shape**: 3, 320, 320, 
Error for compute convolution shape, inputCount:3, outputCount:24, KH:3, KW:3, group:1
inputChannel: 320, batch:3, width:320, height:320. Input data channel may be mismatch with filter channel count
Compute Shape Error for 967
        **Tensor shape**: 3, 320, 320, 
        **Tensor shape**: 1, 400, 80, 
        **Tensor shape**: 1, 100, 80, 
        **Tensor shape**: 1, 1600, 80, 
        **Tensor shape**: 1, 400, 4, 
        **Tensor shape**: 1, 100, 4, 
        **Tensor shape**: 1, 1600, 4, 
Log ID: /home/tannguyen/workspace/GNUS/thirdparty/MNNLoader/test/example.mnn
==================INPUT-DIMS================
Dimension Type: (CAFE/PyTorch/ONNX) uses NCHW as data format
==================OUTPUT-DIMS================
Output : cls_pred_stride_16
Output : cls_pred_stride_32
Output : cls_pred_stride_8
Output : dis_pred_stride_16
Output : dis_pred_stride_32
Output : dis_pred_stride_8
=============================================

```