/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description 常用常量定义模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var constants = require('constants');
 *  ```
 *  
 */
declare module 'constants' {
    /**
     * @description 动态库延迟加载 
     */
    export const RTLD_LAZY: 1;

    /**
     * @description 动态库立即加载 
     */
    export const RTLD_NOW: 2;

    /**
     * @description 动态库全局作用域加载 
     */
    export const RTLD_GLOBAL: 8;

    /**
     * @description 动态库局部作用域加载 
     */
    export const RTLD_LOCAL: 4;

    /**
     * @description 参数列表太长错误 
     */
    export const E2BIG: 7;

    /**
     * @description 没有访问权限错误 
     */
    export const EACCES: 13;

    /**
     * @description 地址已在使用错误 
     */
    export const EADDRINUSE: 48;

    /**
     * @description 地址不可用错误 
     */
    export const EADDRNOTAVAIL: 49;

    /**
     * @description 地址族不支持错误 
     */
    export const EAFNOSUPPORT: 47;

    /**
     * @description 资源暂时不可用错误 
     */
    export const EAGAIN: 35;

    /**
     * @description 连接已在进行中错误 
     */
    export const EALREADY: 37;

    /**
     * @description 文件描述符错误 
     */
    export const EBADF: 9;

    /**
     * @description 错误消息错误 
     */
    export const EBADMSG: 94;

    /**
     * @description 设备或资源忙错误 
     */
    export const EBUSY: 16;

    /**
     * @description 操作取消错误 
     */
    export const ECANCELED: 89;

    /**
     * @description 没有子进程错误 
     */
    export const ECHILD: 10;

    /**
     * @description 连接中止错误 
     */
    export const ECONNABORTED: 53;

    /**
     * @description 连接被拒绝错误 
     */
    export const ECONNREFUSED: 61;

    /**
     * @description 连接重置错误 
     */
    export const ECONNRESET: 54;

    /**
     * @description 资源死锁避免错误 
     */
    export const EDEADLK: 11;

    /**
     * @description 需要目标地址错误 
     */
    export const EDESTADDRREQ: 39;

    /**
     * @description 数学参数超出域错误 
     */
    export const EDOM: 33;

    /**
     * @description 超出配额错误 
     */
    export const EDQUOT: 69;

    /**
     * @description 文件已存在错误 
     */
    export const EEXIST: 17;

    /**
     * @description 错误地址错误 
     */
    export const EFAULT: 14;

    /**
     * @description 文件太大错误 
     */
    export const EFBIG: 27;

    /**
     * @description 主机不可达错误 
     */
    export const EHOSTUNREACH: 65;

    /**
     * @description 标识符已移除错误 
     */
    export const EIDRM: 90;

    /**
     * @description 非法字节序列错误 
     */
    export const EILSEQ: 92;

    /**
     * @description 操作进行中错误 
     */
    export const EINPROGRESS: 36;

    /**
     * @description 函数调用中断错误 
     */
    export const EINTR: 4;

    /**
     * @description 无效参数错误 
     */
    export const EINVAL: 22;

    /**
     * @description 输入/输出错误 
     */
    export const EIO: 5;

    /**
     * @description 套接字已连接错误 
     */
    export const EISCONN: 56;

    /**
     * @description 是目录错误 
     */
    export const EISDIR: 21;

    /**
     * @description 符号链接层次过多错误 
     */
    export const ELOOP: 62;

    /**
     * @description 打开文件过多错误 
     */
    export const EMFILE: 24;

    /**
     * @description 链接过多错误 
     */
    export const EMLINK: 31;

    /**
     * @description 消息太长错误 
     */
    export const EMSGSIZE: 40;

    /**
     * @description 多跳尝试错误 
     */
    export const EMULTIHOP: 95;

    /**
     * @description 文件名太长错误 
     */
    export const ENAMETOOLONG: 63;

    /**
     * @description 网络已关闭错误 
     */
    export const ENETDOWN: 50;

    /**
     * @description 网络连接重置错误 
     */
    export const ENETRESET: 52;

    /**
     * @description 网络不可达错误 
     */
    export const ENETUNREACH: 51;

    /**
     * @description 系统打开文件过多错误 
     */
    export const ENFILE: 23;

    /**
     * @description 无缓冲区空间错误 
     */
    export const ENOBUFS: 55;

    /**
     * @description 无数据可用错误 
     */
    export const ENODATA: 96;

    /**
     * @description 无此设备错误 
     */
    export const ENODEV: 19;

    /**
     * @description 无此文件或目录错误 
     */
    export const ENOENT: 2;

    /**
     * @description 可执行文件格式错误 
     */
    export const ENOEXEC: 8;

    /**
     * @description 无可用锁错误 
     */
    export const ENOLCK: 77;

    /**
     * @description 链接已断开错误 
     */
    export const ENOLINK: 97;

    /**
     * @description 内存不足错误 
     */
    export const ENOMEM: 12;

    /**
     * @description 无所需类型消息错误 
     */
    export const ENOMSG: 91;

    /**
     * @description 协议不可用错误 
     */
    export const ENOPROTOOPT: 42;

    /**
     * @description 设备上无空间错误 
     */
    export const ENOSPC: 28;

    /**
     * @description 流资源不足错误 
     */
    export const ENOSR: 98;

    /**
     * @description 设备不是流错误 
     */
    export const ENOSTR: 99;

    /**
     * @description 功能未实现错误 
     */
    export const ENOSYS: 78;

    /**
     * @description 套接字未连接错误 
     */
    export const ENOTCONN: 57;

    /**
     * @description 不是目录错误 
     */
    export const ENOTDIR: 20;

    /**
     * @description 目录不为空错误 
     */
    export const ENOTEMPTY: 66;

    /**
     * @description 套接字操作非套接字错误 
     */
    export const ENOTSOCK: 38;

    /**
     * @description 操作不支持错误 
     */
    export const ENOTSUP: 45;

    /**
     * @description 不适当的I/O控制操作错误 
     */
    export const ENOTTY: 25;

    /**
     * @description 无此设备或地址错误 
     */
    export const ENXIO: 6;

    /**
     * @description 套接字上不支持操作错误 
     */
    export const EOPNOTSUPP: 102;

    /**
     * @description 值太大无法存储在数据类型错误 
     */
    export const EOVERFLOW: 84;

    /**
     * @description 操作不允许错误 
     */
    export const EPERM: 1;

    /**
     * @description 管道破裂错误 
     */
    export const EPIPE: 32;

    /**
     * @description 协议错误 
     */
    export const EPROTO: 100;

    /**
     * @description 协议不支持错误 
     */
    export const EPROTONOSUPPORT: 43;

    /**
     * @description 协议错误类型错误 
     */
    export const EPROTOTYPE: 41;

    /**
     * @description 结果太大错误 
     */
    export const ERANGE: 34;

    /**
     * @description 只读文件系统错误 
     */
    export const EROFS: 30;

    /**
     * @description 无效的查找错误 
     */
    export const ESPIPE: 29;

    /**
     * @description 无此进程错误 
     */
    export const ESRCH: 3;

    /**
     * @description 文件句柄过时错误 
     */
    export const ESTALE: 70;

    /**
     * @description 定时器到期错误 
     */
    export const ETIME: 101;

    /**
     * @description 连接超时错误 
     */
    export const ETIMEDOUT: 60;

    /**
     * @description 文本文件忙错误 
     */
    export const ETXTBSY: 26;

    /**
     * @description 操作会阻塞错误 
     */
    export const EWOULDBLOCK: 35;

    /**
     * @description 跨设备链接错误 
     */
    export const EXDEV: 18;

    /**
     * @description 低优先级 
     */
    export const PRIORITY_LOW: 19;

    /**
     * @description 低于正常优先级 
     */
    export const PRIORITY_BELOW_NORMAL: 10;

    /**
     * @description 正常优先级 
     */
    export const PRIORITY_NORMAL: 0;

    /**
     * @description 高于正常优先级 
     */
    export const PRIORITY_ABOVE_NORMAL: -7;

    /**
     * @description 高优先级 
     */
    export const PRIORITY_HIGH: -14;

    /**
     * @description 最高优先级 
     */
    export const PRIORITY_HIGHEST: -20;

    /**
     * @description 挂起信号 
     */
    export const SIGHUP: 1;

    /**
     * @description 中断信号 
     */
    export const SIGINT: 2;

    /**
     * @description 退出信号 
     */
    export const SIGQUIT: 3;

    /**
     * @description 非法指令信号 
     */
    export const SIGILL: 4;

    /**
     * @description 跟踪陷阱信号 
     */
    export const SIGTRAP: 5;

    /**
     * @description 中止信号 
     */
    export const SIGABRT: 6;

    /**
     * @description IOT陷阱信号 
     */
    export const SIGIOT: 6;

    /**
     * @description 总线错误信号 
     */
    export const SIGBUS: 10;

    /**
     * @description 浮点异常信号 
     */
    export const SIGFPE: 8;

    /**
     * @description 杀死信号 
     */
    export const SIGKILL: 9;

    /**
     * @description 用户定义信号1 
     */
    export const SIGUSR1: 30;

    /**
     * @description 段错误信号 
     */
    export const SIGSEGV: 11;

    /**
     * @description 用户定义信号2 
     */
    export const SIGUSR2: 31;

    /**
     * @description 管道破裂信号 
     */
    export const SIGPIPE: 13;

    /**
     * @description 闹钟信号 
     */
    export const SIGALRM: 14;

    /**
     * @description 终止信号 
     */
    export const SIGTERM: 15;

    /**
     * @description 子进程终止或停止信号 
     */
    export const SIGCHLD: 20;

    /**
     * @description 继续执行信号 
     */
    export const SIGCONT: 19;

    /**
     * @description 停止执行信号 
     */
    export const SIGSTOP: 17;

    /**
     * @description 终端停止信号 
     */
    export const SIGTSTP: 18;

    /**
     * @description 后台进程尝试读取信号 
     */
    export const SIGTTIN: 21;

    /**
     * @description 后台进程尝试写入信号 
     */
    export const SIGTTOU: 22;

    /**
     * @description 套接字紧急条件信号 
     */
    export const SIGURG: 16;

    /**
     * @description CPU时间限制超出信号 
     */
    export const SIGXCPU: 24;

    /**
     * @description 文件大小限制超出信号 
     */
    export const SIGXFSZ: 25;

    /**
     * @description 虚拟定时器到期信号 
     */
    export const SIGVTALRM: 26;

    /**
     * @description 分析定时器到期信号 
     */
    export const SIGPROF: 27;

    /**
     * @description 窗口大小改变信号 
     */
    export const SIGWINCH: 28;

    /**
     * @description I/O现在可能信号 
     */
    export const SIGIO: 23;

    /**
     * @description 信息请求信号 
     */
    export const SIGINFO: 29;

    /**
     * @description 错误的系统调用信号 
     */
    export const SIGSYS: 12;

    /**
     * @description 符号链接到目录 
     */
    export const UV_FS_SYMLINK_DIR: 1;

    /**
     * @description 符号链接到连接点 
     */
    export const UV_FS_SYMLINK_JUNCTION: 2;

    /**
     * @description 仅打开读取 
     */
    export const O_RDONLY: 0;

    /**
     * @description 仅打开写入 
     */
    export const O_WRONLY: 1;

    /**
     * @description 打开读取和写入 
     */
    export const O_RDWR: 2;

    /**
     * @description 未知目录项类型 
     */
    export const UV_DIRENT_UNKNOWN: 0;

    /**
     * @description 文件目录项类型 
     */
    export const UV_DIRENT_FILE: 1;

    /**
     * @description 目录目录项类型 
     */
    export const UV_DIRENT_DIR: 2;

    /**
     * @description 符号链接目录项类型 
     */
    export const UV_DIRENT_LINK: 3;

    /**
     * @description FIFO目录项类型 
     */
    export const UV_DIRENT_FIFO: 4;

    /**
     * @description 套接字目录项类型 
     */
    export const UV_DIRENT_SOCKET: 5;

    /**
     * @description 字符设备目录项类型 
     */
    export const UV_DIRENT_CHAR: 6;

    /**
     * @description 块设备目录项类型 
     */
    export const UV_DIRENT_BLOCK: 7;

    /**
     * @description 文件类型位字段的位掩码 
     */
    export const S_IFMT: 61440;

    /**
     * @description 常规文件 
     */
    export const S_IFREG: 32768;

    /**
     * @description 目录 
     */
    export const S_IFDIR: 16384;

    /**
     * @description 字符设备 
     */
    export const S_IFCHR: 8192;

    /**
     * @description 块设备 
     */
    export const S_IFBLK: 24576;

    /**
     * @description FIFO 
     */
    export const S_IFIFO: 4096;

    /**
     * @description 符号链接 
     */
    export const S_IFLNK: 40960;

    /**
     * @description 套接字 
     */
    export const S_IFSOCK: 49152;

    /**
     * @description 如果文件不存在则创建文件 
     */
    export const O_CREAT: 512;

    /**
     * @description 确保文件的独占创建 
     */
    export const O_EXCL: 2048;

    /**
     * @description 文件映射标志 
     */
    export const UV_FS_O_FILEMAP: 0;

    /**
     * @description 不分配控制终端 
     */
    export const O_NOCTTY: 131072;

    /**
     * @description 将文件截断为零长度 
     */
    export const O_TRUNC: 1024;

    /**
     * @description 追加到文件末尾 
     */
    export const O_APPEND: 8;

    /**
     * @description 打开目录 
     */
    export const O_DIRECTORY: 1048576;

    /**
     * @description 不跟随符号链接 
     */
    export const O_NOFOLLOW: 256;

    /**
     * @description 同步I/O 
     */
    export const O_SYNC: 128;

    /**
     * @description 同步I/O数据完整性完成 
     */
    export const O_DSYNC: 4194304;

    /**
     * @description 允许打开符号链接 
     */
    export const O_SYMLINK: 2097152;

    /**
     * @description 非阻塞模式 
     */
    export const O_NONBLOCK: 4;

    /**
     * @description 所有者读写执行权限 
     */
    export const S_IRWXU: 448;

    /**
     * @description 所有者读权限 
     */
    export const S_IRUSR: 256;

    /**
     * @description 所有者写权限 
     */
    export const S_IWUSR: 128;

    /**
     * @description 所有者执行权限 
     */
    export const S_IXUSR: 64;

    /**
     * @description 组读写执行权限 
     */
    export const S_IRWXG: 56;

    /**
     * @description 组读权限 
     */
    export const S_IRGRP: 32;

    /**
     * @description 组写权限 
     */
    export const S_IWGRP: 16;

    /**
     * @description 组执行权限 
     */
    export const S_IXGRP: 8;

    /**
     * @description 其他人读写执行权限 
     */
    export const S_IRWXO: 7;

    /**
     * @description 其他人读权限 
     */
    export const S_IROTH: 4;

    /**
     * @description 其他人写权限 
     */
    export const S_IWOTH: 2;

    /**
     * @description 其他人执行权限 
     */
    export const S_IXOTH: 1;

    /**
     * @description 测试文件是否存在 
     */
    export const F_OK: 0;

    /**
     * @description 测试读权限 
     */
    export const R_OK: 4;

    /**
     * @description 测试写权限 
     */
    export const W_OK: 2;

    /**
     * @description 测试执行权限 
     */
    export const X_OK: 1;

    /**
     * @description 独占复制文件标志 
     */
    export const UV_FS_COPYFILE_EXCL: 1;

    /**
     * @description 独占复制文件标志 
     */
    export const COPYFILE_EXCL: 1;

    /**
     * @description 文件克隆复制标志 
     */
    export const UV_FS_COPYFILE_FICLONE: 2;

    /**
     * @description 文件克隆复制标志 
     */
    export const COPYFILE_FICLONE: 2;

    /**
     * @description 文件克隆强制复制标志 
     */
    export const UV_FS_COPYFILE_FICLONE_FORCE: 4;

    /**
     * @description 文件克隆强制复制标志 
     */
    export const COPYFILE_FICLONE_FORCE: 4;

}

