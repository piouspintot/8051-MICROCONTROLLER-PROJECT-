// =============================================
// I2C  - Correct for AT89C51 + PCF8574
// SCL = P2^1  (pin 22 ? PCF8574 pin 14)
// SDA = P2^2  (pin 23 ? PCF8574 pin 15)
// P2 is NOT open-drain, so no pull-up issue
// =============================================
sbit SCL = P2^1;
sbit SDA = P2^2;

void i2c_delay() {
    unsigned char i;
    for(i = 0; i < 255; i++);
}

void i2c_start() {
    SDA = 1; i2c_delay();   // Step 1: SDA high
    SCL = 1; i2c_delay();   // Step 2: SCL high
    SDA = 0; i2c_delay();   // Step 3: SDA falls ? START condition
    SCL = 0; i2c_delay();   // Step 4: SCL low ? ready to send
}

void i2c_stop() {
    SCL = 0; i2c_delay();   // Step 1: SCL low
    SDA = 0; i2c_delay();   // Step 2: SDA low
    SCL = 1; i2c_delay();   // Step 3: SCL rises
    SDA = 1; i2c_delay();   // Step 4: SDA rises ? STOP condition
}

void i2c_write_byte(unsigned char val) {
    unsigned char i;
    for(i = 0; i < 8; i++) {
        SCL = 0; i2c_delay();              // Clock low before changing SDA
        SDA = (val & 0x80) ? 1 : 0;       // Put bit on SDA (MSB first)
        val <<= 1;
        i2c_delay();
        SCL = 1; i2c_delay();              // Clock high ? slave reads bit
        SCL = 0; i2c_delay();              // Clock low ? next bit
    }
    // ACK: release SDA and pulse SCL once
    SCL = 0; i2c_delay();
    SDA = 1; i2c_delay();                  // Release SDA for ACK
    SCL = 1; i2c_delay();                  // Slave pulls SDA low (ACK)
    SCL = 0; i2c_delay();                  // Done
}

void pcf8574_send(unsigned char val) {
    i2c_start();
    i2c_write_byte((LCD_ADDR << 1) | 0x00);  // Address + Write bit (0)
    i2c_write_byte(val);                       // Data to PCF8574
    i2c_stop();
    delay_ms(2);
}