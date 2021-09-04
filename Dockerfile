FROM ubuntu:18.04

RUN rm -rf /var/lib/apt/lists/* && mkdir /var/lib/apt/lists/partial && sed -i "s/archive.ubuntu.com/mirrors.ustc.edu.cn/g" /etc/apt/sources.list && \
  apt-get update -y --fix-missing && \
  apt-get install -y vim gcc git wget && \
  apt-get install -y build-essential autoconf automake libtool intltool && \
  apt-get install -y flex python3 python3-pip cmake gdb m4 lzip

# gmp
RUN cd / && wget -c "https://gmplib.org/download/gmp/gmp-6.2.1.tar.lz" -O "gmp.tar.lz" && \
    lzip -d gmp.tar.lz && tar -xvf gmp.tar && \
    cd /gmp-6.2.1 && ./configure --enable-cxx && make && make check && make install

# java jdk 1.8，default locate: usr/bin/java
RUN apt-get install -y openjdk-8-jdk default-jre
# java jdk 16
# Note that you might need to change your mirror based on your OS and core. Use bash command `arch` and try to use google to help you.
# jdk 16 is installed in /jdk-16/bin/java
RUN cd / && wget https://mirrors.tuna.tsinghua.edu.cn/AdoptOpenJDK/16/jdk/aarch64/linux/ibm-semeru-open-jdk_aarch64_linux_16.0.2_7_openj9-0.27.0.tar.gz && \
    tar -zxvf ibm-semeru-open-jdk_aarch64_linux_16.0.2_7_openj9-0.27.0.tar.gz && mv jdk-16.0.2+7 jdk-16

# Bison 3.0.4
RUN wget -P / https://mirrors.ustc.edu.cn/gnu/bison/bison-3.0.4.tar.gz && \
    cd / && tar -zxvf bison-3.0.4.tar.gz && \
    cd /bison-3.0.4 && \
    ./configure && make && make install

# glog
RUN cd / && \
    git clone https://github.com/google/glog.git && \
    cd glog && \
    mkdir build && \
    cd build && \
    cmake .. && \
    cmake --build . && \
    cmake --build . --target test && \
    cmake --build . --target install

# git clone ABC into /
RUN cd / && \
    git clone https://github.com/vlab-cs-ucsb/ABC.git
  
# Mona
RUN cd / && \
    git clone https://github.com/cs-au-dk/MONA.git && \
    cd MONA && \
    git apply --whitespace=nowarn /ABC/external/mona/mona_abc.patch && \
    libtoolize && aclocal && automake --gnu --add-missing && autoreconf -ivf && \
    ./configure && make all && make install && ldconfig

# ABC
RUN cd /ABC && \
    ./autogen.sh && \
    ./configure && make && make install && ldconfig

RUN mkdir -p /src/autobc/build

ADD . /src/autobc

# build
RUN cd /src/autobc/build && cmake .. && make && cd -

WORKDIR /src/autobc

# 设置CMD
CMD ["/bin/bash"]
