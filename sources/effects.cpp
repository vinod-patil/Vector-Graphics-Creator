/*
 * This source file is part of EasyPaint.
 *
 * Copyright (c) 2012 EasyPaint <https://github.com/Gr1N/EasyPaint>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <QDebug>
#include "effects.h"
#include "imagearea.h"
#include "datasingleton.h"
Effects::Effects(ImageArea *pImageArea, QObject *parent) :
    QObject(parent)
{
    mPImageArea = pImageArea;
}

Effects::~Effects()
{

}

void Effects::gray()
{




 /*

    for(int i(0); i < mPImageArea->getImage()->width(); i++)
    {
        for(int y(0); y < mPImageArea->getImage()->height(); y++)
        {
            QRgb pixel(mPImageArea->getImage()->pixel(i, y));
            int rgb = (int)(0.299 * qRed(pixel) + 0.587 * qGreen(pixel) + 0.114 * qBlue(pixel));
            pixel = qRgb(rgb, rgb, rgb);
            mPImageArea->getImage()->setPixel(i, y, pixel);
        }
    }
    mPImageArea->setEdited(true);
    mPImageArea->update();*/
}

void Effects::negative()
{

    mPImageArea->getImage()->invertPixels(QImage::InvertRgb);
    qDebug()<<"fhfv";
    mPImageArea->setEdited(true);
    mPImageArea->update();
}
