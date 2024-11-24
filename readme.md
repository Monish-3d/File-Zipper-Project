# File Compression and Decompression using Huffman Coding  

This project implements a lossless file compression and decompression tool using **Huffman Coding**, a popular algorithm that minimizes file sizes by encoding data based on character frequency. The program is written in **C++** and is designed to efficiently compress text files and restore them to their original state without any data loss.  

---

## Features  
- **Compression:** Reads a source file, generates Huffman codes, and creates a compressed binary file along with a `.huffmap` file which is stored in the same location as the compressed file.  
- **Decompression:** Reads the compressed binary file and `.huffmap` to reconstruct the original file.  
- **Efficiency:** Utilizes priority queues, bitsets, and unordered maps to ensure fast and memory-efficient operations.  

---

## Technologies Used  
- **Language:** C++  
- **Data Structures:**  
  - **Priority Queue:** To construct the Huffman Tree.  
  - **Unordered Map:** For storing and accessing character-to-code mappings.  
  - **Bitset:** For handling binary data and reducing file size.  

---

## How to Run  

### Prerequisites  
- A C++ compiler (e.g., GCC, Clang, MSVC)  
- C++17 or later  

### Steps  
1. Clone this repository:  
   ```bash
   git clone https://github.com/Monish-3d/File-Zipper-Project.git
   cd huffman-compression
