
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define IMX93

void imx93_get_mac_from_fuse(int dev_id, uint32_t val_0, uint32_t val_1)
{
        uint32_t val[2] = {};
	uint8_t mac[6] = {};
	int is_rev_a0 = 0;
        int ret;

	val[0] = val_0;
	val[1] = val_1;

        if (dev_id == 0) {

                mac[0] = val[1] >> 8;
                mac[1] = val[1];
                mac[2] = val[0] >> 24;
                mac[3] = val[0] >> 16;
                mac[4] = val[0] >> 8;
                mac[5] = val[0];

        } else {

                if (is_rev_a0) {
                        mac[0] = val[1] >> 24;
                        mac[1] = val[1] >> 16;
                        mac[2] = val[0] >> 24;
                        mac[3] = val[0] >> 16;
                        mac[4] = val[0] >> 8;
                        mac[5] = val[0];
                } else {
                        mac[0] = val[0] >> 24;
                        mac[1] = val[0] >> 16;
                        mac[2] = val[0] >> 8;
                        mac[3] = val[0];
                        mac[4] = val[1] >> 24;
                        mac[5] = val[1] >> 16;
                }
        }

        printf("%s: MAC%d: %02x.%02x.%02x.%02x.%02x.%02x\n",
              __func__, dev_id, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        return;
}

void imx8ulp_get_mac_from_fuse(int dev_id, uint32_t val_0, uint32_t val_1)
{                                                                                
        uint32_t val[2] = {};                                                         
	uint8_t mac[6] = {};
        int ret;                                                                 
                                                                                 
	val[0] = val_0;
	val[1] = val_1;

        mac[5] = val[0];                                                         
        mac[4] = val[0] >> 8;                                                    
        mac[3] = val[0] >> 16;                                                   
        mac[2] = val[0] >> 24;                                                   
        mac[1] = val[1];                                                         
        mac[0] = val[1] >> 8;                                                    
                                                                                 
        printf("%s: MAC%d: %02x.%02x.%02x.%02x.%02x.%02x\n",                      
              __func__, dev_id, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]); 
        return;                                                                  
}


#define BD_OUI 0x0019b8
#define BD_OUI2 BD_OUI

int main() {
	uint32_t input[2] = {};
	uint32_t fuses[6] = {}; // Bank 39 , words 3,4,5

	printf("Enter last 3 bytes of MAC address, for eth0\n");
	scanf("%x", &input[0]);
	printf("Value of input in HEX: %06X\n", input[0]);

#ifdef IMX93

	printf("Enter last 3 bytes of MAC address, for eth1\n");
	scanf("%x", &input[1]);
	printf("Value of input in HEX: %06X\n", input[1]);

	fuses[3] = (((BD_OUI & 0x0000ff) << 24) | input[0]);
	printf("Value of fuse 3 in HEX: %08X\n", fuses[3]);

	fuses[4] = (((BD_OUI & 0xffff00) >> 8) | ((input[1] & 0x00ffff) << 16));
	printf("Value of fuse 4 in HEX: %08X\n", fuses[4]);

	fuses[5] = (((BD_OUI2 & 0xffffff) << 8) | ((input[1] & 0xff0000) >> 16));
	printf("Value of fuse 5 in HEX: %08X\n", fuses[5]);
	
	printf("eth0 : ");
	imx93_get_mac_from_fuse(0, fuses[3], fuses[4]);

	printf("eth1 : ");
	imx93_get_mac_from_fuse(1, fuses[5], fuses[4]);
	
	printf("\nfuse prog -y 39 3 0x%08X 0x%08X 0x%08X\n\n", fuses[3], fuses[4], fuses[5]);

#else

	/* IMX8ULP */

	fuses[3] = (((BD_OUI & 0x0000ff) << 24) | input[0]);
	fuses[4] = ((BD_OUI & 0xffff00) >> 8);

	imx8ulp_get_mac_from_fuse(0, fuses[3], fuses[4]);

	printf("\nfuse prog -y 5 3 0x%08X 0x%08X\n\n", fuses[3], fuses[4]);
#endif

	return 0;
}
