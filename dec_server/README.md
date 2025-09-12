# OTP Decryption Server

The decryption server listens for client connections and performs OTP decryption by:
1. Accepting connections from decryption clients only
2. Receiving ciphertext and encryption key from clients
3. Decrypting the ciphertext using the provided key
4. Returning the plaintext to the client

## Usage

```bash
./dec_server <port_number>
```

**Parameters:**
- `port_number`: The port number on which the server will listen for connections
