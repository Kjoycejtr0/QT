/**
  ******************************************************************************
  * �ļ�����: spiflash_diskio.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2015-10-04
  * ��    ��: ����FLASH��FatFS�ļ�ϵͳ�ŽӺ���ʵ��
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-F1Proʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include <string.h>
#include "ff_gen_drv.h"
#include "user_flash.h"


/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
#define SPI_FLASH_SECTOR_SIZE    4096    // ����Flash������С
#define SPI_FLASH_START_SECTOR   0    // ����Flash�ļ�ϵͳFatFSƫ����
#define SPI_FLASH_SECTOR_COUNT   (512-SPI_FLASH_START_SECTOR)    // ����Flash�ļ�ϵͳFatFSռ����������    


/* ˽�б��� ------------------------------------------------------------------*/
static __IO DSTATUS Stat = STA_NOINIT;

/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
DSTATUS SPIFLASH_initialize(BYTE);
DSTATUS SPIFLASH_status(BYTE);
DRESULT SPIFLASH_read(BYTE, BYTE*, DWORD, UINT);

#if _USE_WRITE == 1   // �������д����
  DRESULT SPIFLASH_write (BYTE, const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1   // ������������������
  DRESULT SPIFLASH_ioctl (BYTE, BYTE, void*);
#endif  /* _USE_IOCTL == 1 */

/* ���崮��FLASH�ӿں��� */
const Diskio_drvTypeDef  SPIFLASH_Driver =
{
  SPIFLASH_initialize,                   // ����FLASH��ʼ�� 
  SPIFLASH_status,                       // ����FLASH״̬��ȡ
  SPIFLASH_read,                         // ����FLASH������
#if  _USE_WRITE == 1
  SPIFLASH_write,                        // ����FLASHд����
#endif /* _USE_WRITE == 1 */
  
#if  _USE_IOCTL == 1
  SPIFLASH_ioctl,                         // ��ȡ����FLASH��Ϣ
#endif /* _USE_IOCTL == 1 */
};

/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ����FLASH��ʼ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
DSTATUS SPIFLASH_initialize(BYTE lun)
{
  Stat = STA_NOINIT;
  /* ��ʼ��SPIFLASHIO���� */
  SPI_Flash_Init();
  /* ��ȡ����FLASH״̬ */
  if(SPI_FLASH_ReadDeviceID()==SPI_FLASH_ID)
  {
    Stat &=~STA_NOINIT;
  }
  return Stat;
}

/**
  * ��������: ����FLASH״̬��ȡ
  * �������: lun : ����
  * �� �� ֵ: DSTATUS������FLASH״̬����ֵ
  * ˵    ��: ��
  */
DSTATUS SPIFLASH_status(BYTE lun)
{
  Stat = STA_NOINIT;

  if(SPI_FLASH_ReadDeviceID()==SPI_FLASH_ID)
  {
    Stat &= ~STA_NOINIT;
  }
  return Stat;
}

/**
  * ��������: �Ӵ���FLASH��ȡ���ݵ�������
  * �������: lun : ����
  *           buff����Ŷ�ȡ�����ݻ�����ָ��
  *           sector��������ַ(LBA)
  *           count��������Ŀ
  * �� �� ֵ: DSTATUS���������
  * ˵    ��: ��
  */
DRESULT SPIFLASH_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{ 
  sector+=SPI_FLASH_START_SECTOR;      
  SPI_FLASH_BufferRead(buff, sector*SPI_FLASH_SECTOR_SIZE, count*SPI_FLASH_SECTOR_SIZE);
  return RES_OK;
}

/**
  * ��������: ������������д�뵽����FLASH��
  * �������: lun : ����
  *           buff����Ŵ�д�����ݵĻ�����ָ��
  *           sector��������ַ(LBA)
  *           count��������Ŀ
  * �� �� ֵ: DSTATUS���������
  * ˵    ��: ��
  */
#if _USE_WRITE == 1
DRESULT SPIFLASH_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
  uint32_t write_addr; 
  /* ����ƫ��7MB���ⲿFlash�ļ�ϵͳ�ռ����SPI Flash����9MB�ռ� */
  sector+=SPI_FLASH_START_SECTOR;
  write_addr = sector*SPI_FLASH_SECTOR_SIZE;    
  SPI_FLASH_SectorErase(write_addr);
  SPI_FLASH_BufferWrite((uint8_t *)buff,write_addr,count*SPI_FLASH_SECTOR_SIZE);
  return RES_OK;
}
#endif /* _USE_WRITE == 1 */

/**
  * ��������: ����������Ʋ���(I/O control operation)
  * �������: lun : ����
  *           cmd����������
  *           buff����Ŵ�д����߶�ȡ���ݵĻ�����ָ��
  * �� �� ֵ: DSTATUS���������
  * ˵    ��: ��
  */
#if _USE_IOCTL == 1
DRESULT SPIFLASH_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  
  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = RES_OK;
    break;
  
  /* ��ȡ����FLASH��������Ŀ(DWORD) */
  case GET_SECTOR_COUNT :
    *(DWORD * )buff = SPI_FLASH_SECTOR_COUNT;	
    res = RES_OK;
    break;
  
  /* ��ȡ��д������С(WORD) */
  case GET_SECTOR_SIZE :
    *(WORD * )buff = SPI_FLASH_SECTOR_SIZE;
    res = RES_OK;
    break;
  
  /* ��ȡ�������С(DWORD) */
  case GET_BLOCK_SIZE :
    *(DWORD * )buff = 1;
    res = RES_OK;
    break;
  
  default:
    res = RES_PARERR;
  }
  
  return res;
}
#endif /* _USE_IOCTL == 1 */

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
