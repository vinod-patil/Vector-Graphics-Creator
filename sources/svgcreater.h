#ifndef SVGCREATER_H
#define SVGCREATER_H
#include "vectorline.h"
#include "vectorrect.h"
#include "vectoreli.h"
#include <QStack>
QT_BEGIN_NAMESPACE
class ImageArea;
QT_END_NAMESPACE
class svgcreater
{

private:
    int xstart,ystart,xend,yend;
    QColor col;
    ImageArea *p;
public:
    vectorline vec;
    vectorrect vecr;
    vectoreli vecl;
    svgcreater();
    void startpt(int,int,ImageArea *a);
    void endpt(int,int,QColor);
    void endptr(int,int,QColor);
    void endptl(int,int,QColor);
    void svgline();
    void svgrect();
    void svgelli();
  };

#endif // SVGCREATER_H
