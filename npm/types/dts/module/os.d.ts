/// <reference path="../interface/Service.d.ts" />
/// <reference path="../interface/BufferedStream.d.ts" />
/**
 * @description 操作系统与文件系统处理模块
 * 
 *  使用方法：
 *  ```JavaScript
 *  var os = require('os');
 *  ```
 *  
 */
declare module 'os' {
    /**
     * @description Service 构造函数，参见 Service 
     */
    const Service: typeof Class_Service;

    /**
     * @description 查询当前运行环境主机名 
     *      @return 返回主机名
     *     
     */
    function hostname(): string;

    /**
     * @description 查询当前 CPU 的字节顺序
     *      @return 返回字节顺序
     *     
     */
    function endianness(): string;

    /**
     * @description 查询当前运行环境操作系统名称 
     *      @return 返回系统名称
     *     
     */
    function type(): string;

    /**
     * @description 查询当前运行环境操作系统版本
     *      @return 返回版本信息
     *      
     */
    function release(): string;

    /**
     * @description 查询当前用户目录 
     *      @return 返回目录字符串
     *     
     */
    function homedir(): string;

    /**
     * @description 查询当前 cpu 环境
     *      @return 返回 cpu 类型，可能的结果为 'amd64', 'arm', 'arm64', 'ia32' 
     *     
     */
    function arch(): string;

    /**
     * @description 查询运行环境当前时区 
     */
    const timezone: number;

    /**
     * @description 查询当前运行环境行结尾标识，posix:\"\\n\"；windows:\"\\r\\n\"   
     */
    const EOL: string;

    /**
     * @description 查询运行环境运行时间，以秒为单位
     *      @return 返回表示时间的数值
     *     
     */
    function uptime(): number;

    /**
     * @description 查询运行环境 1分钟，5分钟，15分钟平均负载
     *      @return 返回包含三个负载数据的数组
     *     
     */
    function loadavg(): any[];

    /**
     * @description 查询运行环境总内存，以字节为单位
     *      @return 返回内存数据
     *     
     */
    function totalmem(): number;

    /**
     * @description 查询运行环境可用内存，以字节为单位
     *      @return 返回内存数据
     *     
     */
    function freemem(): number;

    /**
     * @description 查询当前运行环境 cpu 个数和参数
     *      @return 返回包含 cpu 参数的数组，每一项对应一个 cpu
     *     
     */
    function cpus(): any[];

    /**
     * @description 查询当前运行环境 cpu 个数
     *      @return 返回 cpu 个数
     *     
     */
    function cpuNumbers(): number;

    /**
     * @description 查询当前运行环境临时文件目录
     *      @return 返回临时文件目录
     *     
     */
    function tmpdir(): string;

    /**
     * @description 返回当前有效执行用户信息
     *      @param options 用于解释结果字符串的字符编码
     *      @return 当前有效执行用户信息
     *     
     */
    function userInfo(options: object): object;

    /**
     * @description 查询当前运行环境网络信息
     *      @return 返回网卡信息
     *     
     */
    function networkInterfaces(): object;

    /**
     * @description 查询当前主机的打印机信息
     *      @return 返回打印机信息
     *     
     */
    function printerInfo(): any[];

    /**
     * @description 创建一个打印机输出对象
     *      @param name 打印机名称
     *      @return 返回打印机输出对象
     *     
     */
    function openPrinter(name: string): Class_BufferedStream;

    function openPrinter(name: string, callback: (err: Error | undefined | null, retVal: Class_BufferedStream)=>any): void;

    /**
     * @description 查询当前平台名称
     *      @return 返回平台名称，可能的结果为 'darwin', 'freebsd', 'linux', 或 'win32' 
     *      
     */
    function platform(): string;

    /**
     * @description 解析时间字符串或查询运行环境当前时间
     *      @param tmString 时间字符串，缺省则查询当前时间
     *      @return 返回 javascript Date 对象
     *      
     */
    function time(tmString: string): typeof Date;

    /**
     * @description 时间计算函数，根据 part 指定计算时间
     *      @param d 指定用于计算 Date 对象
     *      @param num 指定运算的数值
     *      @param part 指定运算的时间部位，接收值为："year", "month", "day", "hour", "minute", "second"
     *      @return 返回 javascript Date 对象
     *      
     */
    function dateAdd(d: typeof Date, num: number, part: string): typeof Date;

    /**
     * @description 查询当前运行执行文件完整路径 
     */
    const execPath: string;

    /**
     * @description 查询当前进程内存使用报告
     * 
     *      内存报告生成类似以下结果：
     *      ```JavaScript
     *      {
     *        "rss": 8622080,
     *        "heapTotal": 4083456,
     *        "heapUsed": 1621800,
     *        "nativeObjects": 122
     *      }
     *      ```
     *      其中：
     *      - rss 返回进程当前占用物理内存大小
     *      - heapTotal 返回 v8 引擎堆内存大小
     *      - heapUsed 返回 v8 引擎正在使用堆内存大小
     *      - nativeObjects 返回当前有效内置对象数
     *      @return 返回包含内存报告
     *      
     */
    function memoryUsage(): object;

}

