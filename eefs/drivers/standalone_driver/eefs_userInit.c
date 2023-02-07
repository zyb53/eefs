#include "eefs_userInit.h"
#include "devtab.h"
#include "dev_mb85rc.h"
#include "eefs_macros.h"
#include "eefs_fileapi.h"
#include "eefs_filesys.h"
#include <string.h>
#include <stdio.h>


static hal_devtab_entry_t *m_handle = NULL;
static EEFS_FileAllocationTable_t m_fileAllocationTable;

/*--------------------------------------------------------------------------
* @brief      eeprom write lower level function
* @param   Dest : dest buffer
* @param   Src   : source buffer
* @param   Length : buffer length
* @return   None
*/
void eefs_write(void *Dest, void *Src, int Length)
{
    uint32_t targeAddr = (uint32_t)Dest;
    
    if(m_handle != NULL){
        if(hal_io_bwrite(m_handle ,Src ,Length ,targeAddr) <= 0){
        }
    }
}

/*--------------------------------------------------------------------------
* @brief      eeprom read lower level function
* @param   Dest : dest buffer
* @param   Src   : source buffer
* @param   Length : buffer length
* @return   None
*/
void eefs_read(void *Dest, void *Src, int Length)
{
    uint32_t targeAddr = (uint32_t)Src;
    
    if(m_handle != NULL){
        if(hal_io_bread(m_handle ,Dest ,Length ,targeAddr) <= 0){
        }
    }
}

/*-----------------------------------------------------------------------*/
/*
**  Register the EEFS filesystem and assign the 
**  addresses for the EEFS banks. 
*/
int setup_eefs(void)
{
    int32 Status;
    
    m_handle = hal_io_lookup( "/dev/mb85rc");
    if(m_handle == NULL){
        return (-1);
    }
    /* Init eeprom dev. */
    ((hal_devtab_entry_t *)m_handle)->init(m_handle);
    
    /*1.Init eefs lib*/
    EEFS_LibInit();
    /* 2.Check the file allocation table and write repair if it does not exist. */
    EEFS_LIB_EEPROM_READ(&m_fileAllocationTable.Header, (void *)0 , sizeof(EEFS_FileAllocationTableHeader_t));
    if ((m_fileAllocationTable.Header.Magic != EEFS_FILESYS_MAGIC) && (m_fileAllocationTable.Header.Version != EEFS_MAJOR_VERSION)) {/*EEFS_NO_SUCH_DEVICE*/
        /*Config file system*/
        memset(&m_fileAllocationTable ,0x00 ,sizeof(EEFS_FileAllocationTable_t));
        m_fileAllocationTable.Header.Crc = 0;
        m_fileAllocationTable.Header.Magic = EEFS_FILESYS_MAGIC;
        m_fileAllocationTable.Header.Version = EEFS_MAJOR_VERSION;
        m_fileAllocationTable.Header.FreeMemoryOffset = sizeof(EEFS_FileAllocationTable_t);
        m_fileAllocationTable.Header.FreeMemorySize = DEV_MB85RC64_SIZE/*(8*1024 bytes)*/ - sizeof(EEFS_FileAllocationTable_t);
        /*Write a file allocation table to eeprom.*/
        if(hal_io_bwrite(m_handle ,&m_fileAllocationTable ,sizeof(EEFS_FileAllocationTable_t) ,0) <= 0){
            //printf("Error: fail to write eeprom dev. \n");
            return (0);
        }
    }
    
    /*Init eefs lib*/
    Status = EEFS_InitFS("/eefs", 0);
    if(Status != EEFS_SUCCESS){
        //printf("Error: fail to init eefs. \n");
        return (0);
    }
    
    return (1);
}

/*-----------------------------------------------------------------*/
int eefs_test(void)
{
    int32 Status;
    char buff[100] = {0};
    static int32 wrCnt;
    int32 eefs_fd; 
        
    /*Mout test*/
    Status = EEFS_Mount("/eefs","/folder1");
    if (Status != EEFS_SUCCESS){
        printf("Error: fail to mount disk\n");
    }
    /*Write file test.*/
    eefs_fd = EEFS_Creat("/folder1/file1.txt", 0);
    if (eefs_fd != EEFS_SUCCESS){
        printf("Error: fail to creat file1\n");
        return (0);
    }
    
    const char *str = "hello world1!";
    wrCnt = EEFS_Write(eefs_fd, (char *)str,strlen(str));
    EEFS_Close(eefs_fd);
    
    /*Read file test.*/
    eefs_fd = EEFS_Open("/folder1/file1.txt", 0);
    if (eefs_fd != EEFS_SUCCESS){
        printf("Error: fail to write file1\n");
        return (0);
    }
    wrCnt = EEFS_Read(eefs_fd, buff, sizeof(buff));        
    
    EEFS_Close(eefs_fd);
    
    eefs_fd = EEFS_Open("/folder1/file2.txt", 0);
    if (eefs_fd != EEFS_SUCCESS){
        printf("Error: fail to write file1\n");
        return (0);
    }
    wrCnt = EEFS_Read(eefs_fd, buff, sizeof(buff));        
    
    EEFS_Close(eefs_fd);
    
    return (1);
}
