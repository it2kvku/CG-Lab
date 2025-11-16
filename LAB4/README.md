# LAB 4 - Quan Sát Vật Thể 3D

## Mô tả

Chương trình cho phép quan sát vật thể 3D theo 2 cách:

### 4.1. Quay trực tiếp quanh các trục (Direct Rotation)
- Sử dụng phép quay 3D quanh trục OX, OY, OZ
- Điều khiển bằng các phím mũi tên và Q/E

### 4.2. Ma trận biến đổi hệ quan sát (View Matrix)
- Thay đổi phép chiếu: Song song hoặc Phối cảnh
- Thay đổi góc quan sát θ (theta), φ (phi)
- Thay đổi vị trí quan sát R (khoảng cách)
- Phóng to/thu nhỏ D

## Biên dịch và chạy

```bash
g++ -o polygon_mesh_3d.exe polygon_mesh_3d.cpp -lopengl32 -lglu32 -lfreeglut -std=c++11
.\polygon_mesh_3d.exe
```

## Điều khiển

### Chế độ 4.1: Quay trực tiếp (Direct Rotation Mode)

| Phím | Chức năng |
|------|-----------|
| ↑ ↓ | Quay quanh trục X |
| ← → | Quay quanh trục Y |
| Q / E | Quay quanh trục Z (+/-) |
| + / - | Phóng to / Thu nhỏ (D) |

### Chế độ 4.2: Ma trận quan sát (View Matrix Mode)

| Phím | Chức năng |
|------|-----------|
| ↑ ↓ | Thay đổi góc φ (phi) |
| ← → | Thay đổi góc θ (theta) |
| W / S | Thay đổi khoảng cách R |
| + / - | Phóng to / Thu nhỏ (D) |
| Chuột trái | Giảm R (gần hơn) |
| Chuột phải | Tăng R (xa hơn) |

### Điều khiển chung

| Phím | Chức năng |
|------|-----------|
| M | Chuyển đổi giữa 2 chế độ |
| P | Đổi phép chiếu (Song song ↔ Phối cảnh) |
| R | Reset về giá trị ban đầu |
| ESC | Thoát chương trình |

## Tham số mặc định

- **Theta (θ):** 30° - Góc quay quanh trục Y
- **Phi (φ):** 20° - Góc quay quanh trục X  
- **R:** 8.0 - Khoảng cách quan sát
- **D:** 2.0 - Hệ số zoom/scale
- **Phép chiếu:** Phối cảnh (Perspective)

## Ma trận biến đổi quan sát

Ma trận T được tính theo công thức:

```
T = | -sinθ   -cosθ·cosφ   -cosθ·sinφ   0 |
    |  cosθ   -sinθ·cosφ   -sinθ·sinφ   0 |
    |  0       sinφ        -cosφ        R |
    |  0       0            0           1 |
```

Kết hợp với ma trận zoom:
```
S = | D  0  0  0 |
    | 0  D  0  0 |
    | 0  0  D  0 |
    | 0  0  0  1 |
```

Ma trận cuối cùng: **M = S × T**

## Ma trận quay trực tiếp

### Quay quanh trục X:
```
Rx(α) = | 1   0      0     0 |
        | 0   cosα  -sinα  0 |
        | 0   sinα   cosα  0 |
        | 0   0      0     1 |
```

### Quay quanh trục Y:
```
Ry(β) = | cosβ   0   sinβ  0 |
        | 0      1   0     0 |
        | -sinβ  0   cosβ  0 |
        | 0      0   0     1 |
```

### Quay quanh trục Z:
```
Rz(γ) = | cosγ  -sinγ  0  0 |
        | sinγ   cosγ  0  0 |
        | 0      0     1  0 |
        | 0      0     0  1 |
```

## File dữ liệu (data.txt)

Định dạng:
```
<số đỉnh>
<x1> <y1> <z1>
<x2> <y2> <z2>
...
<số mặt>
<số đỉnh mặt 1> <chỉ số đỉnh 1> <chỉ số đỉnh 2> ...
<số đỉnh mặt 2> <chỉ số đỉnh 1> <chỉ số đỉnh 2> ...
...
```

### Ví dụ: Hình lập phương (data.txt)
```
8
-1.0 -1.0 -1.0
1.0 -1.0 -1.0
1.0 1.0 -1.0
-1.0 1.0 -1.0
-1.0 -1.0 1.0
1.0 -1.0 1.0
1.0 1.0 1.0
-1.0 1.0 1.0
6
4 0 1 2 3
4 4 5 6 7
4 0 1 5 4
4 2 3 7 6
4 0 3 7 4
4 1 2 6 5
```

## Hiển thị

- **Hệ trục tọa độ:**
  - Trục X: Màu đỏ
  - Trục Y: Màu xanh lá
  - Trục Z: Màu xanh dương

- **Vật thể:**
  - Cạnh: Màu trắng
  - Đỉnh: Điểm màu vàng

- **Thông tin màn hình:**
  - Chế độ hiện tại
  - Giá trị các tham số
  - Phép chiếu đang dùng
  - Hướng dẫn điều khiển

## Yêu cầu kỹ thuật

✅ Vật thể biểu diễn theo mô hình mặt đa giác (Polygon Mesh)  
✅ Sử dụng sự kiện phím để điều chỉnh góc quay, vị trí quan sát  
✅ Sử dụng sự kiện chuột để điều chỉnh khoảng cách  
✅ Hỗ trợ 2 phương pháp quan sát (4.1 và 4.2)  
✅ Chuyển đổi giữa phép chiếu song song và phối cảnh  
✅ Ma trận biến đổi được tính thủ công (không dùng hàm có sẵn)  

## Lưu ý

- Chương trình tự động đọc file `data.txt` trong cùng thư mục
- Nếu file không tồn tại, chương trình sẽ báo lỗi
- Có thể chỉnh sửa `data.txt` để thay đổi vật thể 3D
- Hệ trục tọa độ luôn được hiển thị để dễ quan sát
- Chế độ 4.1 dùng glRotate (OpenGL built-in)
- Chế độ 4.2 dùng ma trận tự tính (theo yêu cầu đề bài)
