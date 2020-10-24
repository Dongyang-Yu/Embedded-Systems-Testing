# Docker

## 安装镜像仓库

`docker pull agroce/cs499deepstate:deepstate `

## 使用镜像

`docker run -it agroce/cs499deepstate:deepstate `

## 启动容器

```bash
docker start -i <container_id>
```

我自己的是`4c21991422eb`

所以运行

```bash
docker start -i 4c21991422eb
```

## 进入容器

```bash
docker exec -it 4c21991422eb /bin/bash
```

## 查看容器：

```bash
docker ps -a
```

## 退出

```bash
exit
```

## 导出文件到本地

`docker cp 4c21991422eb:/home/user/assignment3 /home/dongyang/Desktop/499FuzzHw3`