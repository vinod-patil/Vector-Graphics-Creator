#include "svgcreater.h"
#include "datasingleton.h"
#include "imagearea.h"
#include <QtXml>
#include <QString>
#include <QtXml>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QChar>
#include <QPainter>
svgcreater::svgcreater()
{
        QDomDocument xml;
        QDomElement svg=xml.createElement("svg");
        svg.setAttribute("width",400);
        svg.setAttribute("version",1.1);
        svg.setAttribute("xmlns","http://www.w3.org/2000/svg");
        svg.setAttribute("height",300);

        xml.appendChild(svg);
        QDir a;
        QString str=xml.toString(),s1;
        int i=str.size();
        i=i-3;
        str[i] = QChar('>');
        i++;
        str[i] = QChar(' ');
        s1=a.currentPath() + "/tmp.svg";
        QFile file(s1);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug()<<"Failed to open";

        }
        else
        {

            QTextStream stream(&file);
            stream <<"\n";
            stream <<str;
            file.close();
            qDebug()<<"Success";
        }

}
void svgcreater::startpt(int a, int b,ImageArea *i)
{

    p=i;
    vec.imgptr(p);
    vecr.imgptr(p);
    vecl.imgptr(p);
    xstart=a;
    ystart=b;
}
void svgcreater::endpt(int a, int b,QColor c)
{
    xend=a;
    yend=b;
    col=c;
    vec.setcordi(xstart,ystart,xend,yend,col);
}
void svgcreater::endptr(int a, int b,QColor c)
{
    xend=a;
    yend=b;
    col=c;
    vecr.setcordi(xstart,ystart,xend,yend,col);
}
void svgcreater::endptl(int a, int b,QColor c)
{
    xend=a;
    yend=b;
    col=c;
    vecl.setcordi(xstart,ystart,xend,yend,col);
}

void svgcreater::svgline()
{


        vec.show();
}
void svgcreater::svgrect()
{

        vecr.show();
}
void svgcreater::svgelli()
{

        vecl.show();
}
