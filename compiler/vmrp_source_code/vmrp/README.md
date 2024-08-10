<a href="https://996.icu"><img src="https://img.shields.io/badge/link-996.icu-red.svg" alt="996.icu" /></a>

# PC版本下载地址

https://github.com/vmrp/vmrp/releases/download/1.0.0/vmrp_win32_20220102.zip

此版本已经实现完整的联网功能，可以运行支持网络通信的mrp了

# web网页版体验地址

https://vmrp.github.io/

https://vmrp.github.io/vmrp_v1.0/main.html

## 捐赠硬件

模拟器的开发需要在真实的手机上测试，如果您有支持mrp的手机愿意捐赠出来用作研究，欢迎联系我

# 实现原理

由于mrpoid模拟器受限于安卓系统，于是决定开发一款真正的模拟器

mrpoid是安卓上的mrp模拟器，c语言开发的mrp是编译后的arm架构机器码，因此在arm芯片上直接加载运行就可以，安卓手机大多都是采用arm架构的cpu，mrpoid就是加载mrp代码到内存中，修改mrp内部的函数表然后运行，因此mrpoid无法在其它架构的设备上运行。

vmrp实现原理与mrpoid基本相同，参考了mrpoid早期的实现原理，不同的地方是vmrp借助unicorn engine实现真正的模拟器，不再依赖arm架构cpu。

以下是工作流程图，经过改进后自身实现的mythroad只用于加载ext，因此多余的部分目前已经被删除，自身实现的mythroad层已经没有运行简单mrp的能力

![工作流程](/doc/images/2.0.jpg)

## 自身实现的mythroad层：

最早的模拟器实现的功能非常有限，于是将整个mythroad层交给arm代码去实现，尝试了ELF加载器的方式加载mythroad层发现有潜在的bug（gcc编译时在mythroad层主要问题是elfloader没有实现对GOT的处理，在gcc编译mrp的功能上主要是r9和r10寄存器的问题，因此放弃ELF加载器，仍然采用ext加载方式。

完整版模拟器将借助mythroad层代码实现，代码在vmrp_arm项目中(vmrp_arm已经停止开发，代码已经复制到本项目的mythroad文件夹中)。

# R9寄存器导致的BUG

因为ext中的mr_c_function_load()函数是第一个函数，在mythroad层调用此函数其实相当于仍然在mythroad层调用mythroad层的东西，它会回调_mr_c_function_new()将mr_extHelper()或mr_helper()函数的地址传回mythroad，所有的事件传递都是通过这个helper函数，helper函数进去的第一件事就是备份r9寄存到r10，然后设置r9寄存器的值，在ext内的所有全局变量的读写都是基于这个寄存器提供的基地址，而在ext内调用mythroad层的函数时，r9和r10寄存器的值并没有恢复，这可能导致严重的问题，这可能就是安卓上mrpoid运行不稳定的原因，从反编译的结果来看，插件化mrp内的ext之间是有恢复r9寄存器的功能，但是没有恢复r10寄存器的功能，在目前能获得的mythroad层代码中没有看到任何恢复r9和r10的操作。

注意，如果想采用elf加载器来实现gcc编译mrp仍然需要解决r9和r10寄存器的问题，因为斯凯使用armcc编译的elf与gcc编译的elf是不同的，虽然都是静态PIE，但是gcc编译的结果仍然保留了GOT表

# 编译方法

目前使用到的工具和支持库：

https://nchc.dl.sourceforge.net/project/mingw-w64/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/i686-8.1.0-release-posix-sjlj-rt_v6-rev0.7z

https://github.com/aquynh/capstone/releases/download/4.0.1/capstone-4.0.1-win32.zip  （只有编译带DEBUG功能时才需要）

https://github.com/unicorn-engine/unicorn/releases/download/1.0.2/unicorn-1.0.2-win32.zip

https://www.libsdl.org/release/SDL2-devel-2.0.10-mingw.tar.gz

可能需要安装zlib，我是直接从官网下载源码安装的

将capstone、SDL2、unicorn解压到./windows文件夹内，在windows下用mingw32-make.exe编译，我的是i686-8.1.0-release-posix-sjlj-rt_v6-rev0版本)
```
$ ls ./windows/ -l
drwxr-xr-x 1 zengming 197121       0  2月 29  2020 capstone-4.0.1-win32
drwxr-xr-x 1 zengming 197121       0  2月 11  2020 SDL2-2.0.10
drwxr-xr-x 1 zengming 197121       0  2月 11  2020 unicorn-1.0.1-win32
```

SDL2在linux可以通过下面的命令安装：
```
sudo apt install libsdl2-dev
```

直接`mingw32-make`即可编译，使用`mingw32-make DEBUG=1`可以编译出带调试功能的版本

另外，需要用斯凯SDK单独编译 /mythroad/build/build_full.bat 生成vmrp.mrp，提取里面的cfunction.ext，这么做的原因是mythroad层代码量非常大，arm编译和gcc编译有一点差别，例如char类型的变量值传递给uint16类型时在arm编译时可能char也是被当成uint8来处理的，而在gcc中这种情况char是有符号的，会导致bug，因此还不敢将全部代码整合到一起，目前正在mythroad分支中尝试合并

编译vmrp.mrp可能会遇到`"<command line>": Error: A1023E: File "..\asm\r9r10.s" could not be opened`，这是因为r9r10.s需要用代码生成，在./mythroad/asm/文件夹下通过`node genR9R10.js 0 1`生成

编译成功后还需要补充bin文件夹里面的文件才能运行：

1. mythroad文件夹: 与真实手机上的文件相同，主要是一些基本的mrp和字体等文件，在./wasm/dist/fs文件夹中能获得
2. cfunction.ext: 从vmrp.mrp内提取，每次编译mythroad层后会生成vmrp.mrp，利用工具提取出来，在./wasm/dist/fs文件夹中能获得一个编译好的文件
3. capstone.dll: 反编译引擎，只有用`make DEBUG=1`编译的main.exe才会依赖这个文件，在`./windows/capstone-4.0.1-win32/capstone.dll`
4. SDL2.dll: 用于图形界面，在`./windows/SDL2-2.0.10/i686-w64-mingw32/bin/SDL2.dll`
5. unicorn.dll: 用于arm指令的执行，在`./windows/unicorn-1.0.2-win32/unicorn.dll`

在改代码的时候严格注意将`./mythroad/`文件夹中的代码看成是另一个项目，不要与其它地方的源码混在一起，这么做的原因是`./mythroad/`是直接复制的vmrp_arm项目，考虑到将来移植到嵌入式系统中的便利性因此要求将`./mythroad/`完全独立开发。

# 参考资料

mrp编辑器:  [Mrpeditor.exe](tool/Mrpeditor.exe)

十六进制方式查看文件:
```shell
hd mythroad/arm.mrp -n 100
```

https://github.com/Yichou/mrpoid2018

https://github.com/alphaSeclab/awesome-reverse-engineering

https://github.com/nationalsecurityagency/ghidra

反汇编: 
```
arm-linux-gnueabi-objdump -b binary --start-address=0x8 -m arm -D game.ext
# 或者用radare2
r2 -a arm -b 32 -s 8 game.ext
```
（推荐）这是我自己写的反汇编工具：[de.c](tool/de.c)


arm汇编学习工具:

https://github.com/linouxis9/ARMStrong

https://github.com/unicorn-engine/unicorn

https://bbs.pediy.com/thread-253868.htm


arm平台函数传递参数，反汇编实例分析:

https://blog.csdn.net/ayu_ag/article/details/50734282

https://blog.csdn.net/gooogleman/article/details/3538033

# mrp中ext的实现原理

最早的mrp实际是由mr文件组成的，mr文件其实就是编译后的lua，后来的mrp则用c语言开发，于是会至少一有个ext文件。

因为mrp标准开发环境是xp系统+ads+vs2005+skysdk，我用的虚拟机都有8G那么大，在了解mrp实现原理后我原本想用TCC编译器做一个可以精简到几M的开发环境，可惜TCC编译器并不支持arm版本的位置无关代码的生成（TCC正式发布的版本目前不支持，可能开发版已经有支持）

# 历史

20200202 第一次成功执行到了mrc_init函数

20200208 成功运行了带图像显示和触屏事件的helloworld

20201013 成功运行了推箱子游戏，实现了mr层的支持

20201224 wasm版本成功

# License

GNU General Public License v3.0
