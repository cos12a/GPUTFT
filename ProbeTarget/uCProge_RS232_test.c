#include "probe_com_cfg.h"
#include "probe_rs232.h"





void main_test(void)
{
    ProbeCom_Init();

    ProbeRS232_Init(115200);
    ProbeRS232_RxIntEn();




}
