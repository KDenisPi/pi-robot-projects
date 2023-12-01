/**
 * @file colors.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef COLORS_MAP_H
#define COLORS_MAP_H

#include <cstdint>

 namespace ldata {

 const uint32_t colors150[150] = {
    0x104060,0x114362,0x124664,
    0x104068,0x11436A,0x12466C,
    0x104070,0x114372,0x124674,
    0x104078,0x11437A,0x12467C,
    0x104080,0x114382,0x124684,
    0x104088,0x11438A,0x12468C,
    0x104090,0x114392,0x124694,
    0x104098,0x11439A,0x12469C,
    0x1040A0,0x1143A2,0x1246A4,
    0x1040A8,0x1143AA,0x1246AC,
    0x1040B0,0x1143B2,0x1246B4,
    0x1040B8,0x1143BA,0x1246BC,
    0x1040C0,0x1143C2,0x1246C4,
    0x1040C8,0x1143CA,0x1246CC,
    0x1040D0,0x1143D2,0x1246D4,
    0x406010,0x436211,0x466412,
    0x406810,0x436A11,0x466C12,
    0x407010,0x437211,0x467412,
    0x407810,0x437A11,0x467C12,
    0x408010,0x438211,0x468412,
    0x408810,0x438A11,0x468C12,
    0x409010,0x439211,0x469412,
    0x409810,0x439A11,0x469C12,
    0x40A010,0x43A211,0x46A412,
    0x40A810,0x43AA11,0x46AC12,
    0x40B010,0x43B211,0x46B412,
    0x40B810,0x43BA11,0x46BC12,
    0x40C010,0x43C211,0x46C412,
    0x40C810,0x43CA11,0x46CC12,
    0x40D010,0x43D211,0x46D412,
    0x40D810,0x43DA11,0x46DC12,
    0x40E010,0x43E211,0x46E412,
    0x40E810,0x43EA11,0x46EC12,
    0x40F010,0x43F211,0x46F412,
    0x40F810,0x43FA11,0x46FC12,
    0x601040,0x621143,0x641246,
    0x681040,0x6A1143,0x6C1246,
    0x701040,0x721143,0x741246,
    0x781040,0x7A1143,0x7C1246,
    0x801040,0x821143,0x841246,
    0x881040,0x8A1143,0x8C1246,
    0x901040,0x921143,0x941246,
    0x981040,0x9A1143,0x9C1246,
    0xA01040,0xA21143,0xA41246,
    0xA81040,0xAA1143,0xAC1246,
    0xB01040,0xB21143,0xB41246,
    0xB81040,0xBA1143,0xBC1246,
    0xC01040,0xC21143,0xC41246,
    0xC81040,0xCA1143,0xCC1246,
    0xD01040,0xD21143,0xD41246
};

const uint32_t colors32[32] = {
    0x1001FF, 0x1030FF, 0x1060FF, 0x1080FF, 0x1096FF, 0x10A6FF, 0x10B6FF, 0x10C6FF, //blue
    0x10FF01, 0x10FF30, 0x10FF60, 0x10FF80, 0x10FF96, 0x10FFA6, 0x10FFB6, 0x10FFC6, //green
    0xA0FF01, 0xC0FF30, 0xF0FF60, 0xFFFF90, 0xFFB001, 0xFFA001, 0xFF9001, 0xFF8001, //yellow
    0xFF0101, 0xFF3001, 0xFF6001, 0xFF6080, 0xFF6090, 0xFF60A0, 0xFF60B0, 0xFF60C0  //red
};

const uint32_t pal_size_32 = 32;

const uint32_t color_black = 0x101010;

}

#endif
