# BUILD INSTRUCTIONS
```bash
mkdir build
cd build
cmake ..
make
./minic <filename>
```

# IMPORTANT NOTE
There is a lot of debug output generated during the processing of the file. Turn of debugging by **commenting**
```cmake
target_compile_definitions(minic PUBLIC DEBUG) #comment me to turn OFF debugging
```
in the root CMakeLists.txt file.