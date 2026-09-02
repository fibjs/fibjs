/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description DOMStringMap 对象表示元素 data-* 属性的键值映射，常用于 dataset 属性
 * 
 *  读写都会实时同步到元素的 data-* 属性：读取时把 data-xxx-yyy 转换为驼峰键，
 *  写入时把驼峰键转换回 data-* 属性名。
 * 
 *  示例:
 *  ```JavaScript
 *  var doc = new DOMParser().parseFromString('<div id="a" data-user-id="1"></div>', 'text/html');
 *  var el = doc.getElementById('a');
 * 
 *  console.log(el.dataset.userId); // "1"
 *  el.dataset.sourceHash = 'abc';
 *  console.log(el.getAttribute('data-source-hash')); // "abc"
 *  delete el.dataset.sourceHash;
 *  ```
 *  
 */
declare class Class_DOMStringMap extends Class_object {
}

