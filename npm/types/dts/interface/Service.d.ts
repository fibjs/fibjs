/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description 系统服务管理对象 
 */
declare class Class_Service extends Class_EventEmitter {
    /**
     * @description 系统服务管理对象构造函数
     *      @param name 服务名称
     *      @param worker 服务运行函数
     *      @param event 服务事件处理
     *      
     */
    constructor(name: string, worker: (...args: any[])=>any, event?: FIBJS.GeneralObject);

    /**
     * @description 开始运行服务实体 
     */
    run(): void;

    run(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 查询和设置服务名称 
     */
    name: string;

    /**
     * @description 查询和绑定服务停止事件，相当于 on("stop", func); 
     */
    onstop: (...args: any[])=>any;

    /**
     * @description 查询和绑定服务暂停事件，相当于 on("pause", func); 
     */
    onpause: (...args: any[])=>any;

    /**
     * @description 查询和绑定服务恢复事件，相当于 on("continue", func); 
     */
    oncontinue: (...args: any[])=>any;

    /**
     * @description 安装服务到系统
     *      @param name 服务名称
     *      @param cmd 服务命令行
     *      @param displayName 服务显示名称
     *      @param description 服务描述信息
     *      
     */
    static install(name: string, cmd: string, displayName?: string, description?: string): void;

    /**
     * @description 从系统中卸载服务 
     *      @param name 服务名称
     *      
     */
    static remove(name: string): void;

    /**
     * @description 启动服务 
     *      @param name 服务名称
     *      
     */
    static start(name: string): void;

    /**
     * @description 停止服务 
     *      @param name 服务名称
     *      
     */
    static stop(name: string): void;

    /**
     * @description 重启服务 
     *      @param name 服务名称
     *      
     */
    static restart(name: string): void;

    /**
     * @description 检测服务是否安装
     *      @param name 服务名称
     *      @return 服务安装返回 True
     *      
     */
    static isInstalled(name: string): boolean;

    /**
     * @description 检测服务是否运行
     *      @param name 服务名称
     *      @return 服务运行返回 True
     *      
     */
    static isRunning(name: string): boolean;

}

