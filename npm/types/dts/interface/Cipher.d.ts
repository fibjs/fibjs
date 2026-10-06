/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Symmetric cipher object that transforms a byte stream with a secret key
 *
 *  A Cipher is the object behind the crypto.createCipheriv family: an encrypting
 *  instance is created by `createCipheriv`/`createCipher`, a decrypting one by
 *  `createDecipheriv`/`createDecipher`. Both sides of a conversation use the same
 *  algorithm, key and IV; the sender feeds the plaintext through a Cipher and the
 *  receiver feeds the ciphertext through a Decipher. Instances are stateful and
 *  single-use: call `update` any number of times and `final` exactly once.
 *
 *  Concepts:
 *  - **Modes and IV**: block ciphers (AES, SM4) need a mode such as 'cbc', 'ctr' or
 *    'gcm' and, for almost every mode, an unpredictable IV of the size reported by
 *    crypto.getCipherInfo. Reusing an IV with the same key destroys the security
 *    guarantees, especially in CTR and GCM; generate it with crypto.randomBytes and
 *    send it next to the ciphertext. ECB uses no IV: pass an empty Buffer.
 *  - **Pipeline and state**: `update` returns the transformed bytes, not the Cipher,
 *    so it cannot be chained; `setAAD`, `setAuthTag` and `setAutoPadding` return the
 *    Cipher and can be chained. `final` produces the last block. The object does not
 *    track its state strictly: after `final` the behavior is unspecified, so drop the
 *    instance instead of reusing it, as Node.js does.
 *  - **Padding**: block modes require the plaintext to be a multiple of the block
 *    size. fibjs pads the last block automatically (PKCS#7) and removes the padding
 *    when decrypting; call `setAutoPadding(false)` when the data is already aligned or
 *    the protocol defines its own padding, then feed exactly the same length.
 *  - **AEAD**: GCM, CCM, OCB and ChaCha20-Poly1305 authenticate the ciphertext with a
 *    tag. Encrypting: `setAAD`, `update`/`final`, then `getAuthTag`. Decrypting:
 *    `setAAD` with the same data, `setAuthTag`, then `update`/`final`; a mismatch makes
 *    `final` throw. The tag is 16 bytes by default; the `authTagLength` option selects
 *    another valid length at creation time (required for CCM and OCB; GCM accepts 4, 8
 *    and 12 to 16 bytes). fibjs does not require you to provide a tag: forgetting
 *    `setAuthTag` on a decipher returns unauthenticated plaintext, so always set it.
 *  - **Output encoding**: `update`, `final` and the ciphertext forms return a Buffer
 *    by default. With an output encoding they return a string produced by a
 *    StringDecoder, so multi-byte characters and partial blocks survive across calls;
 *    once a string encoding is used, every later call must use the same one and
 *    requesting a Buffer afterwards throws "Inconsistent encoding".
 *  - **Keys**: the key may be a Buffer, a string (always decoded as utf8) or a secret
 *    KeyObject; any other KeyObject throws "Invalid key type".
 *
 *  Obtained from:
 *  - `crypto.createCipheriv(algorithm, key, iv[, options])` — encrypt with an explicit
 *    key and IV;
 *  - `crypto.createDecipheriv(algorithm, key, iv[, options])` — the decrypting
 *    counterpart;
 *  - `crypto.createCipher(algorithm, password[, options])` and
 *    `crypto.createDecipher(algorithm, password[, options])` — legacy password-derived
 *    keys, kept for compatibility only.
 *
 *  Example 1 — AES-256-CBC with a fixed key and IV:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  // Fixed key and IV only to keep this example deterministic.
 *  const key = Buffer.from('0123456789abcdef0123456789abcdef');
 *  const iv = Buffer.from('0123456789abcdef');
 *
 *  const cipher = crypto.createCipheriv('aes-256-cbc', key, iv);
 *  const ciphertext = Buffer.concat([cipher.update('The quick brown fox'), cipher.final()]);
 *  console.log(ciphertext.toString('hex'));
 *  // 3fb71aa5df5cb45088bb79102524e2abc7078c035faedf007eb212d98060f5f8
 *
 *  const decipher = crypto.createDecipheriv('aes-256-cbc', key, iv);
 *  const plaintext = Buffer.concat([decipher.update(ciphertext), decipher.final()]);
 *  console.log(plaintext.toString()); // The quick brown fox
 *  ```
 *
 *  Example 2 — AES-256-GCM with additional authenticated data and a tag:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  const key = Buffer.alloc(32, 7); // 32-byte key
 *  const iv = Buffer.alloc(12, 3);  // 12-byte nonce, unique per message in real code
 *
 *  const cipher = crypto.createCipheriv('aes-256-gcm', key, iv);
 *  cipher.setAAD(Buffer.from('v1')); // authenticated but not encrypted
 *  const ciphertext = Buffer.concat([cipher.update('secret message'), cipher.final()]);
 *  const tag = cipher.getAuthTag();
 *  console.log(tag.length); // 16
 *
 *  const decipher = crypto.createDecipheriv('aes-256-gcm', key, iv);
 *  decipher.setAAD(Buffer.from('v1'));
 *  decipher.setAuthTag(tag);
 *  console.log(Buffer.concat([decipher.update(ciphertext), decipher.final()]).toString());
 *  // secret message
 *  ```
 *
 *  Example 3 — streaming updates with a string output encoding:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  const key = Buffer.from('0123456789abcdef0123456789abcdef');
 *  const iv = Buffer.from('0123456789abcdef');
 *
 *  const cipher = crypto.createCipheriv('aes-256-cbc', key, iv);
 *  // Nothing is emitted until a full block is available; concatenate the parts.
 *  const first = cipher.update('hello', 'utf8', 'hex');
 *  const second = cipher.update('world', 'utf8', 'hex');
 *  const last = cipher.final('hex');
 *  console.log(first + second + last);
 *  // c69358c9b5fa6ca34727b9175610fc25
 *  ```
 *
 */
declare class Class_Cipher extends Class_object {
    /**
     * @description Supplies the authentication tag when decrypting an AEAD cipher
     *
     *      Valid only on a decrypting Cipher in an authenticated mode, only once, and
     *      before final(). The buffer form takes the bytes as they are; a string is decoded
     *      with encoding (default "utf8"). The tag length must match the authTagLength
     *      option given to crypto.createDecipheriv (16 by default for GCM and
     *      chacha20-poly1305). Calling it on an encrypting Cipher, twice, or with a wrong
     *      length throws "Invalid setAuthTag"/"Invalid authTagLength"; Node.js enforces the
     *      same rules with its own error codes.
     *
     *      Example: the tag and AAD together authenticate the ciphertext:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const key = Buffer.alloc(32, 7);
     *      const iv = Buffer.alloc(12, 3);
     *      const cipher = crypto.createCipheriv('aes-256-gcm', key, iv);
     *      cipher.setAAD(Buffer.from('v1'));
     *      const ciphertext = Buffer.concat([cipher.update('secret message'), cipher.final()]);
     *      const tag = cipher.getAuthTag();
     *
     *      const decipher = crypto.createDecipheriv('aes-256-gcm', key, iv);
     *      decipher.setAAD(Buffer.from('v1'));
     *      decipher.setAuthTag(tag);
     *      console.log(Buffer.concat([decipher.update(ciphertext), decipher.final()]).toString());
     *      // secret message
     *      ```
     *
     *      @param buffer the authentication tag data
     *      @param encoding the encoding of a string authentication tag data, default "utf8"
     *      @return returns the current Cipher object
     *
     */
    setAuthTag(buffer: Class_Buffer | string, encoding?: string): Class_Cipher;

    /**
     * @description Returns the authentication tag produced by an AEAD encryption
     *
     *      Valid only after final() on an encrypting Cipher in an authenticated mode. The
     *      returned Buffer holds authTagLength bytes (16 unless another length was selected
     *      at creation time). Calling it before final(), on a decrypting Cipher, or on a
     *      non-AEAD algorithm throws "Invalid authTag". The tag is not secret but must be
     *      transmitted with the ciphertext and checked by the receiver; a missing tag on
     *      the decrypting side is not detected, so treat the tag as mandatory.
     *
     *      @return returns the authentication tag data
     *
     */
    getAuthTag(): Class_Buffer;

    /**
     * @description Supplies the additional authenticated data (AAD) of an AEAD cipher
     *
     *      Valid only in authenticated modes and, on both the encrypting and the decrypting
     *      side, before the first update(). The same AAD must be given on both sides: it is
     *      authenticated (any change makes final() throw) but not encrypted. A string is
     *      decoded with options.encoding (default "utf8"); the buffer form takes the bytes
     *      as they are. In CCM mode setAAD() requires options.plaintextLength, the byte
     *      length of the plaintext, on both sides. Calling it on a non-AEAD Cipher or too
     *      late throws "Invalid setAAD" (Node.js reports ERR_CRYPTO_INVALID_STATE).
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options accepted by setAAD
     *      ({
     *          "encoding": "utf8",     // how a string buffer is decoded
     *          "plaintextLength": -1   // CCM only: plaintext byte length, required
     *      })
     *      ```
     *
     *      @param buffer the additional authenticated data
     *      @param options the additional authenticated data options to use
     *      @return returns the current Cipher object
     *
     */
    setAAD(buffer: Class_Buffer | string, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Enables or disables automatic PKCS#7 padding
     *
     *      Enabled by default. Leave it on for block modes when the data is not aligned to
     *      the block size; call setAutoPadding(false) when the protocol defines its own
     *      padding and pass whole blocks. With padding disabled, final() throws when the
     *      accumulated length is not a multiple of the block size. It must be called before
     *      final() and works on both encrypting and decrypting Ciphers; Node.js provides
     *      the same switch.
     *
     *      Example: aligned data with padding disabled round-trips exactly:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const key = Buffer.alloc(32, 1);
     *      const iv = Buffer.alloc(16, 2);
     *      const cipher = crypto.createCipheriv('aes-256-cbc', key, iv);
     *      cipher.setAutoPadding(false);
     *      const ciphertext = Buffer.concat([cipher.update(Buffer.alloc(16, 5)), cipher.final()]);
     *      console.log(ciphertext.length); // 16
     *
     *      const decipher = crypto.createDecipheriv('aes-256-cbc', key, iv);
     *      decipher.setAutoPadding(false);
     *      const plaintext = Buffer.concat([decipher.update(ciphertext), decipher.final()]);
     *      console.log(plaintext.equals(Buffer.alloc(16, 5))); // true
     *      ```
     *
     *      @param autoPadding specifies whether to pad automatically
     *      @return returns the current Cipher object
     *
     */
    setAutoPadding(autoPadding?: boolean): Class_Cipher;

    /**
     * @description Transforms the next part of the data stream
     *
     *      data may be a Buffer or a string decoded with inputEncoding (default "utf8"); a
     *      Buffer ignores inputEncoding. The return value is the transformed data, not the
     *      Cipher: with outputEncoding "buffer" (default) a Buffer, otherwise a string in
     *      that encoding. The string form goes through a StringDecoder, so a multi-byte
     *      character or a partial block may be buffered and returned by a later call. Once
     *      a string output encoding is used, all later update()/final() calls must use the
     *      same encoding, otherwise "Inconsistent encoding" is thrown. In CCM mode the
     *      message length is bounded by the mode (about 2^24-1 bytes); the bound depends on
     *      the IV length. Calling update() after final() is not supported and may throw an
     *      OpenSSL error; Node.js raises ERR_CRYPTO_INVALID_STATE instead.
     *
     *      @param data the data to update
     *      @param inputEncoding the encoding of the input data, default "utf8"
     *      @param outputEncoding the encoding of the output data
     *      @return returns the updated data
     *
     */
    update(data: Class_Buffer | string, inputEncoding?: string, outputEncoding?: string): any;

    /**
     * @description Finalizes the stream and returns the last transformed block
     *
     *      Writes the pending padding (when enabled) or the buffered block and closes the
     *      Cipher; call it exactly once. In an authenticated mode, encryption final()
     *      computes the tag later returned by getAuthTag(), while decryption final()
     *      verifies the tag supplied with setAuthTag and throws when it does not match (the
     *      message comes from OpenSSL). For CCM decryption the authentication result is
     *      reported here even when update() already returned the plaintext. outputEncoding
     *      must match any string encoding used by update() before; with "buffer" a Buffer
     *      is returned. After final() the instance must not be reused; Node.js throws
     *      ERR_CRYPTO_INVALID_STATE on reuse.
     *
     *      @param outputEncoding the encoding of the output data
     *      @return returns the updated data
     *
     */
    final(outputEncoding?: string): any;

}

