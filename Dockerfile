FROM ubuntu:18.04

RUN apt-get update -y && \
  apt-get install -y vim gcc git wget && \
  apt-get install -y build-essential autoconf automake libtool intltool && \
  apt-get install -y flex python3 python3-pip cmake

# 安装java jdk 1.8，可以直接使用/usr/bin/java运行
RUN apt-get install -y openjdk-8-jdk default-jre
# 安装java jdk 16
# 注意，你可能需要根据你的内核来决定用哪个镜像源，下面的这条命令可能需要更改
# 安装后，根目录会出现一个新的文件夹/jdk-16.0.2+7，重命名后为/jdk-16
# 可以使用/jdk-16/bin/java来启动
RUN cd / && wget https://mirrors.tuna.tsinghua.edu.cn/AdoptOpenJDK/16/jdk/aarch64/linux/ibm-semeru-open-jdk_aarch64_linux_16.0.2_7_openj9-0.27.0.tar.gz && \
    tar -zxvf ibm-semeru-open-jdk_aarch64_linux_16.0.2_7_openj9-0.27.0.tar.gz && mv jdk-16.0.2+7 jdk-16

# 安装Bison 3.0.4，通常来说，apt直接得到的是2.x版本，需要重新安装
RUN wget -P / https://mirrors.ustc.edu.cn/gnu/bison/bison-3.0.4.tar.gz && \
    cd / && tar -zxvf bison-3.0.4.tar.gz && \
    cd /bison-3.0.4 && \
    ./configure && make && make install

# 安装glog
RUN cd / && \
    git clone https://github.com/google/glog.git && \
    cd glog && \
    mkdir build && \
    cd build && \
    cmake .. && \
    cmake --build . && \
    cmake --build . --target test && \
    cmake --build . --target install

# 克隆ABC到根目录
RUN cd / && \
    git clone https://github.com/vlab-cs-ucsb/ABC.git
  
# 安装Mona，并且应用ABC的补丁
RUN cd / && \
    git clone https://github.com/cs-au-dk/MONA.git && \
    cd MONA && \
    git apply --whitespace=nowarn /ABC/external/mona/mona_abc.patch && \
    libtoolize && aclocal && automake --gnu --add-missing && autoreconf -ivf && \
    ./configure && make all && make install && ldconfig

# 安装ABC
RUN cd /ABC && \
    ./autogen.sh && \
    ./configure && make && make install && ldconfig

RUN mkdir -p /src/autobc/build

ADD . /src/autobc

# 尝试编译
RUN cd /src/autobc/build && cmake .. && make && cd -

WORKDIR /src/autobc

# 设置CMD
CMD ["/bin/bash"]
