#include "vectorrect.h"
#include "ui_vectorrect.h"
#include "imagearea.h"
#include <QtGui>
#include <QColor>
#include <QtXml>
#include <QDebug>
#include <QDir>
#include <QFile>
vectorrect::vectorrect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::vectorrect)
{
    sr=sb=sg=0;
    fr=fb=fg=255;
    ui->setupUi(this);
}
void vectorrect::setcordi(int x1,int y1,int x2,int y2,QColor a)
{
    xstart=x1;
    ystart=y1;
    height=y2-y1;
    width=x2-x1;
    co=a;
    str1=QString::number(xstart) + "," +QString::number(ystart);
    str2=QString::number(width) + "X" +QString::number(height);
    ui->lineEdit->setText(str1);
    ui->lineEdit_2->setText(str2);
    ui->label_6->setText(co.name());
    ui->label_6->setPalette(QPalette(co));
    ui->label_6->setAutoFillBackground(true);
    fr=co.red();
    fg=co.green();
    fb=co.blue();
    QColor tm=Qt::black;
    ui->label_7->setText(tm.name());
    ui->label_7->setPalette(QPalette(tm));
    ui->label_7->setAutoFillBackground(true);
}



vectorrect::~vectorrect()
{
    delete ui;
}

void vectorrect::imgptr(ImageArea *i)
{
        p=i;
}


void vectorrect::on_pushButton_clicked()
{
    QColor color=co;
    color = QColorDialog::getColor(Qt::green, this, "Select Color", QColorDialog::DontUseNativeDialog);
    if (color.isValid())
    {
        ui->label_6->setText(color.name());
        ui->label_6->setPalette(QPalette(color));
        ui->label_6->setAutoFillBackground(true);
        fr=color.red();
        fg=color.green();
        fb=color.blue();
    }
}

void vectorrect::on_pushButton_2_clicked()
{
    QColor color;
    color = QColorDialog::getColor(Qt::green, this, "Select Color", QColorDialog::DontUseNativeDialog);
    if (color.isValid())
    {
        ui->label_7->setText(color.name());
        ui->label_7->setPalette(QPalette(color));
        ui->label_7->setAutoFillBackground(true);
        sr=color.red();
        sg=color.green();
        sb=color.blue();
     }
}

void vectorrect::on_horizontalSlider_valueChanged(int value)
{
    QString str;
    float f;
    f=value/100.0;
    str.setNum(f,'g',1);
    ui->label_8->setText(str);
}

void vectorrect::on_horizontalSlider_2_valueChanged(int value)
{
    QString str;
    float f;
    f=value/100.0;
    str.setNum(f,'g',1);
    ui->label_9->setText(str);
}

void vectorrect::on_pushButton_3_clicked()
{
    w=ui->spinBox->value();

    QString str1,s1;
    int i,j;

    str1=ui->lineEdit->text();

    for (i = 0,j=0; str1.at(i) != QChar(',') ; ++i)
    {

        s1[j]=str1.at(i);
        j++;
    }
    bool ok;
    xstart=s1.toInt(&ok,10);
    s1="";

    for(j=0,++i;i <str1.size();++i)
    {

        s1[j]=str1.at(i);
        j++;
    }
    ystart=s1.toInt(&ok,10);
    str1="";
    s1="";

    str1=ui->lineEdit_2->text();

    for (i = 0,j=0; str1.at(i) != QChar('X') ; ++i)
    {

        s1[j]=str1.at(i);
        j++;
    }
    width=s1.toInt(&ok,10);
    s1="";
    for(j=0,++i;i <str1.size();++i)
    {

        s1[j]=str1.at(i);
        j++;
    }
    height=s1.toInt(&ok,10);

    fo=(ui->horizontalSlider->value())/100.0;
    so=(ui->horizontalSlider_2->value())/100.0;
    updateimg();
    xmlrecta();
}
void vectorrect::updateimg()
{
    QColor fill(fr,fg,fb);
    QColor stroke(sr,sg,sb);
    QPainter painter(p->getImage());
    painter.setPen(QPen(stroke,w,Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(QBrush(fill));
    painter.drawRect(xstart,ystart,width+1,height+1);
    p->setEdited(true);
    painter.end();
    p->update();

}

void vectorrect::xmlrecta()
{
    QString str;
    QDomDocument svg;
    QDomElement rect=svg.createElement("rect");
    rect.setAttribute("x",xstart);
    rect.setAttribute("y",ystart);
    rect.setAttribute("width",width);
    rect.setAttribute("height",height);
    rect.setAttribute("fill","rgb(" + QString::number(fr) + "," + QString::number(fg) + "," + QString::number(fb) + ")" );
    rect.setAttribute("stroke","rgb(" + QString::number(sr) + "," + QString::number(sg) + "," + QString::number(sb) + ")" );
    rect.setAttribute("stroke-width",w);
    rect.setAttribute("stroke-opacity",so);
    rect.setAttribute("fill-opacity",fo);
    svg.appendChild(rect);
    str=svg.toString();
    qDebug()<<str;

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
            qDebug()<<"Success";
        }



}
