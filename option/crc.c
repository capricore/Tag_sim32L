#include "crc.h"

uint16_t crc_initial(void)
{
	return INITIAL_VALUE;
}

uint16_t crc_step(uint16_t crc, byte b)
{
	uint8_t v = 0x80;
	uint8_t bit;
	uint8_t xor_flag;

	for (bit = 0; bit < 8; bit++) {
		if (crc & 0x8000)
			xor_flag = 1;
		else 
			xor_flag = 0;

		crc <<= 1;

		if (b & v) 
			crc += 1;

		if (xor_flag)
			crc ^= CRC_POLY;
		
		v >>= 1;
	}
	return crc;
}

uint16_t crc_finish(uint16_t crc)
{
	uint8_t bit;
	uint8_t xor_flag;
	for(bit = 0; bit < 16; bit++) {
        	if(crc & 0x8000)
            		xor_flag= 1;
        	else
            		xor_flag= 0;

		crc <<= 1;

        	if(xor_flag)
			crc ^= CRC_POLY;
    	}
	return crc;
}

//第一次做crc  prev_crc的值为INITIAL_VALUE(0xffff)
uint16_t count_num_crc = 0;
uint16_t crc_calculate(uint16_t prev_crc, uint8_t *bytes, uint16_t len)
{
	uint16_t i;
	uint16_t curr_crc = prev_crc;
	count_num_crc++;
	for (i = 0 ; i < len; i++) 
		curr_crc = crc_step(curr_crc, bytes[i]);
	return curr_crc;
}

/** @brief crc-ccitt initial value. */

uint16_t crc_cnt = 0;
uint16_t crc_compute(uint8_t *buf, uint16_t size,uint16_t initialValue,uint8_t isLast)
{
   uint16_t index, crc;
   uint8_t v, xor_flag, byte, bit;
   crc_cnt++;
   //crc = INITIAL_VALUE;
   crc = initialValue;
   for(index = 0; index < size; index++) 
   {
      byte = buf[index];
      /*
	Align test bit with leftmost bit of the message byte.
      */
      v = 0x80;

      for(bit = 0; bit < 8; bit++) 
      {
	 if(crc & 0x8000)
            xor_flag= 1;
	 else
            xor_flag= 0;

	 crc = crc << 1;
            
	 /*  Append next bit of message to end of CRC if it is not zero.
	     The zero bit placed there by the shift above need not be
	     changed if the next bit of the message is zero. */
	 if(byte & v)
	    crc= crc + 1;

	 if(xor_flag)
	    crc = crc ^ CRC_POLY;

	 /* Align test bit with next bit of the message byte. */
	 v = v >> 1;
    }
}

   /* We have to augment the crc in order to comply with the ccitt spec. */
   if(isLast)
   {
	    for(bit = 0; bit < 16; bit++) 
	    {
	        if(crc & 0x8000)
	            xor_flag= 1;
	        else
	            xor_flag= 0;

			crc = crc << 1;

	        if(xor_flag)
	            crc = crc ^ CRC_POLY;
	    }
	}
   	return crc;
}


uint16_t crc_ccitt_update(uint16_t crc, uint8_t data) 
{
	data ^= (uint8_t)(crc);
	data ^= data << 4;

	return ((((uint16_t) data << 8) | (crc >> 8)) ^ (uint8_t) (data >> 4) ^ ((uint16_t) data << 3));
}
