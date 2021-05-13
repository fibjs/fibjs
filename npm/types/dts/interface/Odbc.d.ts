/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/DbConnection.d.ts" />
/**
 * @description Odbc 数据库连接对象
 */
declare class Class_Odbc extends Class_DbConnection {
    /**
     * @description 查询和设置数据库编码，缺省 "utf8" 
     */
    codec: string;

}

