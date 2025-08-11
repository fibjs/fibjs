const path = require("path");
const http = require('http');
const ws = require("ws");
const child_process = require('child_process');

const svr = new http.Server(8081, {
    '/shell': ws.upgrade(socket => {
        console.log('New WebSocket connection established');

        // 检测默认shell
        const defaultShell = process.env.SHELL || '/bin/bash';
        const isZsh = defaultShell.includes('zsh');
        const isBash = defaultShell.includes('bash');

        console.log(`Using shell: ${defaultShell}`);

        // Spawn a shell process with PTY support
        const shell = process.platform === 'win32' ? 'cmd.exe' : defaultShell;
        const args = process.platform === 'win32' ? [] : ['-i']; // Interactive mode
        const child = child_process.spawn(shell, args, {
            stdio: 'pty',
            cols: 80,
            rows: 24,
            env: {
                ...process.env,
                TERM: 'xterm-256color',
                COLORTERM: 'truecolor',
                SHELL: defaultShell,
                // Make sure terminal is interactive
                PS1: isZsh ? '%n@%m:%~%# ' : '\\u@\\h:\\w\\$ ',
                // Enable colors
                CLICOLOR: '1',
                FORCE_COLOR: '1'
            }
        });

        console.log(`PTY created: ${child.cols}x${child.rows}`);

        // Handle child process output (PTY combines stdout and stderr)
        child.stdout.on('data', (data) => {
            if (socket.readyState === ws.OPEN) {
                socket.send(JSON.stringify({
                    type: 'data',
                    data: data.toString()
                }));
            }
        });

        // Handle child process exit
        child.on('exit', (code, signal) => {
            console.log(`Child process exited with code ${code}, signal ${signal}`);
            if (socket.readyState === ws.OPEN) {
                socket.send(JSON.stringify({
                    type: 'data',
                    data: `\r\nProcess exited with code: ${code}\r\n`
                }));
                socket.close();
            }
        });

        // Handle child process errors
        child.on('error', (error) => {
            console.error('Child process error:', error);
            if (socket.readyState === ws.OPEN) {
                socket.send(JSON.stringify({
                    type: 'data',
                    data: `\r\nProcess error: ${error.message}\r\n`
                }));
            }
        });

        // Handle WebSocket messages (user input and resize commands)
        socket.onmessage = (event) => {
            try {
                const message = JSON.parse(event.data);

                if (message.type === 'resize') {
                    // Handle terminal resize
                    const { cols, rows } = message;
                    if (cols > 0 && rows > 0) {
                        child.resize(cols, rows);

                        // Send confirmation back to client
                        socket.send(JSON.stringify({
                            type: 'resize_ack',
                            cols: child.cols,
                            rows: child.rows
                        }));
                    }
                } else if (message.type === 'data') {
                    // Handle regular input data
                    child.stdin.write(message.data);
                }
            } catch (error) {
                // Fallback: treat as plain text input for backward compatibility
                child.stdin.write(event.data);
            }
        };

        // Handle WebSocket close
        socket.onclose = () => {
            console.log('WebSocket connection closed');
            if (!child.killed) {
                child.kill('SIGTERM');
            }
        };

        // Handle WebSocket errors
        socket.onerror = (error) => {
            console.error('WebSocket error:', error);
        };

        // Send welcome message with initial terminal info
        socket.send(JSON.stringify({
            type: 'init',
            data: 'Welcome to FibJS Web Terminal with Resize Support!\r\n',
            cols: child.cols,
            rows: child.rows
        }));
    }),
    '*': path.resolve(__dirname, 'public')
});

console.log('Starting web terminal server on http://localhost:8081');
svr.start();
