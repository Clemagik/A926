#安装使用vscode

## 1、vccode安装及使用
VScode官方安装包下载:[https://code.visualstudio.com/Download](https://code.visualstudio.com/Download)

VScode官方安装参考文档:[https://code.visualstudio.com/docs/setup/linux](https://code.visualstudio.com/docs/setup/linux)

VScode官方调试使用参考文档:[https://code.visualstudio.com/docs/cpp/cpp-debug](https://code.visualstudio.com/docs/cpp/cpp-debug)

VScode官方调试配置参考文档:[https://code.visualstudio.com/docs/cpp/launch-json-reference](https://code.visualstudio.com/docs/cpp/launch-json-reference)

## 2、调试环境搭建及工程编译

- 在Manhattan工程顶层目录下，可通过以下命令进行工程交叉调试配置
```shell
make  <ProjectName>-vscode PRO=<ProjectName>
```
- 进入工程目录后执行以下命令进入工作区
```shell
code <ProjectName>.code-workspace
```
命令执行完毕将会进入vscode可视化调试阶段。

在需要编译的**源文件**界面上，点击右上方“△”图案，或者按下**Ctrl + Alt +N**可进行文件的编译并烧录至开发板。

- 在Manhattan工程顶层目录下，可通过以下命令进行工程配置信息清除
```shell
make  <ProjectName>-clean PRO=<ProjectName>
```
##3、开始调试
F5启动调试，可以通过F9设断点，F11单步等来调试程序。
