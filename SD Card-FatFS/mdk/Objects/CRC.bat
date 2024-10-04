for /f %%i in ('dir /b stm32f407.bin') do (
set indexdx=%%~zi
)
ECHO %indexdx%

.\srec_cat.exe stm32f407.bin -Binary -crop 0 %indexdx% -STM32_Little_Endian %indexdx% -o app.bin -Binary