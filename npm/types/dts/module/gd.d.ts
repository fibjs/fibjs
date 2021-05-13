/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Image.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description 图像文件处理模块
 * 
 *  基础模块。可用于创建和操作图像文件，引用方式：
 *  ```JavaScript
 *  var gd = require('gd');
 *  ```
 *  
 */
declare module 'gd' {
    /**
     * @description 图像格式常量，标示当前图像来源为未知 
     */
    export const NONE: 0;

    /**
     * @description 图像格式常量，标示当前图像来源为 jpeg 格式数据 
     */
    export const JPEG: 1;

    /**
     * @description 图像格式常量，标示当前图像来源为 gif 格式数据 
     */
    export const GIF: 2;

    /**
     * @description 图像格式常量，标示当前图像来源为 png 格式数据 
     */
    export const PNG: 3;

    /**
     * @description 图像格式常量，标示当前图像来源为 tiff 格式数据 
     */
    export const TIFF: 4;

    /**
     * @description 图像格式常量，标示当前图像来源为 bmp 格式数据 
     */
    export const BMP: 5;

    /**
     * @description 图像格式常量，标示当前图像来源为 webp 格式数据 
     */
    export const WEBP: 6;

    /**
     * @description 图像类型常量，标示当前图像为真彩色图像 
     */
    export const TRUECOLOR: 0;

    /**
     * @description 图像类型常量，标示当前图像为调色板图像 
     */
    export const PALETTE: 1;

    /**
     * @description 扇形绘制样式，绘制一条连接开始和结束点的圆弧 
     */
    export const ARC: 0;

    /**
     * @description 扇形绘制样式，绘制一条连接原点，开始和结束点的直线 
     */
    export const CHORD: 1;

    /**
     * @description 扇形绘制样式，绘制不填充的扇形 
     */
    export const NOFILL: 2;

    /**
     * @description 扇形绘制样式，绘制一条连接起点和终点的弧和连接原点的直线 
     */
    export const EDGED: 4;

    /**
     * @description 镜像方向，横向做镜像处理 
     */
    export const HORIZONTAL: 1;

    /**
     * @description 镜像方向，纵向做镜像处理 
     */
    export const VERTICAL: 2;

    /**
     * @description 镜像方向，横向和纵向都做镜像处理 
     */
    export const BOTH: 3;

    /**
     * @description 旋转方向，向左旋转 
     */
    export const LEFT: 1;

    /**
     * @description 旋转方向，向右旋转 
     */
    export const RIGHT: 2;

    /**
     * @description 滤波器类型：用平均移除法来达到轮廓效果
     */
    export const MEAN_REMOVAL: 0;

    /**
     * @description 滤波器类型：用边缘检测来突出图像的边缘 
     */
    export const EDGEDETECT: 1;

    /**
     * @description 滤波器类型：使图像浮雕化 
     */
    export const EMBOSS: 2;

    /**
     * @description 滤波器类型：模糊图像 
     */
    export const SELECTIVE_BLUR: 3;

    /**
     * @description 滤波器类型：用高斯算法模糊图像 
     */
    export const GAUSSIAN_BLUR: 4;

    /**
     * @description 滤波器类型：将图像中所有颜色反转 
     */
    export const NEGATE: 5;

    /**
     * @description 滤波器类型：将图像转换为灰度图 
     */
    export const GRAYSCALE: 6;

    /**
     * @description 滤波器类型：使图像更柔滑，用arg1设定柔滑级别 
     */
    export const SMOOTH: 7;

    /**
     * @description 滤波器类型：改变图像的亮度，用arg1设定亮度级别，取值范围是-255~255 
     */
    export const BRIGHTNESS: 8;

    /**
     * @description 滤波器类型：改变图像的对比度，用arg1设定对比度级别，取值范围是0~100 
     */
    export const CONTRAST: 9;

    /**
     * @description 滤波器类型：改变图像的色调，用arg1、arg2、arg3分别指定red、blue、green，每种颜色范围是0~255，arg4为透明度，取值返回是0~127 
     */
    export const COLORIZE: 10;

    /**
     * @description 创建一个新图像
     *      @param width 指定图像宽度
     *      @param height 指定图像高度
     *      @param color 指定图像类型，允许值为 gd.TRUECOLOR 或 gd.PALETTE
     *      @return 返回创建成功的图像对象
     *      
     */
    function create(width: number, height: number, color: number): Class_Image;

    function create(width: number, height: number, color: number, callback: (err: Error | undefined | null, retVal: Class_Image)=>any): void;

    /**
     * @description 从格式数据中解码图像
     *      @param data 给定解码的图像数据
     *      @return 返回解码成功的图像对象
     *      
     */
    function load(data: Class_Buffer): Class_Image;

    function load(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Image)=>any): void;

    /**
     * @description 从流对象中解码图像
     *      @param stm 给定图像数据所在的流对象
     *      @return 返回解码成功的图像对象
     *      
     */
    function load(stm: Class_SeekableStream): Class_Image;

    function load(stm: Class_SeekableStream, callback: (err: Error | undefined | null, retVal: Class_Image)=>any): void;

    /**
     * @description 从指定文件中解码图像
     *      @param fname 指定文件名
     *      @return 返回解码成功的图像对象
     *      
     */
    function load(fname: string): Class_Image;

    function load(fname: string, callback: (err: Error | undefined | null, retVal: Class_Image)=>any): void;

    /**
     * @description 通过 rgb 颜色分量生成组合颜色
     *      @param red 红色分量，范围为 0-255
     *      @param green 绿色分量，范围为 0-255
     *      @param blue 蓝色分量，范围为 0-255
     *      @return 返回组合颜色
     *      
     */
    function rgb(red: number, green: number, blue: number): number;

    /**
     * @description 通过 rgba 颜色分量生成组合颜色
     *      @param red 红色分量，范围为 0-255
     *      @param green 绿色分量，范围为 0-255
     *      @param blue 蓝色分量，范围为 0-255
     *      @param alpha 透明分量，范围为 0.0-1.0
     *      @return 返回组合颜色
     *      
     */
    function rgba(red: number, green: number, blue: number, alpha: number): number;

    /**
     * @description 通过 hsl 颜色分量生成组合颜色
     *      @param hue 色相分量，范围为 0-360
     *      @param saturation 饱和度分量，范围为 0.0-1.0
     *      @param lightness 亮度分量，范围为 0.0-1.0
     *      @return 返回组合颜色
     *      
     */
    function hsl(hue: number, saturation: number, lightness: number): number;

    /**
     * @description 通过 hsla 颜色分量生成组合颜色
     *      @param hue 色相分量，范围为 0-360
     *      @param saturation 饱和度分量，范围为 0.0-1.0
     *      @param lightness 亮度分量，范围为 0.0-1.0
     *      @param alpha 透明分量，范围为 0.0-1.0
     *      @return 返回组合颜色
     *      
     */
    function hsla(hue: number, saturation: number, lightness: number, alpha: number): number;

    /**
     * @description 通过 hsb 颜色分量生成组合颜色
     *      @param hue 色相分量，范围为 0-360
     *      @param saturation 饱和度分量，范围为 0.0-1.0
     *      @param brightness 明亮程度分量，范围为 0.0-1.0
     *      @return 返回组合颜色
     *      
     */
    function hsb(hue: number, saturation: number, brightness: number): number;

    /**
     * @description 通过 hsba 颜色分量生成组合颜色
     *      @param hue 色相分量，范围为 0-360
     *      @param saturation 饱和度分量，范围为 0.0-1.0
     *      @param brightness 明亮程度分量，范围为 0.0-1.0
     *      @param alpha 透明分量，范围为 0.0-1.0
     *      @return 返回组合颜色
     *      
     */
    function hsba(hue: number, saturation: number, brightness: number, alpha: number): number;

    /**
     * @description 通过字符串生成组合颜色
     *      @param color 指定颜色的字符串，如："#ff0000", "ff0000", "#f00", "f00"
     *      @return 返回组合颜色
     *      
     */
    function color(color: string): number;

}

