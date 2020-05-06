/*
 * zbuffer_manager.c
 *
 *  Created on: 2014年11月5日
 *      Author: shell.albert
 */

#include <zbuffer_manager.h>
#include <zdebug.h>
/**
 * 16 bytes fixed frame head for sync.
 */
const zint8_t fixed_frame_header[] =
  { ///<
    0xCC, 0xCC, 0xCC, 0xCC, ///<
	0x55, 0x55, 0x55, 0x55, ///<
	0xFF, 0xFF, 0xFF, 0xFF, ///<
	0x0, 0x0, 0x0, 0x0, ///<
    };

/**
 * memory map a plain buffer to FPGA frame structure for easy operation.
 */
zint32_t
zmmap_buffer_to_fpga_frame (zuint8_t *pbuffer, zfpga_dev *fpga)
{
  if (pbuffer == NULL || fpga == NULL)
    {
      printf ("invalid parameters!\n");
      return -1;
    }

  fpga->z_framehead = (pbuffer + framehead_offset);

  fpga->z_spotnumber = (pbuffer + spotnumber_offset);
  /**
   * (x,y).
   */
  fpga->z_left_top_x_coordinate = (pbuffer + left_top_x_coordinate_offset);
  fpga->z_left_top_y_coordinate = (pbuffer + left_top_y_coordinate_offset);
  /**
   * width*height.
   */
  fpga->z_picture_width = (pbuffer + subtitles_pixel_matrix_width_offset);
  fpga->z_picture_height = (pbuffer + subtitles_pixel_matrix_height_offset);
  fpga->z_timein = (pbuffer + subtitles_time_in_offset);
  fpga->z_timeout = (pbuffer + subtitles_time_out_offset);
  fpga->z_reserved = (pbuffer + reserved_offset);
  fpga->z_data_length = (pbuffer + subtitles_pixel_matrix_data_length_offset);
  fpga->z_data_body = (pbuffer + subtitles_pixel_matrix_data_body_offset);
  return 0;
}
/**
 * the end of file,tagged by zhangshaoyan.
 * shell.albert@gmail.com
 */
