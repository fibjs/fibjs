/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description CSSStyleDeclaration 对象表示一个 CSS 声明块，常用于元素的 style 属性
 * 
 *  CSSStyleDeclaration 是对元素 style 属性（内联样式）的编程访问接口。
 * 
 *  示例:
 *  ```JavaScript
 *  var doc = new DOMParser().parseFromString('<div></div>', 'text/html');
 *  var el = doc.documentElement;
 * 
 *  // 通过 cssText 批量设置内联样式
 *  el.style.cssText = 'max-width: 100%; height: auto;';
 * 
 *  // 单个属性读写，属性名使用驼峰格式，与浏览器一致
 *  el.style.width = '400px';
 *  console.log(el.style.width);        // "400px"
 *  console.log(el.getAttribute('style')); // "max-width: 100%; height: auto; width: 400px"
 * 
 *  // 使用标准方法操作声明
 *  el.style.setProperty('display', 'none', 'important');
 *  console.log(el.style.getPropertyValue('display')); // "none"
 *  console.log(el.style.getPropertyPriority('display')); // "important"
 *  el.style.removeProperty('display');
 * 
 *  // 自定义属性（CSS 变量）同样受支持
 *  el.style.setProperty('--gap', '8px');
 *  console.log(el.style.getPropertyValue('--gap')); // "8px"
 *  ```
 *  
 */
declare class Class_CSSStyleDeclaration extends Class_object {
    /**
     * @description 查询和设置声明块的文本形式。查询时返回当前 style 属性值；设置时按 CSS 声明块解析并替换全部内容
     *     
     */
    cssText: string;

    /**
     * @description 返回声明块中声明的数量 
     */
    readonly length: number;

    /**
     * @description 返回指定索引处声明的属性名
     *      @param index 声明的索引
     *      @return 返回属性名，如果索引超出范围则返回空字符串
     *      
     */
    item(index: number): string;

    /**
     * @description 查询指定 CSS 属性的值
     *      @param property 指定 CSS 属性名（短横线格式，如 "max-width"），大小写不敏感
     *      @return 返回属性值，如果未设置则返回空字符串
     *      
     */
    getPropertyValue(property: string): string;

    /**
     * @description 查询指定 CSS 属性是否带有 !important 优先级
     *      @param property 指定 CSS 属性名（短横线格式）
     *      @return 如果带有 !important 则返回 "important"，否则返回空字符串
     *      
     */
    getPropertyPriority(property: string): string;

    /**
     * @description 设置 CSS 属性值
     * 
     *      设置已存在的属性时原位替换其值，新属性追加到声明块末尾。value 为空时等价于删除该属性。
     *      @param property 指定 CSS 属性名（短横线格式），也支持以 "--" 开头的自定义属性
     *      @param value 指定 CSS 属性值，为空时删除该属性
     *      @param priority 指定优先级，可设为 "important" 表示 !important，缺省为空
     *      
     */
    setProperty(property: string, value: string, priority?: string): void;

    /**
     * @description 删除指定 CSS 属性
     *      @param property 指定要删除的 CSS 属性名（短横线格式）
     *      @return 返回被删除的属性值，如果属性不存在则返回空字符串
     *      
     */
    removeProperty(property: string): string;

}

