
#include "system_config.h"
#include "system_type.h"

#include "power_api.h"

bool power_stat=true;

void power_init(void) {
    // init port ..
    
    /*
    
        �ⲿ��·��û������,ԭ����������:
        ����һ���Ĵ�����AND ������.[out ϵͳ��Դ,in1 �����Դ,in2 �Ƿ���Ҫ����]
        ���ϵ�ʱ���Ĵ���һ���ø�,AND �Ϳ���������,�ػ���ʱ��ѼĴ����õ�,AND ��û������..
    
    */
    
}

void power_shutdown(void) {
    if (power_stat) {
        power_stat=false;
        
        //  �����ⲿ����
        
    }
}
