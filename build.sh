#!/bin/bash

gcc -o keygen keygen/keygen.c
gcc -o enc_server enc_server/enc_server.c
gcc -o enc_client enc_client/enc_client.c
gcc -o dec_server dec_server/dec_server.c
gcc -o dec_client dec_client/dec_client.c
