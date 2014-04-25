#include "vectoreli.h"
#include "ui_vectoreli.h"
#include "imagearea.h"
#include <QtGui>
#include <QDebug>
#include <QtXml>
#include <QDir>
#include <QFile>
#include <QtGui/QPainter>
vectoreli::vectoreli(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::vectoreli)
{
    sr=sb=sg=0;
    fr=fb=fg=255;
    ui->setupUi(this);

}
void vectoreli::setcordi(int x1,int y1,int x2,int y2,QColor a)
{
    rx=(x2-x1)/2;
    ry=(y2-y1)/2;
    cx=x1+rx;
    cy=y1+ry;
    co=a;
    str1=QString::number(cx) + "," +QString::number(cy);
    str2=QString::number(rx) + "," +QString::number(ry);
    ui->lineEdit->setText(str1);
    ui->lineEdit_2->setText(str2);
    ui->label_4->setText(co.name());
    ui->label_4->setPalette(QPalette(co));
    ui->label_4->setAutoFillBackground(true);
    fr=co.red();
    fg=co.green();
    fb=co.blue();
    QColor tm=Qt::black;
    ui->label_4->setText(tm.name());
    ui->label_4->setPalette(QPalette(tm));
    ui->label_4->setAutoFillBackground(true);
}
void vectoreli::imgptr(ImageArea *i)
{
        p=i;
}

vectoreli::~vectoreli()
{
    delete ui;
}

void vectoreli::on_pushButton_clicked()
{
    QColor color=co;
    color = QColorDialog::getColor(Qt::green, this, "Select Color", QColorDialog::DontUseNativeDialog);
    if (color.isValid())
    {
        ui->label_4->setText(color.name());
        ui->label_4->setPalette(QPalette(color));
        ui->label_4->setAutoFillBackground(true);
        sr=color.red();
        sg=color.green();
        sb=color.blue();
    }
}


void vectoreli::on_pushButton_2_clicked()
{
    QColor color;
    color = QColorDialog::getColor(Qt::green, this, "Select Color", QColorDialog::DontUseNativeDialog);
    if (color.isValid())
    {
        ui->label_5->setText(color.name());
        ui->label_5->setPalette(QPalette(color));
        ui->label_5->setAutoFillBackground(true);
        fr=color.red();
        fg=color.green();
        fb=color.blue();
    }
}

void vectoreli::on_pushButton_3_clicked()
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
    cx=s1.toInt(&ok,10);
    s1="";

    for(j=0,++i;i <str1.size();++i)
    {

        s1[j]=str1.at(i);
        j++;
    }
    cy=s1.toInt(&ok,10);
    str1="";
    s1="";

    str1=ui->lineEdit_2->text();

    for (i = 0,j=0; str1.at(i) != QChar(',') ; ++i)
    {

        s1[j]=str1.at(i);
        j++;
    }
    rx=s1.toInt(&ok,10);
    qDebug()<<rx;
    s1="";
    for(j=0,++i;i <str1.size();++i)
    {

        s1[j]=str1.at(i);
        qDebug()<<s1[j];
        j++;
    }
    ry=s1.toInt(&ok,10);
    updateimg();
    xmleli();
}
void vectoreli::updateimg()
{
    QColor fill(fr,fg,fb);
    QColor stroke(sr,sg,sb);
    QPainter painter(p->getImage());
    painter.setPen(QPen(stroke,w,Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(QBrush(fill));
    painter.drawEllipse(QPoint(cx,cy),rx+2,ry+2);
    p->setEdited(true);
    painter.end();
    p->update();
}

void vectoreli::xmleli()
{
    QString str;
    QDomDocument svg;
    QDomElement elli=svg.createElement("ellipse");
    elli.setAttribute("cx",cx);
    elli.setAttribute("cy",cy);
    elli.setAttribute("rx",rx);
    elli.setAttribute("ry",ry);
    elli.setAttribute("fill","rgb(" + QString::number(fr) + "," + QString::number(fg) + "," + QString::number(fb) + ")" );
    elli.setAttribute("stroke","rgb(" + QString::number(sr) + "," + QString::number(sg) + "," + QString::number(sb) + ")" );
    elli.setAttribute("stroke-width",w);
    svg.appendChild(elli);
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

void vectoreli::on_pushButton_4_clicked()
{


}


