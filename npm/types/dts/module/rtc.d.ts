/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/RTCPeerConnection.d.ts" />
/// <reference path="../interface/RTCSessionDescription.d.ts" />
/// <reference path="../interface/RTCIceCandidate.d.ts" />
/// <reference path="../interface/RTCListener.d.ts" />
/**
 * @description The rtc module establishes WebRTC peer connections: it negotiates a session between two endpoints through the signaling channel of the application and then carries text or binary data over data channels without a central relay, which suits file transfer, chat and bidirectional streams between browsers or between fibjs processes
 *
 *  Main capabilities:
 *
 *  - **Peer connections**: `RTCPeerConnection` creates offers and answers, applies local and remote
 *    descriptions and reports the connection states;
 *  - **Data channels**: `RTCPeerConnection.createDataChannel()` and the `datachannel` event return
 *    `RTCDataChannel` objects that carry `String` or `Buffer` messages;
 *  - **Signaling objects**: `RTCSessionDescription` and `RTCIceCandidate` hold the two object types
 *    exchanged during signaling;
 *  - **Incoming handshakes**: `RTCListener` binds a UDP port, validates the incoming WebRTC
 *    Direct credentials and creates the answering peer connection; several listeners may coexist
 *    on different ports;
 *  - **Servers and tuning**: `startServer()` runs an embedded STUN/TURN server, `setSctpSettings()`
 *    tunes the global SCTP parameters and `loglevel` controls the library log.
 *
 *  Concepts:
 *
 *  - **Signaling is application work**: this module produces and consumes session descriptions and
 *    ICE candidates but never transports them. The peers exchange them through whatever channel the
 *    application provides (HTTP, WebSocket, a shared file, or directly when both peers live in one
 *    process, as in the examples below).
 *  - **Offer and answer**: the initiating side creates a data channel first (or otherwise has
 *    something to negotiate), `createOffer()` resolves with the local description,
 *    `setLocalDescription()` applies it and `setRemoteDescription()` applies the peer's answer. On
 *    the answering side applying the remote offer is enough for the library to generate the answer,
 *    which `createAnswer()` then resolves with. A connection with nothing to negotiate never
 *    produces a description.
 *  - **ICE, STUN and TURN**: ICE gathers candidate transport addresses and checks connectivity on
 *    each candidate pair. STUN discovers the address a NAT presents to the outside, and TURN relays
 *    the traffic when no direct path exists. When `iceServers` is omitted the constructor
 *    configures the public server `stun:stun.l.google.com:19302`; pass `iceServers: []` to stay
 *    completely local.
 *  - **Host candidates**: two peers on the same host, whether in one process or in two, connect
 *    through host candidates only; they need no external service and no network beyond a usable
 *    local interface.
 *  - **Data channels**: a data channel is an SCTP stream over the DTLS transport. It is ordered and
 *    reliable by default; `ordered`, `maxPacketLifeTime` and `maxRetransmits` trade ordering or
 *    retransmission for latency, while `negotiated` and `id` let both sides create the same channel
 *    without in-band negotiation.
 *  - **States**: `connectionState` runs `new` to `connecting` to `connected` (or `failed` /
 *    `disconnected`) and ends in `closed`; `iceConnectionState` follows the connectivity checks,
 *    `iceGatheringState` reports candidate gathering and `signalingState` reports the offer/answer
 *    phase. Data channels open after the transport is connected, and `close()` releases the
 *    connection together with its channels.
 *
 *  Import:
 *  ```JavaScript
 *  const rtc = require('rtc');
 *  ```
 *
 *  Example 1 — two peers in one process exchange a text message:
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
 *  pc2.ondatachannel = (ev) => {
 *      const dc2 = ev.channel;
 *      dc2.onmessage = (mev) => dc2.send('echo: ' + mev.data);
 *  };
 *
 *  let reply = null;
 *  dc1.onopen = () => dc1.send('hello');
 *  dc1.onmessage = (ev) => { reply = ev.data; };
 *
 *  pc1.createOffer()
 *      .then((offer) => pc1.setLocalDescription(offer).then(() => pc2.setRemoteDescription(offer)))
 *      .then(() => pc2.createAnswer())
 *      .then((answer) => pc2.setLocalDescription(answer).then(() => pc1.setRemoteDescription(answer)))
 *      .then(() => {
 *          const deadline = Date.now() + 8000;
 *          while (reply === null && Date.now() < deadline) {
 *              while (toPc1.length) pc1.addIceCandidate(toPc1.shift());
 *              while (toPc2.length) pc2.addIceCandidate(toPc2.shift());
 *              coroutine.sleep(10);
 *          }
 *          pc1.close();
 *          pc2.close();
 *          if (reply !== 'echo: hello') {
 *              console.error('the peers did not exchange a message');
 *              process.exit(1);
 *          }
 *          console.log(reply); // echo: hello
 *      })
 *      .catch((err) => {
 *          console.error(err.message);
 *          process.exit(1);
 *      });
 *  ```
 *
 *  Example 2 — the same loop carries binary data:
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
 *  const dc1 = pc1.createDataChannel('binary');
 *  pc2.ondatachannel = (ev) => {
 *      const dc2 = ev.channel;
 *      dc2.onmessage = (mev) => {
 *          console.log('peer received a Buffer:', Buffer.isBuffer(mev.data));
 *          dc2.send(Buffer.from(mev.data.toString().toUpperCase()));
 *      };
 *  };
 *
 *  let reply = null;
 *  dc1.onopen = () => dc1.send(Buffer.from('ping'));
 *  dc1.onmessage = (ev) => { reply = ev.data; };
 *
 *  pc1.createOffer()
 *      .then((offer) => pc1.setLocalDescription(offer).then(() => pc2.setRemoteDescription(offer)))
 *      .then(() => pc2.createAnswer())
 *      .then((answer) => pc2.setLocalDescription(answer).then(() => pc1.setRemoteDescription(answer)))
 *      .then(() => {
 *          const deadline = Date.now() + 8000;
 *          while (reply === null && Date.now() < deadline) {
 *              while (toPc1.length) pc1.addIceCandidate(toPc1.shift());
 *              while (toPc2.length) pc2.addIceCandidate(toPc2.shift());
 *              coroutine.sleep(10);
 *          }
 *          pc1.close();
 *          pc2.close();
 *          if (!Buffer.isBuffer(reply) || reply.toString() !== 'PING') {
 *              console.error('the peers did not exchange a Buffer');
 *              process.exit(1);
 *          }
 *          console.log('reply:', reply.toString()); // reply: PING
 *      })
 *      .catch((err) => {
 *          console.error(err.message);
 *          process.exit(1);
 *      });
 *  ```
 *
 *  Example 3 — accept an incoming WebRTC Direct peer:
 *  ```JavaScript
 *  const rtc = require('rtc');
 *
 *  const listener = new rtc.RTCListener({
 *      certPem: process.env.RTC_CERT_PEM,
 *      keyPem: process.env.RTC_KEY_PEM
 *  });
 *  listener.on('connection', (ev) => {
 *      ev.peerConnection.ondatachannel = (dcEv) => console.log('channel', dcEv.channel.label);
 *  });
 *  listener.listen(3222, '127.0.0.1');
 *  console.log('listening on', listener.address().port);
 *  // ...
 *  listener.close();
 *  ```
 *
 *  Notes:
 *
 *  - `RTCDataChannel` is not a member of the module: obtain it from
 *    `RTCPeerConnection.createDataChannel()` or from the `datachannel` event.
 *  - `RTCListener` holds the process alive while it is bound; `close()` releases the hold and
 *    stops accepting, while connections accepted before the call keep running.
 *  - A connection with a remote description holds the process alive until `close()` is called; on a
 *    connection with nothing to negotiate, `createOffer()` never resolves and also holds the
 *    process.
 *  - `startServer()` has no matching stop API: the server lives as long as the process.
 *
 */
declare module 'rtc' {
    /**
     * @description The WebRTC connection class, see RTCPeerConnection
     *
     *      `new rtc.RTCPeerConnection(options)` creates a peer connection; it is the entry point of a
     *      session and the source of data channels, session descriptions and ICE candidates.
     *
     */
    const RTCPeerConnection: typeof Class_RTCPeerConnection;

    type RTCPeerConnection = Class_RTCPeerConnection;

    /**
     * @description The WebRTC session description class, see RTCSessionDescription
     *
     *      Wraps one of the two objects exchanged during signaling; `RTCPeerConnection` methods also
     *      accept a plain object with the same `type` and `sdp` fields instead of an instance.
     *
     */
    const RTCSessionDescription: typeof Class_RTCSessionDescription;

    type RTCSessionDescription = Class_RTCSessionDescription;

    /**
     * @description The WebRTC ICE candidate class, see RTCIceCandidate
     *
     *      Wraps one of the two objects exchanged during signaling; `RTCPeerConnection.addIceCandidate`
     *      also accepts a plain object with the same `candidate` and `sdpMid` fields instead of an
     *      instance.
     *
     */
    const RTCIceCandidate: typeof Class_RTCIceCandidate;

    type RTCIceCandidate = Class_RTCIceCandidate;

    /**
     * @description The WebRTC Direct listener class, see RTCListener
     *
     *      One RTCListener owns one UDP port, one certificate and the admission policy for incoming
     *      WebRTC Direct connections; accepted peers are delivered as ready RTCPeerConnection objects.
     *
     */
    const RTCListener: typeof Class_RTCListener;

    type RTCListener = Class_RTCListener;

    /**
     * @description starts the embedded STUN/TURN server
     *
     *      Starts a STUN/TURN server that other peers can use for NAT traversal; the call returns as
     *      soon as the server is created. There is no API to stop it: the server runs until the process
     *      exits. The config object accepts the following fields, all optional:
     *         - `credentials` - object of `username`: `password` pairs accepted by TURN, empty by default
     *         - `maxAllocations` - maximum number of TURN allocations, 0 for the library default
     *         - `maxPeers` - maximum number of peers, 0 for the library default
     *         - `bindAddress` - local address to bind, any by default
     *         - `port` - listening port, 3478 by default
     *         - `relayPortRangeBegin` - first port of the TURN relay range, 0 by default
     *         - `relayPortRangeEnd` - last port of the TURN relay range, 0 by default
     *
     *      Example — run a local STUN/TURN server until the process is stopped:
     *      ```JavaScript
     *      // requires: long-running
     *      const rtc = require('rtc');
     *
     *      rtc.startServer({ port: 0, bindAddress: '127.0.0.1', maxPeers: 16 });
     *      console.log('server started');
     *      setTimeout(() => {}, 30000); // keep the process and the server alive
     *      ```
     *
     *      @param config server configuration
     *
     */
    function startServer(config: {
        credentials?: FIBJS.StringMap;
        maxAllocations?: number;
        maxPeers?: number;
        bindAddress?: string;
        port?: number;
        relayPortRangeBegin?: number;
        relayPortRangeEnd?: number;
    }): void;

    /**
     * @description sets the WebRTC global SCTP parameters
     *
     *      Applies the process-wide SCTP parameters of the underlying library. The settings take effect
     *      immediately for new connections and do not change connections that already exist. Every
     *      field is optional; a field with a wrong type throws 20005, while values are passed through
     *      without range validation. The following fields are supported:
     *         - `recvBufferSize` - receive buffer size in bytes, 1MiB by default
     *         - `sendBufferSize` - send buffer size in bytes, 1MiB by default
     *         - `maxChunksOnQueue` - maximum number of chunks in the queue, 10K by default
     *         - `initialCongestionWindow` - initial congestion window in MTU, 10 by default
     *         - `maxBurst` - maximum burst size in MTU, 10 by default
     *         - `congestionControlModule` - congestion control module: 0 RFC2581 (default), 1 HSTCP,
     *           2 H-TCP, 3 RTCC
     *         - `delayedSackTimeMs` - delayed acknowledgement time in milliseconds, 20 by default
     *         - `minRetransmitTimeoutMs` - minimum retransmit timeout in milliseconds, 200 by default
     *         - `maxRetransmitTimeoutMs` - maximum retransmit timeout in milliseconds, 10000 by default
     *         - `initialRetransmitTimeoutMs` - initial retransmit timeout in milliseconds, 1000 by
     *           default
     *         - `maxRetransmitAttempts` - maximum number of retransmit attempts, 5 by default
     *         - `heartbeatIntervalMs` - heartbeat interval in milliseconds, 10000 by default
     *
     *      Example — enlarge the receive buffer for new connections:
     *      ```JavaScript
     *      const rtc = require('rtc');
     *
     *      rtc.setSctpSettings({ recvBufferSize: 2 * 1024 * 1024, maxRetransmitAttempts: 3 });
     *      console.log('SCTP settings applied');
     *      ```
     *
     *      @param settings SCTP parameters
     *
     */
    function setSctpSettings(settings: {
        recvBufferSize?: number;
        sendBufferSize?: number;
        maxChunksOnQueue?: number;
        initialCongestionWindow?: number;
        maxBurst?: number;
        congestionControlModule?: number;
        delayedSackTimeMs?: number;
        minRetransmitTimeoutMs?: number;
        maxRetransmitTimeoutMs?: number;
        initialRetransmitTimeoutMs?: number;
        maxRetransmitAttempts?: number;
        heartbeatIntervalMs?: number;
    }): void;

    /**
     * @description queries and sets the WebRTC log level
     *
     *      Gets or sets the log level of the underlying WebRTC library; the new level takes effect
     *      immediately. The following levels are accepted: `none` (the default, no output), `error`,
     *      `warning`, `info`, `debug`, `verbose` and `fatal`. An unknown value throws ERR_INVALID_STATE and leaves
     *      the previous level unchanged.
     *
     *      Example — raise the level, then reject an unknown value:
     *      ```JavaScript
     *      const rtc = require('rtc');
     *
     *      console.log('before:', rtc.loglevel); // before: none
     *      rtc.loglevel = 'debug';
     *      console.log('after:', rtc.loglevel); // after: debug
     *      try {
     *          rtc.loglevel = 'quiet';
     *      } catch (err) {
     *          console.log('rejected:', err.message); // rejected: Invalid log level: 'quiet'.
     *      }
     *      console.log('unchanged:', rtc.loglevel); // unchanged: debug
     *      ```
     *
     */
    var loglevel: string;

}

