# Makefile中 -I -L -l区别

我们用gcc编译程序时，可能会用到“-I”（大写i），“-L”（大写l），“-l”（小写l）等参数，下面做个记录：
例：
`gcc -o hello hello.c -I /home/hello/include -L /home/hello/lib -lworld`
上面这句表示在编译hello.c时：

- `-I /home/hello/include`表示将`/home/hello/include`目录作为第一个寻找头文件的目录，寻找的顺序是：/home/hello/include-->/usr/include-->/usr/local/include
- -L /home/hello/lib表示将/home/hello/lib目录作为第一个寻找库文件的目录，寻找的顺序是：/home/hello/lib-->/lib-->/usr/lib-->/usr/local/lib
-  -lworld表示在上面的lib的路径中寻找libworld.so动态库文件（如果gcc编译选项中加入了“-static”表示寻找libworld.a静态库文件）



# 调用第三方库 需要构建 静态链接库

> https://blog.csdn.net/qq_29933439/article/details/78897796

1、编译文件生成静态库。

```makefile
all:
	gcc action.c -o action.o
	ar -rc action.a action.o
```



2、把相关的.h文件放入D:\VS2005安装目录\VC\include路径下。

3、把相应的.lib文件放在D:\VS2005安装目录\VC\lib路径下。

4、包含头文件-调用。

