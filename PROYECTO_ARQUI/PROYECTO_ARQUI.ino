#include <I2C_RTC.h>
#include <Wire.h>
#include <LCD_I2C.h>

LCD_I2C lcd_i2c(0x27, 20, 4); // Dirección I2C: 0x27, 20 columnas, 4 filas

void setup() {
  lcd_i2c.begin();
  lcd_i2c.backlight(); // Encender la luz de fondo del LCD

 
}

void loop() {
  // Leer el valor del sensor de humedad
  int humedad = analogRead(A1);
  int luz = analogRead(A3);
  int temp = analogRead(A0);
  temp = (5.0 * temp * 100.0) / 1024.0;

  // Limpiar la pantalla LCD y mostrar la humedad en tiempo real
  lcd_i2c.clear();
  lcd_i2c.setCursor(0, 0); // Establecer posición del cursor en la primera línea (0, 0)
  lcd_i2c.print("HUMEDAD TIERRA: ");
  lcd_i2c.print(humedad);

  // Mostrar los otros datos sin actualizar
  lcd_i2c.setCursor(0, 1); // Establecer posición del cursor en la segunda línea (0, 1)
  lcd_i2c.print("TEMP AMBIENTE: ");
  lcd_i2c.print(temp);

  lcd_i2c.setCursor(0, 2); // Establecer posición del cursor en la tercera línea (0, 2)
  lcd_i2c.print("LUZ: ");
  lcd_i2c.print(luz);


  lcd_i2c.setCursor(0, 3); // Establecer posición del cursor en la cuarta línea (0, 3)
  lcd_i2c.print("HORA: ");

  // Esperar un breve período antes de la próxima lectura
  delay(1000); // 1 segundo (ajusta este valor según sea necesario)
}

