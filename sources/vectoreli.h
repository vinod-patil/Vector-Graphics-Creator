#ifndef VECTORELI_H
#define VECTORELI_H

#include <QDialog>
#include <QString>
#include <QImage>
QT_BEGIN_NAMESPACE
class ImageArea;
QT_END_NAMESPACE

namespace Ui {
    class vectoreli;
}

class vectoreli : public QDialog
{
    Q_OBJECT

public:
    explicit vectoreli(QWidget *parent = 0);
    void xmleli();
    void setcordi(int,int,int,int,QColor);
    void updateimg();
    void imgptr(ImageArea *a);
    ~vectoreli();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::vectoreli *ui;
    QString str1,str2;
    int cx,cy,rx,ry,w,fr,fg,fb,sr,sg,sb;
   QColor co;
   ImageArea *p;
};

#endif // VECTORELI_H
