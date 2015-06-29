
#include "sram_api.h"

#include "memory_api.h"
#include "memory_type.h"

static struct {
    long memory_size;
    long memory_available;
    long memory_unavailable;
    long memory_userate;
} kernel_memory_statistics;

void kernel_memory_api_init(void) {
    kernel_memory_statistics.memory_size=sram_get_size();
    kernel_memory_statistics.memory_available=kernel_memory_statistics.memory_size;
    kernel_memory_statistics.memory_unavailable=0;
    kernel_memory_statistics.memory_userate=0;
}

long memory_statistics(const memory_statistics_flag in_memory_statistics_flag) {
    switch (in_memory_statistics_flag) {
        case GET_MEMORY_SIZE:
            return kernel_memory_statistics.memory_size;
        case GET_MEMORY_AVAILABLE:
            return kernel_memory_statistics.memory_available;
        case GET_MEMORY_UNAVAILABLE:
            return kernel_memory_statistics.memory_unavailable;
        case GET_MEMORY_USERATE:
            return kernel_memory_statistics.memory_userate;
        case GET_ALLOC_COUNT:
            return 0;
    }
    return 0;
}

static void memory_statistics_calculate(const long new_memory_avail) {
    kernel_memory_statistics.memory_unavailable+=new_memory_avail;
    kernel_memory_statistics.memory_available=kernel_memory_statistics.memory_size-kernel_memory_statistics.memory_unavailable;
    kernel_memory_statistics.memory_userate=(kernel_memory_statistics.memory_unavailable*100)/kernel_memory_statistics.memory_size;
}

point_memory_alloc_header first_alloc=(point_memory_alloc_header)INVALID_MEMORY_ADDRESS;

void* memory_alloc(const long memory_alloc_length) {
    memory_alloc_header new_alloc_header;
    memory_alloc_ender  new_alloc_ender;
    memory_alloc_header find_alloc_header;
    memory_alloc_ender  find_alloc_ender;
    memory_address exchange_address=(memory_address)INVALID_MEMORY_ADDRESS;
    long memory_block_distance=0;
    long new_alloc_length=sizeof(memory_alloc_header)+memory_alloc_length+sizeof(memory_alloc_ender);
    if (0<memory_alloc_length && new_alloc_length<kernel_memory_statistics.memory_available) {
        if (INVALID_MEMORY_ADDRESS!=(int)first_alloc) {
            sram_read(first_alloc,&find_alloc_header,sizeof(memory_alloc_header));
            sram_read((void*)((long)find_alloc_header.alloc_address+find_alloc_header.length),&find_alloc_ender,sizeof(memory_alloc_ender));
            while (INVALID_MEMORY_ADDRESS!=(long)find_alloc_ender.point_next) {
                memory_block_distance=(long)find_alloc_ender.point_next-(long)find_alloc_header.alloc_address-find_alloc_header.length-sizeof(memory_alloc_ender);
                if (new_alloc_length<=memory_block_distance) {
                    new_alloc_header.point_last=(void*)((long)find_alloc_header.alloc_address-sizeof(memory_alloc_header));
                    new_alloc_header.alloc_address=(void*)(SRAM_ADDRESS_START+(long)find_alloc_header.alloc_address+find_alloc_header.length+sizeof(memory_alloc_ender)+sizeof(memory_alloc_header));
                    new_alloc_header.length=memory_alloc_length;
                    new_alloc_header.alloc_header_flag='F';
                    new_alloc_header.data_protect=READ_WRITE;
                    new_alloc_header.data_share=SHARE_NO;
                    new_alloc_header.data_kernel=0;
                    sram_write((void*)((long)new_alloc_header.alloc_address-sizeof(memory_alloc_header)),&new_alloc_header,sizeof(memory_alloc_header));
                    
                    new_alloc_ender.point_next=(void*)(SRAM_ADDRESS_START+(unsigned long)find_alloc_ender.point_next);
                    sram_write((void*)((long)new_alloc_header.alloc_address+new_alloc_header.length),&new_alloc_ender,sizeof(memory_alloc_ender));
                    
                    exchange_address=(memory_address)((long)new_alloc_header.alloc_address-sizeof(memory_alloc_header));
                    sram_write(find_alloc_ender.point_next,&exchange_address,sizeof(memory_address));
                    sram_write((void*)((long)find_alloc_header.alloc_address+find_alloc_header.length),&exchange_address,sizeof(memory_address));
                    memory_statistics_calculate(sizeof(memory_alloc_header)+memory_alloc_length+sizeof(memory_alloc_ender));
                    return new_alloc_header.alloc_address;
                }
                sram_read(find_alloc_ender.point_next,&find_alloc_header,sizeof(memory_alloc_header));
                sram_read((void*)((long)find_alloc_header.alloc_address+find_alloc_header.length),&find_alloc_ender,sizeof(memory_alloc_ender));
            }
            if (new_alloc_length<=(kernel_memory_statistics.memory_size-((long)find_alloc_header.alloc_address+find_alloc_header.length+sizeof(memory_alloc_ender)))) {
                new_alloc_header.point_last=(void*)((long)find_alloc_header.alloc_address-sizeof(memory_alloc_header));
                new_alloc_header.alloc_address=(void*)(SRAM_ADDRESS_START+(long)find_alloc_header.alloc_address+find_alloc_header.length+sizeof(memory_alloc_ender)+sizeof(memory_alloc_header));
                new_alloc_header.length=memory_alloc_length;
                new_alloc_header.alloc_header_flag='F';
                new_alloc_header.data_protect=READ_WRITE;
                new_alloc_header.data_share=SHARE_NO;
                new_alloc_header.data_kernel=0;
                sram_write((void*)((long)new_alloc_header.alloc_address-sizeof(memory_alloc_header)),&new_alloc_header,sizeof(memory_alloc_header));
                
                new_alloc_ender.point_next=(void*)INVALID_MEMORY_ADDRESS;
                sram_write((void*)((long)new_alloc_header.alloc_address+new_alloc_header.length),&new_alloc_ender,sizeof(memory_alloc_ender));
                
                exchange_address=(memory_address)((long)new_alloc_header.alloc_address-sizeof(new_alloc_header));
                sram_write((void*)((long)find_alloc_header.alloc_address+find_alloc_header.length),&exchange_address,sizeof(memory_address));
                memory_statistics_calculate(sizeof(memory_alloc_header)+memory_alloc_length+sizeof(memory_alloc_ender));
                return new_alloc_header.alloc_address;
            }
            return (void*)INVALID_MEMORY_ADDRESS;
        }
        first_alloc=0;
        new_alloc_header.point_last=(void*)INVALID_MEMORY_ADDRESS;
        new_alloc_header.alloc_address=(void*)(SRAM_ADDRESS_START+sizeof(memory_alloc_header));
        new_alloc_header.length=memory_alloc_length;
        new_alloc_header.alloc_header_flag='F';
        new_alloc_header.data_protect=READ_WRITE;
        new_alloc_header.data_share=SHARE_NO;
        new_alloc_header.data_kernel=0;
        sram_write(0,&new_alloc_header,sizeof(memory_alloc_header));
        
        new_alloc_ender.point_next=(void*)INVALID_MEMORY_ADDRESS;
        sram_write((void*)((long)new_alloc_header.alloc_address+memory_alloc_length),&new_alloc_ender,sizeof(memory_alloc_ender));
        memory_statistics_calculate(sizeof(memory_alloc_header)+memory_alloc_length+sizeof(memory_alloc_ender));
        return new_alloc_header.alloc_address;
    }
    return (void*)INVALID_MEMORY_ADDRESS;
}
bool memory_free(void* alloc_memory) {  //  WARNING!  由于没有对数据进行清理,所以会有内存泄漏..  2014/7/2
    memory_alloc_header alloc_header;
    memory_alloc_ender  alloc_ender;
    memory_alloc_header last_alloc_header;
    memory_alloc_ender  last_alloc_ender;
    memory_address null_address=(memory_address)INVALID_MEMORY_ADDRESS;
    if (INVALID_MEMORY_ADDRESS==(int)alloc_memory)
        return false;
    if (INVALID_MEMORY_ADDRESS!=(int)first_alloc) {
        sram_read(first_alloc,&alloc_header,sizeof(memory_alloc_header));
        while(true) {
            sram_read((void*)((long)alloc_header.alloc_address+alloc_header.length),&alloc_ender,sizeof(memory_alloc_ender));
            if (alloc_header.alloc_address==alloc_memory) {
                if (INVALID_MEMORY_ADDRESS==(long)alloc_header.point_last && INVALID_MEMORY_ADDRESS==(long)alloc_ender.point_next) {
                    first_alloc=(point_memory_alloc_header)INVALID_MEMORY_ADDRESS;
                } else if (INVALID_MEMORY_ADDRESS==(long)alloc_header.point_last && INVALID_MEMORY_ADDRESS!=(long)alloc_ender.point_next) {
                    first_alloc=alloc_ender.point_next;
                    sram_write(alloc_ender.point_next,&null_address,sizeof(memory_address));
                } else if (INVALID_MEMORY_ADDRESS!=(long)alloc_header.point_last && INVALID_MEMORY_ADDRESS!=(long)alloc_ender.point_next) {
                    sram_read(alloc_header.point_last,&last_alloc_header,sizeof(memory_alloc_header));
                    sram_read((void*)((long)last_alloc_header.alloc_address+last_alloc_header.length),&last_alloc_ender,sizeof(memory_alloc_ender));
                    last_alloc_ender.point_next=alloc_ender.point_next;
                    sram_write((void*)((long)last_alloc_header.alloc_address+last_alloc_header.length),&last_alloc_ender,sizeof(memory_alloc_ender));
                    sram_write(alloc_ender.point_next,&alloc_header.point_last,sizeof(memory_address));
                } else if (INVALID_MEMORY_ADDRESS!=(long)alloc_header.point_last && INVALID_MEMORY_ADDRESS==(long)alloc_ender.point_next) {
                    sram_write((void*)((long)alloc_header.point_last+sizeof(memory_alloc_header)+alloc_header.length),&null_address,sizeof(memory_address));
                }
                
                memory_statistics_calculate(-(sizeof(alloc_header)+alloc_header.length+sizeof(memory_alloc_ender)));
                return true;
            }
            if (INVALID_MEMORY_ADDRESS==(long)alloc_ender.point_next)
                break;
            sram_read(alloc_ender.point_next,&alloc_header,sizeof(memory_alloc_header));
        }
    }
    return false;
}
