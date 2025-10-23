# Image-Steganography
A C-based Image Steganography project that hides and extracts secret data within BMP images using LSB (Least Significant Bit) encoding. It ensures secure data concealment without affecting image quality, featuring efficient bitwise operations and file handling.

📘 Overview

Image Steganography is the art of hiding information inside digital images in such a way that it remains invisible to the human eye.
This project allows users to encode (hide) a secret message or file within a BMP image and later decode it to retrieve the hidden content.

⚙️ Features

🔐 Hide secret data within a 24-bit BMP image.

🧩 Retrieve hidden data from a stego image.

🧮 Uses bitwise manipulation for efficient encoding.

💾 Works entirely via command-line interface.

🎨 Maintains the original image’s visual quality.

🧠 Core Concepts

File I/O in C

Bitwise Operators (&, |, <<, >>)

Command-Line Arguments

BMP File Structure (Header + Pixel Data)

🧰 Technologies Used
Component	Details
Language	C
Image Format	BMP (24-bit)
Compiler	GCC
IDE (optional)	Visual Studio Code / Code::Blocks
🚀 How It Works

Encoding Process

- Reads the secret file and BMP image.
- Modifies the least significant bits of the image pixels to embed secret data.
- Produces a new image file (stego.bmp) containing the hidden message.
  

Decoding Process

- Reads the stego.bmp.
- Extracts the secret data from the LSBs of each pixel.
- Reconstructs and saves the hidden file.


🖥️ Usage

🔹 Compilation
gcc encode.c decode.c main.c -o steganography

🔹 Encoding
./steganography -e source.bmp secret.txt stego.bmp

- source.bmp → Original image
- secret.txt → File or message to hide
- stego.bmp → Output image containing hidden data
  

🔹 Decoding
./steganography -d stego.bmp output.txt

- stego.bmp → Image with hidden data
- output.txt → Extracted secret message



