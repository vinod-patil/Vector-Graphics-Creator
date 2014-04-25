#include "imagearea.h"
#include "paintinstruments.h"
#include "datasingleton.h"
#include "undocommand.h"

#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QFileDialog>
#include <QtCore/QDebug>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPrinter>
#include <QtGui/QPrintDialog>
#include <QtCore/QTimer>
#include <QtGui/QImageReader>
#include <QtGui/QImageWriter>
#include <QtGui/QUndoStack>
#include <QDir>
#include <QtXml>
#include <QDir>
#include <QDebug>

ImageArea::ImageArea(const bool &isOpen, const QString &filePath, QWidget *parent) :
    QWidget(parent), mIsEdited(false), mIsPaint(false), mIsResize(false)
{
    setMouseTracking(true);

    mRightButtonPressed = false;
    mFilePath.clear();
    makeFormatsFilters();
    initializeImage();
    mZoomFactor = 1;
    p=this;
    mPaintInstruments = new PaintInstruments(this);
    mAdditionalTools = new AdditionalTools(this);
    mEffects = new Effects(p);

    mUndoStack = new QUndoStack(this);

    if(isOpen && filePath.isEmpty())
    {
        open();
    }
    else if(isOpen && !filePath.isEmpty())
    {
        open(filePath);
    }
    else
    {
        QPainter *painter = new QPainter(mImage);
        painter->fillRect(0, 0,
                          DataSingleton::Instance()->getBaseSize().width(),
                          DataSingleton::Instance()->getBaseSize().height(),
                          Qt::white);
        painter->end();

        resize(mImage->rect().right() + 6,
               mImage->rect().bottom() + 6);
    }

    QTimer *autoSaveTimer = new QTimer(this);
    autoSaveTimer->setInterval(DataSingleton::Instance()->getAutoSaveInterval());
    connect(autoSaveTimer, SIGNAL(timeout()), this, SLOT(autoSave()));
    connect(mAdditionalTools, SIGNAL(sendNewImageSize(QSize)), this, SIGNAL(sendNewImageSize(QSize)));

    autoSaveTimer->start();
}

ImageArea::~ImageArea()
{

}

void ImageArea::initializeImage()
{
    mImage = new QImage(DataSingleton::Instance()->getBaseSize(),
                        QImage::Format_ARGB32_Premultiplied);
}

void ImageArea::open()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open image..."), "",
                                                    mOpenFilter, 0,
                                                    QFileDialog::DontUseNativeDialog);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if(!filePath.isEmpty())
    {
        mImage->load(filePath);
        *mImage = mImage->convertToFormat(QImage::Format_ARGB32_Premultiplied);
        mFilePath = filePath;

        resize(mImage->rect().right() + 6,
               mImage->rect().bottom() + 6);
    }
    QApplication::restoreOverrideCursor();
}



void ImageArea::open(const QString &filePath)
{
    if(mImage->load(filePath))
    {
        *mImage = mImage->convertToFormat(QImage::Format_ARGB32_Premultiplied);
        mFilePath = filePath;

        resize(mImage->rect().right() + 6,
               mImage->rect().bottom() + 6);
    }
}

void ImageArea::save()
{
    if(mFilePath.isEmpty())
    {
        saveAs();
    }
    else
    {
        mImage->save(mFilePath);
        mIsEdited = false;
    }
}

void ImageArea::saveAs()
{
    QString filter;
    QString fileName(getFileName());
    if(fileName.isEmpty())
    {
        fileName = tr("Untitled image");
    }
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save image..."), fileName, mSaveFilter
                                                    /*tr("*.png;;*.jpg;;*.jpeg;;*.bmp;;*.xbm;;*.xpm")*/,
                                                    &filter,QFileDialog::DontUseNativeDialog);




    QApplication::setOverrideCursor(Qt::WaitCursor);
    //parse file extension
    if(!filePath.isEmpty())
    {
        QString extension;
        //we should test it on windows, because of different slashes
        QString temp = filePath.split("/").last();
        //if user entered some extension
        if(temp.contains('.'))
        {
            temp = temp.split('.').last();
            if(QImageWriter::supportedImageFormats().contains(temp.toAscii()))
                extension = temp;
            else
                extension = "png"; //if format is unknown, save it as png format, but with user extension
        }
        else
        {
            extension = filter.split('.').last().remove(')');
            filePath += '.' + extension;
        }
        if(extension=="svg")
        {
            QString s;
            int i=filePath.size();
            i--;

            while(filePath[i]!='.')
            {
                i--;
            }
            i--;
            int e=i;
            qDebug()<<"e="<<e;
            while(filePath[i]!='/')
            {
                qDebug()<<filePath[i];
                i--;
            }
            int j;
            for(++i,j=0;i<=e;i++,j++)
            {
                s[j]=filePath[i];
                qDebug()<<s[j];
            }
            bool ok;
            QString str="</svg>";
            QDir a;
            QFile file(a.currentPath() + "/tmp.svg");
                if(!file.open(QIODevice::Append | QIODevice::Text))
                {
                    qDebug()<<"Failed to open";
                }
                else
                {
                    QTextStream stream(&file);
                    stream <<"\n";
                    stream <<str;
                    file.close();
                }
             ok=a.rename("tmp.svg",s + ".svg");

        }
        else
        {
            mImage->save(filePath, extension.toLatin1().data());
        }
        mFilePath = filePath;
        mIsEdited = false;
    }
    QApplication::restoreOverrideCursor();
}

void ImageArea::autoSave()
{
    if(mIsEdited && !mFilePath.isEmpty() && DataSingleton::Instance()->getIsAutoSave())
    {
        mImage->save(mFilePath);
        mIsEdited = false;
    }
}

void ImageArea::print()
{
    QPrinter *printer = new QPrinter();
    QPrintDialog *printDialog = new QPrintDialog(printer);
    if(printDialog->exec())
    {
        QPainter painter(printer);
        QRect rect = painter.viewport();
        QSize size = mImage->size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(mImage->rect());
        painter.drawImage(0, 0, *mImage);
    }
}

void ImageArea::resizeImage()
{
    mAdditionalTools->resizeImage();
    emit sendNewImageSize(mImage->size());
}

void ImageArea::resizeCanvas()
{
    mAdditionalTools->resizeCanvas(mImage->width(), mImage->height(), true);
    emit sendNewImageSize(mImage->size());
}

void ImageArea::rotateImage(bool flag)
{
    mAdditionalTools->rotateImage(flag);
    emit sendNewImageSize(mImage->size());
}

void ImageArea::zoomImage(qreal factor)
{
    mAdditionalTools->zoomImage(factor);
}

void ImageArea::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(event->pos().x() < mImage->rect().right() + 6 &&
                event->pos().x() > mImage->rect().right() &&
                event->pos().y() > mImage->rect().bottom() &&
                event->pos().y() < mImage->rect().bottom() + 6)
        {
            mIsResize = true;
            setCursor(Qt::SizeFDiagCursor);        
        }
        else
        {
            switch(DataSingleton::Instance()->getInstrument())
            {
            case NONE:
                break;
            case COLORPICKER: case MAGNIFIER:
                mIsPaint = true;
                break;
            case PEN: case ERASER: case SPRAY: case FILL:
                mPaintInstruments->setStartPoint(event->pos());
                mPaintInstruments->setEndPoint(event->pos());
                mIsPaint = true;
                mUndoStack->push(new UndoCommand(mImage, *this));
                break;
            case LINE: case RECTANGLE: case ELLIPSE:
                mPaintInstruments->setStartPoint(event->pos());
                svg.startpt(event->x(),event->y(),p);
                mPaintInstruments->setEndPoint(event->pos());
                mIsPaint = true;
                mImageCopy = *mImage;
                mUndoStack->push(new UndoCommand(mImage, *this));
                break;
            }
        }
    }
    if(event->button() == Qt::RightButton)
    {
        mRightButtonPressed = true;
        restoreCursor();
        switch(DataSingleton::Instance()->getInstrument())
        {
        case NONE: case ERASER:
            break;
        case COLORPICKER: case MAGNIFIER:
            mIsPaint = true;
            break;
        case PEN: case SPRAY:  case FILL:
            mPaintInstruments->setStartPoint(event->pos());
            mPaintInstruments->setEndPoint(event->pos());
            mIsPaint = true;
            mUndoStack->push(new UndoCommand(mImage, *this));
            break;
        case LINE: case RECTANGLE: case ELLIPSE:
            mPaintInstruments->setStartPoint(event->pos());
            mPaintInstruments->setEndPoint(event->pos());
            mIsPaint = true;
            mImageCopy = *mImage;
            mUndoStack->push(new UndoCommand(mImage, *this));
            break;
        }
    }
}

void ImageArea::mouseMoveEvent(QMouseEvent *event)
{
    if(mIsResize)
    {
         mAdditionalTools->resizeCanvas(event->x(), event->y());
         emit sendNewImageSize(mImage->size());
    }
    else if(event->pos().x() < mImage->rect().right() + 6 &&
            event->pos().x() > mImage->rect().right() &&
            event->pos().y() > mImage->rect().bottom() &&
            event->pos().y() < mImage->rect().bottom() + 6)
    {
        setCursor(Qt::SizeFDiagCursor);
    }
    else
    {
        restoreCursor();
    }
    if(event->pos().x() < mImage->width() &&
            event->pos().y() < mImage->height())
    {
        emit sendCursorPos(event->pos());
        if(DataSingleton::Instance()->getInstrument() == COLORPICKER)
        {
            QRgb pixel(mImage->pixel(event->pos()));
            QColor getColor(pixel);
            emit sendColor(getColor);
        }
    }
    if((event->buttons() & Qt::LeftButton) && mIsPaint)
    {
        switch(DataSingleton::Instance()->getInstrument())
        {
        case NONE: case MAGNIFIER: case COLORPICKER:
        case FILL:
            break;
        case PEN:
            mPaintInstruments->setEndPoint(event->pos());
            mPaintInstruments->line(false);
            mPaintInstruments->setStartPoint(event->pos());
            break;
        case ERASER:
            mPaintInstruments->setEndPoint(event->pos());
            mPaintInstruments->line(false, true);
            mPaintInstruments->setStartPoint(event->pos());
            break;
        case LINE:
            mPaintInstruments->setEndPoint(event->pos());
            *mImage = mImageCopy;
            mPaintInstruments->line(false);
            break;
        case RECTANGLE:
            mPaintInstruments->setEndPoint(event->pos());
            *mImage = mImageCopy;
            mPaintInstruments->rectangle(false);
            break;
        case ELLIPSE:
            mPaintInstruments->setEndPoint(event->pos());
            *mImage = mImageCopy;
            mPaintInstruments->ellipse(false);
            break;
        case SPRAY:
            mPaintInstruments->setEndPoint(event->pos());
            mPaintInstruments->spray(false);
            mPaintInstruments->setStartPoint(event->pos());
            break;
        }
    }
    if((event->buttons() & Qt::RightButton) && mIsPaint)
    {
        switch(DataSingleton::Instance()->getInstrument())
        {
        case NONE: case ERASER: case MAGNIFIER: case COLORPICKER:
        case FILL:
            break;
        case PEN:
            mPaintInstruments->setEndPoint(event->pos());
            mPaintInstruments->line(true);
            mPaintInstruments->setStartPoint(event->pos());
            break;
        case LINE:
            mPaintInstruments->setEndPoint(event->pos());
            *mImage = mImageCopy;
            mPaintInstruments->line(true);
            break;
        case RECTANGLE:
            mPaintInstruments->setEndPoint(event->pos());
            *mImage = mImageCopy;
            mPaintInstruments->rectangle(true);
            break;
        case ELLIPSE:
            mPaintInstruments->setEndPoint(event->pos());
            *mImage = mImageCopy;
            mPaintInstruments->ellipse(true);
            break;
        case SPRAY:
            mPaintInstruments->setEndPoint(event->pos());
            mPaintInstruments->spray(true);
            mPaintInstruments->setStartPoint(event->pos());
            break;
        }
    }
}

void ImageArea::mouseReleaseEvent(QMouseEvent *event)
{
    if(mIsResize)
    {
       mIsResize = false;
       restoreCursor();
    }
    if(event->button() == Qt::LeftButton && mIsPaint)
    {
        mPaintInstruments->setEndPoint(event->pos());
        switch(DataSingleton::Instance()->getInstrument())
        {
        case NONE:
            break;
        case MAGNIFIER:
            if(mAdditionalTools->zoomImage(2.0))
            {
                setZoomFactor(2.0);
            }
            break;
        case PEN:
            mPaintInstruments->line(false);
            mIsPaint = false;
            break;
        case ERASER:
            mPaintInstruments->line(false, true);
            mIsPaint = false;
            break;
        case LINE:
            *mImage = mImageCopy;
            svg.endpt(event->x(),event->y(),DataSingleton::Instance()->getPrimaryColor());
            svg.svgline();
            mPaintInstruments->line(false);
            mIsPaint = false;
            break;
        case COLORPICKER:
            mPaintInstruments->setStartPoint(event->pos());
            mPaintInstruments->setEndPoint(event->pos());
            mPaintInstruments->colorPicker(false);
            mIsPaint = false;
            emit sendPrimaryColorView();
            emit sendRestorePreviousInstrument();
            break;
        case RECTANGLE:
            *mImage = mImageCopy;
            svg.endptr(event->x(),event->y(),DataSingleton::Instance()->getSecondaryColor());
            svg.svgrect();
            mPaintInstruments->rectangle(false);
            mIsPaint = false;
            break;
        case ELLIPSE:
            *mImage = mImageCopy;
            svg.endptl(event->x(),event->y(),DataSingleton::Instance()->getSecondaryColor());
            svg.svgelli();
            mPaintInstruments->ellipse(false);
            mIsPaint = false;
            break;
        case SPRAY:
            mPaintInstruments->spray(false);
            mIsPaint = false;
            break;
        case FILL:
            mPaintInstruments->fill(false);
            mIsPaint = false;
            break;
        }
    }
    if(event->button() == Qt::RightButton && mIsPaint)
    {
        mRightButtonPressed = false;
        restoreCursor();
        mPaintInstruments->setEndPoint(event->pos());
        switch(DataSingleton::Instance()->getInstrument())
        {
        case NONE: case ERASER:
            break;
        case MAGNIFIER:
            if(mAdditionalTools->zoomImage(0.5))
            {
                setZoomFactor(0.5);
            }
            break;
        case PEN:
            mPaintInstruments->line(true);
            mIsPaint = false;
            break;
        case LINE:
            *mImage = mImageCopy;
            mPaintInstruments->line(true);
            mIsPaint = false;
            break;
        case COLORPICKER:
            mPaintInstruments->setStartPoint(event->pos());
            mPaintInstruments->setEndPoint(event->pos());
            mPaintInstruments->colorPicker(true);
            mIsPaint = false;
            emit sendSecondaryColorView();
            emit sendRestorePreviousInstrument();
            break;
        case RECTANGLE:
            *mImage = mImageCopy;
            mPaintInstruments->rectangle(true);
            mIsPaint = false;
            break;
        case ELLIPSE:
            *mImage = mImageCopy;
            mPaintInstruments->ellipse(true);
            mIsPaint = false;
            break;
        case SPRAY:
            mPaintInstruments->spray(true);
            mIsPaint = false;
            break;
        case FILL:
            mPaintInstruments->fill(true);
            mIsPaint = false;
            break;
        }
    }
}

void ImageArea::paintEvent(QPaintEvent *event)
{
    QPainter *painter = new QPainter(this);
    QRect *rect = new QRect(event->rect());

    painter->setBrush(QBrush(QPixmap(":media/textures/transparent.jpg")));
    painter->drawRect(mImage->rect());

    painter->drawImage(*rect, *mImage, *rect);

    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(Qt::black));
    painter->drawRect(QRect(mImage->rect().right(),
                            mImage->rect().bottom(), 6, 6));

    painter->end();
}

void ImageArea::restoreCursor()
{
    switch(DataSingleton::Instance()->getInstrument())
    {
    case MAGNIFIER:
        mPixmap = new QPixmap(":/media/instruments-icons/cursor_loupe.png");
        mCurrentCursor = new QCursor(*mPixmap);
        setCursor(*mCurrentCursor);
        break;
    case NONE:
        mCurrentCursor = new QCursor(Qt::ArrowCursor);
        setCursor(*mCurrentCursor);
        break;
    case ERASER: case PEN:
        ImageArea::drawCursor();
        mCurrentCursor = new QCursor(*mPixmap);
        setCursor(*mCurrentCursor);
        break;
    case COLORPICKER:
        mPixmap = new QPixmap(":/media/instruments-icons/cursor_pipette.png");
        mCurrentCursor = new QCursor(*mPixmap);
        setCursor(*mCurrentCursor);
        break;
    case RECTANGLE: case ELLIPSE: case LINE:
        mCurrentCursor = new QCursor(Qt::CrossCursor);
        setCursor(*mCurrentCursor);
        break;
    case SPRAY:
        mPixmap = new QPixmap(":/media/instruments-icons/cursor_spray.png");
        mCurrentCursor = new QCursor(*mPixmap);
        setCursor(*mCurrentCursor);
        break;
    case FILL:
        mPixmap = new QPixmap(":/media/instruments-icons/cursor_fill.png");
        mCurrentCursor = new QCursor(*mPixmap);
        setCursor(*mCurrentCursor);
        break;
    }
}

void ImageArea::drawCursor()
{
    QPainter painter;
    mPixmap = new QPixmap(25, 25);
    QPoint center(13, 13);
    switch(DataSingleton::Instance()->getInstrument())
    {
    case NONE: case LINE: case COLORPICKER: case MAGNIFIER: case  SPRAY:
    case FILL: case RECTANGLE: case ELLIPSE:
        break;
    case PEN: case ERASER:
        mPixmap->fill(QColor(0, 0, 0, 0));
        break;
    }
    painter.begin(mPixmap);
    switch(DataSingleton::Instance()->getInstrument())
    {
    case NONE: case LINE: case COLORPICKER: case MAGNIFIER: case  SPRAY:
    case FILL: case RECTANGLE: case ELLIPSE:
        break;
    case PEN:
        if(mRightButtonPressed)
        {
            painter.setPen(QPen(DataSingleton::Instance()->getSecondaryColor()));
            painter.setBrush(QBrush(DataSingleton::Instance()->getSecondaryColor()));
        }
        else
        {
            painter.setPen(QPen(DataSingleton::Instance()->getPrimaryColor()));
            painter.setBrush(QBrush(DataSingleton::Instance()->getPrimaryColor()));
        }
        painter.drawEllipse(center, DataSingleton::Instance()->getPenSize()/2,
                        DataSingleton::Instance()->getPenSize()/2);
        break;
    case ERASER:
        painter.setBrush(QBrush(Qt::white));
        painter.drawEllipse(center, DataSingleton::Instance()->getPenSize()/2,
                        DataSingleton::Instance()->getPenSize()/2);
        break;
    }
    painter.setPen(Qt::black);
    painter.drawPoint(13, 13);
    painter.drawPoint(13, 3);
    painter.drawPoint(13, 5);
    painter.drawPoint(13, 21);
    painter.drawPoint(13, 23);
    painter.drawPoint(3, 13);
    painter.drawPoint(5, 13);
    painter.drawPoint(21, 13);
    painter.drawPoint(23, 13);
    painter.setPen(Qt::white);
    painter.drawPoint(13, 12);
    painter.drawPoint(13, 14);
    painter.drawPoint(12, 13);
    painter.drawPoint(14, 13);
    painter.drawPoint(13, 4);
    painter.drawPoint(13, 6);
    painter.drawPoint(13, 20);
    painter.drawPoint(13, 22);
    painter.drawPoint(4, 13);
    painter.drawPoint(6, 13);
    painter.drawPoint(20, 13);
    painter.drawPoint(22, 13);
    painter.end();
}

void ImageArea::makeFormatsFilters()
{
    QList<QByteArray> ba = QImageReader::supportedImageFormats();
    //make "all supported" part
    mOpenFilter = "All supported (";
    foreach (QByteArray temp, ba)
        mOpenFilter += "*." + temp + " ";
    mOpenFilter[mOpenFilter.length() - 1] = ')'; //delete last space
    mOpenFilter += ";;";

    //using ";;" as separator instead of "\n", because Qt's docs recomended it :)
    if(ba.contains("bmp"))
        mOpenFilter += "Windows Bitmap(*.bmp);;";
    if(ba.contains("gif"))
        mOpenFilter += "Graphic Interchange Format(*.gif);;";
    if(ba.contains("jpg") || ba.contains("jpeg"))
        mOpenFilter += "Joint Photographic Experts Group(*.jpg *.jpeg);;";
    if(ba.contains("mng"))
        mOpenFilter += "Multiple-image Network Graphics(*.mng);;";
    if(ba.contains("png"))
        mOpenFilter += "Portable Network Graphics(*.png);;";
    if(ba.contains("pbm"))
        mOpenFilter += "Portable Bitmap(*.pbm);;";
    if(ba.contains("pgm"))
        mOpenFilter += "Portable Graymap(*.pgm);;";
    if(ba.contains("ppm"))
        mOpenFilter += "Portable Pixmap(*.ppm);;";
    if(ba.contains("tiff") || ba.contains("tif"))
        mOpenFilter += "Tagged Image File Format(*.tiff, *tif);;";
    if(ba.contains("xbm"))
        mOpenFilter += "X11 Bitmap(*.xbm);;";
    if(ba.contains("xpm"))
        mOpenFilter += "X11 Pixmap(*.xpm);;";
    if(ba.contains("svg"))
        mOpenFilter += "Scalable Vector Graphics(*.svg);;";

    mOpenFilter += "All Files(*.*)";

    //make saveFilter
    ba = QImageWriter::supportedImageFormats();
    if(ba.contains("bmp"))
        mSaveFilter += "Windows Bitmap(*.bmp)";
    if(ba.contains("jpg") || ba.contains("jpeg"))
        mSaveFilter += ";;Joint Photographic Experts Group(*.jpg)";
    if(ba.contains("png"))
        mSaveFilter += ";;Portable Network Graphics(*.png)";
    if(ba.contains("ppm"))
        mSaveFilter += ";;Portable Pixmap(*.ppm)";
    if(ba.contains("tiff") || ba.contains("tif"))
        mSaveFilter += ";;Tagged Image File Format(*.tiff)";
    if(ba.contains("xbm"))
        mSaveFilter += ";;X11 Bitmap(*.xbm)";
    if(ba.contains("xpm"))
        mSaveFilter += ";;X11 Pixmap(*.xpm)";
        mSaveFilter += ";;Scalable Vector Graphics(*.svg)";
    }
