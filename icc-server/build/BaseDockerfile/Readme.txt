#通过--net=host参数改变容器网络模式
docker run -it --name test --net=host -dit 3e7b8ec3b86e