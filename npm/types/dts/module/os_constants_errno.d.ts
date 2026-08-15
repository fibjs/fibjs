/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description os_constants errno 子模块，包含 POSIX 错误码常量
 * 
 *  引用方法：
 *  ```JavaScript
 *  var errno = require('os').constants.errno
 *  ```
 *  
 */
declare module 'os_constants_errno' {
    /**
     * @description 参数列表过长 
     */
    export const E2BIG: 7;

    /**
     * @description 权限不足 
     */
    export const EACCES: 13;

    /**
     * @description 地址已被占用 
     */
    export const EADDRINUSE: 98;

    /**
     * @description 地址不可用 
     */
    export const EADDRNOTAVAIL: 99;

    /**
     * @description 地址族不受支持 
     */
    export const EAFNOSUPPORT: 97;

    /**
     * @description 资源暂时不可用，可重试 
     */
    export const EAGAIN: 11;

    /**
     * @description 操作已在执行中 
     */
    export const EALREADY: 114;

    /**
     * @description 无效的文件描述符 
     */
    export const EBADF: 9;

    /**
     * @description 无效的消息 
     */
    export const EBADMSG: 74;

    /**
     * @description 设备或资源忙 
     */
    export const EBUSY: 16;

    /**
     * @description 操作已取消 
     */
    export const ECANCELED: 125;

    /**
     * @description 没有子进程 
     */
    export const ECHILD: 10;

    /**
     * @description 连接被中止 
     */
    export const ECONNABORTED: 103;

    /**
     * @description 连接被拒绝 
     */
    export const ECONNREFUSED: 111;

    /**
     * @description 连接被重置 
     */
    export const ECONNRESET: 104;

    /**
     * @description 资源死锁 
     */
    export const EDEADLK: 35;

    /**
     * @description 需要目标地址 
     */
    export const EDESTADDRREQ: 89;

    /**
     * @description 数学参数超出函数定义域 
     */
    export const EDOM: 33;

    /**
     * @description 磁盘配额超限 
     */
    export const EDQUOT: 122;

    /**
     * @description 文件已存在 
     */
    export const EEXIST: 17;

    /**
     * @description 无效的内存地址 
     */
    export const EFAULT: 14;

    /**
     * @description 文件过大 
     */
    export const EFBIG: 27;

    /**
     * @description 主机不可达 
     */
    export const EHOSTUNREACH: 113;

    /**
     * @description 标识符已删除 
     */
    export const EIDRM: 43;

    /**
     * @description 非法字节序列 
     */
    export const EILSEQ: 84;

    /**
     * @description 操作正在进行中 
     */
    export const EINPROGRESS: 115;

    /**
     * @description 被信号中断 
     */
    export const EINTR: 4;

    /**
     * @description 无效的参数 
     */
    export const EINVAL: 22;

    /**
     * @description I/O 错误 
     */
    export const EIO: 5;

    /**
     * @description 套接字已连接 
     */
    export const EISCONN: 106;

    /**
     * @description 路径是目录 
     */
    export const EISDIR: 21;

    /**
     * @description 符号链接层数过多 
     */
    export const ELOOP: 40;

    /**
     * @description 打开的文件过多 
     */
    export const EMFILE: 24;

    /**
     * @description 链接数过多 
     */
    export const EMLINK: 31;

    /**
     * @description 消息过长 
     */
    export const EMSGSIZE: 90;

    /**
     * @description 多跳尝试 
     */
    export const EMULTIHOP: 72;

    /**
     * @description 文件名过长 
     */
    export const ENAMETOOLONG: 36;

    /**
     * @description 网络已关闭 
     */
    export const ENETDOWN: 100;

    /**
     * @description 连接被网络重置 
     */
    export const ENETRESET: 102;

    /**
     * @description 网络不可达 
     */
    export const ENETUNREACH: 101;

    /**
     * @description 系统文件表溢出 
     */
    export const ENFILE: 23;

    /**
     * @description 无可用缓冲区 
     */
    export const ENOBUFS: 105;

    /**
     * @description 无可用数据 
     */
    export const ENODATA: 61;

    /**
     * @description 无此设备 
     */
    export const ENODEV: 19;

    /**
     * @description 文件或目录不存在 
     */
    export const ENOENT: 2;

    /**
     * @description 可执行文件格式错误 
     */
    export const ENOEXEC: 8;

    /**
     * @description 无可用锁 
     */
    export const ENOLCK: 37;

    /**
     * @description 链接已断开 
     */
    export const ENOLINK: 67;

    /**
     * @description 内存不足 
     */
    export const ENOMEM: 12;

    /**
     * @description 没有指定类型的消息 
     */
    export const ENOMSG: 42;

    /**
     * @description 协议不可用 
     */
    export const ENOPROTOOPT: 92;

    /**
     * @description 设备上没有剩余空间 
     */
    export const ENOSPC: 28;

    /**
     * @description 没有可用的流资源 
     */
    export const ENOSR: 63;

    /**
     * @description 不是流设备 
     */
    export const ENOSTR: 60;

    /**
     * @description 功能未实现 
     */
    export const ENOSYS: 38;

    /**
     * @description 套接字未连接 
     */
    export const ENOTCONN: 107;

    /**
     * @description 路径不是目录 
     */
    export const ENOTDIR: 20;

    /**
     * @description 目录非空 
     */
    export const ENOTEMPTY: 39;

    /**
     * @description 不是套接字 
     */
    export const ENOTSOCK: 88;

    /**
     * @description 不支持的操作 
     */
    export const ENOTSUP: 95;

    /**
     * @description 不适当的 ioctl 操作 
     */
    export const ENOTTY: 25;

    /**
     * @description 无此设备或地址 
     */
    export const ENXIO: 6;

    /**
     * @description 套接字不支持的操作 
     */
    export const EOPNOTSUPP: 95;

    /**
     * @description 数值过大 
     */
    export const EOVERFLOW: 75;

    /**
     * @description 操作不允许 
     */
    export const EPERM: 1;

    /**
     * @description 管道破裂 
     */
    export const EPIPE: 32;

    /**
     * @description 协议错误 
     */
    export const EPROTO: 71;

    /**
     * @description 协议不受支持 
     */
    export const EPROTONOSUPPORT: 93;

    /**
     * @description 套接字协议类型错误 
     */
    export const EPROTOTYPE: 91;

    /**
     * @description 结果超出范围 
     */
    export const ERANGE: 34;

    /**
     * @description 只读文件系统 
     */
    export const EROFS: 30;

    /**
     * @description 无效的 seek 操作 
     */
    export const ESPIPE: 29;

    /**
     * @description 没有此进程 
     */
    export const ESRCH: 3;

    /**
     * @description 过期的文件句柄 
     */
    export const ESTALE: 116;

    /**
     * @description 定时器过期 
     */
    export const ETIME: 62;

    /**
     * @description 操作超时 
     */
    export const ETIMEDOUT: 110;

    /**
     * @description 文本文件忙 
     */
    export const ETXTBSY: 26;

    /**
     * @description 操作会阻塞 
     */
    export const EWOULDBLOCK: 11;

    /**
     * @description 跨设备链接 
     */
    export const EXDEV: 18;

}

