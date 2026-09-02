/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/// <reference path="../interface/DOMTokenList.d.ts" />
/// <reference path="../interface/DOMStringMap.d.ts" />
/// <reference path="../interface/CSSStyleDeclaration.d.ts" />
/// <reference path="../interface/XmlDocumentFragment.d.ts" />
/// <reference path="../interface/XmlNamedNodeMap.d.ts" />
/// <reference path="../interface/XmlAttr.d.ts" />
/// <reference path="../interface/XmlNodeList.d.ts" />
/**
 * @description XmlElement 对象表示 XML 文档中的元素
 * 
 */
declare class Class_XmlElement extends Class_XmlNode {
    /**
     * @description 查询元素的命名空间的 URI。如果选定的节点无命名空间，则该属性返回 NULL
     *     
     */
    readonly namespaceURI: string;

    /**
     * @description 查询和设置元素的命名空间前缀。如果选定的节点无命名空间，则该属性返回 NULL
     *     
     */
    prefix: string;

    /**
     * @description 查询元素的本地名称。如果选定的节点无命名空间，则该属性等同于 nodeName
     *     
     */
    readonly localName: string;

    /**
     * @description 返回元素的标签名
     *     
     */
    readonly tagName: string;

    /**
     * ! 查询和设置元素的 id 属性
     *     
     */
    id: string;

    /**
     * ! 查询和设置元素的 src 属性，仅在 html 模式有效。读取时返回 style 之外的常规属性值，写入时同步到属性，空字符串删除属性
     *     
     */
    src: string;

    /**
     * ! 查询和设置元素的 alt 属性，仅在 html 模式有效
     *     
     */
    alt: string;

    /**
     * ! 查询和设置元素的 href 属性，仅在 html 模式有效
     *     
     */
    href: string;

    /**
     * ! 查询和设置元素的 title 属性，仅在 html 模式有效
     *     
     */
    title: string;

    /**
     * ! 查询和设置元素的 value 属性，仅在 html 模式有效
     *     
     */
    value: string;

    /**
     * ! 查询和设置元素的 name 属性，仅在 html 模式有效
     *     
     */
    name: string;

    /**
     * ! 查询和设置元素的 type 属性，仅在 html 模式有效
     *     
     */
    type: string;

    /**
     * ! 查询和设置元素的 rel 属性，仅在 html 模式有效
     *     
     */
    rel: string;

    /**
     * ! 查询和设置元素的 target 属性，仅在 html 模式有效
     *     
     */
    target: string;

    /**
     * ! 查询和设置元素的 placeholder 属性，仅在 html 模式有效
     *     
     */
    placeholder: string;

    /**
     * ! 查询和设置选定元素后代的 HTML 文本，仅在 html 模式有效。查询时，返回元素节点内所有子节点的 HTML 编码；设置时，删除所有子节点，并用指定的 HTML 解码后替换它们。
     *     
     */
    innerHTML: string;

    /**
     * ! 查询选定元素及其后代的 HTML 文本，仅在 html 模式有效。查询时，返回元素及节点内所有子节点的 HTML 编码。
     *     
     */
    outerHTML: string;

    /**
     * ! 查询和设置元素的 class 属性，仅在 html 模式有效
     *     
     */
    className: string;

    /**
     * @description 返回一个 DOMTokenList 对象，包含元素的 class 属性的标记列表，仅在 html 模式有效
     *     
     */
    readonly classList: Class_DOMTokenList;

    /**
     * @description 返回一个 DOMStringMap 对象，包含元素所有 data-* 属性的键值对，仅在 html 模式有效。属性名会从 data-xxx-yyy 格式转换为 xxxYyy 驼峰格式，对对象的读写会实时同步到元素的 data-* 属性
     *     
     */
    readonly dataset: Class_DOMStringMap;

    /**
     * @description 返回元素的 style 属性对应的 CSSStyleDeclaration 对象，仅在 html 模式有效
     * 
     *     通过该对象可以读取或修改元素的内联样式，修改结果会同步到元素的 style 属性，并反映在 outerHTML/innerHTML 序列化中。CSS 属性名使用驼峰格式，例如 style.maxWidth 对应 max-width。
     *     
     */
    readonly style: Class_CSSStyleDeclaration;

    /**
     * @description 返回 template 元素的内容，仅对 template 元素有效，返回一个包含其子节点的 DocumentFragment
     *     
     */
    readonly content: Class_XmlDocumentFragment;

    /**
     * @description 返回包含被选节点属性的 NamedNodeMap。如果被选节点不是元素，则该属性返回 NULL。
     *     
     */
    readonly attributes: Class_XmlNamedNodeMap;

    /**
     * @description 查询当前元素是否拥有任何属性
     *      @return 如果当前元素拥有属性则返回 true，否则返回 false
     *     
     */
    hasAttributes(): boolean;

    /**
     * @description 通过名称查询属性的值
     *      @param name 指定查询的属性名
     *      @return 返回属性的值
     *     
     */
    getAttribute(name: string): string;

    /**
     * @description 通过命名空间 URI 和名称来获取属性值
     *      @param namespaceURI 指定查询的命名空间 URI
     *      @param localName 指定查询的属性名
     *      @return 返回属性的值
     *     
     */
    getAttributeNS(namespaceURI: string, localName: string): string;

    /**
     * @description 返回指定名称的属性节点
     * 
     *      该方法返回一个 XmlAttr 对象，表示当前元素的指定名称的属性。如果没有具有指定名称的属性，则返回 NULL。
     *      @param name 指定查询的属性名
     *      @return 返回指定名称的 XmlAttr 对象，如果没有具有指定名称的属性，则返回 NULL
     *     
     */
    getAttributeNode(name: string): Class_XmlAttr;

    /**
     * @description 返回具有命名空间 URI 和名称的属性节点
     * 
     *      该方法返回一个 XmlAttr 对象，表示当前元素的指定命名空间 URI 和名称的属性。如果没有具有指定名称的属性，则返回 NULL。
     *      @param namespaceURI 指定查询的命名空间 URI
     *      @param localName 指定查询的属性名
     *      @return 返回指定名称的 XmlAttr 对象，如果没有具有指定名称的属性，则返回 NULL
     *     
     */
    getAttributeNodeNS(namespaceURI: string, localName: string): Class_XmlAttr;

    /**
     * @description 创建或改变某个新属性
     * 
     *      该方法把指定的属性设置为指定的值。如果不存在具有指定名称的属性，该方法将创建一个新属性
     *      @param name 指定要设置的属性名
     *      @param value 指定要设置的属性值
     *     
     */
    setAttribute(name: string, value: string): void;

    /**
     * @description 创建或改变具有命名空间的属性
     * 
     *      该方法与 setAttribute 方法类似，只是要创建或设置的属性由命名空间 URI 和限定名（由名字空间前缀、冒号和名字空间中的本地名构成）共同指定。除了可以改变一个属性的值以外，使用该方法还可以改变属性的名字空间前缀
     *      @param namespaceURI 指定要设置的命名空间 URI
     *      @param qualifiedName 指定要设置的属性名
     *      @param value 指定要设置的属性值
     *     
     */
    setAttributeNS(namespaceURI: string, qualifiedName: string, value: string): void;

    /**
     * @description 设置指定的属性对象
     * 
     *      该方法将指定的 XmlAttr 对象设置为当前元素的属性。如果当前元素已经有同名的属性，则该方法将替换它
     *      @param attr 指定要设置的 XmlAttr 对象
     *      @return 返回被替换的 XmlAttr 对象，如果没有被替换则返回 NULL
     *     
     */
    setAttributeNode(attr: Class_XmlAttr): Class_XmlAttr;

    /**
     * @description 通过名称删除指定的属性
     *      @param name 指定删除的属性名
     *     
     */
    removeAttribute(name: string): void;

    /**
     * @description 通过命名空间和名称删除指定的属性
     *      @param namespaceURI 指定要删除的命名空间 URI
     *      @param localName 指定删除的属性名
     *     
     */
    removeAttributeNS(namespaceURI: string, localName: string): void;

    /**
     * @description 删除指定的属性节点
     * 
     *      该方法将从当前元素的属性列表中删除指定的 XmlAttr 对象。如果当前元素没有指定的属性，则该方法无效
     *      @param attr 指定要删除的 XmlAttr 对象
     *      @return 返回被删除的 XmlAttr 对象，如果没有被删除则返回 NULL
     *     
     */
    removeAttributeNode(attr: Class_XmlAttr): Class_XmlAttr;

    /**
     * @description 查询当前节点是否拥有指定名称的属性
     *      @param name 指定查询的属性名称
     *      @return 如果当前元素节点拥有指定属性，则返回 true，否则返回 false
     *     
     */
    hasAttribute(name: string): boolean;

    /**
     * @description 查询当前节点是否拥有指定命名空间和名称的属性
     *      @param namespaceURI 指定要查询的命名空间 URI
     *      @param localName 指定查询的属性名称
     *      @return 如果当前元素节点拥有指定属性，则返回 true，否则返回 false
     *     
     */
    hasAttributeNS(namespaceURI: string, localName: string): boolean;

    /**
     * @description 返回拥有指定名称的所有元素的 XmlNodeList
     * 
     *      该方法将遍历指定元素的子孙节点，返回一个 XmlElement 节点的 XmlNodeList 对象，表示所有具有指定标签名的文档元素。元素在返回的数组中的顺序就是它们出现在文档源代码中的顺序。
     * 
     *      XmlDocument 接口也定义了 getElementsByTagName 方法，它与该方法相似，但遍历整个文档，而不是遍历某个元素的子孙节点。
     *      @param tagName 需检索的标签名。值 "*" 匹配所有的标签
     *      @return 节点树中具有指定标记的 XmlElement 节点的 XmlNodeList 集合。返回的元素节点的顺序就是它们在源文档中出现的顺序。
     *     
     */
    getElementsByTagName(tagName: string): Class_XmlNodeList;

    /**
     * @description 返回拥有指定命名空间和名称的所有元素的 XmlNodeList
     * 
     *      该方法与 getElementsByTagName 方法相似，只是想获取的元素的标记名被指定为命名空间 URI 和在命名空间中定义的本地名的组合。
     *      @param namespaceURI 指定要查询的命名空间 URI
     *      @param localName 需检索的标签名。值 "*" 匹配所有的标签
     *      @return 节点树中具有指定标记的 XmlElement 节点的 XmlNodeList 集合。返回的元素节点的顺序就是它们在源文档中出现的顺序。
     *     
     */
    getElementsByTagNameNS(namespaceURI: string, localName: string): Class_XmlNodeList;

    /**
     * @description 返回拥有指定 id 属性的元素
     * 
     *      该方法将遍历指定元素的子孙节点，返回一个 XmlElement 节点对象，表示第一个具有指定 id 属性的文档元素。。
     * 
     *      XmlDocument 接口也定义了 getElementsByTagName 方法，它与该方法相似，但遍历整个文档，而不是遍历某个元素的子孙节点。
     *      @param id 需检索的 id
     *      @return 节点树中具有指定 id 属性的 XmlElement 节点
     *     
     */
    getElementById(id: string): Class_XmlElement;

    /**
     * @description 返回带有指定 class 名称的所有元素的一个节点列表
     * 
     *      该方法将返回一个 XmlNodeList 对象（可以作为只读数组处理），该对象存放文档中具有指定 class 名的所有 XmlElement 节点，它们存放的顺序就是在源文档中出现的顺序。 XmlNodeList 对象是“活”的，即如果在文档中添加或删除了指定标签名的元素，它的内容会自动进行必要的更新。
     *      @param className 需检索的 class 名称
     *      @return 文档树中具有指定 class 名的 XmlElement 节点的 XmlNodeList 集合。返回的元素节点的顺序就是它们在源文档中出现的顺序。
     *     
     */
    getElementsByClassName(className: string): Class_XmlNodeList;

    /**
     * @description 返回符合指定 CSS 选择器的元素的 XmlNodeList
     * 
     *      该方法将返回一个 XmlNodeList 对象（可以作为只读数组处理），该对象存放文档中符合指定 CSS 选择器的所有 XmlElement 节点，它们存放的顺序就是在源文档中出现的顺序。 XmlNodeList 对象是“活”的，即如果在文档中添加或删除了符合指定选择器的元素，它的内容会自动进行必要的更新。
     *      @param selectors 指定 CSS 选择器
     *      @return 符合指定 CSS 选择器的 XmlElement 节点
     *     
     */
    querySelector(selectors: string): Class_XmlElement;

    /**
     * @description 返回符合指定 CSS 选择器的所有元素的 XmlNodeList
     * 
     *      该方法将返回一个 XmlNodeList 对象（可以作为只读数组处理），该对象存放文档中符合指定 CSS 选择器的所有 XmlElement 节点，它们存放的顺序就是在源文档中出现的顺序。 XmlNodeList 对象是“活”的，即如果在文档中添加或删除了符合指定选择器的元素，它的内容会自动进行必要的更新。
     *      @param selectors 指定 CSS 选择器
     *      @return 符合指定 CSS 选择器的 XmlElement 节点的 XmlNodeList 集合。返回的元素节点的顺序就是它们在源文档中出现的顺序。
     *     
     */
    querySelectorAll(selectors: string): Class_XmlNodeList;

    /**
     * @description 查询当前元素是否匹配指定的 CSS 选择器
     *      @param selectors 指定 CSS 选择器
     *      @return 如果当前元素匹配指定选择器，则返回 true，否则返回 false
     *     
     */
    matches(selectors: string): boolean;

    /**
     * @description 向上查找匹配指定 CSS 选择器的祖先元素
     *      @param selectors 指定 CSS 选择器
     *      @return 返回匹配的最近祖先元素，如果没有匹配则返回 null
     *     
     */
    closest(selectors: string): Class_XmlElement;

    /**
     * @description 在当前元素的子节点末尾添加一个或多个节点
     * 
     *      该方法将指定的节点添加到当前元素的子节点列表末尾。字符串参数会自动转换为文本节点。
     *      @param nodes 要添加的一个或多个节点，可以是节点对象或字符串
     *     
     */
    append(...nodes: any[]): void;

    /**
     * @description 在当前元素的子节点开头添加一个或多个节点
     * 
     *      该方法将指定的节点添加到当前元素的子节点列表开头。字符串参数会自动转换为文本节点。
     *      @param nodes 要添加的一个或多个节点，可以是节点对象或字符串
     *     
     */
    prepend(...nodes: any[]): void;

    /**
     * @description 替换当前元素的所有子节点
     * 
     *      该方法将当前元素的所有子节点替换为指定的节点。字符串参数会自动转换为文本节点。如果不传入任何参数，则清空所有子节点。
     *      @param nodes 要设置的一个或多个节点，可以是节点对象或字符串
     *     
     */
    replaceChildren(...nodes: any[]): void;

    /**
     * @description 在指定位置插入一个元素节点
     * 
     *      position 参数可以是以下值之一：
     *      - 'beforebegin': 在当前元素之前插入
     *      - 'afterbegin': 在当前元素的第一个子节点之前插入
     *      - 'beforeend': 在当前元素的最后一个子节点之后插入
     *      - 'afterend': 在当前元素之后插入
     * 
     *      @param position 指定插入位置
     *      @param element 要插入的元素节点
     *      @return 返回插入的元素，如果插入失败则返回 null
     *     
     */
    insertAdjacentElement(position: string, element: Class_XmlElement): Class_XmlElement;

    /**
     * @description 在指定位置插入 HTML 文本
     * 
     *      position 参数可以是以下值之一：
     *      - 'beforebegin': 在当前元素之前插入
     *      - 'afterbegin': 在当前元素的第一个子节点之前插入
     *      - 'beforeend': 在当前元素的最后一个子节点之后插入
     *      - 'afterend': 在当前元素之后插入
     * 
     *      @param position 指定插入位置
     *      @param html 要插入的 HTML 文本
     *     
     */
    insertAdjacentHTML(position: string, html: string): void;

    /**
     * @description 在指定位置插入文本节点
     * 
     *      position 参数可以是以下值之一：
     *      - 'beforebegin': 在当前元素之前插入
     *      - 'afterbegin': 在当前元素的第一个子节点之前插入
     *      - 'beforeend': 在当前元素的最后一个子节点之后插入
     *      - 'afterend': 在当前元素之后插入
     * 
     *      @param position 指定插入位置
     *      @param text 要插入的文本
     *     
     */
    insertAdjacentText(position: string, text: string): void;

    /**
     * @description 切换元素上的布尔属性
     * 
     *      如果属性存在则移除，如果不存在则添加。
     *      @param name 要切换的属性名称
     *      @return 如果操作后属性存在则返回 true，否则返回 false
     *     
     */
    toggleAttribute(name: string): boolean;

    /**
     * @description 切换元素上的布尔属性
     * 
     *      根据 force 参数强制添加或移除属性。
     *      @param name 要切换的属性名称
     *      @param force 如果为 true 则强制添加属性，如果为 false 则强制移除属性
     *      @return 如果操作后属性存在则返回 true，否则返回 false
     *     
     */
    toggleAttribute(name: string, force: boolean): boolean;

}

