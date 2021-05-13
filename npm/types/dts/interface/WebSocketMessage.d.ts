/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Message.d.ts" />
/**
 * @description websocket 消息对象
 * 
 *  创建方法：
 *  ```JavaScript
 *  var ws = require("ws");
 * 
 *  var msg = new ws.Message();
 *  ``` 
 */
declare class Class_WebSocketMessage extends Class_Message {
    /**
     * @description 包处理消息对象构造函数
     * 	@param type websocket 消息类型，缺省为 websocket.BINARY
     * 	@param masked websocket 消息掩码，缺省为 true
     *     @param compress 标记消息是否压缩，缺省为 false
     * 	@param maxSize 最大包尺寸，以 MB 为单位，缺省为 67108864(64M)
     * 	 
     */
    constructor(type: number, masked?: boolean, compress?: boolean, maxSize?: number);

    /**
     * @description 查询和读取 websocket 掩码标记，缺省为 true 
     */
    masked: boolean;

    /**
     * @description 查询和读取 websocket 压缩状态，缺省为 false 
     */
    compress: boolean;

    /**
     * @description 查询和设置最大包尺寸，以字节为单位，缺省为 67108864(64M) 
     */
    maxSize: number;

}

