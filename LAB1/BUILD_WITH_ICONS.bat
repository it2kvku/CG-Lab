@echo off
chcp 65001 >nul
color 0B
cls

echo ================================================
echo    BIÊN DỊCH LẠI TẤT CẢ VỚI ICON
echo ================================================
echo.

REM Kiểm tra windres có tồn tại không
where windres >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARNING] windres không tìm thấy!
    echo Icon sẽ không được thêm vào executable.
    echo Tiếp tục biên dịch không có icon...
    echo.
    goto :compile_without_icons
)

echo [1/5] Biên dịch Bresenham Line với icon...
windres resource_line.rc -o resource_line.o
if %errorlevel% equ 0 (
    g++ bresenham_line_interactive.cpp resource_line.o -o bresenham_line.exe -lfreeglut -lopengl32 -lglu32
    if %errorlevel% equ 0 (
        echo [OK] Bresenham Line có icon đỏ
        del resource_line.o
    ) else (
        echo [ERROR] Lỗi khi link!
    )
) else (
    echo [ERROR] Lỗi khi compile resource!
)
echo.

echo [2/5] Biên dịch Midpoint Circle với icon...
windres resource_circle.rc -o resource_circle.o
if %errorlevel% equ 0 (
    g++ midpoint_circle_interactive.cpp resource_circle.o -o midpoint_circle.exe -lfreeglut -lopengl32 -lglu32
    if %errorlevel% equ 0 (
        echo [OK] Midpoint Circle có icon xanh dương
        del resource_circle.o
    ) else (
        echo [ERROR] Lỗi khi link!
    )
) else (
    echo [ERROR] Lỗi khi compile resource!
)
echo.

echo [3/5] Biên dịch Ellipse với icon...
windres resource_ellipse.rc -o resource_ellipse.o
if %errorlevel% equ 0 (
    g++ ellipse_interactive.cpp resource_ellipse.o -o ellipse.exe -lfreeglut -lopengl32 -lglu32
    if %errorlevel% equ 0 (
        echo [OK] Ellipse có icon xanh lá
        del resource_ellipse.o
    ) else (
        echo [ERROR] Lỗi khi link!
    )
) else (
    echo [ERROR] Lỗi khi compile resource!
)
echo.

echo [4/5] Biên dịch Curves với icon...
windres resource_curves.rc -o resource_curves.o
if %errorlevel% equ 0 (
    g++ curves_interactive.cpp resource_curves.o -o curves.exe -lfreeglut -lopengl32 -lglu32
    if %errorlevel% equ 0 (
        echo [OK] Curves có icon tím
        del resource_curves.o
    ) else (
        echo [ERROR] Lỗi khi link!
    )
) else (
    echo [ERROR] Lỗi khi compile resource!
)
echo.

echo [5/5] Biên dịch 2D Transformations với icon...
windres resource_transform.rc -o resource_transform.o
if %errorlevel% equ 0 (
    g++ transformations_2d.cpp resource_transform.o -o transformations_2d.exe -lfreeglut -lopengl32 -lglu32
    if %errorlevel% equ 0 (
        echo [OK] Transformations có icon vàng
        del resource_transform.o
    ) else (
        echo [ERROR] Lỗi khi link!
    )
) else (
    echo [ERROR] Lỗi khi compile resource!
)
echo.

goto :done

:compile_without_icons
echo [1/5] Bresenham Line...
g++ bresenham_line_interactive.cpp -o bresenham_line.exe -lfreeglut -lopengl32 -lglu32
echo.

echo [2/5] Midpoint Circle...
g++ midpoint_circle_interactive.cpp -o midpoint_circle.exe -lfreeglut -lopengl32 -lglu32
echo.

echo [3/5] Ellipse...
g++ ellipse_interactive.cpp -o ellipse.exe -lfreeglut -lopengl32 -lglu32
echo.

echo [4/5] Curves...
g++ curves_interactive.cpp -o curves.exe -lfreeglut -lopengl32 -lglu32
echo.

echo [5/5] 2D Transformations...
g++ transformations_2d.cpp -o transformations_2d.exe -lfreeglut -lopengl32 -lglu32
echo.

:done
echo ================================================
echo          HOÀN THÀNH!
echo ================================================
echo.
echo Các file executable đã được tạo:
echo   - bresenham_line.exe     (Icon đỏ - LINE)
echo   - midpoint_circle.exe    (Icon xanh dương - Circle)
echo   - ellipse.exe            (Icon xanh lá - Ellipse)
echo   - curves.exe             (Icon tím - Curves)
echo   - transformations_2d.exe (Icon vàng - 2D)
echo.
echo Icon files:
dir /b *.ico 2>nul
echo.
pause
