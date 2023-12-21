const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');

const server = new WebSocket.Server({
    port: 8080,
    verifyClient: (info, cb) => {
        console.log('WebSocket handshake received:');
        console.log('Request origin:', info.origin);
        console.log('Request secure:', info.secure);
        console.log('Request resource:', info.req.url);
        console.log('Request headers:', info.req.headers);

        // Validate and sanitize the resource request
        const sanitizedPath = sanitizeResourcePath(info.req.url);
        if (sanitizedPath === null) {
            // Invalid resource request, reject the connection
            cb(false, 400, 'Bad Request');
            return;
        }
		const requestedResource = info.req.url;
        // Store the WebSocket connection along with the associated file path
        const filePath = __dirname + sanitizedPath; // Adjust the base directory accordingly
        const socket = { connection: info.req.connection, filePath: filePath };
        activeSockets.push(socket);

        cb(true, 200, 'OK');
		
		//server.emit('resource', requestedResource);
    }
});

const activeSockets = [];

server.on('resource', (requestedResource) => {
    // Event handler for the resource request
    console.log('Resource requested:', requestedResource);

    // Read the content of the file asynchronously
    fs.readFile(__dirname + requestedResource, (err, data) => {
        if (err) {
            console.error('Error reading file:', err);
            return;
        }

        // Iterate over connected clients and send the file content to each
        server.clients.forEach((client) => {
            if (client.readyState === WebSocket.OPEN) {
                console.log('Sending data');
                //client.send(data);
				client.send('1');
            }
        });
    });
});

server.on('connection', (socket) => {
    console.log('Client connected');

    socket.on('message', (message) => {
        console.log(`Received message: ${message}`);
        // Handle different types of messages
        if (String(message) === 'GET_FILE_SIZE') {
            // Find the associated file path for this WebSocket connection
            const socketInfo = activeSockets.find((s) => s.connection === socket._socket);
            if (socketInfo) {
                const filePath = socketInfo.filePath;
				// Log out socket information
				console.log(`Sending file size to socket: ${socket._socket.remoteAddress}:${socket._socket.remotePort}`);
                // Respond with the size of the file
                fs.stat(filePath, (err, stats) => {
                    if (err) {
                        console.error('Error getting file size:', err);
                        return;
                    }
                    const fileSize = stats.size.toString();
					const response = `${fileSize}\n`;
					console.log('send file size:', fileSize);
                    socket.send(response);
                });
            }
        }
		if(String(message) === 'GET_FILE') {
            // Find the associated file path for this WebSocket connection
            const socketInfo = activeSockets.find((s) => s.connection === socket._socket);
            if (socketInfo) {
                const filePath = socketInfo.filePath;
				// Log out socket information
				console.log(`Sending file size to socket: ${socket._socket.remoteAddress}:${socket._socket.remotePort}`);
                // Respond with the file
				fs.readFile(filePath, (err, data) => {
                    if (err) {
                        console.error('Error getting file:', err);
                        return;
                    }
					//const response = `${data}WSEOF`;
					const delimiterBuffer = Buffer.from('WSEOF');
					const response = Buffer.concat([data, delimiterBuffer]);
                    socket.send(response);
                });
            }			
		}else {
            // Handle other types of messages as needed
        }
    });

    socket.on('close', () => {
        console.log('Client disconnected');

        // Remove the WebSocket connection from the activeSockets array
        const index = activeSockets.findIndex((s) => s.connection === socket._socket);
        if (index !== -1) {
            activeSockets.splice(index, 1);
        }
    });
});

// Function to validate and sanitize the resource path
function sanitizeResourcePath(resourcePath) {
    // Prevent traversal attacks by normalizing and making sure path starts with /resources/

    const normalizedPath = path.normalize(resourcePath);
    const isValidPath = normalizedPath.startsWith('\\resources\\') || normalizedPath.startsWith('/resources/');

    return isValidPath ? normalizedPath : null;
}
