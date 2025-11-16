"""
Script tạo icon cho các ứng dụng LAB1
Tạo các icon PNG đơn giản cho từng chương trình
"""

from PIL import Image, ImageDraw, ImageFont
import os

def create_icon(text, filename, bg_color, text_color):
    """Tạo icon với text"""
    size = 256
    img = Image.new('RGB', (size, size), bg_color)
    draw = ImageDraw.Draw(img)
    
    # Vẽ gradient background
    for y in range(size):
        shade = int(255 * (1 - y / size * 0.3))
        color = tuple(int(c * shade / 255) for c in bg_color)
        draw.rectangle([0, y, size, y+1], fill=color)
    
    # Vẽ viền
    border_width = 8
    draw.rectangle([0, 0, size, size], outline=text_color, width=border_width)
    
    # Vẽ text
    try:
        font = ImageFont.truetype("arial.ttf", 80)
    except:
        font = ImageFont.load_default()
    
    # Tính vị trí để center text
    bbox = draw.textbbox((0, 0), text, font=font)
    text_width = bbox[2] - bbox[0]
    text_height = bbox[3] - bbox[1]
    position = ((size - text_width) // 2, (size - text_height) // 2 - 10)
    
    # Vẽ shadow
    shadow_offset = 4
    draw.text((position[0] + shadow_offset, position[1] + shadow_offset), 
              text, fill=(0, 0, 0), font=font)
    
    # Vẽ text chính
    draw.text(position, text, fill=text_color, font=font)
    
    # Vẽ subtitle
    try:
        small_font = ImageFont.truetype("arial.ttf", 24)
    except:
        small_font = ImageFont.load_default()
    
    subtitle = "LAB 1"
    bbox = draw.textbbox((0, 0), subtitle, font=small_font)
    sub_width = bbox[2] - bbox[0]
    sub_position = ((size - sub_width) // 2, size - 50)
    draw.text(sub_position, subtitle, fill=text_color, font=small_font)
    
    # Lưu file
    img.save(filename, 'PNG')
    print(f"Created: {filename}")
    
    # Tạo các kích thước khác nhau cho .ico
    sizes = [(16, 16), (32, 32), (48, 48), (64, 64), (128, 128), (256, 256)]
    icons = []
    for size_tuple in sizes:
        icon_img = img.resize(size_tuple, Image.Resampling.LANCZOS)
        icons.append(icon_img)
    
    ico_filename = filename.replace('.png', '.ico')
    icons[0].save(ico_filename, format='ICO', sizes=[(s[0], s[1]) for s in sizes])
    print(f"Created: {ico_filename}")

def main():
    """Tạo tất cả icon"""
    icons = [
        {
            'text': 'LINE',
            'filename': 'icon_line.png',
            'bg_color': (220, 50, 50),      # Đỏ
            'text_color': (255, 255, 255)   # Trắng
        },
        {
            'text': '○',
            'filename': 'icon_circle.png',
            'bg_color': (50, 150, 220),     # Xanh dương
            'text_color': (255, 255, 255)
        },
        {
            'text': '⬭',
            'filename': 'icon_ellipse.png',
            'bg_color': (100, 200, 100),    # Xanh lá
            'text_color': (255, 255, 255)
        },
        {
            'text': '∿',
            'filename': 'icon_curves.png',
            'bg_color': (200, 100, 200),    # Tím
            'text_color': (255, 255, 255)
        },
        {
            'text': '2D',
            'filename': 'icon_transform.png',
            'bg_color': (220, 180, 50),     # Vàng
            'text_color': (50, 50, 50)      # Xám đậm
        },
        {
            'text': 'LAB1',
            'filename': 'app_icon.png',
            'bg_color': (70, 130, 180),     # Steel blue
            'text_color': (255, 255, 255)
        }
    ]
    
    print("Creating icons for LAB1 applications...")
    print("=" * 50)
    
    for icon_info in icons:
        create_icon(
            icon_info['text'],
            icon_info['filename'],
            icon_info['bg_color'],
            icon_info['text_color']
        )
    
    print("=" * 50)
    print("All icons created successfully!")
    print("\nTo use icons with executables:")
    print("1. Add resource.rc to compilation")
    print("2. Compile with: windres resource.rc -o resource.o")
    print("3. Link: g++ ... resource.o -o program.exe")

if __name__ == '__main__':
    # Kiểm tra PIL có được cài đặt không
    try:
        from PIL import Image, ImageDraw, ImageFont
        main()
    except ImportError:
        print("Error: PIL/Pillow not installed!")
        print("Please install: pip install Pillow")
        print("\nCreating simple text file instead...")
        
        # Tạo file hướng dẫn
        with open('icon_instructions.txt', 'w', encoding='utf-8') as f:
            f.write("HƯỚNG DẪN TẠO ICON CHO ỨNG DỤNG LAB1\n")
            f.write("=" * 50 + "\n\n")
            f.write("Để tạo icon, cần cài đặt Pillow:\n")
            f.write("  pip install Pillow\n\n")
            f.write("Hoặc sử dụng các công cụ online:\n")
            f.write("  1. https://www.favicon-generator.org/\n")
            f.write("  2. https://convertio.co/png-ico/\n")
            f.write("  3. https://www.icoconverter.com/\n\n")
            f.write("Màu sắc đề xuất cho từng chương trình:\n")
            f.write("  - Bresenham Line:  Đỏ (#DC3232)\n")
            f.write("  - Midpoint Circle: Xanh dương (#3296DC)\n")
            f.write("  - Ellipse:         Xanh lá (#64C864)\n")
            f.write("  - Curves:          Tím (#C864C8)\n")
            f.write("  - 2D Transform:    Vàng (#DCB432)\n")
            f.write("  - LAB1 Main:       Steel Blue (#4682B4)\n")
        
        print("Created: icon_instructions.txt")
