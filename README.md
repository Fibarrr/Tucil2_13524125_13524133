# Octree Voxelization

<div align="center">

### Kontributor

</div>

<div align="center">
  <strong>
    <table align="center">
      <tr align="center">
        <td><strong>NIM</strong></td>
        <td><strong>Nama</strong></td>
      </tr>
      <tr align="center">
        <td>13524133</td>
        <td>Muhammad Daffa Arrizki Yanma</td>
      </tr>
      <tr align="center">
        <td>13524125</td>
        <td>Muhammad Rafi Akbar</td>
      </tr>
    </table>
  </strong>
</div>

<br>

---

## Penjelasan Singkat Program

Program ini adalah implementasi dari algoritma **Divide and Conquer** menggunakan struktur data **Octree** untuk melakukan voxelization pada model 3D.

Proses voxelization dilakukan dengan mengidentifikasi leaf node (voxel) dari octree yang menyentuh surface model. Triangle-box intersection diimplementasikan menggunakan AABB (Axis-Aligned Bounding Box) untuk quick rejection test, disertai dengan Separating Axis Theorem (SAT) untuk validasi.

Hasilnya adalah file OBJ baru yang merepresentasikan model original dalam bentuk voxel dengan resolusi yang dapat disesuaikan melalui parameter depth. Program juga menyediakan statistik detail mengenai jumlah node per kedalaman dan waktu eksekusi keseluruhan.

---

## Instalasi

### Requirements

- **C++ Standard**: C++11 atau lebih tinggi
- **Compiler**: G++ (GCC) 7.0 atau lebih tinggi
- **Build Tool**: Make (opsional, bisa compile manual)

## Cara Mengkompilasi & Menjalankan

### Opsi 1: Menggunakan Makefile (Direkomendasikan)

**Kompilasi dan Jalankan Program:**
```bash
make run
```

**Hanya Kompilasi:**
```bash
make build
```

**Bersihkan File Kompilasi:**
```bash
make clean
```

### Opsi 2: Kompilasi Manual

**Windows:**
```bash
# compile
g++ -std=c++11 -O2 src/main.cpp -o bin/main_parser_test.exe

# run
bin/main_parser_test.exe
```

**Linux/UNIX:**
```bash
# compile
g++ -std=c++11 -O2 src/main.cpp -o bin/main_parser_test

# Set permission 
chmod +x bin/main_parser_test

# run
./bin/main_parser_test
```

---

## Panduan Penggunaan

### Menjalankan Program

Setelah folder project sudah tersedia, cukup jalankan:

```bash
make run
```


### Parameter Input

1. **Path File OBJ**
   - Contoh: `test/teapot.obj`
   - File harus berisi vertices (v) dan triangular faces (f)

2. **Kedalaman Maksimum (Maximum Depth)**
   - Masukkan integer ≥ 1
   - Kedalaman lebih tinggi = resolusi voxel lebih halus, waktu proses lebih lama
   - Contoh nilai kedalaman:
     - Depth 2
     - Depth 3
     - Depth 5

### Output Program

Program menghasilkan:
- **File OBJ Voxelized**: `[nama_file]-voxelized.obj`
- **Statistik**:
  - Jumlah voxel yang dibuat
  - Total vertices dan faces pada output
  - Waktu eksekusi keseluruhan
  - Node per level kedalaman

### Contoh Eksekusi

```
========== OCTREE VOXEL ==========

Masukkan path file (contoh: test/cow.obj): test/teapot.obj
Masukkan depth: 5

Membaca file: test/teapot.obj
Vertex input: 530
Triangle input: 1024

========== HASIL ==========
Jumlah voxel      : 3560
Jumlah vertex     : 28480
Jumlah faces      : 42720
Kedalaman octree  : 5
Waktu eksekusi    : 0.245 detik
Path file output  : test/teapot-voxelized.obj

--- Statistik node per kedalaman ---
0 : 1
1 : 8
2 : 32
3 : 168
4 : 800
5 : 3560

--- Node yang tidak perlu ditelusuri per kedalaman ---
1 : 4
2 : 11
3 : 68
4 : 355
5 : 1701
```

---

## 📁 Struktur Repository

```
Tucil2_13524125_13524133/
├── bin/
│   └── main_parser_test      # Executable (hasil kompilasi)
├── doc/
│   └── Tucil2_13524125_13524133.pdf           
├── src/
│   ├── main.cpp              # Program utama & user interface
│   └── parser.cpp            # octree voxelization
├── test/
│    └── <OBJ files>           # Sample model 3D
├── Makefile                  # Script build
└── README.md

```




