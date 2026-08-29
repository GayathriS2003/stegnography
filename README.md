# Steganography

This project hides secret message inside an image.

## What is Steganography?

Hiding secret data inside normal image. No one can know data exists.

## How it Works

1. Take image pixels
2. Change last bit (LSB) of pixel with secret message bit
3. Image looks same but contains secret message
4. For decoding, read last bits to get message

## How to Run

Encode:
./a.out -e original.bmp secret.txt output.bmp

Decode:
./a.out -d output.bmp


- encode.c
- decode.c
- main.h
