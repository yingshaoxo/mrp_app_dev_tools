# mrp dev tools

> Do not use newest version, has back doors. For example, skysdk2024, it means in 2024, still have people modify the sdk. And you can't even offline view the 2024 docs because they require network to reverse your IP address.

A app template is inside "./an_app_template", but it does not even support some basic c sytax, either c89 or c99.

The compiler is inside of the "./compiler", but most of time you just use the "./an_app_template/compile_and_run.sh".

I suddently found the arm compiler here they use has copyright, you have to pay to use. They use "adsv12", I have never heard any hardware code compiler needs to pay to work.

As for the "MaoPao" platform, it seems like they have a strong censorship in apps or games, and they even set a backdoor in http or socket module, so that it has a whitelist, can control what website you can visit, what you can't. 

You can't even visit localhost or LAN, because of that limitation. It is freedom killer.

Strongly recommend you do offline test, because nobody know what would happen when you install a "virus" (I say it is "virus" because the sdk is not open sourced).

> The 7z compressed version of this folder is only about 37MB.

<!--
解压至任意文件夹即可开始开发mrp！
小蟀QQ：1126390395

一、complier文件夹为编译环境，非专业人士请勿动
二、store和手速解压为项目模板，可仿照/复制创建新项目
	build.bat：编译项目
	run.bat：编译并运行项目(调用vmrp)
三、报毒SDKCrack.exe
	加白名单或者删除即可
-->
