# community-IM-Server-client
轻量级的IM登录服务，网管服务， 安全采用thrift+AES256加密，数据库采用mysql，win32/linux平台

一、概论

这个平台是基于跨平台网络库socketliet实现的基于thrift的协议跨平台、跨语言的轻量级RPC IM服务器。
二、组件

1.登录服务

    负责用户密码校验，网关服务的负载均衡（rabin）。

2.网关服务

    负责登录获取的session接入网关及验证。

3.测试client

    提供与登录服务、网关服务业务的全流程thrift rpc通信模拟。

4.业务服务器

    处于网关服务器之后，需要读者自行开发。
三、编译说明

编译工具采用vs2010 x64环境开发，数据库采用mysql、存储过程。

四、linux环境

支持，但暂有bug，后面有时间再fix掉。
