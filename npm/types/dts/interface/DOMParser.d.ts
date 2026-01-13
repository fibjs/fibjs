/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/XmlDocument.d.ts" />
/**
 * @description DOMParser 接口提供将 XML 或 HTML 源代码字符串解析为 DOM Document 的能力
 * 
 *  DOMParser 可以将字符串中的 XML 或 HTML 源代码解析为 DOM 文档：
 *  ```JavaScript
 *  const parser = new DOMParser();
 *  
 *  // 解析 HTML
 *  const htmlDoc = parser.parseFromString('<html><body>Hello</body></html>', 'text/html');
 *  console.log(htmlDoc.body.textContent); // 输出: Hello
 *  
 *  // 解析 XML
 *  const xmlDoc = parser.parseFromString('<root><item>data</item></root>', 'text/xml');
 *  console.log(xmlDoc.documentElement.nodeName); // 输出: root
 *  ```
 * 
 *  支持的 MIME 类型包括：
 *  - text/html - 解析为 HTML 文档
 *  - text/xml - 解析为 XML 文档
 *  - application/xml - 解析为 XML 文档
 *  - application/xhtml+xml - 解析为 XHTML 文档
 *  - image/svg+xml - 解析为 SVG 文档
 * 
 */
declare class Class_DOMParser extends Class_object {
    /**
     * @description 构造一个 DOMParser 对象 
     */
    constructor();

    /**
     * @description 将字符串解析为 DOM 文档
     *      @param string 要解析的 HTML 或 XML 字符串
     *      @param mimeType 指定文本类型，支持 "text/html", "text/xml", "application/xml", "application/xhtml+xml", "image/svg+xml"
     *      @return 返回解析后的 XmlDocument 对象
     *     
     */
    parseFromString(string: string, mimeType: string): Class_XmlDocument;

}

