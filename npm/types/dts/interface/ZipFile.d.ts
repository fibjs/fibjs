/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description zip 文件访问对象 
 */
declare class Class_ZipFile extends Class_object {
    /**
     * @description 获取文件名列表
     * 	 @return 返回包含文件名的列表对象
     * 	 
     */
    namelist(): any[];

    /**
     * @description 获取文件信息列表 
     * 	 文件信息包含字段有：filename, date, compress_type, compress_size, file_size, password, data
     * 	 @return 返回包含文件信息的列表对象
     * 	 
     */
    infolist(): any[];

    /**
     * @description 获取文件信息
     * 	 文件信息包含字段有：filename, date, compress_type, compress_size, file_size, password, data
     * 	 @param member 指定要获取信息的文件名
     * 	 @return 返回文件信息对象
     * 	
     */
    getinfo(member: string): object;

    /**
     * @description 返回从压缩文件读取的数据
     * 	 @param member 指定要读取的文件名
     * 	 @param password 解压密码, 默认没有密码
     * 	 @return 返回文件的所有数据
     * 	 
     */
    read(member: string, password: string): Class_Buffer;

    /**
     * @description 解压所有文件
     * 	 @param password 解压密码, 默认没有密码
     * 	 @return 包含所有文件数据及信息的列表
     *      
     */
    readAll(password: string): any[];

    /**
     * @description 解压指定文件
     * 	 @param member 指定要解压的文件名
     * 	 @param path 指定要解压到的路径
     * 	 @param password 解压密码, 默认没有密码
     * 	 
     */
    extract(member: string, path: string, password: string): void;

    /**
     * @description 解压所有文件到指定路径
     * 	 @param path 指定要解压到的路径
     * 	 @param password 解压密码, 默认没有密码
     * 	 
     */
    extractAll(path: string, password: string): void;

    /**
     * @description 写入指定文件到压缩文件
     * 	 @param filename 指定要写入的文件
     * 	 @param inZipName 压缩在zip文件内的文件名
     * 	 @param password 解压密码, 默认没有密码
     * 	 
     */
    write(filename: string, inZipName: string, password: string): void;

    /**
     * @description 关闭打开的zip文件 
     */
    close(): void;

}

