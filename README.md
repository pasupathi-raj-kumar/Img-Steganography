## 🖼️ Image Steganography - C Console Application
A console-based application in C for hiding and extracting secret text messages using the Least Significant Bit (LSB) technique in 24-bit BMP images.

## 📌 Features

- 🔐 Encode a text message from secret.txt into a BMP image (beautiful.bmp)
- 🔍 Decode and retrieve the hidden message from the stego image
- 💡 Validates image capacity before encoding
- 🧑‍💻 Simple command-line interface for encoding and decoding

## 🛠️ Built With
 - Language: C
 - Compiler: GCC
 - Platform: Cross-platform (Tested on Linux and Windows)

## 🗂️ Project Structure
 - beautiful.bmp     # Sample input BMP image for encoding
 - secret.txt        # Text file containing the secret message
 - common.h          # Shared macros and definitions
 - types.h           # Type definitions and data structures
 - encode.c          # Core encoding logic
 - encode.h          # Header for encode functions
 - decode.c          # Core decoding logic
 - decode.h          # Header for decode functions
 - test_encode.c     # Entry point / test runner for encoding (main function)

## 💾 Data Handling
 - Input BMP: beautiful.bmp
 - Secret Message: secret.txt
 - Encoded Image Output: (usually named within the program logic, e.g., stego.bmp)
 - Decoded Message Output: Printed on console or written to a text file depending on implementation

## 🔧 Compilation Command
- gcc *.c (or)gcc test_encode.c encode.c decode.c
- ./a.out      # Linux/macOS
- ./a.exe      # Windows (if compiled with .exe)
  
## 📋 Typical Usage
 - Put your secret message in secret.txt
 - Ensure beautiful.bmp is present 

## Run the executable to:
 - Encode message → modifies BMP to store data
 - Decode message → retrieves data from the stego image

## 🙋‍♂️ Author
Developed by Pasupathi Raj Kumar M.

## 📄 License
This project is licensed under the MIT License – feel free to use, modify, and share.
