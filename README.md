# Dockerfile搭建所有环境

```bash
# 搭建镜像
docker build . -t autobc:latest
# 容器启动
docker run -itd --name autobc -p 22:22 --restart=always autobc:latest
# 进入容器
docker exec autobc /bin/bash
```