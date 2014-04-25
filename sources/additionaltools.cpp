#include "additionaltools.h"
#include "imagearea.h"
#include "resizedialog.h"

#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QLabel>
#include <QtGui/QTransform>
#include <QSize>

AdditionalTools::AdditionalTools(ImageArea *pImageArea, QObject *parent) :
    QObject(parent)
{
    mPImageArea = pImageArea;
    mZoomedFactor = 1;
}

AdditionalTools::~AdditionalTools()
{

}

void AdditionalTools::resizeCanvas(int width, int height, bool flag)
{
    if(flag)
    {
        ResizeDialog resizeDialog(QSize(width, height));
        if(resizeDialog.exec() == QDialog::Accepted)
        {
            QSize newSize = resizeDialog.getNewSize();
            width = newSize.width();
            height = newSize.height();
        }
    }

    if(width < 1 || height < 1)
        return;
    QImage *tempImage = new QImage(width, height, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(tempImage);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::white));
    painter.drawRect(QRect(0, 0, width, height));
    painter.drawImage(0, 0, *mPImageArea->getImage());
    painter.end();

    mPImageArea->setImage(*tempImage);

    mPImageArea->resize(mPImageArea->getImage()->rect().right() + 6,
                        mPImageArea->getImage()->rect().bottom() + 6);
    mPImageArea->setEdited(true);
}

void AdditionalTools::resizeImage()
{
    ResizeDialog resizeDialog(mPImageArea->getImage()->size());
    if(resizeDialog.exec() == QDialog::Accepted)
    {
        mPImageArea->setImage(mPImageArea->getImage()->scaled(resizeDialog.getNewSize()));
        mPImageArea->resize(mPImageArea->getImage()->rect().right() + 6,
                            mPImageArea->getImage()->rect().bottom() + 6);
        mPImageArea->setEdited(true);
    }
}

void AdditionalTools::rotateImage(bool flag)
{
    QTransform transform;
    if(flag)
    {
        transform.rotate(90);
    }
    else
    {
        transform.rotate(-90);
    }
    mPImageArea->setImage(mPImageArea->getImage()->transformed(transform));
    mPImageArea->resize(mPImageArea->getImage()->rect().right() + 6,
                        mPImageArea->getImage()->rect().bottom() + 6);
    mPImageArea->update();
    mPImageArea->setEdited(true);
}

bool AdditionalTools::zoomImage(qreal factor)
{
    mZoomedFactor *= factor;
    if(mZoomedFactor < 0.25)
    {
        mZoomedFactor = 0.25;
        return false;
    }
    else if(mZoomedFactor > 4)
    {
        mZoomedFactor = 4;
        return false;
    }
    else
    {
        mPImageArea->setImage(mPImageArea->getImage()->transformed(QTransform::fromScale(factor, factor)));
        mPImageArea->resize((mPImageArea->rect().width())*factor, (mPImageArea->rect().height())*factor);
        emit sendNewImageSize(mPImageArea->size());
        mPImageArea->setEdited(true);
        return true;
    }
}
