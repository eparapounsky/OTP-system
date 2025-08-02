# OTP Encryption Client

The encryption client reads plaintext and encryption key files, connects to the encryption server, and receives the encrypted ciphertext.

## Usage

```bash
./enc_client <plaintext_file> <key_file> <port_number>
```

**Parameters:**
- `plaintext_file`: Path to file containing the plaintext to encrypt
- `key_file`: Path to file containing the encryption key
- `port_number`: Port number of the encryption server
