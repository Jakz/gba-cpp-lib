extern "C"
{
  typedef struct bios_obj_affine_set_src_
  {
    signed short sx;
    signed short sy;
    unsigned short angle;
  } __attribute__((aligned(4))) bios_obj_affine_set_src;
  
  typedef struct bios_obj_affine_set_dst_
  {
    signed short dx1;
    signed short dx2;
    signed short dy1;
    signed short dy2;
  } __attribute__((aligned(4))) bios_obj_affine_set_dst;
  
  void bios_obj_affine_set(const bios_obj_affine_set_src* src, const bios_obj_affine_set_dst* dst);
  
}