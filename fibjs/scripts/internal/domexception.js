'use strict';

// DOMException implementation following the WHATWG specification:
// https://webidl.spec.whatwg.org/#idl-DOMException

// Map from DOMException name to legacy numeric code
const nameToCode = new Map([
    ['IndexSizeError', 1],
    ['DOMStringSizeError', 2],
    ['HierarchyRequestError', 3],
    ['WrongDocumentError', 4],
    ['InvalidCharacterError', 5],
    ['NoDataAllowedError', 6],
    ['NoModificationAllowedError', 7],
    ['NotFoundError', 8],
    ['NotSupportedError', 9],
    ['InUseAttributeError', 10],
    ['InvalidStateError', 11],
    ['SyntaxError', 12],
    ['InvalidModificationError', 13],
    ['NamespaceError', 14],
    ['InvalidAccessError', 15],
    ['ValidationError', 16],
    ['TypeMismatchError', 17],
    ['SecurityError', 18],
    ['NetworkError', 19],
    ['AbortError', 20],
    ['URLMismatchError', 21],
    ['QuotaExceededError', 22],
    ['TimeoutError', 23],
    ['InvalidNodeTypeError', 24],
    ['DataCloneError', 25],
]);

class DOMException {
    constructor(message = '', name = 'Error') {
        this.name = String(name);
        this.message = String(message);
        // Capture stack trace for debugging
        if (Error.captureStackTrace) {
            Error.captureStackTrace(this, new.target ?? DOMException);
        } else {
            const err = new Error(message);
            this.stack = err.stack;
        }
    }

    get code() {
        return nameToCode.get(this.name) ?? 0;
    }

    get [Symbol.toStringTag]() {
        return 'DOMException';
    }

    // WebIDL stringifier: "name: message" (or just the name for an empty message)
    toString() {
        return this.message ? `${this.name}: ${this.message}` : this.name;
    }
}

// Legacy error code constants (static class properties)
DOMException.INDEX_SIZE_ERR = 1;
DOMException.DOMSTRING_SIZE_ERR = 2;
DOMException.HIERARCHY_REQUEST_ERR = 3;
DOMException.WRONG_DOCUMENT_ERR = 4;
DOMException.INVALID_CHARACTER_ERR = 5;
DOMException.NO_DATA_ALLOWED_ERR = 6;
DOMException.NO_MODIFICATION_ALLOWED_ERR = 7;
DOMException.NOT_FOUND_ERR = 8;
DOMException.NOT_SUPPORTED_ERR = 9;
DOMException.INUSE_ATTRIBUTE_ERR = 10;
DOMException.INVALID_STATE_ERR = 11;
DOMException.SYNTAX_ERR = 12;
DOMException.INVALID_MODIFICATION_ERR = 13;
DOMException.NAMESPACE_ERR = 14;
DOMException.INVALID_ACCESS_ERR = 15;
DOMException.VALIDATION_ERR = 16;
DOMException.TYPE_MISMATCH_ERR = 17;
DOMException.SECURITY_ERR = 18;
DOMException.NETWORK_ERR = 19;
DOMException.ABORT_ERR = 20;
DOMException.URL_MISMATCH_ERR = 21;
DOMException.QUOTA_EXCEEDED_ERR = 22;
DOMException.TIMEOUT_ERR = 23;
DOMException.INVALID_NODE_TYPE_ERR = 24;
DOMException.DATA_CLONE_ERR = 25;

// Register as global (same pattern as webstream.js)
globalThis.DOMException = DOMException;

module.exports = DOMException;
