#include "vectorline.h"
#include "ui_vectorline.h"
#include "imagearea.h"
#include <QtGui>
#include <QtXml>
#include <QDebug>
#include <QDir>
#include <QFile>
vectorline::vectorline(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::vectorline)
{
    ui->setupUi(this);
    r=255;
    g=255;
    b=255;

    int frameStyle = QFrame::Sunken | QFrame::Panel;
    ui->label_4->setFrameStyle(frameStyle);


}
void vectorline::setcordi(int x1,int y1,int x2,int y2,QColor a)
{
    x11=x1;
    y11=y1;
    x22=x2;
    y22=y2;
    co=a;
    str1=QString::number(x11) + "," +QString::number(y11);
    str2=QString::number(x22) + "," +QString::number(y22);
    ui->lineEdit->setText(str1);
    ui->lineEdit_2->setText(str2);
    ui->label_4->setText(co.name());
    ui->label_4->setPalette(QPalette(co));
    ui->label_4->setAutoFillBackground(true);
    r=co.red();
    g=co.green();
    b=co.blue();
}


vectorline::~vectorline()
{
    delete ui;
}
void vectorline::imgptr(ImageArea *i)
{
        p=i;
}

void vectorline::on_pushButton_clicked()
{
    QColor color=co;
    color = QColorDialog::getColor(Qt::green, this, "Select Color", QColorDialog::DontUseNativeDialog);
    if (color.isValid())
    {
        ui->label_4->setText(color.name());
        ui->label_4->setPalette(QPalette(color));
        ui->label_4->setAutoFillBackground(true);
        r=color.red();
        g=color.green();
        b=color.blue();
    }
}

void vectorline::on_pushButton_2_clicked()
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
    x11=s1.toInt(&ok,10);

    s1="";
    for(j=0,++i;i <str1.size();++i)
    {

        s1[j]=str1.at(i);
        j++;
    }
    y11=s1.toInt(&ok,10);

    str1="";
    s1="";
    str1=ui->lineEdit_2->text();

    for (i = 0,j=0; str1.at(i) != QChar(',') ; ++i)
    {

        s1[j]=str1.at(i);
        j++;
    }
    x22=s1.toInt(&ok,10);
    s1="";
    for(j=0,++i;i <str1.size();++i)
    {

        s1[j]=str1.at(i);
        j++;
    }
    y22=s1.toInt(&ok,10);

    xmlline();
    updateimg();
}
void vectorline::updateimg()
{
    QColor color(r,g,b);
    QPainter painter(p->getImage());
    painter.setPen(QPen(color,w,Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(x11,y11,x22,y22);
    p->setEdited(true);
    painter.end();
    p->update();
}

void vectorline::xmlline()
{
    QString str;
    QDomDocument svg;
    QDomElement line=svg.createElement("line");
    line.setAttribute("x1",x11);
    line.setAttribute("y1",y11);
    line.setAttribute("x2",x22);
    line.setAttribute("y2",y22);
    line.setAttribute("stroke","rgb(" + QString::number(r) + "," + QString::number(g) + "," + QString::number(b) + ")" );
    line.setAttribute("stroke-width",w);
    svg.appendChild(line);
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
