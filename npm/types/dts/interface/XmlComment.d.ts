/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlCharacterData.d.ts" />
/**
 * @description The XmlComment object represents the content of a comment node in a document
 *
 * An XmlComment node represents a comment in an XML document.
 * The comment content (that is, the text between <!-- and -->) can be accessed with the data property inherited from the XmlCharacterData interface or the nodeValue property inherited from the XmlNode interface. The comment content can be manipulated with the various methods inherited from the XmlCharacterData interface.
 *
 * Use XmlDocument.createComment() to create a comment object.
 *
 */
declare class Class_XmlComment extends Class_XmlCharacterData {
}

