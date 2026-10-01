/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description The DOMStringMap object represents the key-value mapping of the data-* attributes of an element, commonly used for the dataset property
 *
 *  Reads and writes are synchronized to the data-* attributes of the element in real time: reads convert data-xxx-yyy to a camelCase key,
 *  and writes convert the camelCase key back to the data-* attribute name.
 *
 *  Example:
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

