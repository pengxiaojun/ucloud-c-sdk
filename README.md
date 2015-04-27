## Welcome to use ucloud-c-sdk ##

**ucloud-c-sdk** 是专门针对[ucloud](http://wwww.ucloud.cn) api的一个简洁，轻量级的基于C的sdk, 主要包含以下功能:

- uhost
- umon
- unet
- http签名
- http request parameter处理 

### 源码目录树介绍 ###

- [3rdparty](https://gitcafe.com/pengxj/ucloud-c-sdk/tree/master/3rdparty): 第三方依赖库(json-c)
- [include](https://gitcafe.com/pengxj/ucloud-c-sdk/tree/master/include): sdk头文件 (包含 `Doxyfile`)
- [src](https://gitcafe.com/pengxj/ucloud-c-sdk/tree/master/src): sdk源码
- [test](https://gitcafe.com/pengxj/ucloud-c-sdk/tree/master/test): 测试代码
- [tool](https://gitcafe.com/pengxj/ucloud-c-sdk/tree/master/tool): ucloudadm命令行管理工具

### Dependency ###

- [curl](http://curl.haxx.se): Suse下安装: `$ zypper in libcurl-devel`
- [Doxygen](http://www.stack.nl/~dimitri/doxygen): 用于生成文档，如果缺少该依赖,并不影响SDK编译与运行.Suse下安装: `zypper in doxygen doxywizard`

### 编译步骤 ###

 一键编译源码，测试代码，命令行管理工具并生成sdk文档
   
`$ ./make.sh`

### 注意事项 ###

在使用该sdk之前，必须设置 **UCLOUD_PUBLIC_KEY** 以及 **UCLOUD_PRIVATE_KEY**

`$ echo "export UCLOUD_PUBLIC_KEY=your-ucloud-public-key" >> ~/.bashrc`

`$ echo "export UCLOUD_PRIVATE_KEY=your-ucloud-private-key" >> ~/.bashrc`

### Quick Start ###

请参考[wiki](https://gitcafe.com/pengxj/ucloud-c-sdk/wiki/Quick-start)

### ucloudadm命令行工具 ###

ucloudadm是用于管理ucloud host, umon, unet的命令行工具. 
详细请参考[wiki](https://gitcafe.com/pengxj/ucloud-c-sdk/wiki/ucloudadm-tool-usage)
