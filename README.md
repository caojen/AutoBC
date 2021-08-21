# Dockerfile搭建所有环境

```bash
# 搭建镜像
docker build . -t autobc:latest
# 容器启动
docker run -itd --name autobc -p 22:22 --restart=always autobc:latest
# 进入容器
docker exec -it autobc /bin/bash
```

> 已经有一个已经搭建好的镜像部署到DockerHub上。你也可以使用``docker pull opentextfile/autobc:latest``来代替上面的搭建镜像操作。

> 如果你确保你的宿主机上有root权限，你也可以根据Dockerfile来进行部署。（将Dockerfile的相关命令自己在宿主机上跑一遍。镜像的基础容器是Ubuntu:18.04）注意，如果没有root权限，那么相关依赖的安装非常麻烦。

# 可执行文件
本节中将会介绍autobc所给出的可执行文件以及它们的作用。

所有的可执行文件在搭建镜像完成之后将会位于``./build``目录下。

## rander
rander的功能是随机生成“邻近的LTL公式”。你需要提供以下参数来执行rander：
1. nuXmv执行文件路径。如果你使用了docker搭建，那么该版本的nuXmv已经提供，存放位置为``./bin/nuXmv-2.0.0-Linux``，如果你使用了其他发行版，那么你需要到官网下载对应的nuXmv
2. 确定你需要的LTL公式的最少数量。
3. 确定你需要的LTL公式的深度范围。
4. 确定每次搜索的时间。推荐为5～15秒。
5. 确定你所需要的LTL公式中，变量的数量。变量是从小写字母b开始，到小写字母z结束，最多25个。例如，3个变量代表``b,c,d``
6. 指明输出结果存放位置.

使用rander的方法如下：
```bash
./rander nuXmv执行文件路径 LTL公式的最少数量 LTL公式的最低深度 LTL公式的最高深度 搜索时间 变量数目 结果输出文件
```
例如：
```
./rander /src/autobc/bin/nuXmv-2.0.0-Linux 100 5 20 10 3 /tmp/rander.output
```

在生成的结果输出文件中，奇数行代表的是SAT的公式，偶数行代表的是UNSAT的公式。

## autobc
用于BC修复。

BC修复需要使用jdk8和jdk16，docker镜像已经安装好相关内容了。

使用``./autobc --help``来获得所有支持的参数列表：
```bash
# ./autobc --help
Usage: ./build/autobc -b|--bcfile ${bcfile} -g|--global ${global} [-i|--ignore] [-8|--jdk1.8] ${jdk1.8} [-6|--jdk16] ${jdk16} -k|--k ${k} -l|--level ${level} -e|--likelyhood ${likelyhood} -m|--modelcounting ${modelcounting} 

-h, --help : Show Help Page.
-b, --bcfile bcfile 
                        [required] Specify BCs File.
-g, --global global 
                        [required] Specify domains and goals File.
-i, --ignore 
                        If specify, the first line of bcfile will be ignored
-8, --jdk1.8 jdk1.8 
                        Specify JDK 1.8 java execute path. Default: /usr/bin/java
-6, --jdk16 jdk16 
                        Specify JDK 16 java execute path. Default: /usr/bin/java
-k, --k k 
                        [required] Specify K of Model Counting.
-l, --level level 
                        [required] Specify Level of our algorithm.
-e, --likelyhood likelyhood 
                        [required] Specify LikelyHood.jar.
-m, --modelcounting modelcounting 
                        [required] Specify ModelCounting.jar.
```

下面以minepipe为例子，对参数进行说明：

1. -g, --global: 指定Domain和Goal文件的位置。这个文件的格式例：
```
Domains: []((p && X(p)) -> X(X(! h)))
Goals: [](h -> X(p)), [](m -> X(! p))
```
两者各占一行，每个公式之间用逗号和空格进行分隔.

2. -b, --bcfile: 指定所有BC。由于搜索BC时间比较慢，因此你需要提供BC的输入文件。

```
minepipe
F(h & m)
...
```

其中，第一行可以使用``-i, --ignore``参数来忽略；然后，每一行代表一个BC。

3. -i, --ignore: 如果指定，那么bcfile的第一行不会被读取.

4. -8, --jdk1.8: 指定jdk1.8的java执行文件位置，在Docker部署的情况下，请输入``/usr/bin/java``

5. -6, --jdk16 : 指定jdk16的java执行文件位置，在Docker部署的情况下，请输入``/jdk-16/bin/java``

6. -k, --k     : 指定在进行ModelCounting时，使用的k的大小

7. -l, --level : 指定在进行BC修复的时候，修复的层数

8. -e, --likelyhood: 指定likelyhood.jar文件的位置。在Docker部署的情况下，请输入``/src/autobc/Lasso-BC/likelyhood.jar``

9. -m, --modelcounting: 指定modelcounting.jar文件的位置。在Docker部署的情况下，请输入``/src/autobc/jar/modelcounting``

随后，修复的结果将会输出到标准输出中。
一个可用的参数列表如下，为了方便观察，已经将该列表用json的形式展示：
```json
[
  "--global",
  "/src/autobc/Lasso-BC/minepump/minepump.txt",
  "--bcfile",
  "/src/autobc/Lasso-BC/minepump/generalBC.txt",
  "--ignore",
  "-8",
  "/usr/bin/java",
  "-6",
  "/jdk-16/bin/java",
  "-k",
  "100",
  "--level",
  "1",
  "--likelyhood",
  "/src/autobc/Lasso-BC/likelyhood.jar",
  "--modelcounting",
  "/src/autobc/jar/modelcounting.jar"
  "-n",
  "/src/autobc/bin/nuXmv-2.0.0-Linux"
]
```
