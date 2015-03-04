#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

uint8 spi_flash_clk_set(uint8 mode);  /*mode=0, spi clk=10MHz.   mode=1,spi_clk=20MHz*/
void spi_flash_erase(void);
int8 spi_flash_sr_read(void);
int32 spi_flash_sr_write(uint8 SR);
int32 spi_flash_read(uint32 addr, uint8 *data, uint16 len);
int32 spi_flash_write(uint32 addr, uint8 *data, uint16 len);

void spi_flash_erase_CE(void);
void spi_flash_erase_sector(uint32 address);
void spi_flash_erase_block(uint32 address);
int32 spi_flash_write_func(uint32 addr, uint8 *data, uint16 len);
uint8 spi_flash_update_fw_done(uint8 type);
uint8 spi_flash_update_fw(uint8 type, uint32 offset, uint8 *pdata, uint16 len);

uint8 dump_spi_flash_fw(uint8 TYPE);
uint8 dump_spi_flash(uint32 start, uint32 end);

#endif /* SPI_FLASH_H_ */

