# 2023年电赛H题
## PL
平台：vivado 2023.1  
主控：Zynq7007
### 建立工程
打开终端，切到PL文件夹，运行。
~~~
    vivado -mode batch -source my_h.tcl
~~~
**请保证vivado在环境变量里**

## PS
系统：rt_thread  
数学库：NE10  
将PS/src/里的文件添入你的vitis工程里，按下述更改编译指令。
### ASM
~~~
-mthumb -march=armv7-a -mfpu=neon -mfloat-abi=hard -marm
~~~

### C/C++ compiler
~~~
-c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=neon-fp16 -mfloat-abi=hard -ftree-vectorize -ffast-math
~~~

### C/C++ linker
~~~
-mcpu=cortex-a9 -mfpu=neon-fp16 -mfloat-abi=hard -ffast-math -Wl,-build-id=none -specs=Xilinx.spec -Wl,-Map,test.map
~~~
**工程编译器用g++，然后把src文件夹的编译器改为gcc,之后把ne10内的cpp文件的编译器改为g++。rt_thread的编译优化必须为O0**

最后使用PS/asm_vector.s 替换掉工程硬件平台的asm_vector.s 一般情况下在/硬件平台路径/ps7_cortexa9_0/standalone_domain/bsp/ps7_cortexa9_0/libsrc/standalone_v8_1/src  
fsbl的不要替换