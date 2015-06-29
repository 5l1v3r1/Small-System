
#include "sram_api.h"

#include "memory_api.h"
#include "memory_type.h"

bool memory_page_set_protect(void* alloc_memory_address,const protect_flag flag) {
    point_memory_alloc_header alloc_header=(point_memory_alloc_header)alloc_memory_address;
    if ((long)alloc_header->alloc_address==(long)alloc_memory_address) {
        alloc_header->data_protect=flag;
        return true;
    }
    return false;
}
bool memory_page_set_share(void* alloc_memory_address,const share_flag flag) {
    point_memory_alloc_header alloc_header=(point_memory_alloc_header)alloc_memory_address;
    if ((long)alloc_header->alloc_address==(long)alloc_memory_address) {
        alloc_header->data_share=flag;
        return true;
    }
    return false;
}
bool memory_page_set_kernel(void* alloc_memory_address,const bool enable) {
    point_memory_alloc_header alloc_header=(point_memory_alloc_header)alloc_memory_address;
    if ((long)alloc_header->alloc_address==(long)alloc_memory_address) {
        alloc_header->data_kernel=enable;
        return true;
    }
    return false;
}

void memory_read_int(const void* read_memory_address,int* out_data) {
    sram_read(read_memory_address,out_data,sizeof(int));
}
void memory_read_long(const void* read_memory_address,long* out_data) {
    sram_read(read_memory_address,out_data,sizeof(long));
}
void memory_read_float(const void* read_memory_address,float* out_data) {
    sram_read(read_memory_address,out_data,sizeof(float));
}
void memory_read_double(const void* read_memory_address,double* out_data) {
    sram_read(read_memory_address,out_data,sizeof(double));
}
void memory_read_char(const void* read_memory_address,char* out_data) {
    sram_read(read_memory_address,out_data,sizeof(char));
}
void memory_read_string(const void* read_memory_address,char* out_data,const memory_length read_memory_data_length) {
    sram_read(read_memory_address,out_data,read_memory_data_length);
}

bool memory_write_int(void* write_memory_address,const int write_memory_data) {
    void* find_alloc_header=write_memory_address;
    memory_alloc_header alloc_header;
    do {
        sram_read(find_alloc_header,&alloc_header,sizeof(memory_alloc_header));
        if ('F'==alloc_header.alloc_header_flag) {
            if ((long)alloc_header.alloc_address<=(long)write_memory_address && ((long)write_memory_address-sizeof(int))<((long)alloc_header.alloc_address+alloc_header.length)) {
                if (READ_WRITE==alloc_header.data_protect) {
                    sram_write(write_memory_address,(void*)&write_memory_data,sizeof(int));
                    return true;
                }
                return false;
            }
        }
        find_alloc_header=(void*)((long)find_alloc_header-1);
    } while ((long)find_alloc_header>=0);
    return false;
}
bool memory_write_long(void* write_memory_address,const long write_memory_data) {
    void* find_alloc_header=write_memory_address;
    memory_alloc_header alloc_header;
    do {
        sram_read(find_alloc_header,&alloc_header,sizeof(memory_alloc_header));
        if ('F'==alloc_header.alloc_header_flag) {
            if ((long)alloc_header.alloc_address<=(long)write_memory_address && ((long)write_memory_address-sizeof(long))<((long)alloc_header.alloc_address+alloc_header.length)) {
                if (READ_WRITE==alloc_header.data_protect) {
                    sram_write(write_memory_address,(void*)&write_memory_data,sizeof(long));
                    return true;
                }
                return false;
            }
        }
        find_alloc_header=(void*)((long)find_alloc_header-1);
    } while ((long)find_alloc_header>=0);
    return false;
}
bool memory_write_float(void* write_memory_address,const float write_memory_data) {
    void* find_alloc_header=write_memory_address;
    memory_alloc_header alloc_header;
    do {
        sram_read(find_alloc_header,&alloc_header,sizeof(memory_alloc_header));
        if ('F'==alloc_header.alloc_header_flag) {
            if ((long)alloc_header.alloc_address<=(long)write_memory_address && ((long)write_memory_address-sizeof(float))<((long)alloc_header.alloc_address+alloc_header.length)) {
                if (READ_WRITE==alloc_header.data_protect) {
                    sram_write(write_memory_address,(void*)&write_memory_data,sizeof(float));
                    return true;
                }
                return false;
            }
        }
        find_alloc_header=(void*)((long)find_alloc_header-1);
    } while ((long)find_alloc_header>=0);
    return false;
}
bool memory_write_double(void* write_memory_address,const double write_memory_data) {
    void* find_alloc_header=write_memory_address;
    memory_alloc_header alloc_header;
    do {
        sram_read(find_alloc_header,&alloc_header,sizeof(memory_alloc_header));
        if ('F'==alloc_header.alloc_header_flag) {
            if ((long)alloc_header.alloc_address<=(long)write_memory_address && ((long)write_memory_address-sizeof(double))<((long)alloc_header.alloc_address+alloc_header.length)) {
                if (READ_WRITE==alloc_header.data_protect) {
                    sram_write(write_memory_address,(void*)&write_memory_data,sizeof(double));
                    return true;
                }
                return false;
            }
        }
        find_alloc_header=(void*)((long)find_alloc_header-1);
    } while ((long)find_alloc_header>=0);
    return false;
}
bool memory_write_string(void* write_memory_address,const char* write_memory_data,const memory_length write_memory_data_length) {
    void* find_alloc_header=write_memory_address;
    memory_alloc_header alloc_header;
    do {
        sram_read(find_alloc_header,&alloc_header,sizeof(memory_alloc_header));
        if ('F'==alloc_header.alloc_header_flag) {
            if ((long)alloc_header.alloc_address<=(long)write_memory_address && ((long)write_memory_address-write_memory_data_length)<((long)alloc_header.alloc_address+alloc_header.length)) {
                if (READ_WRITE==alloc_header.data_protect) {
                    sram_write(write_memory_address,(void*)&write_memory_data,write_memory_data_length);
                    return true;
                }
                return false;
            }
        }
        find_alloc_header=(void*)((long)find_alloc_header-1);
    } while ((long)find_alloc_header>=0);
    return false;
}

bool memory_write_char(void* write_memory_address,const char write_memory_data) {
    return memory_write_string(write_memory_address,&write_memory_data,sizeof(char));
}

void memory_set(void* address,char setchar,long length) {
    while (length--)
        *(char*)((long)address+length)=setchar;
}

void memory_copy(void* dest_addr,const void* sorc_addr,long length) {
    while (length--)
        *(char*)((long)dest_addr+length)=*(char*)((long)sorc_addr+length);
}

bool memory_cmp(const void* addr_1,const void* addr_2,long length) {
    while (length--)
        if (*(char*)addr_1!=*(char*)addr_2)
            return false;
    return true;
}
