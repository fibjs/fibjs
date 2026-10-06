/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description RTCSessionDescription wraps one session description (SDP) of a WebRTC session
 *
 *  A description is the document a peer offers or answers with. The class holds its `type` and its
 *  `sdp` text and is one of the two object types exchanged during signaling (the other one is
 *  RTCIceCandidate). The RTCPeerConnection methods accept both an instance and a plain object with
 *  the same fields, so the class is mainly useful when the signaling channel must carry a typed
 *  object.
 *
 *  Concepts:
 *
 *  - **SDP content**: the text starts with `v=0` and carries the origin, session name and timing
 *    lines (`o=`, `s=`, `t=`), then one `m=` line per media stream - a data-channel-only session has
 *    a single `m=application ... UDP/DTLS/SCTP webrtc-datachannel` line - and the attributes that
 *    matter for connectivity: `a=ice-ufrag`, `a=ice-pwd`, `a=fingerprint`, `a=setup`, `a=mid`,
 *    `a=candidate` and `a=max-message-size`.
 *  - **Types**: `offer` starts a negotiation, `answer` accepts one, `pranswer` is a provisional
 *    answer and `rollback` cancels a pending offer; a type the library does not know is reported as
 *    `unspec`.
 *  - **Normalization**: the constructor parses the text and `sdp` returns the description regenerated
 *    from the parsed fields, which is not necessarily byte-identical to the input - the origin
 *    session id is regenerated on every construction, so two round trips of the same text differ.
 *    Compare parsed content, not strings.
 *  - **Acceptance**: the connection methods take either an instance or a plain object with `type` and
 *    `sdp`, which is converted through this constructor, so both fields are required either way.
 *
 *  Obtained from:
 *  - `new rtc.RTCSessionDescription({ type, sdp })` — wraps one description text, both fields are
 *    required;
 *  - `createOffer()` and `createAnswer()` — resolve with plain objects of the same shape; wrap them
 *    when a class instance is needed.
 *
 *  Example 1 — wrap a generated offer and read its media line:
 *  ```JavaScript
 *  const rtc = require('rtc');
 *
 *  const pc = new rtc.RTCPeerConnection({ iceServers: [] });
 *  pc.createDataChannel('chat');
 *
 *  pc.createOffer().then((offer) => {
 *      const desc = new rtc.RTCSessionDescription(offer);
 *      console.log(desc.type); // offer
 *      desc.sdp.split('\r\n').forEach((line) => {
 *          if (line.startsWith('m=')) console.log(line);
 *          // m=application 9 UDP/DTLS/SCTP webrtc-datachannel
 *      });
 *      console.log('regenerated:', desc.sdp !== offer.sdp); // regenerated: true
 *      pc.close();
 *  }).catch((err) => {
 *      console.error(err.message);
 *      process.exit(1);
 *  });
 *  ```
 *
 *  Example 2 — pass an instance to one side of the signaling and a plain object to the other:
 *  ```JavaScript
 *  const rtc = require('rtc');
 *  const coroutine = require('coroutine');
 *
 *  const pc1 = new rtc.RTCPeerConnection({ iceServers: [] });
 *  const pc2 = new rtc.RTCPeerConnection({ iceServers: [] });
 *  const toPc1 = [];
 *  const toPc2 = [];
 *  pc1.onicecandidate = (ev) => { if (ev.candidate) toPc2.push(ev.candidate); };
 *  pc2.onicecandidate = (ev) => { if (ev.candidate) toPc1.push(ev.candidate); };
 *
 *  const dc1 = pc1.createDataChannel('chat');
 *  let opened = false;
 *  dc1.onopen = () => { opened = true; };
 *  pc2.ondatachannel = () => {};
 *
 *  pc1.createOffer()
 *      .then((offer) => pc1.setLocalDescription(new rtc.RTCSessionDescription(offer))
 *          .then(() => pc2.setRemoteDescription(offer)))
 *      .then(() => pc2.createAnswer())
 *      .then((answer) => pc2.setLocalDescription(answer)
 *          .then(() => pc1.setRemoteDescription(new rtc.RTCSessionDescription(answer))))
 *      .then(() => {
 *          const deadline = Date.now() + 8000;
 *          while (!opened && Date.now() < deadline) {
 *              while (toPc1.length) pc1.addIceCandidate(toPc1.shift());
 *              while (toPc2.length) pc2.addIceCandidate(toPc2.shift());
 *              coroutine.sleep(10);
 *          }
 *          pc1.close();
 *          pc2.close();
 *          if (!opened) {
 *              console.error('the peers did not connect');
 *              process.exit(1);
 *          }
 *          console.log('connected with typed descriptions');
 *          // connected with typed descriptions
 *      })
 *      .catch((err) => {
 *          console.error(err.message);
 *          process.exit(1);
 *      });
 *  ```
 *
 *  Notes:
 *
 *  - fibjs has no string conversion for the class: converting an instance to a string throws 20024.
 *
 */
declare class Class_RTCSessionDescription extends Class_object {
    /**
     * @description constructs a session description object from a description object
     *
     *      The description object must contain both `type` (the kind of the description) and `sdp` (the
     *      SDP text, a string; another type throws 20005). A missing field throws TypeError 20002. The
     *      text is parsed at construction, so text the library cannot parse may throw 20024, and an
     *      unknown `type` is accepted and reported as `unspec` by the `type` member.
     *
     *      Example — wrap a short description and read its type:
     *      ```JavaScript
     *      const rtc = require('rtc');
     *
     *      const desc = new rtc.RTCSessionDescription({ type: 'offer', sdp: 'v=0\r\n' });
     *      console.log(desc.type); // offer
     *      ```
     *
     *      @param description initialization parameter
     *
     */
    constructor(description?: FIBJS.GeneralObject);

    /**
     * @description gets the type of the description
     *
     *      Returns `offer`, `answer`, `pranswer`, `rollback` or `unspec` for a type the library does not
     *      recognize. The type decides what `RTCPeerConnection.setLocalDescription` does with the
     *      description: fibjs applies an offer only.
     *
     */
    readonly type: string;

    /**
     * @description gets the session description text
     *
     *      Returns the SDP regenerated from the parsed fields, not the exact text passed to the
     *      constructor: lines may be normalized or added and the origin session id is regenerated, so
     *      two constructions of the same text can differ. This is the effective description and the
     *      value to send to the peer.
     *
     */
    readonly sdp: string;

}

