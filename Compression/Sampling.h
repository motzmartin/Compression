#ifndef __SAMPLING_H__
#define __SAMPLING_H__

void SAMPLING_CrCbDownsample(unsigned char* plane,
    int width,
    int height,
    int samplingX,
    int samplingY,
    unsigned char* downsampledPlane);

void SAMPLING_CrCbUpsample(unsigned char* downsampledPlane,
    int width,
    int height,
    int samplingX,
    int samplingY,
    unsigned char* upsampledPlane);

#endif