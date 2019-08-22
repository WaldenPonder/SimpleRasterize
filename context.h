#ifndef CONTEXT_H
#define CONTEXT_H

#include "Object.h"

class Context : public Object
{
public:
    __class__(Context)

    Matrix viewMatrix_;
    Matrix projectionMatrix_;

    Vec3 clearColor_;

    int width_, height_;

    vector<Vec3Array> colorBuffer_;
    vector<FloatArray> depthBuffer_;
};

namespace g {
   extern Context context;
   extern int rotation_axis;
}



#endif // CONTEXT_H
