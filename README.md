# AsioFileManager
Parsing and loading any format file support by Genuis Project
Current file format support
- MNN format
- **TODO**: Add more format file

## Design
<img src="https://docs.google.com/drawings/d/e/2PACX-1vRwOrh66iXLfMRpXgLl7MGrqu8EQ2qPcEYGS3PPqbcGmxkgyoE3STljXSylcsrCEFZzgomTqRsskcui/pub?w=960&h=720">

## Depend on the library

- MNN: [https://github.com/alibaba/MNN](https://github.com/alibaba/MNN)
- Google Test: [https://github.com/google/googletest](https://github.com/google/googletest)

## Build with Linux
```sh
git clone https://github.com/GeniusVentures/AsyncIOManager
cd AsyncIOManager
git checkout dev_ipfs_refactoring
mkdir .build
cd .build
# <MNN_repository_root_directory> variable should point to the root directory of the MNN repository as its name implies
cmake -DMNN_ROOT_DIR=<MNN_repository_root_directory> ..
cmake --build .
```

## Build with Windows
TODO: Need to update

## Build with Mac OS
TODO: Need to update

## Support GTest


## Run the example
```bash
<TODO> Update this part

```
